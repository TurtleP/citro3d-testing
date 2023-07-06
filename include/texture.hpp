#pragma once

#include <citro3d.h>
#include <tex3ds.h>

#include "graphics.hpp"
#include "object.hpp"
#include "quad.hpp"

namespace love
{
    class Texture : public Object
    {
      public:
        Texture(const std::string& path);

        void Draw(Graphics& graphics, const Matrix4& matrix);

        static inline Type type = Type("Texture", &Object::type);

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