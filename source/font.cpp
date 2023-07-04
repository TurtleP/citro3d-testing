#include "font.hpp"

#include "drawcommand.hpp"
#include "graphics.hpp"
#include "renderer.hpp"

#include <utf8.h>

using namespace love;

Font::Font(Rasterizer* rasterizer) :
    rasterizers({ rasterizer }),
    useSpacesAsTab(false),
    scale(rasterizer->GetScale())
{
    this->height = rasterizer->GetHeight();

    if (!rasterizer->HasGlyph(Font::TAB_GLYPH))
        this->useSpacesAsTab = true;

    this->LoadVolatile();
}

bool Font::LoadVolatile()
{
    this->glyphs.clear();
    this->textures.clear();
    this->CreateTexture();

    return true;
}

void Font::CreateTexture()
{}

void Font::GetCodepointsFromString(const ColoredStrings& strings, ColoredCodepoints& out)
{
    if (strings.empty())
        return;

    out.codepoints.reserve(strings[0].string.size());

    for (const auto& string : strings)
    {
        if (string.string.size() == 0)
            continue;

        IndexedColor color = { string.color, (int)out.codepoints.size() };
        out.colors.push_back(color);

        GetCodepointsFromString(string.string, out.codepoints);
    }

    if (out.colors.size() == 1)
    {
        IndexedColor color = out.colors[0];
        if (color.index == 0 && color.color == Color::WHITE)
            out.colors.pop_back();
    }
}

void Font::GetCodepointsFromString(std::string_view text, Codepoints& out)
{
    out.reserve(text.size());

    try
    {
        Utf8Iterator start(text.begin(), text.begin(), text.end());
        Utf8Iterator end(text.end(), text.begin(), text.end());

        while (start != end)
        {
            auto glyph = *start++;
            out.push_back(glyph);
        }
    }
    catch (utf8::exception& exception)
    {
        throw love::Exception("UTF-8 decoding error: %s", exception.what());
    }
}

GlyphData* Font::GetRasterizerGlyphData(uint32_t glyph)
{
    if (glyph == Font::TAB_GLYPH && this->useSpacesAsTab)
    {
        auto* space = this->rasterizers[0]->GetGlyphData(Font::SPACE_GLYPH);
        auto format = space->GetFormat();

        GlyphData::GlyphMetrics metrics {};
        metrics.advance  = space->GetAdvance() * Font::SPACES_PER_TAB;
        metrics.bearingX = space->GetBearingX();
        metrics.bearingY = space->GetBearingY();

        GlyphData::GlyphSheetInfo info {};
        info.index  = space->GetSheet();
        info.top    = space->GetTop();
        info.left   = space->GetLeft();
        info.right  = space->GetRight();
        info.bottom = space->GetBottom();

        space->Release();
        return new GlyphData(glyph, metrics, info);
    }
    for (const auto& rasterizer : this->rasterizers)
    {
        if (rasterizer->HasGlyph(glyph))
            return rasterizer->GetGlyphData(glyph);
    }

    return this->rasterizers[0]->GetGlyphData(glyph);
}

const Font::Glyph& Font::AddGlyph(uint32_t glyph)
{
    StrongReference<GlyphData> data(this->GetRasterizerGlyphData(glyph));

    auto width  = data->GetWidth();
    auto height = data->GetHeight();

    Glyph _glyph {};

    if (width > 0 && height > 0)
    {
        _glyph.sheet   = data->GetSheet();
        _glyph.spacing = std::floor(data->GetAdvance() / 1.5f);
        std::fill_n(_glyph.vertices, 4, vertex::Vertex {});

        _glyph.texture = (C3D_Tex*)malloc(sizeof(C3D_Tex));

        if (!_glyph.texture)
            throw love::Exception("Failed to allocate texture");

        TGLP_s* info = fontGetGlyphInfo(this->rasterizers[0]->GetFont());

        int sheet = data->GetSheet();

        _glyph.texture->data   = fontGetGlyphSheetTex(this->rasterizers[0]->GetFont(), sheet);
        _glyph.texture->fmt    = (GPU_TEXCOLOR)info->sheetFmt;
        _glyph.texture->size   = info->sheetSize;
        _glyph.texture->width  = info->sheetWidth;
        _glyph.texture->height = info->sheetHeight;
        _glyph.texture->param =
            GPU_TEXTURE_MAG_FILTER(GPU_LINEAR) | GPU_TEXTURE_MIN_FILTER(GPU_LINEAR) |
            GPU_TEXTURE_WRAP_S(GPU_CLAMP_TO_BORDER) | GPU_TEXTURE_WRAP_T(GPU_CLAMP_TO_BORDER);
        _glyph.texture->border   = 0;
        _glyph.texture->lodParam = 0;

        const auto offset = 1.0f;
        const auto color  = Color(Color::WHITE).array();

        const auto left   = data->GetLeft();
        const auto top    = data->GetTop();
        const auto right  = data->GetRight();
        const auto bottom = data->GetBottom();

        // clang-format off
        const vertex::Vertex vertices[0x04] = {
             /* x                y                  z                u      v      */
            {{ -offset,          -offset,           0.0f }, color, { left,  top    }},
            {{ -offset,          (height + offset), 0.0f }, color, { left,  bottom }},
            {{ (width + offset), (height + offset), 0.0f }, color, { right, bottom }},
            {{ (width + offset), -offset,           0.0f }, color, { right, top    }}
        };
        // clang-format on

        for (size_t index = 0; index < 0x04; index++)
        {
            _glyph.vertices[index] = vertices[index];

            _glyph.vertices[index].position[0] += data->GetBearingX();
            _glyph.vertices[index].position[1] += data->GetBearingY();
        }
    }

    this->glyphs[glyph] = _glyph;
    return this->glyphs[glyph];
}

