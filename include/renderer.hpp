#pragma once

#include <array>

#include "color.hpp"
#include "framebuffer.hpp"

namespace love
{
    class Renderer
    {
      public:
        Renderer();

        static Renderer& Instance()
        {
            static Renderer renderer;
            return renderer;
        }

        Framebuffer* GetCurrent() const
        {
            return this->current;
        }

        void BindFramebuffer(size_t index = 0);

        void Clear(const Color& color);

        void Present();

      private:
        std::array<Framebuffer, 0x03> framebuffers;
        Framebuffer* current;

        bool inFrame;
    };
} // namespace love