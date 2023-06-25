#include "framebuffer.hpp"

using namespace love;

Framebuffer::Framebuffer() : target(nullptr), width(0), height(0), viewport {}, scissor {}
{}

Framebuffer::~Framebuffer()
{
    if (this->target)
        C3D_RenderTargetDelete(this->target);
}

void Framebuffer::Create(int screen)
{
    this->id = screen;
    Mtx_Identity(&this->modelView);

    switch (screen)
    {
        case 0 ... 1:
        {
            this->SetSize(400, 240, GFX_TOP, (screen == 0) ? GFX_LEFT : GFX_RIGHT);
            break;
        }
        case 2:
        {
            this->SetSize(320, 240, GFX_BOTTOM, GFX_LEFT);
            break;
        }
        default:
            break;
    }
}

void Framebuffer::SetSize(int width, int height, gfxScreen_t screen, gfx3dSide_t side)
{
    this->target = C3D_RenderTargetCreate(height, width, GPU_RB_RGBA8, GPU_RB_DEPTH16);

    if (this->target)
        C3D_RenderTargetSetOutput(this->target, screen, side, Framebuffer::DISPLAY_FLAGS);

    this->viewport = { 0, 0, width, height };
    this->scissor  = { 0, 0, width, height };

    this->SetViewport();
    this->SetScissor();
}

void Framebuffer::SetViewport(const Rect& viewport)
{
    Rect newViewport = viewport;
    if (viewport == Rect::EMPTY)
        newViewport = this->viewport;

    C3D_SetViewport(newViewport.x, newViewport.y, newViewport.w, newViewport.h);
    Mtx_OrthoTilt(&this->projView, 0, newViewport.w, newViewport.h, 0, -10, 10, false);
}

void Framebuffer::SetScissor(const Rect& scissor)
{
    if (viewport == Rect::EMPTY)
        C3D_SetScissor(GPU_SCISSOR_DISABLE, 0, 0, this->scissor.w, this->scissor.h);
    else
        C3D_SetScissor(GPU_SCISSOR_DISABLE, scissor.x, scissor.y, scissor.w, scissor.h);
}

void Framebuffer::UpdateProjection(std::pair<int8_t, int8_t> locations)
{
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, locations.first, &this->projView);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, locations.second, &this->modelView);
}