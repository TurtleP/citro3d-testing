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
#include <cmath>
#include <span>

static love::Matrix4 defaultMatrix;

enum DrawMode
{
    DRAW_LINE,
    DRAW_FILL
};

enum ArcMode
{
    ARC_OPEN,
    ARC_CLOSED,
    ARC_PIE
};

static int first = 0;

static void polyFill(std::span<love::Vector2> points, const Color& color,
                     bool skipLastVertex = true)
{
    const int count = points.size() - (skipLastVertex ? 1 : 0);

    love::DrawCommand command(count);
    defaultMatrix.TransformXY(command.Positions().get(), points.data(), count);
    command.FillVertices(color);

    love::Renderer::Instance().Render(command);
}

static void drawRectangle(float x, float y, float width, float height, const Color& color)
{
    std::array<love::Vector2, 0x05> points = { love::Vector2(x, y), love::Vector2(x, y + height),
                                               love::Vector2(x + width, y + height),
                                               love::Vector2(x + width, y), love::Vector2(x, y) };

    polyFill(points, color);
}

static void drawRoundedRectangle(float x, float y, float width, float height, float rx, float ry,
                                 int points, const Color& color)
{
    if (rx <= 0 || ry <= 0)
    {
        drawRectangle(x, y, width, height, color);
        return;
    }

    if (width >= 0.02f)
        rx = std::min(rx, width / 2.0f - 0.01f);

    if (height >= 0.02f)
        ry = std::min(ry, height / 2.0f - 0.01f);

    points = std::max(points / 4, 1);

    const float halfPi = static_cast<float>(LOVE_M_PI / 2);
    float angleShift   = halfPi / ((float)points + 1.0f);

    int pointCount = (points + 2) * 4;

    love::Vector2 coords[pointCount + 1] {};
    float phi = 0.0f;

    for (int index = 0; index <= points + 2; ++index, phi += angleShift)
    {
        coords[index].x = x + rx * (1 - cosf(phi));
        coords[index].y = y + ry * (1 - sinf(phi));
    }

    phi = halfPi;

    for (int index = points + 2; index <= 2 * (points + 2); ++index, phi += angleShift)
    {
        coords[index].x = x + width - rx * (1 + cosf(phi));
        coords[index].y = y + ry * (1 - sinf(phi));
    }

    phi = 2 * halfPi;

    for (int index = 2 * (points + 2); index <= 3 * (points + 2); ++index, phi += angleShift)
    {
        coords[index].x = x + width - rx * (1 + cosf(phi));
        coords[index].y = y + height - ry * (1 + sinf(phi));
    }

    phi = 3 * halfPi;

    for (int index = 3 * (points + 2); index <= 4 * (points + 2); ++index, phi += angleShift)
    {
        coords[index].x = x + rx * (1 - cosf(phi));
        coords[index].y = y + height - ry * (1 + sinf(phi));
    }

    coords[pointCount] = coords[0];
    polyFill(std::span(coords, pointCount + 1), color);
}

static int calculateEllipsePoints(float rx, float ry)
{
    auto pixelScale = 1.0f;
    auto points     = sqrtf(((rx + ry) / 2.0f) * 20.0f * pixelScale);

    return std::max(points, 8.0f);
}

void drawEllipse(DrawMode mode, float x, float y, float a, float b, int points, const Color& color)
{
    float twoPi = (float)(LOVE_M_TAU);

    if (points <= 0)
        points = 1;

    const float angleShift = (twoPi / points);
    float phi              = 0.0f;

    int extraPoints = 1 + (mode == DRAW_FILL ? 1 : 0);
    love::Vector2 polygonCoords[points + extraPoints] {};
    love::Vector2* coords = polygonCoords;

    if (mode == DRAW_FILL)
    {
        coords[0].x = x;
        coords[0].y = y;
        coords++;
    }

    for (int index = 0; index < points; ++index, phi += angleShift)
    {
        coords[index].x = x + a * cosf(phi);
        coords[index].y = y + b * sinf(phi);
    }

    coords[points] = coords[0];

    polyFill(std::span(polygonCoords, points + extraPoints), color, false);
}

