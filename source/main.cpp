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
#include "timer.hpp"
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

    Shader::defaults[Shader::STANDARD_DEFAULT]->Attach();

    auto clearColor = Color { 0.55, 0.45, 0.86, 1.0f };

    consoleInit(GFX_BOTTOM, NULL);

    auto& instance = love::FontModule::Instance();
    StrongReference<Rasterizer> rasterizer(instance.NewRasterizer(16.0f), Acquire::NORETAIN);

    auto* font      = new love::Font(rasterizer.Get());
    float fontAngle = 0.0f;

    love::Font::ColoredString hello {};
    hello.string = "Hello";
    hello.color  = Color(1, 1, 1, 1);

    love::Font::ColoredString world {};
    world.string = " World!";
    world.color  = Color(1, 1, 0, 1);

    love::Font::ColoredString lenny {};
    lenny.string = " *lenny*";
    lenny.color  = Color(0.45f, 0.07f, 0.12f, 1.0f);

    love::Font::ColoredStrings strings = { hello, world, lenny };

    love::Timer::Instance().Step();

    const auto SCREEN_WIDTH  = 400;
    const auto SCREEN_HEIGHT = 240;

    const auto FONT_WIDTH  = font->GetWidth("Hello World! *lenny*");
    const auto FONT_HEIGHT = font->GetHeight();

    const auto textPosition =
        Vector2((SCREEN_WIDTH - FONT_WIDTH) * 0.5f, (SCREEN_HEIGHT - FONT_HEIGHT) * 0.5);

    while (aptMainLoop())
    {
        hidScanInput();

        if (hidKeysDown() & KEY_START)
            break;

        printf("\x1b[1;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime() * 6.0f);
        printf("\x1b[2;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime() * 6.0f);
        printf("\x1b[3;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage() * 100.0f);

        float dt = love::Timer::Instance().Step();

        love::Graphics::Instance().Origin();

        /* render top screen */

        love::Renderer::Instance().BindFramebuffer();
        love::Renderer::Instance().Clear(clearColor);

        love::Graphics::Instance().SetColor({ 1, 1, 1, 1 });

        fontAngle += dt;
        const auto textMatrix =
            love::Matrix4(textPosition.x + FONT_WIDTH / 2, textPosition.y + FONT_HEIGHT / 2,
                          fontAngle, 1, 1, FONT_WIDTH / 2, FONT_HEIGHT / 2, 0, 0);
        love::Graphics::Instance().Print(strings, font, textMatrix);

        love::Renderer::Instance().Present();

        love::Timer::Instance().Sleep(0.001);
    }

    cfguExit();
    romfsExit();
}
