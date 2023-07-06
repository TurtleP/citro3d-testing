#pragma once

#include <3ds.h>

#include "rasterizer.hpp"

namespace love
{
    class FontModule
    {
      public:
        static FontModule& Instance()
        {
            static FontModule instance;
            return instance;
        }

        static constexpr auto FONT_ARCHIVE = 0x0004009B00014002ULL;

        static CFNT_s* LoadSystemFont(CFG_Region region);

        Rasterizer* NewRasterizer(float size, CFG_Region region = CFG_REGION_USA);

      private:
    };
} // namespace love