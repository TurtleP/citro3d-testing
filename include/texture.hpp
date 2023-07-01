#pragma once

#include <citro3d.h>
#include <tex3ds.h>

#include "graphics.hpp"
#include "quad.hpp"

namespace love
{
    class Texture
    {
      public:
        Texture(const std::string& path);

        void Draw(Graphics& graphics, const Matrix4& matrix);

        C3D_Tex* GetTexture()
        {
            return this->texture;
        }

        ~Texture();

      private:
        int width;
        int height;

        C3D_Tex* texture;
        Quad* quad;

        bool valid;
    };
} // namespace love