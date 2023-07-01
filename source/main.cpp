#include <3ds.h>

#include <citro3d.h>

#include "color.hpp"
#include "drawcommand.hpp"
#include "framebuffer.hpp"
#include "graphics.hpp"
#include "logfile.hpp"
#include "matrix.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "vector.hpp"

#include <array>
#include <cmath>
#include <span>
#include <vector>

int main(int argc, char** argv)
{
    love::Renderer::Instance();
    love::Graphics::Instance();

    if (Result rc = romfsInit(); R_FAILED(rc))
        return 0;

    for (size_t index = 0; index < love::Shader::STANDARD_MAX_ENUM; index++)
        love::Shader::defaults[index] = new love::Shader();

    const auto clearColor  = Color { 0, 1, 0, 1 };
    const auto pacmanColor = Color { 1, 1, 0, 1 };

    consoleInit(GFX_BOTTOM, NULL);

    const auto pacmanMouth = M_TAU / 12;

    auto* texture              = new love::Texture("dio.t3x");
    const auto texturePosition = love::Matrix4(0, 0, 0, 1, 1, 0, 0, 0, 0);

    while (aptMainLoop())
    {
        hidScanInput();

        if (hidKeysDown() & KEY_START)
            break;

        printf("\x1b[1;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime() * 6.0f);
        printf("\x1b[2;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime() * 6.0f);
        printf("\x1b[3;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage() * 100.0f);

        love::Graphics::Instance().Origin();

        /* render top screen */

        love::Renderer::Instance().BindFramebuffer();
        love::Renderer::Instance().Clear(clearColor);

        texture->Draw(love::Graphics::Instance(), texturePosition);

        // love::Graphics::Instance().Arc(love::Graphics::DRAW_FILL, love::Graphics::ARC_PIE, 200,
        // 120,
        //                                60, pacmanMouth, M_TAU - pacmanMouth, pacmanColor);

        // drawArc(DRAW_FILL, ARC_PIE, 100, 60, 20, M_PI / 6, (M_PI * 2) - M_PI / 6, pacmanColor);
        // drawCircle(DRAW_FILL, 200, 120, 30, 16, pacmanColor);
        // drawCircle(DRAW_FILL, 100, 60, 15, 16, pacmanColor);
        // drawCircle(DRAW_FILL, 300, 60, 15, 16, pacmanColor);

        /* render bottom screen */
        // love::Renderer::Instance().BindFramebuffer(2);
        // love::Renderer::Instance().Clear(clearColor);

        love::Renderer::Instance().Present();
    }

    delete texture;

    romfsExit();
}
