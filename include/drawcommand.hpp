#pragma once

#include <memory>

#include "buffer.hpp"
#include "color.hpp"
#include "exception.hpp"
#include "logfile.hpp"
#include "vertex.hpp"

namespace love
{
    struct DrawCommand
    {
      public:
        DrawCommand(int vertexCount) :
            positions {},
            vertices {},
            count(vertexCount),
            size(vertexCount * vertex::VERTEX_SIZE)
        {
            try
            {
                this->positions = std::make_unique<Vector2[]>(vertexCount);
                this->vertices  = std::make_unique<vertex::Vertex[]>(vertexCount);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }

            LOG("%zu", this->size);
        }

        const std::unique_ptr<Vector2[]>& Positions() const
        {
            return this->positions;
        }

        const std::unique_ptr<vertex::Vertex[]>& Vertices() const
        {
            return this->vertices;
        }

        void FillVertices(const Color& color)
        {
            this->buffer = std::make_shared<DrawBuffer>(this->size);

            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { 0, 0 }
                };
                // clang-format on
            }

            this->buffer->Upload(this->vertices.get(), this->size);
        }

      private:
        std::unique_ptr<Vector2[]> positions;
        std::unique_ptr<vertex::Vertex[]> vertices;

        size_t count;
        size_t size;

        std::shared_ptr<DrawBuffer> buffer;
    };
} // namespace love