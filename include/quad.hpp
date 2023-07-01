#pragma once

#include "vector.hpp"

namespace love
{
    class Quad
    {
      public:
        struct Viewport
        {
            double x, y;
            double w, h;
        };

        Quad(const Viewport& viewport, double sourceWidth, double sourceHeight);

        void Refresh(const Viewport& viewport, double sourceWidth, double sourceHeight);

        const Vector2* GetVertices() const
        {
            return this->vertices;
        }

        const Vector2* GetTextureCoords() const
        {
            return this->textureCoords;
        }

      private:
        Viewport viewport;
        Vector2 vertices[0x04];
        Vector2 textureCoords[0x04];

        double sourceWidth;
        double sourceHeight;
    };
} // namespace love