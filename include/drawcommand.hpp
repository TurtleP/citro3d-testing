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
        enum TEXENV_MODE
        {
            TEXENV_MODE_PRIMITIVE,
            TEXENV_MODE_TEXTURE,
            TEXENV_MODE_MAX_ENUM
        };

        DrawCommand(int vertexCount, vertex::PrimitiveType mode = vertex::PRIMITIVE_TRIANGLE_FAN) :
            mode(mode),
            positions {},
            count(vertexCount),
            size(vertexCount * vertex::VERTEX_SIZE),
            texture(nullptr),
            texEnvMode(TEXENV_MODE_MAX_ENUM)
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
            this->SetTexEnv(TEXENV_MODE_PRIMITIVE);

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

        void FillVertices(Color* colors)
        {
            this->SetTexEnv(TEXENV_MODE_PRIMITIVE);

            this->buffer  = std::make_shared<DrawBuffer>(this->size);
            auto vertices = this->buffer->GetBuffer();

            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = colors[index].array(),
                    .texcoord = { 0, 0 }
                };
                // clang-format on
            }
        }

        void FillVertices(const Color& color, const Vector2* textureCoords)
        {
            this->SetTexEnv(TEXENV_MODE_TEXTURE);

            this->buffer  = std::make_shared<DrawBuffer>(this->size);
            auto vertices = this->buffer->GetBuffer();

            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { textureCoords[index].x, textureCoords[index].y }
                };
                // clang-format on
            }

            this->buffer->FlushDataCache();
        }

        vertex::PrimitiveType mode;
        std::unique_ptr<Vector2[]> positions;

        size_t count;
        size_t size;

        C3D_Tex* texture;

        std::shared_ptr<DrawBuffer> buffer;

      private:
        TEXENV_MODE texEnvMode;
        void* indicies;

        void SetTexEnv(TEXENV_MODE mode)
        {
            if (mode == this->texEnvMode)
                return;

            C3D_TexEnv* env = C3D_GetTexEnv(0);
            C3D_TexEnvInit(env);

            GPU_TEVSRC source;
            GPU_COMBINEFUNC combineFunc;

            switch (mode)
            {
                case TEXENV_MODE_PRIMITIVE:
                {
                    source      = GPU_PRIMARY_COLOR;
                    combineFunc = GPU_REPLACE;
                    break;
                }
                case TEXENV_MODE_TEXTURE:
                {
                    source      = GPU_TEXTURE0;
                    combineFunc = GPU_MODULATE;
                    break;
                }
                default:
                    throw love::Exception("Not allowed.");
            }

            C3D_TexEnvSrc(env, C3D_Both, source, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
            C3D_TexEnvFunc(env, C3D_Both, combineFunc);

            this->texEnvMode = mode;
        }
    };
} // namespace love
