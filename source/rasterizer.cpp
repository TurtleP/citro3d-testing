#include "rasterizer.hpp"
#include "fontmodule.hpp"

#include "logfile.hpp"

using namespace love;

Rasterizer::Rasterizer(CFG_Region region, int size) : glyphCount(-1), metrics {}
{
    this->face = FontModule::LoadSystemFont(region);

    FINF_s* info   = fontGetInfo(this->face);
    TGLP_s* sheets = info->tglp;

    /* font is 30px tall */
    this->scale = size / 30.0f;

    this->metrics.advance = sheets->maxCharWidth * this->scale;
    this->metrics.ascent  = info->ascent * this->scale;
    this->metrics.descent = (info->height - info->ascent) * this->scale;
    this->metrics.height  = info->height * this->scale;
}

Rasterizer::~Rasterizer()
{
    if (this->face)
        linearFree(this->face);
}

GlyphData* Rasterizer::GetGlyphData(uint32_t glyph) const
{
    GlyphData::GlyphMetrics metrics {};
    fontGlyphPos_s out;

    int index = fontGlyphIndexFromCodePoint(this->face, glyph);
    fontCalcGlyphPos(&out, this->face, index, GLYPH_POS_CALC_VTXCOORD, this->scale, this->scale);

    metrics.height   = this->metrics.height;
    metrics.width    = out.width;
    metrics.advance  = out.xAdvance;
    metrics.bearingX = out.xOffset;
    metrics.bearingY = this->metrics.ascent;

    auto* glyphData = new GlyphData(glyph, metrics, PIXELFORMAT_LA8_UNORM);

    TGLP_s* glyphInfo = this->face->finf.tglp;

    const uint8_t* pixels = &glyphInfo->sheetData[glyphInfo->sheetSize * index];
    uint8_t* destination  = (uint8_t*)glyphData->GetData();

    uint32_t pitch = ((glyphInfo->cellWidth * 8) + 3) & 3;

    for (int y = 0; y < (int)glyphInfo->cellHeight; y++)
    {
        for (int x = 0; x < (int)glyphInfo->cellWidth; x++)
        {
            uint8_t v = ((pixels[x / 8]) & (1 << (7 - (x % 8)))) ? 255 : 0;
            destination[2 * (y * glyphInfo->cellWidth + x) + 0] = 255;
            destination[2 * (y * glyphInfo->cellWidth + x) + 1] = v;
        }
        pixels += pitch;
    }

    return glyphData;
}