const Font::Glyph& Font::FindGlyph(uint32_t glyph)
{
    const auto iterator = this->glyphs.find(glyph);

    if (iterator != this->glyphs.end())
        return iterator->second;

    return this->AddGlyph(glyph);
}

std::vector<Font::DrawCommand> Font::GenerateVertices(const ColoredCodepoints& text,
                                                      const Color& constantColor,
                                                      std::vector<vertex::Vertex>& vertices,
                                                      float extraSpacing, Vector2 offset,
                                                      TextInfo* info)
{
    float dx = offset.x;
    float dy = offset.y;

    float heightOffset = 0.0f;

    int maxWidth = 0;
    std::vector<DrawCommand> commands;

    /* reserve max possible vertex size */
    size_t startSize = vertices.size();
    vertices.reserve(startSize + text.codepoints.size() * 4);

    uint32_t previousGlyph = 0;
    Color currentColor     = constantColor;

    int colorIndex        = 0;
    const auto colorCount = (int)text.colors.size();

    for (int index = 0; index < (int)text.codepoints.size(); index++)
    {
        /* current glyph to work on */
        const auto glyph = text.codepoints[index];

        /* gamma correct the glyph's color */
        if (colorIndex < colorCount && text.colors[colorIndex].index == index)
        {
            auto glyphColor = text.colors[colorIndex].color;

            glyphColor.r = std::min(std::max(glyphColor.r, 0.0f), 1.0f);
            glyphColor.g = std::min(std::max(glyphColor.g, 0.0f), 1.0f);
            glyphColor.b = std::min(std::max(glyphColor.b, 0.0f), 1.0f);
            glyphColor.a = std::min(std::max(glyphColor.a, 0.0f), 1.0f);

            currentColor = glyphColor;
            colorIndex++;
        }

        if (glyph == Font::NEWLINE_GLYPH)
        {
            if (dx > maxWidth)
                maxWidth = dx;

            /* wrap the new line, make sure it doesn't get printed */
            dy += std::floor(this->GetHeight() * this->GetLineHeight() + 0.5f);
            dx            = offset.x;
            previousGlyph = 0;

            continue;
        }

        if (glyph == Font::CARRIAGE_GLYPH)
            continue;

        const auto& glyphData = this->FindGlyph(glyph);
        dx += 0.0f; /* this->GetKerning - no kerning on 3DS fonts */

        if (glyphData.texture != nullptr)
        {
            for (int j = 0; j < 4; j++)
            {
                vertices.push_back(glyphData.vertices[j]);

                vertices.back().position[0] += dx;
                vertices.back().position[1] += dy + heightOffset;
                vertices.back().color = currentColor.array();
            }

            if (commands.empty() || commands.back().texture != glyphData.texture)
            {
                DrawCommand command {};
                command.start   = (int)vertices.size() - 4;
                command.count   = 0;
                command.texture = glyphData.texture;
                command.sheet   = glyphData.sheet;

                commands.push_back(command);
            }

            commands.back().count += 4;
        }

        /* advance the x position */
        dx += glyphData.spacing;

        if (glyph == Font::SPACE_GLYPH && extraSpacing != 0.0f)
            dx = std::floor(dx + extraSpacing);

        previousGlyph = glyph;
    }

    /* texture binds are expensive, so we should sort by that first */
    const auto drawsort = [](const DrawCommand& a, const DrawCommand& b) -> bool {
        if (a.sheet < b.sheet)
            return a.sheet < b.sheet;
        else
            return a.start < b.start;
    };

    std::sort(commands.begin(), commands.end(), drawsort);

    if (dx > maxWidth)
        maxWidth = (int)dx;

    if (info != nullptr)
    {
        info->width = maxWidth - offset.x;

        const auto height = this->GetHeight() * this->GetLineHeight() + 0.5f;
        info->height      = (int)dy + (dx > 0.0f ? std::floor(height) : 0) - offset.y;
    }

    return commands;
}

void Font::Print(Graphics& graphics, const ColoredStrings& text, const Matrix4& transform,
                 const Color& color)
{
    ColoredCodepoints codepoints {};
    Font::GetCodepointsFromString(text, codepoints);

    std::vector<vertex::Vertex> vertices {};
    auto commands = this->GenerateVertices(codepoints, color, vertices);

    this->Render(graphics, transform, commands, vertices);
}

void Font::Render(Graphics& graphics, const Matrix4& transform,
                  const std::vector<DrawCommand>& commands,
                  const std::vector<vertex::Vertex>& vertices)
{
    if (vertices.empty() || commands.empty())
        return;

    Matrix4 matrix(graphics.GetTransform(), transform);

    for (const auto& command : commands)
    {
        love::DrawCommand drawCommand(command.count);
        drawCommand.texture = command.texture;

        matrix.TransformXYVert(drawCommand.Positions().get(), &vertices[command.start],
                               command.count);
        drawCommand.FillVertices(vertices.data());

        Renderer::Instance().Render(drawCommand);
    }
}