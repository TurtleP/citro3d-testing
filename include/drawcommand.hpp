#pragma once

#include <memory>

#include "buffer.hpp"
#include "color.hpp"
#include "exception.hpp"
#include "texture.hpp"
#include "vertex.hpp"

namespace love
{
    struct DrawCommand
    {
        static inline float depth        = 0.0f;
        static constexpr float MIN_DEPTH = 1.0f / 16384.0f;

      public:
        DrawCommand(int vertexCount, vertex::TriangleIndexMode mode = vertex::TRIANGLE_FAN) :
            positions {},
            count(vertexCount),
            size(vertexCount * vertex::VERTEX_SIZE),
            texture(nullptr)
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

            if (this->texture)
                C3D_TexBind(0, this->texture->GetTexture());

            this->buffer->FlushDataCache();
        }

        void FillVertices(const Color& color, const Vector2* textureCoords)
        {
            this->buffer  = std::make_shared<DrawBuffer>(this->count);
            auto vertices = this->buffer->GetBuffer();

            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { vertex::normto16t(textureCoords[index].x), vertex::normto16t(textureCoords[index].y) }
                };
                // clang-format on
            }
        }

        vertex::TriangleIndexMode mode;
        std::unique_ptr<Vector2[]> positions;

        size_t count;
        size_t size;

        Texture* texture;

        std::shared_ptr<DrawBuffer> buffer;
    };
} // namespace love
