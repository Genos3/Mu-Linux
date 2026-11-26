// DCFont.cpp - stub rasterizer + trivial FontStack for the DC build.
//
// DCFont: generated outline-box glyphs (see the TODO in DCFont.h).
// FontStack: UTF-8 decode + per-codepoint glyphs, no HarfBuzz, no bidi, no
// fallback chain - everything maps to the primary face. This is intentionally
// the minimum needed for Core/Font.h's portable path (Measure/DrawTextMask) to
// work on ASCII/Latin text. MU_TEXT_SIMPLE is implied on DC; do not define it,
// the stub is always in effect here.

#include "Platform/Dreamcast/DCFont.h"
#include "Text/TextShaper.h"
#include "Text/TextCore.h"
#include <algorithm>
#include <cstring>

namespace Platform {
namespace {
constexpr uint32_t kAsciiMax = 0x7e;
}

// Generated in DCFontBaked.cpp (offline GDI+ rasterization of Arial).
bool DCFont_BakedGlyph(uint32_t cp, int& advance, int& left, int& top,
                       int& w, int& h, const uint8_t*& cov);

DCFont::DCFont() = default;
DCFont::~DCFont() = default;

FontGlyph DCFont::BuildBox(uint32_t codepoint) const
{
    FontGlyph out{};
    out.advance = std::max(6, height_ / 2);
    out.left = 1;
    out.top = (height_ * 3) / 4;
    out.width = std::max(4, height_ / 2);
    out.height = height_ - 6 > 4 ? height_ - 6 : 4;
    const size_t pixels = static_cast<size_t>(out.width) * static_cast<size_t>(out.height);
    out.coverage.assign(pixels, 0);
    // 1 px outline so strings are visible in the atlas/editor.
    for (int x = 0; x < out.width; ++x) {
        out.coverage[x] = 255;
        out.coverage[static_cast<size_t>(out.height - 1) * out.width + x] = 255;
    }
    for (int y = 0; y < out.height; ++y) {
        out.coverage[static_cast<size_t>(y) * out.width] = 255;
        out.coverage[static_cast<size_t>(y) * out.width + out.width - 1] = 255;
    }
    // Empty interior for spaces, dot pattern for everything else.
    if (codepoint == ' ') {
        std::fill(out.coverage.begin(), out.coverage.end(), 0);
        return out;
    }
    for (size_t i = out.width + 1; i + out.width + 1 < pixels; i += 2)
        if (i % static_cast<size_t>(out.width) != 0 && i % static_cast<size_t>(out.width) != static_cast<size_t>(out.width - 1))
            out.coverage[i] = 80;
    return out;
}

bool DCFont::Load(std::vector<uint8_t> bytes, int height, std::string& error, long faceIndex)
{
    (void)bytes; (void)faceIndex;
    height_ = height > 0 ? height : 16;
    error.clear();
    return true;   // bytes are ignored by the stub rasterizer
}

bool DCFont::LoadFile(const std::string& path, int height, std::string& error, long faceIndex)
{
    (void)path; (void)faceIndex;
    height_ = height > 0 ? height : 16;
    error.clear();
    return true;
}

bool DCFont::LoadFamily(const char* family, bool bold, bool italic, int height,
                        int width, bool underline, bool strikeOut, std::string& error,
                        const char* assetRoot)
{
    (void)family; (void)bold; (void)italic; (void)width; (void)underline;
    (void)strikeOut; (void)assetRoot;
    height_ = height > 0 ? height : 16;
    error.clear();
    return true;
}

std::vector<std::string> DCFont::InstalledFamilies() { return {"Dreamcast Stub"}; }
void DCFont::SetAntialias(bool enabled) { (void)enabled; }

uint32_t DCFont::DecodeByte(uint8_t byte)
{
    // Latin-1 pass-through; plenty for the ASCII/CP1252 subset shown by the stub.
    return byte < 0x80 ? byte : byte;
}

const FontGlyph& DCFont::Glyph(uint32_t codepoint)
{
    int advance = 0, left = 0, top = 0, w = 0, h = 0;
    const uint8_t* cov = nullptr;
    if (DCFont_BakedGlyph(codepoint, advance, left, top, w, h, cov)) {
        cache_ = FontGlyph{};
        cache_.advance = advance;
        cache_.left = left;
        cache_.top = top;
        cache_.width = w;
        cache_.height = h;
        if (w > 0 && h > 0)
            cache_.coverage.assign(cov, cov + (size_t)w * h);
        return cache_;
    }
    cache_ = BuildBox(codepoint);
    return cache_;
}

const FontGlyph& DCFont::GlyphIndex(uint32_t glyph) { return Glyph(glyph); }
bool DCFont::HasCodepoint(uint32_t codepoint) const { return codepoint <= 0x024f; }   // Latin blocks
void* DCFont::ShapingFont() { return nullptr; }
FontMetrics DCFont::Metrics() const { return {height_ - height_ / 4, height_}; }

int DCFont::Measure(const uint32_t* text, size_t count)
{
    if (!text) return 0;
    int width = 0;
    for (size_t i = 0; i < count; ++i) {
        int advance = 0, left = 0, top = 0, w = 0, h = 0;
        const uint8_t* cov = nullptr;
        if (DCFont_BakedGlyph(text[i], advance, left, top, w, h, cov))
            width += advance;
        else
            width += std::max(6, height_ / 2);
    }
    return width;
}

} // namespace Platform

// ---------------------------------------------------------------------------
// TextEngine::FontStack - same contract as TextShaper.cpp but with a trivial
// UTF-8 pass-through layout. No bidi, no shaping, no fallbacks.
// ---------------------------------------------------------------------------
namespace TextEngine {
// Cache* is never set on DC (default member initializer in TextShaper.h keeps
// it null), so the implicit lifetime requirements stay trivial and no complete
// nested-Cache definition is needed here. The ctor and Primary() are already
// inline in TextShaper.h.
FontStack::~FontStack() = default;
void FontStack::Reset(Platform::PortableFont* primary, int height, bool bold)
{
    m_Primary = primary;
    m_Height = height;
    m_Bold = bold;
}

Platform::PortableFont* FontStack::FontFor(char32_t) { return m_Primary; }
Platform::PortableFont* FontStack::SystemFontFor(char32_t) { return m_Primary; }
Platform::PortableFont* FontStack::ColorEmojiFont() { return m_Primary; }
std::size_t FontStack::FallbackCount() { return 0; }

const ShapedText& FontStack::Shape(std::string_view utf8)
{
    static ShapedText result;
    result.glyphs.clear();
    result.width = 0;
    if (!m_Primary) return result;

    int x = 0;
    std::size_t offset = 0;
    while (offset < utf8.size()) {
        char32_t codepoint = 0;
        const std::size_t consumed = TextEngine::Decode(utf8, offset, codepoint);
        if (consumed == 0) break;
        ShapedGlyph glyph;
        glyph.font = m_Primary;
        glyph.glyph = static_cast<uint32_t>(codepoint);
        glyph.x = x;
        glyph.y = 0;
        glyph.cluster = static_cast<uint32_t>(offset);
        x += m_Primary->GlyphIndex(glyph.glyph).advance;
        offset += consumed;
        result.glyphs.push_back(glyph);
    }
    result.width = x;
    return result;
}

void SetFallbackFontDirectory(const std::string& /*directory*/) {}
void SetColorEmoji(bool /*enabled*/) {}

} // namespace TextEngine
