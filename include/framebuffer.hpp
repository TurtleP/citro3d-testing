#pragma once

#include <3ds.h>

#include <citro3d.h>

#include "math.hpp"

namespace love
{
    class Framebuffer
    {
      public:
        Framebuffer();

        ~Framebuffer();

        void Create(int id);

        void Destroy();

        int GetWidth() const
        {
            return this->width;
        }

        int GetHeight() const
        {
            return this->height;
        }

        C3D_RenderTarget* GetTarget() const
        {
            return this->target;
        }

        C3D_Mtx& GetModelView()
        {
            return this->modelView;
        }

        C3D_Mtx& GetProjectionView()
        {
            return this->projView;
        }

        void UpdateProjection(std::pair<int8_t, int8_t> locations);

        void SetViewport(const Rect& viewport = Rect::EMPTY);

        void SetScissor(const Rect& scissor = Rect::EMPTY);

        Rect GetViewport() const
        {
            return this->viewport;
        }

        Rect GetScissor() const
        {
            return this->scissor;
        }

      private:
        static constexpr auto DISPLAY_FLAGS = GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
                                              GX_TRANSFER_RAW_COPY(0) |
                                              GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
                                              GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
                                              GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);

        void SetSize(int width, int height, gfxScreen_t screen, gfx3dSide_t side);

        const Rect CalculateBounds(const Rect& rect);

        C3D_RenderTarget* target;

        C3D_Mtx modelView;
        C3D_Mtx projView;

        int id;

        int width;
        int height;

        Rect viewport;
        Rect scissor;
    };
} // namespace love