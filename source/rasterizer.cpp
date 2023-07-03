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

    return new GlyphData(glyph, metrics, PIXELFORMAT_RGBA8_UNORM);
}