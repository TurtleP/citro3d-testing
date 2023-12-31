#include <memory>
#include <string>

#include "logfile.hpp"
#include "renderer.hpp"
#include "shader.hpp"

using namespace love;

#define SHADERS_DIR "romfs:/shaders/"

#define DEFAULT_SHADER (SHADERS_DIR "main_v_pica.shbin")

static bool loadShaderFile(const char* filepath, std::unique_ptr<uint32_t[]>& data,
                           DVLB_s*& program, std::string& error)
{
    FILE* file = std::fopen(filepath, "r");

    if (!file)
    {
        error = "File does not exist.";
        std::fclose(file);
        return false;
    }

    std::fseek(file, 0, SEEK_END);
    long size = std::ftell(file);
    std::rewind(file);

    try
    {
        data = std::make_unique<uint32_t[]>(size / 4);
    }
    catch (std::bad_alloc&)
    {
        error = "Not enough memory.";
        return false;
    }

    long readSize = (long)std::fread(data.get(), 1, size, file);

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

    if (!loadShaderFile(DEFAULT_SHADER, this->data, this->binary, error))
        throw love::Exception("Failed to load shader.");

    shaderProgramInit(&this->program);
    shaderProgramSetVsh(&this->program, &this->binary->DVLE[0]);

    this->uLoc_mdlView = shaderInstanceGetUniformLocation(this->program.vertexShader, "mdlvMtx");
    this->uLoc_projMtx = shaderInstanceGetUniformLocation(this->program.vertexShader, "projMtx");
}

Shader::~Shader()
{
    for (int i = 0; i < STANDARD_MAX_ENUM; i++)
    {
        if (this == Shader::defaults[i])
            Shader::defaults[i] = nullptr;
    }

    if (Shader::current == this)
        Shader::AttachDefault(STANDARD_DEFAULT);

    shaderProgramFree(&this->program);
    DVLB_Free(this->binary);
}

void Shader::Attach()
{
    if (Shader::current != this)
    {
        C3D_BindProgram(&this->program);
        Shader::current = this;
    }
}

void Shader::AttachDefault(StandardShader type)
{
    Shader* defaultShader = Shader::defaults[type];

    if (defaultShader == nullptr)
    {
        current = nullptr;
        return;
    }

    if (current != defaultShader)
        defaultShader->Attach();
}
