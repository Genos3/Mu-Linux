#pragma once
// DCFont.h - portable font face for the Dreamcast build.
//
// Same API as Platform/SDL3/SDLFont.h (on purpose): the portable font path in
// Core/Font.h stays byte-identical, only the implementation differs. The class
// below is a STUB RASTERIZER: every glyph is a generated outline box, so text
// is visible and measurable but not legible. It exists so M0/M1 links and
// boots without FreeType/HarfBuzz on DC.
//
// TODO(text): real FreeType 2 port (kos-ports freetype) or a compact 5x7/8x8
// bitmap font for the MU character set (ASCII + Latin-1 + the CP1252 extras
// in Data/Local). TextShaper.cpp (HarfBuzz) stays excluded from the DC build;
// FontStack is implemented in DCFont.cpp with a trivial UTF-8 pass-through.

#include <vector>
#include <cstdint>
#include <string>
#include "Platform/SDL3/SDLFont.h"   // only for Platform::FontGlyph / FontMetrics (header is dependency-free)

namespace Platform {
class DCFont {
public:
    DCFont();
    ~DCFont();
    DCFont(const DCFont&) = delete;
    DCFont& operator=(const DCFont&) = delete;

    bool Load(std::vector<uint8_t> bytes, int height, std::string& error, long faceIndex = 0);
    bool LoadFile(const std::string& path, int height, std::string& error, long faceIndex = 0);
    bool LoadFamily(const char* family, bool bold, bool italic, int height,
                    int width, bool underline, bool strikeOut, std::string& error,
                    const char* assetRoot = nullptr);

    static std::vector<std::string> InstalledFamilies();
    static void SetAntialias(bool enabled);
    static uint32_t DecodeByte(uint8_t byte);   // resources: Windows-1252

    const FontGlyph& Glyph(uint32_t codepoint);
    const FontGlyph& GlyphIndex(uint32_t glyph);
    bool HasCodepoint(uint32_t codepoint) const;
    void* ShapingFont();                        // always nullptr on DC (no HarfBuzz)
    FontMetrics Metrics() const;
    int Measure(const uint32_t* text, size_t count);
private:
    int height_ = 16;
    FontGlyph cache_{};
    FontGlyph BuildBox(uint32_t codepoint) const;
};
} // namespace Platform