static void drawEllipse(DrawMode mode, float x, float y, float a, float b, const Color& color)
{
    drawEllipse(mode, x, y, a, b, calculateEllipsePoints(a, b), color);
}

static void drawCircle(DrawMode mode, float x, float y, float radius, int points,
                       const Color& color)
{
    drawEllipse(mode, x, y, radius, radius, points, color);
}

static void Circle(DrawMode mode, float x, float y, float radius, const Color& color)
{
    drawEllipse(mode, x, y, radius, radius, color);
}

static void drawArc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                    float angle2, int points, const Color& color)
{
    if (points <= 0 || angle1 == angle2)
        return;

    if (fabs(angle1 - angle2) >= LOVE_M_TAU)
    {
        drawCircle(mode, x, y, radius, points, color);
        return;
    }

    const float angleShift = (angle2 - angle1) / points;

    if (angleShift == 0.0f)
        return;

    const auto sharpAngle = fabsf(angle1 - angle2) < LOVE_TORAD(4);
    if (mode == DRAW_LINE && arcMode == ARC_CLOSED && sharpAngle)
        arcMode = ARC_OPEN;

    if (mode == DRAW_FILL && arcMode == ARC_OPEN)
        arcMode = ARC_CLOSED;

    float phi = angle1;

    love::Vector2* coords = nullptr;
    int numCoords         = 0;

    // clang-format off
    const auto createPoints = [&](love::Vector2* coordinates)
    {
        for (int index = 0; index <= points; ++index, phi += angleShift)
        {
            coordinates[index].x = x + radius * cosf(phi);
            coordinates[index].y = y + radius * sinf(phi);
        }
    };
    // clang-format on

    if (arcMode == ARC_PIE)
    {
        numCoords = points + 3;
        coords    = new love::Vector2[numCoords];

        coords[0] = coords[numCoords - 1] = love::Vector2(x, y);
        createPoints(coords + 1);
    }
    else if (arcMode == ARC_OPEN)
    {
        numCoords = points + 1;
        coords    = new love::Vector2[numCoords];

        createPoints(coords);
    }
    else
    {
        numCoords = points + 2;
        coords    = new love::Vector2[numCoords];

        createPoints(coords);
        coords[numCoords - 1] = coords[0];
    }

    polyFill(std::span(coords, numCoords), color);
    delete[] coords;
}

static void drawArc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                    float angle2, const Color& color)
{
    float points = calculateEllipsePoints(radius, radius);
    float angle  = fabsf(angle1 - angle2);

    if (angle < (float)LOVE_M_TAU)
        points *= angle / (float)LOVE_M_TAU;

    drawArc(mode, arcMode, x, y, radius, angle1, angle2, (int)(points + 0.5f), color);
}

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

    const Color pacmanColor { 1.0f, 0, 0, 1 };

    while (aptMainLoop())
    {
        hidScanInput();

        if (hidKeysDown() & KEY_START)
            break;

        defaultMatrix.SetIdentity();

        /* render top screen */

        love::Renderer::Instance().BindFramebuffer();
        love::Renderer::Instance().Clear(clearColor);

        drawArc(DRAW_FILL, ARC_PIE, 200, 120, 10, M_PI / 6, (M_PI * 2) - M_PI / 6, pacmanColor);
        drawArc(DRAW_FILL, ARC_PIE, 100, 60, 20, M_PI / 6, (M_PI * 2) - M_PI / 6, pacmanColor);
        // drawCircle(DRAW_FILL, 200, 120, 30, 16, pacmanColor);
        // drawCircle(DRAW_FILL, 100, 60, 15, 16, pacmanColor);
        // drawCircle(DRAW_FILL, 300, 60, 15, 16, pacmanColor);

        /* render bottom screen */
        // love::Renderer::Instance().BindFramebuffer(2);
        // love::Renderer::Instance().Clear(clearColor);

        love::Renderer::Instance().Present();
    }

    for (auto* shader : love::Shader::defaults)
        delete shader;
}