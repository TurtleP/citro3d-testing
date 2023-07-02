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
#include <limits.h>
#include <span>
#include <unistd.h>
#include <vector>

int main(int argc, char** argv)
{
    love::Renderer::Instance();
    love::Graphics::Instance();

    if (Result rc = romfsInit(); R_FAILED(rc))
        return 0;

    for (size_t index = 0; index < love::Shader::STANDARD_MAX_ENUM; index++)
        love::Shader::defaults[index] = new love::Shader();

    const auto clearColor  = Color { 0, 0, 0, 1 };
    const auto pacmanColor = Color { 1, 1, 0, 1 };

    consoleInit(GFX_BOTTOM, NULL);

    const auto pacmanMouth = M_TAU / 12;

    char cwd_tmp[PATH_MAX] {};
    std::string cwd {};

    if (getcwd(cwd_tmp, PATH_MAX))
        cwd = cwd_tmp;

    LOG("%s", cwd.c_str());

    std::string filepath       = cwd + "/dio.t3x";
    auto* texture              = new love::Texture(filepath);
    const auto texturePosition = love::Matrix4(0, 0, 0, 1, 1, 0, 0, 0, 0);

    const auto mode    = love::Graphics::DRAW_FILL;
    const auto arcMode = love::Graphics::ARC_PIE;

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

        love::Graphics::Instance().SetColor({ 1, 1, 1, 1 });
        texture->Draw(love::Graphics::Instance(), texturePosition);

        love::Graphics::Instance().SetColor({ 1, 0, 0, 1 });
        love::Graphics::Instance().Rectangle(love::Graphics::DRAW_LINE, 2, 2, 32, 32,
                                             love::Graphics::Instance().GetColor());

        love::Graphics::Instance().Rectangle(love::Graphics::DRAW_FILL, 336, 176, 32, 32, 4, 4, 8,
                                             love::Graphics::Instance().GetColor());

        love::Graphics::Instance().SetColor(pacmanColor);
        love::Graphics::Instance().Arc(mode, arcMode, 200, 120, 20, pacmanMouth,
                                       M_TAU - pacmanMouth, love::Graphics::Instance().GetColor());

        love::Graphics::Instance().Arc(love::Graphics::DRAW_LINE, arcMode, 200, 120, 60,
                                       pacmanMouth, M_TAU - pacmanMouth,
                                       love::Graphics::Instance().GetColor());

        love::Graphics::Instance().SetColor({ 0, 1, 0, 1 });
        love::Graphics::Instance().Circle(love::Graphics::DRAW_FILL, 8, 8, 8,
                                          love::Graphics::Instance().GetColor());

        love::Graphics::Instance().Circle(love::Graphics::DRAW_LINE, 320, 160, 8,
                                          love::Graphics::Instance().GetColor());

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
