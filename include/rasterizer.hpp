#pragma once

#include <3ds.h>

#include "glyphdata.hpp"

namespace love
{
    class Rasterizer
    {
      public:
        struct FontMetrics
        {
            int advance;
            int ascent;
            int descent;
            int height;
        };

        Rasterizer(CFG_Region region, int size);

        ~Rasterizer();

        GlyphData* GetGlyphData(uint32_t glyph) const;

      private:
        int glyphCount;
        float scale;

        FontMetrics metrics;
        CFNT_s* face;
    };
} // namespace love