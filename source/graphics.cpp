#include "graphics.hpp"

#include "drawcommand.hpp"
#include "font.hpp"

#include "polyline/types/beveljoin.hpp"
#include "polyline/types/miterjoin.hpp"
#include "polyline/types/nonejoin.hpp"
#include "renderer.hpp"

using namespace love;

Graphics::Graphics()
{
    this->transformStack.reserve(0x10);
    this->transformStack.push_back(love::Matrix4 {});

    this->state.reserve(0x0A);
    this->state.push_back(DisplayState {});

    this->pixelScaleStack.reserve(0x0A);
    this->pixelScaleStack.push_back(1.0);
}

void Graphics::Polyline(const std::span<Vector2> points)
{

    float halfWidth     = this->GetLineWidth() * 0.5f;
    LineJoin lineJoin   = this->GetLineJoin();
    LineStyle lineStyle = this->GetLineStyle();

    float pixelSize   = 1.0f / std::max((float)this->pixelScaleStack.back(), 0.000001f);
    bool shouldSmooth = lineStyle == LINE_SMOOTH;

    if (lineJoin == LINE_JOIN_NONE)
    {
        NoneJoinPolyline line;
        line.render(points.data(), points.size(), halfWidth, pixelSize, shouldSmooth);

        line.draw(this);
    }
    else if (lineJoin == LINE_JOIN_BEVEL)
    {
        BevelJoinPolyline line;
        line.render(points.data(), points.size(), halfWidth, pixelSize, shouldSmooth);

        line.draw(this);
    }
    else if (lineJoin == LINE_JOIN_MITER)
    {
        MiterJoinPolyline line;
        line.render(points.data(), points.size(), halfWidth, pixelSize, shouldSmooth);

        line.draw(this);
    }
}

void Graphics::Polyfill(DrawMode mode, std::span<love::Vector2> points, const Color& color,
                        bool skipLastVertex)
{
    if (mode == DRAW_LINE)
    {
        this->Polyline(points);
    }
    else
    {
        const auto transform = transformStack.back();
        bool is2D            = transform.IsAffine2DTransform();

        const int count = points.size() - (skipLastVertex ? 1 : 0);
        DrawCommand command(count);

        if (is2D)
            transform.TransformXY(command.Positions().get(), points.data(), command.count);

        command.FillVertices(color);

        love::Renderer::Instance().Render(command);
    }
}

void Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height,
                         const Color& color)
{
    std::array<love::Vector2, 0x05> points = { love::Vector2(x, y), love::Vector2(x, y + height),
                                               love::Vector2(x + width, y + height),
                                               love::Vector2(x + width, y), love::Vector2(x, y) };

    this->Polyfill(mode, points, color);
}

void Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                         float ry, int points, const Color& color)
{
    if (rx <= 0 || ry <= 0)
    {
        this->Rectangle(mode, x, y, width, height, color);
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
    this->Polyfill(mode, std::span(coords, pointCount + 1), color);
}

int Graphics::CalculateEllipsePoints(float rx, float ry)
{
    auto pixelScale = 1.0f;
    auto points     = sqrtf(((rx + ry) / 2.0f) * 20.0f * pixelScale);

    return std::max(points, 8.0f);
}

void Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b, int points,
                       const Color& color)
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

    this->Polyfill(mode, std::span(polygonCoords, points + extraPoints), color, false);
}

void Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b, const Color& color)
{
    this->Ellipse(mode, x, y, a, b, this->CalculateEllipsePoints(a, b), color);
}

void Graphics::Circle(DrawMode mode, float x, float y, float radius, int points, const Color& color)
{
    this->Ellipse(mode, x, y, radius, radius, points, color);
}

void Graphics::Circle(DrawMode mode, float x, float y, float radius, const Color& color)
{
    this->Ellipse(mode, x, y, radius, radius, color);
}

void Graphics::Arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                   float angle2, int points, const Color& color)
{
    if (points <= 0 || angle1 == angle2)
        return;

    if (fabs(angle1 - angle2) >= LOVE_M_TAU)
    {
        this->Circle(mode, x, y, radius, points, color);
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

    this->Polyfill(mode, std::span(coords, numCoords), color);
    delete[] coords;
}

void Graphics::Arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                   float angle2, const Color& color)
{
    float points = this->CalculateEllipsePoints(radius, radius);
    float angle  = fabsf(angle1 - angle2);

    if (angle < (float)LOVE_M_TAU)
        points *= angle / (float)LOVE_M_TAU;

    this->Arc(mode, arcMode, x, y, radius, angle1, angle2, (int)(points + 0.5f), color);
}

void Graphics::Print(const Font::ColoredStrings& strings, Font* font, const Matrix4& matrix)
{
    font->Print(*this, strings, matrix, this->state.back().foreground);
}

void Graphics::Print(const Font::ColoredStrings& strings, const Matrix4& matrix)
{
    this->Print(strings, this->state.back().font.Get(), matrix);
}

void Graphics::Printf(const Font::ColoredStrings& strings, float wrap, Font::AlignMode align,
                      const Matrix4& matrix)
{
    if (this->state.back().font.Get() != nullptr)
        this->Printf(strings, this->state.back().font.Get(), wrap, align, matrix);
}

void Graphics::Printf(const Font::ColoredStrings& strings, Font* font, float wrap,
                      Font::AlignMode align, const Matrix4& matrix)
{
    font->Printf(*this, strings, wrap, align, matrix, this->state.back().foreground);
}