#include <3ds.h>

#include <citro3d.h>

#include "color.hpp"
#include "drawcommand.hpp"
#include "framebuffer.hpp"
#include "logfile.hpp"
#include "matrix.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "vector.hpp"

#include <array>

int main(int argc, char** argv)
{
    love::Renderer::Instance();

    if (Result rc = romfsInit(); R_FAILED(rc))
        return 0;

    for (size_t index = 0; index < love::Shader::STANDARD_MAX_ENUM; index++)
        love::Shader::defaults[index] = new love::Shader();

    const auto clearColor = Color { 0, 1, 0, 1 };

    C3D_AttrInfo* attributes = C3D_GetAttrInfo();
    AttrInfo_Init(attributes);

    AttrInfo_AddLoader(attributes, 0, GPU_FLOAT, 3); // position
    AttrInfo_AddLoader(attributes, 1, GPU_FLOAT, 4); // color
    AttrInfo_AddLoader(attributes, 2, GPU_FLOAT, 2); // texcoord

    const Color triangleColor { 1.0f, 0, 0, 1 };
    std::array<love::Vector2, 0x03> triangleVertices {
        love::Vector2 {200.0f, 200.0f},
        love::Vector2 {100.0f,  40.0f},
        love::Vector2 {300.0f,  40.0f},
    };

    love::Matrix4 defaultMatrix;

    while (aptMainLoop())
    {
        hidScanInput();

        if (hidKeysDown() & KEY_START)
            break;

        /* render top screen */
        love::Shader::defaults[love::Shader::STANDARD_DEFAULT]->Attach();

        love::Renderer::Instance().BindFramebuffer();
        love::Renderer::Instance().Clear(clearColor);

        love::DrawCommand command(3);

        defaultMatrix.TransformXY(command.Positions().get(), triangleVertices.data(), 3);
        command.FillVertices(triangleColor);

        C3D_DrawArrays(GPU_TRIANGLES, 0, 3);

        /* render bottom screen */
        love::Renderer::Instance().BindFramebuffer(2);
        love::Renderer::Instance().Clear(clearColor);

        love::Renderer::Instance().Present();
    }

    for (auto* shader : love::Shader::defaults)
        delete shader;
}