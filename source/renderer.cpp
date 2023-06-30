#include "renderer.hpp"
#include "shader.hpp"

#include "logfile.hpp"

using namespace love;

Renderer::Renderer() : inFrame(false)
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 2);

    for (uint8_t index = 0; index < this->framebuffers.size(); index++)
        framebuffers[index].Create(index);

    C3D_CullFace(GPU_CULL_NONE);
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

    C3D_AttrInfo* attributes = C3D_GetAttrInfo();
    AttrInfo_Init(attributes);

    AttrInfo_AddLoader(attributes, 0, GPU_FLOAT, 3); // position
    AttrInfo_AddLoader(attributes, 1, GPU_FLOAT, 4); // color
    AttrInfo_AddLoader(attributes, 2, GPU_SHORT, 2); // texcoord
}

Renderer::~Renderer()
{
    C3D_Fini();
    gfxExit();
}

void Renderer::BindFramebuffer(size_t index)
{
    if (!this->inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        this->inFrame = true;
    }

    this->current = &this->framebuffers[index];
    C3D_FrameDrawOn(this->current->GetTarget());
}

void Renderer::Clear(const Color& color)
{
    C3D_RenderTargetClear(this->current->GetTarget(), C3D_CLEAR_ALL, color.abgr(), 0);
}

void Renderer::Present()
{
    if (this->inFrame)
    {
        C3D_FrameEnd(0);

        this->commands.clear();
        this->inFrame = false;
    }
}

bool Renderer::Render(DrawCommand& command)
{
    love::Shader::defaults[love::Shader::STANDARD_DEFAULT]->Attach();

    if (!command.buffer->IsValid())
        return false;

    this->current->UpdateProjection(Shader::current->GetUniformLocations());

    command.buffer->SetBufInfo();
    C3D_DrawArrays(GPU_TRIANGLE_FAN, 0, command.count);

    this->commands.push_back(command.buffer);

    return true;
}
