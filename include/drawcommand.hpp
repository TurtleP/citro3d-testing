#pragma once

#include <memory>

#include "buffer.hpp"
#include "color.hpp"
#include "exception.hpp"
#include "vertex.hpp"

namespace love
{
    struct DrawCommand
    {
        static inline float depth        = 0.0f;
        static constexpr float MIN_DEPTH = 1.0f / 16384.0f;

      public:
        DrawCommand(int vertexCount) :
            positions {},
            count(vertexCount),
            size(vertexCount * vertex::VERTEX_SIZE)
        {
            try
            {
                this->positions = std::make_unique<Vector2[]>(vertexCount);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }
        }

        const std::unique_ptr<Vector2[]>& Positions() const
        {
            return this->positions;
        }

        void FillVertices(const Color& color)
        {
            this->buffer  = std::make_shared<DrawBuffer>(this->size);
            auto vertices = this->buffer->GetBuffer();

            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { 0, 0 }
                };
                // clang-format on
            }

            this->buffer->FlushDataCache();
        }

        std::unique_ptr<Vector2[]> positions;

        size_t count;
        size_t size;

        std::shared_ptr<DrawBuffer> buffer;
    };
} // namespace love
