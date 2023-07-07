#include <3ds.h>

#include <citro3d.h>

#include "color.hpp"
#include "drawcommand.hpp"
#include "font.hpp"
#include "fontmodule.hpp"
#include "framebuffer.hpp"
#include "graphics.hpp"
#include "logfile.hpp"
#include "matrix.hpp"
#include "rasterizer.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "strongreference.hpp"
#include "texture.hpp"
#include "vector.hpp"

#include <array>
#include <cmath>
#include <filesystem>
#include <limits.h>
#include <span>
#include <unistd.h>
#include <vector>

using namespace love;

int main(int argc, char** argv)
{
    love::Renderer::Instance();
    love::Graphics::Instance();

    cfguInit();

    if (Result rc = romfsInit(); R_FAILED(rc))
        return 0;

    for (size_t index = 0; index < love::Shader::STANDARD_MAX_ENUM; index++)
        Shader::defaults[index] = new love::Shader();

    auto clearColor = Color { 0, 0, 0, 1 };

    consoleInit(GFX_BOTTOM, NULL);

    char cwd_tmp[PATH_MAX] {};
    std::string cwd {};

    if (getcwd(cwd_tmp, PATH_MAX))
        cwd = cwd_tmp;

    auto& instance = love::FontModule::Instance();
    StrongReference<Rasterizer> rasterizer(instance.NewRasterizer(16.0f), Acquire::NORETAIN);

    auto* font              = new love::Font(rasterizer.Get());
    const auto textPosition = love::Matrix4(200, 120, 0, 1, 1, 0, 0, 0, 0);

    love::Font::ColoredString hello {};
    hello.string = "Hello";
    hello.color  = Color(1, 1, 1, 1);

    love::Font::ColoredString world {};
    world.string = " World!";
    world.color  = Color(1, 1, 1, 1);

    love::Font::ColoredStrings strings = { hello, world };

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
        love::Graphics::Instance().Print(strings, font, textPosition);

        love::Renderer::Instance().Present();
    }

    cfguExit();
    romfsExit();
}
