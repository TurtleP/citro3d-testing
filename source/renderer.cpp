#include "renderer.hpp"
#include "shader.hpp"

#include "logfile.hpp"

using namespace love;

Renderer::Renderer() : current(nullptr), currentTexture(nullptr), inFrame(false)
{
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 2);

    for (uint8_t index = 0; index < this->framebuffers.size(); index++)
        framebuffers[index].Create(index);

    C3D_CullFace(GPU_CULL_NONE);
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL);

    C3D_AttrInfo* attributes = C3D_GetAttrInfo();
    AttrInfo_Init(attributes);

    AttrInfo_AddLoader(attributes, 0, GPU_FLOAT, 3); // position
    AttrInfo_AddLoader(attributes, 1, GPU_FLOAT, 4); // color
    AttrInfo_AddLoader(attributes, 2, GPU_FLOAT, 2); // texcoord
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

        this->commands.clear();
        this->inFrame = true;
    }

    this->current = &this->framebuffers[index];
    C3D_FrameDrawOn(this->current->GetTarget());

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

bool Renderer::CheckHandle(C3D_Tex* texture)
{
    if (!texture)
        return false;

    if (this->currentTexture == texture)
        return false;

    this->currentTexture = texture;
    return true;
}

bool Renderer::Render(DrawCommand& command)
{
    love::Shader::defaults[love::Shader::STANDARD_DEFAULT]->Attach();

    if (!command.buffer->IsValid())
        return false;

    if (command.handles.size() > 0)
    {
        if (this->CheckHandle(command.handles.back()))
            C3D_TexBind(0, this->currentTexture);
    }

    auto mode = vertex::GetMode(command.mode);

    C3D_SetBufInfo(command.buffer->GetBuffer());
    C3D_DrawArrays(mode, 0, command.count);

    this->commands.push_back(command.buffer);

    return true;
}
