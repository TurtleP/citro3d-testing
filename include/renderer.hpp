#pragma once

#include <array>
#include <memory>
#include <vector>

#include "color.hpp"
#include "drawcommand.hpp"
#include "framebuffer.hpp"

namespace love
{
    class Renderer
    {
      public:
        Renderer();

        ~Renderer();

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

        bool Render(DrawCommand& command);

      private:
        std::array<Framebuffer, 0x03> framebuffers;
        Framebuffer* current;

        bool inFrame;

        std::vector<std::shared_ptr<DrawBuffer>> commands;
    };
} // namespace love