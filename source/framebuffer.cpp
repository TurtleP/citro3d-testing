#include "framebuffer.hpp"

using namespace love;

Framebuffer::Framebuffer() : target(nullptr), width(0), height(0), viewport {}, scissor {}
{}

Framebuffer::~Framebuffer()
{}

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

void Framebuffer::Destroy()
{
    if (this->target)
        C3D_RenderTargetDelete(this->target);

    this->target = nullptr;
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

const Rect Framebuffer::CalculateBounds(const Rect& bounds)
{
    // clang-format off
    const uint32_t left   = this->height > (bounds.y + bounds.h) ? this->height - (bounds.y + bounds.h) : 0;
    const uint32_t top    = this->width  > (bounds.x + bounds.w) ? this->width - (bounds.x + bounds.w) : 0;
    const uint32_t right  = this->height - bounds.y;
    const uint32_t bottom = this->width  - bounds.x;
    // clang-format on

    return { (int)left, (int)top, (int)right, (int)bottom };
}

void Framebuffer::SetViewport(const Rect& viewport)
{
    Rect newViewport = viewport;
    if (viewport == Rect::EMPTY)
        newViewport = this->viewport;

    // const auto new viewPort = calculateBounds(viewport);
    // C3D_SetViewport(newViewport.x, newViewport.y, newViewport.w, newViewport.h);
    Mtx_OrthoTilt(&this->projView, newViewport.x, newViewport.w, newViewport.h, newViewport.y, -10,
                  10, true);
}

void Framebuffer::SetScissor(const Rect& scissor)
{
    const bool enable    = (scissor != Rect::EMPTY);
    GPU_SCISSORMODE mode = enable ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

    Rect newScissor = this->CalculateBounds(scissor);
    if (viewport == Rect::EMPTY)
        newScissor = this->CalculateBounds(this->scissor);

    C3D_SetScissor(mode, newScissor.x, newScissor.y, newScissor.w, newScissor.h);
}

void Framebuffer::UpdateProjection(std::pair<int8_t, int8_t> locations)
{
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, locations.first, &this->projView);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, locations.second, &this->modelView);
}