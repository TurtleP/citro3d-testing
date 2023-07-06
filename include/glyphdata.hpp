#pragma once

#include <memory>
#include <string>

#include "data.hpp"
#include "pixelformat.hpp"

namespace love
{
    class GlyphData : public Data
    {
      public:
        static inline Type type = Type("GlyphData", &Data::type);

        struct GlyphMetrics
        {
            int width;
            int height;
            int advance;
            int bearingX;
            int bearingY;
        };

        struct GlyphSheetInfo
        {
            int index;
            float left;
            float top;
            float right;
            float bottom;
        };

        GlyphData(uint32_t glyph, GlyphMetrics metrics, GlyphSheetInfo info);

        GlyphData(const GlyphData& other);

        ~GlyphData()
        {}

        virtual GlyphData* Clone() const override;

        virtual void* GetData() const override;

        virtual size_t GetSize() const override;

        PixelFormat GetFormat() const;

        size_t GetPixelSize() const;

        int GetHeight() const;

        int GetWidth() const;

        uint32_t GetGlyph() const;

        std::string GetGlyphString() const;

        int GetAdvance() const;

        int GetBearingX() const;

        int GetBearingY() const;

        int GetMinX() const;

        int GetMinY() const;

        int GetMaxX() const;

        int GetMaxY() const;

        int GetSheetIndex() const
        {
            return this->sheetInfo.index;
        }

        float GetLeft() const
        {
            return this->sheetInfo.left;
        }

        float GetTop() const
        {
            return this->sheetInfo.top;
        }

        float GetRight() const
        {
            return this->sheetInfo.right;
        }

        float GetBottom() const
        {
            return this->sheetInfo.bottom;
        }

      protected:
        uint32_t glyph;

        GlyphMetrics metrics;
        GlyphSheetInfo sheetInfo;

        std::unique_ptr<uint8_t[]> data;
        PixelFormat format;
    };
} // namespace love
