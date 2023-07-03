#include "fontmodule.hpp"
#include "exception.hpp"

#include <memory>

using namespace love;

static constexpr const char* const fontPaths[] = {
    "font:/cbf_std.bcfnt.lz",
    "font:/cbf_zh-Hans-CN.bcfnt.lz",
    "font:/cbf_ko-Hang-KR.bcfnt.lz",
    "font:/cbf_zh-Hant-TW.bcfnt.lz",
};

static CFNT_s* loadFromArchive(uint64_t title, const char* path)
{
    std::unique_ptr<uint8_t[]> fontData;
    uint32_t size = 0;

    Result result = 0;
    CFNT_s* font  = nullptr;

    result = romfsMountFromTitle(title, MEDIATYPE_NAND, "font");

    if (R_FAILED(result))
        return nullptr;

    std::FILE* file = std::fopen(path, "rb");

    if (!file)
    {
        std::fclose(file);
        return nullptr;
    }

    std::fseek(file, 0, SEEK_END);
    size = (uint32_t)std::ftell(file);
    std::rewind(file);

    try
    {
        fontData = std::make_unique<uint8_t[]>(size);
    }
    catch (std::bad_alloc&)
    {
        throw love::Exception("Not enough memory.");
    }

    romfsUnmount("font");

    uint32_t fontSize = *(uint32_t*)fontData.get() >> 0x08;
    font              = (CFNT_s*)linearAlloc(fontSize);

    if (!decompress_LZ11(font, fontSize, nullptr, fontData.get() + 4, size - 4))
    {
        linearFree(font);
        throw love::Exception("Failed to decompress '%s'", path);
    }

    fontFixPointers(font);

    return font;
}

static size_t getFontIndex(CFG_Region region)
{
    switch (region)
    {
        default:
        case CFG_REGION_JPN ... CFG_REGION_AUS:
            return 0;
        case CFG_REGION_CHN:
            return 1;
        case CFG_REGION_KOR:
            return 2;
        case CFG_REGION_TWN:
            return 3;
    }
}

CFNT_s* FontModule::LoadSystemFont(CFG_Region region)
{
    size_t index         = getFontIndex(region);
    uint8_t systemRegion = 0;

    Result result = CFGU_SecureInfoGetRegion(&systemRegion);

    if (R_FAILED(result) || index == getFontIndex((CFG_Region)systemRegion))
        return fontGetSystemFont();

    return loadFromArchive(FontModule::FONT_ARCHIVE | (index << 8), fontPaths[index]);
}