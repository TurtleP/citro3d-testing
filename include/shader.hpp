#pragma once

#include <3ds.h>

#include <citro3d.h>

namespace love
{
    class Shader
    {
      public:
        enum StandardShader
        {
            STANDARD_DEFAULT, //< primitives
            STANDARD_MAX_ENUM
        };

        static inline Shader* defaults[StandardShader::STANDARD_MAX_ENUM] { nullptr };

        Shader();

        static inline Shader* current = nullptr;

        ~Shader();

        void Attach();

        void AttachDefault(StandardShader type);

        std::pair<int8_t, int8_t> GetUniformLocations()
        {
            return std::make_pair(this->uLoc_projMtx, this->uLoc_mdlView);
        }

      private:
        DVLB_s* binary;
        shaderProgram_s program;

        int8_t uLoc_projMtx;
        int8_t uLoc_mdlView;
    };
} // namespace love