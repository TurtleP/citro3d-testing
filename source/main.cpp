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
    float textAngle = 0.0f;

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

    const auto TEXT_WIDTH  = font->GetWidth("Hello World! *lenny*");
    const auto TEXT_HEIGHT = font->GetHeight();

    const auto CENTER_POSITION = Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    const auto textMatrix      = love::Matrix4(CENTER_POSITION.x, CENTER_POSITION.y, 0, 1, 1,
                                               TEXT_WIDTH / 2, TEXT_HEIGHT / 2, 0, 0);

    while (aptMainLoop())
    {
        hidScanInput();

        if (hidKeysDown() & KEY_START)
            break;

        // printf("\x1b[1;1HCPU:     %6.2f%%\x1b[K", C3D_GetProcessingTime() * 6.0f);
        // printf("\x1b[2;1HGPU:     %6.2f%%\x1b[K", C3D_GetDrawingTime() * 6.0f);
        // printf("\x1b[3;1HCmdBuf:  %6.2f%%\x1b[K", C3D_GetCmdBufUsage() * 100.0f);

        float dt = love::Timer::Instance().Step();

        love::Graphics::Instance().Origin();

        /* render top screen */

        love::Renderer::Instance().BindFramebuffer();
        love::Renderer::Instance().Clear(clearColor);

        love::Graphics::Instance().SetColor({ 1, 1, 1, 1 });

        textAngle += dt;

        // love::Graphics::Instance().Push();

        love::Graphics::Instance().Translate(CENTER_POSITION.x, CENTER_POSITION.y);
        love::Graphics::Instance().Rotate(textAngle);
        love::Graphics::Instance().Translate(-CENTER_POSITION.x, -CENTER_POSITION.y);

        love::Graphics::Instance().Print(strings, font, textMatrix);

        love::Renderer::Instance().BindFramebuffer(1);
        love::Renderer::Instance().Clear(clearColor);

        love::Renderer::Instance().BindFramebuffer(2);
        love::Renderer::Instance().Clear(clearColor);

        love::Renderer::Instance().Present();

        love::Timer::Instance().Sleep(0.001);
    }

    cfguExit();
    romfsExit();
}
