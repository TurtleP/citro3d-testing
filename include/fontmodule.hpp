#pragma once

#include <3ds.h>

namespace love
{
    class FontModule
    {
      public:
        static constexpr auto FONT_ARCHIVE = 0x0004009B00014002ULL;

        static CFNT_s* LoadSystemFont(CFG_Region region);

      private:
    };
} // namespace love