#pragma once

#include <3ds.h>

#include "glyphdata.hpp"
#include "object.hpp"

#include <utf8.h>

namespace love
{
    using Utf8Iterator = utf8::iterator<std::string_view::const_iterator>;

    class Rasterizer : public Object
    {
      public:
        struct FontMetrics
        {
            int advance;
            int ascent;
            int descent;
            int height;
        };

        static inline Type type = Type("Rasterizer", &Object::type);

        const float GetHeight() const
        {
            return this->metrics.height;
        }

        CFNT_s* GetFont() const
        {
            return this->face;
        }

        const float GetScale() const
        {
            return this->scale;
        }

        Rasterizer(CFG_Region region, int size);

        virtual ~Rasterizer();

        GlyphData* GetGlyphData(uint32_t glyph) const;

        GlyphData* GetGlyphData(std::string_view text) const;

        const bool HasGlyph(uint32_t glyph) const;

        const bool HasGlyphs(std::string_view text) const;

        const float GetKerning(uint32_t left, uint32_t right) const;

      private:
        int glyphCount;
        float scale;

        FontMetrics metrics;
        CFNT_s* face;
    };
} // namespace love