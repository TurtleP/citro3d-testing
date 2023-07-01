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

    C3D_TexEnv* env = nullptr;

    env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);

    // texenv1.rgb = mix(texclr.rgb, texenv0.rgb, vtx.blend.y);
    env = C3D_GetTexEnv(1);
    C3D_TexEnvInit(env);
    C3D_TexEnvSrc(env, C3D_RGB, GPU_TEXTURE0, GPU_PREVIOUS, GPU_TEXTURE3);
    C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR,
                    GPU_TEVOP_RGB_ONE_MINUS_SRC_ALPHA);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_INTERPOLATE);

    // env = C3D_GetTexEnv(0);
    // C3D_TexEnvInit(env);
    // C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    // C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);

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

bool Renderer::Render(DrawCommand& command)
{
    love::Shader::defaults[love::Shader::STANDARD_DEFAULT]->Attach();

    if (!command.buffer->IsValid())
        return false;

    this->current->UpdateProjection(Shader::current->GetUniformLocations());

    auto mode = vertex::GetMode(command.mode);

    if (command.texture)
        C3D_TexBind(0, command.texture);

    C3D_DrawArrays(mode, 0, command.count);

    this->commands.push_back(command.buffer);

    return true;
}
