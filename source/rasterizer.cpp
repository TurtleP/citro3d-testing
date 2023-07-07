#include "rasterizer.hpp"
#include "fontmodule.hpp"

#include "exception.hpp"
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

    GlyphData::GlyphSheetInfo info {};
    info.index = out.sheetIndex;

    info.top    = out.texcoord.top;
    info.left   = out.texcoord.left;
    info.right  = out.texcoord.right;
    info.bottom = out.texcoord.bottom;

    return new GlyphData(glyph, metrics, info);
}

GlyphData* Rasterizer::GetGlyphData(std::string_view text) const
{
    uint32_t codepoint = 0;

    try
    {
        codepoint = utf8::peek_next(text.begin(), text.end());
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return this->GetGlyphData(codepoint);
}

const bool Rasterizer::HasGlyph(uint32_t glyph) const
{
    int index        = fontGlyphIndexFromCodePoint(this->face, glyph);
    const auto* info = fontGetInfo(this->face);

    return index != info->alterCharIndex;
}

const bool Rasterizer::HasGlyphs(std::string_view text) const
{
    if (text.size() == 0)
        return false;

    try
    {
        Utf8Iterator start(text.begin(), text.begin(), text.end());
        Utf8Iterator end(text.end(), text.begin(), text.end());

        while (start != end)
        {
            uint32_t codepoint = *start++;

            if (!this->HasGlyph(codepoint))
                return false;
        }
    }
    catch (utf8::exception& e)
    {
        throw love::Exception("UTF-8 decoding error: %s", e.what());
    }

    return true;
}

const float Rasterizer::GetKerning(uint32_t, uint32_t) const
{
    return 0.0f;
}