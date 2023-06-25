#include "renderer.hpp"
#include "shader.hpp"

using namespace love;

Renderer::Renderer() : inFrame(false)
{
    gfxInitDefault();
    gfxSet3D(true);

    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

    for (uint8_t index = 0; index < this->framebuffers.size(); index++)
        framebuffers[index].Create(index);

    C3D_CullFace(GPU_CULL_NONE);
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
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

    if (Shader::current != nullptr)
        this->current->UpdateProjection(Shader::current->GetUniformLocations());
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
        this->inFrame = false;
    }
}