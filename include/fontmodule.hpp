#pragma once

#include <3ds.h>

#include "data.hpp"
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

        static CFNT_s* LoadFromFile(const void* data, size_t size);

        Rasterizer* NewRasterizer(const void* data, size_t dataSize, float size = 16.0f);

        Rasterizer* NewRasterizer(float size, CFG_Region region = CFG_REGION_USA);

      private:
    };
} // namespace love