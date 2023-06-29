#include <memory>
#include <string>

#include "logfile.hpp"
#include "renderer.hpp"
#include "shader.hpp"

using namespace love;

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_SHADER (SHADERS_DIR "main_v_pica.shbin")

static bool loadShaderFile(const char* filepath, DVLB_s*& program, std::string& error)
{
    FILE* file = std::fopen(filepath, "r");

    if (!file)
    {
        error = "File does not exist.";
        std::fclose(file);
        return false;
    }

    std::unique_ptr<uint32_t[]> data;

    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::rewind(file);

    try
    {
        data = std::make_unique<uint32_t[]>(size);
    }
    catch (std::bad_alloc&)
    {
        error = "Not enough memory.";
        return false;
    }

    uint32_t readSize = (uint32_t)std::fread(data.get(), 1, size, file);

    if (readSize != size)
    {
        error = "Failed to read whole file.";
        std::fclose(file);
        return false;
    }

    std::fclose(file);
    program = DVLB_ParseFile(data.get(), size);

    return true;
}

Shader::Shader()
{
    std::string error {};

    if (!loadShaderFile(DEFAULT_SHADER, this->binary, error))
        return;

    shaderProgramInit(&this->program);
    shaderProgramSetVsh(&this->program, &this->binary->DVLE[0]);

    this->uLoc_mdlView = shaderInstanceGetUniformLocation(this->program.vertexShader, "mdlvMtx");
    this->uLoc_projMtx = shaderInstanceGetUniformLocation(this->program.vertexShader, "projMtx");
}

Shader::~Shader()
{
    shaderProgramFree(&this->program);
    DVLB_Free(this->binary);
}

void Shader::Attach()
{
    if (Shader::current != this)
    {
        LOG("Binding shader!");
        C3D_BindProgram(&this->program);
        LOG("Updating projection!");
        Renderer::Instance().GetCurrent()->UpdateProjection(this->GetUniformLocations());
        LOG("Done!");
        Shader::current = this;
    }
}
