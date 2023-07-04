#pragma once

#include "color.hpp"
#include "matrix.hpp"
#include "rasterizer.hpp"
#include "strongreference.hpp"
#include "vertex.hpp"

#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

namespace love
{
    class Graphics;

    class Font
    {
      public:
        enum AlignMode
        {
            ALIGN_LEFT,
            ALIGN_CENTER,
            ALIGN_RIGHT,
            ALIGN_JUSTIFY,
            ALIGN_MAX_ENUM
        };

        struct TextInfo
        {
            int width;
            int height;
        };

        struct ColoredString
        {
            std::string string;
            Color color;
        };

        struct IndexedColor
        {
            Color color;
            int index;
        };

        using Codepoints = std::vector<uint32_t>;

        struct ColoredCodepoints
        {
            Codepoints codepoints;
            std::vector<IndexedColor> colors;
        };

        struct Glyph
        {
            C3D_Tex* texture;
            int spacing;
            vertex::Vertex vertices[0x04];
            int sheet;
        };

        struct DrawCommand
        {
            C3D_Tex* texture;
            int sheet;
            int start;
            int count;
        };

        using ColoredStrings = std::vector<ColoredString>;

        Font(Rasterizer* rasterizer);

        void Print(Graphics& graphics, const ColoredStrings& text, const Matrix4& transform,
                   const Color& color);

        const float GetHeight() const
        {
            return this->height;
        }

        const float GetLineHeight() const
        {
            return 1.0f;
        }

      private:
        static constexpr uint32_t TAB_GLYPH      = 9;
        static constexpr uint32_t SPACE_GLYPH    = 32;
        static constexpr uint32_t NEWLINE_GLYPH  = 10;
        static constexpr uint32_t CARRIAGE_GLYPH = 13;

        static constexpr int SPACES_PER_TAB = 4;

        static void GetCodepointsFromString(const ColoredStrings& strings, ColoredCodepoints& out);

        static void GetCodepointsFromString(std::string_view text, Codepoints& out);

        GlyphData* GetRasterizerGlyphData(uint32_t glyph);

        const Glyph& AddGlyph(uint32_t glyph);

        std::vector<DrawCommand> GenerateVertices(const ColoredCodepoints& codepoints,
                                                  const Color& color,
                                                  std::vector<vertex::Vertex>& vertices,
                                                  float extraSpacing = 0.0f, Vector2 offset = {},
                                                  TextInfo* info = nullptr);

        const Glyph& FindGlyph(uint32_t glyph);

        void Render(Graphics& graphics, const Matrix4& transform,
                    const std::vector<DrawCommand>& commands,
                    const std::vector<vertex::Vertex>& vertices);

        std::vector<StrongReference<Rasterizer>> rasterizers;

        float height;
        bool useSpacesAsTab;
        std::unordered_map<uint32_t, Glyph> glyphs;
    };
} // namespace love