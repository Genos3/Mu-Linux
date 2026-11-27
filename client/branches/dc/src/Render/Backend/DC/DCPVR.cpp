// DCPVR.cpp - first real Dreamcast render backend (2D path).
//
// Implements the GL-named symbols the dispatch layer binds (Dispatch.cpp)
// against KOS/PVR directly. BufferedDraw batches become PVR polygons and
// uploaded RGBA buffers become twiddled 16-bit PVR textures. MeshDraw and
// Skinning stay no-ops until the 3D pass exists.
//
// Frame bracket: GfxDevice::Clear() opens the PVR scene; DCPresent::Present()
// calls DCPVR_SceneFinish() to close it. Submit() opens defensively so a
// caller that skips Clear still emits valid primitives.

#include "Render/Backend/Api.h"
#include "Render/Backend/GfxDevice.h"
#include "Render/Gfx.h"
#include "Render/RenderState.h"
#include "Render/TerrainDraw.h"
#include "Render/RenderBackend.h"
#include "Render/RenderProfile.h"
#include "Render/Backend/GL/ProfileQueries.h"
#include "Render/Backend/GL/TerrainRenderer.h"
#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/Dreamcast/DCAssetCache.h"
#include "Platform/IFileSystem.h"
#include <dc/pvr.h>
#include <cstring>
#include <vector>
#include <algorithm>
#include <cmath>

extern int TestSceneMapId;

namespace {
// The game uses < 200 concurrent handles; 2048 entries wasted ~100 KB of BSS
// and the guest hit std::bad_alloc at the login screen with only 16 MB RAM.
constexpr int kMaxTextures = 1024;   // object textures live in slots >= 0x300
// pvrtex .dt header (32 bytes, little-endian) - see kos/utils/pvrtex/file_dctex.h
struct DtHeader {
    char fourcc[4];
    uint32_t chunk_size;
    uint8_t version, header_size, codebook_size, colors_used;
    uint16_t w, h;
    uint32_t pvr_type, pad1, pad2, pad3;
};
constexpr uint32_t kDtModeMask = 0xFC000000;
constexpr uint32_t kDtSizeMask = 0x0000003F;
struct Tex {
    pvr_ptr_t ptr = nullptr;
    int w = 0, h = 0;
    uint32_t fmt = PVR_TXRFMT_RGB565;
    DtHeader dt{};        // non-zero fourcc => native VQ texture
    bool used = false;
};
Tex g_textures[kMaxTextures];
Gfx::Texture g_bound = 0;
bool g_blend = false, g_alphaTest = false, g_textureEnabled = true;
Gfx::Factor g_blendSrc = Gfx::Factor::SrcAlpha;
Gfx::Factor g_blendDst = Gfx::Factor::OneMinusSrcAlpha;
bool g_depthTest = true;   // game toggles this: 3D on, 2D UI off
bool g_depthMask = true;   // glDepthMask(false) passes must still read depth
float g_clear[4] = {0, 0, 0, 1};
bool g_sceneOpen = false;

uint32_t ToArgb(const float* c)
{
    auto part = [](float v) -> uint32_t {
        int n = static_cast<int>(v * 255.0f);
        return static_cast<uint32_t>(n < 0 ? 0 : (n > 255 ? 255 : n));
    };
    return (part(c[3]) << 24) | (part(c[0]) << 16) | (part(c[1]) << 8) | part(c[2]);
}
uint16_t Pack565(unsigned char r, unsigned char g, unsigned char b)
{
    return static_cast<uint16_t>(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}
uint16_t Pack4444(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    return static_cast<uint16_t>(((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4));
}
int Pow2(int v)
{
    int n = 1;
    while (n < v && n < 1024) n <<= 1;
    return n;
}
pvr_blend_mode_t ToPvrBlend(Gfx::Factor f)
{
    // PVR2 exposes only 8 factors. The src-color variants have no direct
    // hardware equivalent; approximate them rather than leaving them unset.
    switch (f) {
    case Gfx::Factor::Zero:                return PVR_BLEND_ZERO;
    case Gfx::Factor::One:                 return PVR_BLEND_ONE;
    case Gfx::Factor::SrcColor:            return PVR_BLEND_ONE;
    case Gfx::Factor::OneMinusSrcColor:    return PVR_BLEND_ZERO;
    case Gfx::Factor::SrcAlpha:            return PVR_BLEND_SRCALPHA;
    case Gfx::Factor::OneMinusSrcAlpha:    return PVR_BLEND_INVSRCALPHA;
    case Gfx::Factor::DstAlpha:            return PVR_BLEND_DESTALPHA;
    case Gfx::Factor::OneMinusDstAlpha:    return PVR_BLEND_INVDESTALPHA;
    case Gfx::Factor::DstColor:            return PVR_BLEND_DESTCOLOR;
    case Gfx::Factor::OneMinusDstColor:    return PVR_BLEND_INVDESTCOLOR;
    case Gfx::Factor::SrcAlphaSaturate:    return PVR_BLEND_SRCALPHA;
    }
    return PVR_BLEND_ONE;
}
struct Batch {
    pvr_poly_hdr_t hdr;
    std::vector<pvr_vertex_t> verts;
};
std::vector<Batch> g_opaque, g_translucent;
// Reuse small Batch objects across frames to cut per-frame allocation churn.
// Bounded: retaining the large terrain buffers permanently OOMs the DC heap.
constexpr std::size_t kPoolMaxBatches = 64;
constexpr std::size_t kPoolMaxVerts = 256;
std::vector<Batch> g_batchPool;
bool s_wrapUv = false;   // set per Submit: wrap tile-space terrain UVs only
void Emit(std::vector<pvr_vertex_t>& out, const float* xyz, const float* uv, uint32_t argb, bool eol)
{
    pvr_vertex_t v;
    std::memset(&v, 0, sizeof(v));
    v.flags = eol ? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX;
    v.x = xyz[0]; v.y = xyz[1]; v.z = xyz[2];
    // TexParameter() is a no-op on PVR. Terrain UVs are tile-space (e.g. 58.5)
    // and need wrapping into [0,1); UI/atlas quads must keep their raw UVs.
    if (s_wrapUv) {
        v.u = uv[0] - std::floor(uv[0]);
        v.v = uv[1] - std::floor(uv[1]);
    } else {
        v.u = uv[0]; v.v = uv[1];
    }
    v.argb = argb; v.oargb = 0;
    out.push_back(v);
}
void FlushList(pvr_list_t list, std::vector<Batch>& batches)
{
    if (batches.empty()) return;
    pvr_list_begin(list);
    for (const Batch& batch : batches) {
        if (batch.verts.empty()) continue;
        pvr_prim(&batch.hdr, sizeof(batch.hdr));
        for (const pvr_vertex_t& v : batch.verts) pvr_prim(&v, sizeof(v));
    }
    pvr_list_finish();
    // Recycle only small batches; large terrain buffers are freed instead of
    // retained permanently (the DC heap cannot afford ~1.6 MB of pinned buffers).
    while (!batches.empty()) {
        Batch& b = batches.back();
        if (g_batchPool.size() < kPoolMaxBatches && b.verts.capacity() <= kPoolMaxVerts)
            g_batchPool.push_back(std::move(b));
        batches.pop_back();   // frees the buffer unless it was moved into the pool
    }
}
} // namespace

void DCPVR_SceneBegin()
{
    if (g_sceneOpen) return;
    pvr_scene_begin();
    g_sceneOpen = true;
}
void DCPVR_SceneFinish()
{
    if (!g_sceneOpen) return;
    FlushList(PVR_LIST_OP_POLY, g_opaque);
    FlushList(PVR_LIST_TR_POLY, g_translucent);
    pvr_scene_finish();
    g_sceneOpen = false;
}

// Upload a validated .dt payload to PVR VRAM. Source is either an open disc
// file (streamed in 16 KB pieces) or a cached in-memory copy from the ring.
static bool UploadDt(const DtHeader& hdr, std::FILE* f,
                     const unsigned char* mem, unsigned memSize,
                     unsigned* outHandle, int* outW, int* outH)
{
    const std::size_t headerBytes = (std::size_t)(hdr.header_size + 1) * 32;
    if (headerBytes > hdr.chunk_size) return false;
    const std::size_t payloadSize = hdr.chunk_size - headerBytes;
    pvr_ptr_t vram = pvr_mem_malloc(payloadSize);
    if (!vram) return false;

    alignas(32) unsigned char scratch[16384];
    std::size_t offset = 0;
    while (offset < payloadSize) {
        const std::size_t chunk = std::min(sizeof(scratch), payloadSize - offset);
        const unsigned char* src;
        if (mem) {
            src = mem + headerBytes + offset;   // aligned: malloc + 32-byte header + 16 KB steps
        } else {
            if (::fread(scratch, 1, chunk, f) != chunk) { pvr_mem_free(vram); return false; }
            src = scratch;
        }
        pvr_txr_load(src, (pvr_ptr_t)((char*)vram + offset), chunk);
        offset += chunk;
    }

    for (int i = 0; i < kMaxTextures; ++i) {
        if (g_textures[i].used) continue;
        Tex& tex = g_textures[i];
        tex = Tex{};
        tex.used = true;
        tex.ptr = vram;
        tex.dt = hdr;
        tex.w = 1 << (((hdr.pvr_type >> 3) & 7) + 3);
        tex.h = 1 << ((hdr.pvr_type & 7) + 3);
        tex.fmt = 0;
        if (outHandle) *outHandle = static_cast<unsigned>(i + 1);
        if (outW) *outW = tex.w;
        if (outH) *outH = tex.h;
        return true;
    }
    pvr_mem_free(vram);
    return false;
}

static bool ValidDt(const DtHeader& hdr, unsigned fileSize)
{
    return fileSize >= sizeof(DtHeader) &&
           ::memcmp(hdr.fourcc, "DcTx", 4) == 0 && hdr.version <= 1 &&
           hdr.chunk_size > sizeof(DtHeader) && hdr.chunk_size <= 4 * 1024 * 1024 &&
           hdr.chunk_size == fileSize;
}

// Load a pre-converted pvrtex .dt texture straight into PVR VRAM. No stb
// decode, no RGBA staging buffer in main RAM: essential for 16 MB.
extern "C" bool DCPVR_LoadDT(const char* path, unsigned* outHandle, int* outW, int* outH)
{
    // Ring-cache hit: prefetched bytes are already in main RAM, no disc read.
    unsigned cachedSize = 0;
    const unsigned char* cached = DCAssetCache::Acquire(path, &cachedSize);
    if (cached) {
        DtHeader hdr{};
        std::memcpy(&hdr, cached, sizeof(hdr));
        bool ok = ValidDt(hdr, cachedSize) &&
                  UploadDt(hdr, nullptr, cached, cachedSize, outHandle, outW, outH);
        DCAssetCache::Release(path);
        return ok;
    }

    // Route through the DC filesystem so relative "Data\..." paths get the
    // /cd/ prefix + backslash folding (raw fopen can't resolve them on KOS).
    DCAssetCache::LockFile();
    std::FILE* f = Platform::GetFileSystem().Open(path, "rb");
    if (!f) { DCAssetCache::UnlockFile(); return false; }
    DtHeader hdr{};
    if (::fread(&hdr, 1, sizeof(hdr), f) != sizeof(hdr) ||
        ::memcmp(hdr.fourcc, "DcTx", 4) != 0 || hdr.version > 1 ||
        hdr.chunk_size <= sizeof(hdr) || hdr.chunk_size > 4 * 1024 * 1024) {
        ::fclose(f); DCAssetCache::UnlockFile(); return false;
    }
    const std::size_t headerBytes = (std::size_t)(hdr.header_size + 1) * 32;
    if (::fseek(f, (long)headerBytes, SEEK_SET) != 0) { ::fclose(f); DCAssetCache::UnlockFile(); return false; }
    bool ok = UploadDt(hdr, f, nullptr, 0, outHandle, outW, outH);
    ::fclose(f);
    DCAssetCache::UnlockFile();
    return ok;
}

// --- GfxDevice -----------------------------------------------------------
namespace GfxDevice { namespace GL {
void Reset() {
    g_bound = 0; g_blend = false; g_alphaTest = false; g_textureEnabled = true;
    g_depthTest = true; g_depthMask = true;
    g_blendSrc = Gfx::Factor::SrcAlpha;
    g_blendDst = Gfx::Factor::OneMinusSrcAlpha;
}
void ApplyCap(Gfx::Cap cap, bool on)
{
    if (cap == Gfx::Cap::Blend) g_blend = on;
    else if (cap == Gfx::Cap::AlphaTest) g_alphaTest = on;
    else if (cap == Gfx::Cap::Texture2D) g_textureEnabled = on;
    else if (cap == Gfx::Cap::DepthTest) g_depthTest = on;
}
void ApplyBlend(Gfx::Factor source, Gfx::Factor destination)
{
    g_blendSrc = source;
    g_blendDst = destination;
    g_blend = !(source == Gfx::Factor::One && destination == Gfx::Factor::Zero);
}
void ApplyDepthFunc(Gfx::Compare) {}
void ApplyDepthMask(bool write) { g_depthMask = write; }
void ApplyTexture(Gfx::Texture texture) { g_bound = texture; }
void ApplyViewport(int, int, int, int) {}
void ClearColor(float r, float g, float b, float a)
{
    g_clear[0] = r; g_clear[1] = g; g_clear[2] = b; g_clear[3] = a;
    pvr_set_bg_color(r, g, b);
}
void Clear(unsigned)
{
    DCPVR_SceneBegin();   // frame bracket starts here
    pvr_set_bg_color(g_clear[0], g_clear[1], g_clear[2]);
}
void Flush() {}
void ClearErrors() {}
void PostProcess(bool, float) {}
bool SupportsShadowMaps() { return false; }
void ShadowScene(bool, const float*, float) {}
void ShadowCasting(bool) {}
void Atmosphere(int, const float*, float, float) {}
void ReadPixelsRGB(int, int, int, int, void*) {}
int MaxTextureSize() { return 1024; }
void MaxViewportDims(int out[2]) { if (out) { out[0] = 640; out[1] = 480; } }
const char* GetDeviceInfo(Gfx::DeviceInfo info)
{
    switch (info) {
    case Gfx::DeviceInfo::Vendor: return "Sega";
    case Gfx::DeviceInfo::Renderer: return "PowerVR2 (KOS)";
    default: return "KallistiOS";
    }
}
Gfx::Texture CreateTexture()
{
    for (int i = 0; i < kMaxTextures; ++i) {
        if (!g_textures[i].used) {
            g_textures[i] = Tex{};
            g_textures[i].used = true;
            return static_cast<Gfx::Texture>(i + 1);
        }
    }
    return 0;
}
void DeleteTexture(Gfx::Texture texture)
{
    if (texture == 0 || texture > kMaxTextures) return;
    Tex& tex = g_textures[texture - 1];
    if (tex.ptr) pvr_mem_free(tex.ptr);
    tex = Tex{};
    if (g_bound == texture) g_bound = 0;
}
void TexImage2D(int components, int width, int height, Gfx::Format format, const void* pixels)
{
    if (g_bound == 0 || g_bound > kMaxTextures || !pixels || width <= 0 || height <= 0) return;
    Tex& tex = g_textures[g_bound - 1];
    const int w = Pow2(width), h = Pow2(height);
    if (w > 1024 || h > 1024) return;
    if (tex.ptr) { pvr_mem_free(tex.ptr); tex.ptr = nullptr; }
    tex.w = w; tex.h = h;

    const unsigned char* src = static_cast<const unsigned char*>(pixels);
    const int bpp = components == 3 ? 3 : 4;   // HUD/UI bitmaps can be RGB-only
    const bool hasAlpha = format == Gfx::Format::RGBA && components == 4;
    bool useAlpha = false;
    if (hasAlpha) {
        // Only scan the source pixels: the PVR padding is not in `pixels`.
        for (std::size_t i = 3; i < (std::size_t)width * height * bpp; i += bpp) {
            if (src[i] != 255) { useAlpha = true; break; }
        }
    }
    tex.fmt = useAlpha ? PVR_TXRFMT_ARGB4444 : PVR_TXRFMT_RGB565;
    // PVR needs power-of-two dimensions; zero-fill the padding cells instead
    // of reading past the end of a non-power-of-two source buffer.
    std::vector<uint16_t> converted(static_cast<std::size_t>(w) * h, 0);
    for (int row = 0; row < height; ++row) {
        for (int col = 0; col < width; ++col) {
            const unsigned char* s = src + ((std::size_t)row * width + col) * bpp;
            converted[(std::size_t)row * w + col] =
                useAlpha ? Pack4444(s[0], s[1], s[2], s[3]) : Pack565(s[0], s[1], s[2]);
        }
    }
    tex.ptr = pvr_mem_malloc(converted.size() * sizeof(uint16_t));
    if (!tex.ptr) {
        Platform::DCLog("PVR tex %u: out of VRAM (%dx%d)", g_bound, w, h);
        return;
    }
    pvr_txr_load_ex(converted.data(), tex.ptr, static_cast<uint32_t>(w), static_cast<uint32_t>(h), PVR_TXRLOAD_16BPP);
}
void TexSubImage2D(int x, int y, int w, int h, Gfx::Format format, const void* pixels)
{
    if (g_bound == 0 || g_bound > kMaxTextures || !pixels) return;
    Tex& tex = g_textures[g_bound - 1];
    if (!tex.ptr || tex.w <= 0 || tex.h <= 0 || w <= 0 || h <= 0) return;
    if (x < 0 || y < 0 || x + w > tex.w || y + h > tex.h) return;

    // Write directly into the twiddled VRAM layout (no CPU shadow - RAM is tight).
    // pvr_txr_load_ex twiddles by default, so the atlas uses the same layout.
    const uint8_t* src = (const uint8_t*)pixels;
    uint16_t* dst = (uint16_t*)tex.ptr;
    const bool useAlpha = (tex.fmt == PVR_TXRFMT_ARGB4444);
    for (int row = 0; row < h; ++row) {
        for (int col = 0; col < w; ++col) {
            const int px = x + col, py = y + row;
            unsigned idx = 0, bit = 1;
            int shift = 0;
            while (bit < (unsigned)tex.w) {
                // KOS TWIDOUT(x,y) = TWIDTAB(y) | (TWIDTAB(x) << 1):
                // y bits go to even positions, x bits to odd positions.
                if (py & (int)bit) idx |= 1u << (2 * shift);
                if (px & (int)bit) idx |= 1u << (2 * shift + 1);
                bit <<= 1; ++shift;
            }
            const uint8_t* s = src + ((size_t)row * w + col) * 4;
            dst[idx] = useAlpha ? Pack4444(s[0], s[1], s[2], s[3])
                                : Pack565(s[0], s[1], s[2]);
        }
    }
}
void TexParameter(Gfx::TexParam, Gfx::TexValue) {}
void TexBorderColor(const float*) {}
void SetUnpackAlignment(int) {}
int UnpackAlignment() { return 4; }
}} // namespace GfxDevice::GL

// --- BufferedDraw --------------------------------------------------------
namespace BufferedDraw { namespace GL {
bool Initialize() { return true; }
void Release() {}
void Suspend() {}
void Activate(unsigned, unsigned, unsigned) {}
void DrawIndexed(unsigned, unsigned, const Vertex*, std::size_t, std::size_t) {}
void Submit(unsigned mode, const Vertex* vertices, std::size_t count)
{
    if (!vertices || count == 0) return;
    DCPVR_SceneBegin();

    const Tex* tex = (g_textureEnabled && g_bound && g_bound <= kMaxTextures)
                   ? &g_textures[g_bound - 1] : nullptr;
    const bool textured = tex && tex->ptr && tex->w > 0;
    if (g_bound >= 0x300) {
        static int s_hiLog = 0;
        if (s_hiLog < 6) {
            ++s_hiLog;
            Platform::DCLog("HITEX: bound=%u textured=%d ptr=%p w=%u",
                            g_bound, (int)textured, tex ? (void*)tex->ptr : nullptr,
                            tex ? tex->w : 0);
        }
    }
    if (TestSceneMapId >= 0 && count > 200) {
        static int s_submitLog = 0;
        if (s_submitLog < 4) {
            ++s_submitLog;
            Platform::DCLog("PVR_SUBMIT[%d]: n=%u texEnabled=%d bound=%u textured=%d depth=%d cull=%d blend=%d mvpT=(%.0f,%.0f,%.0f)",
                            s_submitLog, (unsigned)count, (int)g_textureEnabled, g_bound,
                            (int)textured, (int)g_depthTest,
                            (int)RenderState::Enabled(Gfx::Cap::CullFace), (int)g_blend,
                            RenderState::ModelViewProjection()[12],
                            RenderState::ModelViewProjection()[13],
                            RenderState::ModelViewProjection()[14]);
        }
    }
    bool translucent = g_blend;
    for (std::size_t i = 0; i < count && !translucent; ++i)
        if (vertices[i].color[3] < 0.999f) translucent = true;
    // Glyph quads are solid coverage rectangles - opaque list avoids the
    // translucent-list alpha modulation that was dropping them entirely.
    if (!textured && count > 100 && !g_depthTest) translucent = false;
    const pvr_list_t list = translucent ? PVR_LIST_TR_POLY : PVR_LIST_OP_POLY;

    pvr_poly_cxt_t cxt;
    if (textured)
        pvr_poly_cxt_txr(&cxt, list, tex->dt.fourcc[0] == 'D' ? (tex->dt.pvr_type & kDtModeMask) : tex->fmt,
                         tex->w, tex->h, tex->ptr, PVR_FILTER_NEAREST);
    else
        pvr_poly_cxt_col(&cxt, list);
    cxt.blend.src_enable = translucent;
    cxt.blend.dst_enable = translucent;
    if (translucent) {
        // Honour the blend factors the game selected (e.g. additive SrcAlpha/One)
        // instead of forcing the standard alpha blend for every translucent draw.
        cxt.blend.src = ToPvrBlend(g_blendSrc);
        cxt.blend.dst = ToPvrBlend(g_blendDst);
    }
    // Use the real Z buffer for 3D (game has depth test enabled); for 2D UI
    // the game disables it - use DEPTHCMP_ALWAYS so the TA's sort is harmless.
    // Translucent polys are TA-autosorted (autosort_disabled = 0); KOS libGL
    // uses GEQUAL for that list so equal-depth fragments survive the sort.
    // Opaque keeps GREATER (near = larger after the Z flip).
    cxt.depth.comparison = g_depthTest
        ? (translucent ? PVR_DEPTHCMP_GEQUAL : PVR_DEPTHCMP_GREATER)
        : PVR_DEPTHCMP_ALWAYS;
    cxt.depth.write = g_depthTest && g_depthMask;
    // Follow the same CullFace cap as the GL/WebGPU backends, but keep
    // alpha-blended geometry two-sided: semitransparent faces and flat rigging
    // (ropes/sails) must stay visible from both sides. The ship is a vanilla
    // model (0x3c < MODEL_MAX_VANILLA) so the catalog's two-sided flag never
    // reaches it.
    cxt.gen.culling = (RenderState::Enabled(Gfx::Cap::CullFace) && !translucent)
                    ? PVR_CULLING_CW : PVR_CULLING_NONE;
    pvr_poly_hdr_t hdr;
    pvr_poly_compile(&hdr, &cxt);
    if (textured && tex->dt.fourcc[0] == 'D' && (tex->dt.pvr_type & (1u << 30))) {
        // VQ texture: codebook occupies the low 2 KB before the texture base.
        // Small codebooks need the pointer shifted so the 2 KB block aligns.
        const unsigned codebook = (tex->dt.codebook_size + 1) * 8;
        const uintptr_t adjusted = reinterpret_cast<uintptr_t>(tex->ptr) - 2048 + codebook;
        hdr.mode2 = (hdr.mode2 & ~kDtSizeMask) | (tex->dt.pvr_type & kDtSizeMask);
        hdr.mode3 = (tex->dt.pvr_type & kDtModeMask) |
                    static_cast<uint32_t>(0x1ffffffu & (adjusted >> 3));
    }

    Batch batch;
    if (!g_batchPool.empty()) {
        batch = std::move(g_batchPool.back());
        g_batchPool.pop_back();
        batch.verts.clear();          // keep the allocated capacity
    }
    batch.hdr = hdr;

    // The GL backend transforms vertices in its vertex shader; the DC backend
    // must do it here. Producers pass world/object-space positions for 3D and
    // already-ortho screen coordinates for 2D - both go through MVP + viewport
    // exactly like SetRasterPosition(). Without this, the login background is
    // drawn with raw world coordinates (thousands of pixels) and its textures
    // tile across the whole framebuffer.
    const float* mvp = RenderState::ModelViewProjection();
    const int* vp = RenderState::Get().viewport;
    auto project = [&](const float* in, float* out) {
        const float cx = mvp[0]*in[0] + mvp[4]*in[1] + mvp[8]*in[2] + mvp[12];
        const float cy = mvp[1]*in[0] + mvp[5]*in[1] + mvp[9]*in[2] + mvp[13];
        const float cz = mvp[2]*in[0] + mvp[6]*in[1] + mvp[10]*in[2] + mvp[14];
        float cw = mvp[3]*in[0] + mvp[7]*in[1] + mvp[11]*in[2] + mvp[15];
        if (cw <= 0.0001f) cw = 0.0001f;   // avoid divide-by-zero behind camera
        out[0] = vp[0] + (cx/cw + 1.0f) * vp[2] * 0.5f;
        // GL viewport coords are bottom-up; PVR's framebuffer is top-down.
        out[1] = vp[1] + vp[3] - (cy/cw + 1.0f) * vp[3] * 0.5f;
        // PVR depth is cleared to 0 and uses GREATER, so near must map to 1.
        out[2] = 1.0f - (cz/cw + 1.0f) * 0.5f;
    };

    // Floor tile-space UVs into [0,1) for large (terrain) batches. The floor is
    // a no-op for UVs already in range, so misclassifying a large UI batch is
    // harmless - unlike the old TestSceneMapId gate, this also works in real scenes.
    s_wrapUv = count > 200;
    auto triangle = [&](const Vertex& a, const Vertex& b, const Vertex& c, bool eol) {
        float pa[3], pb[3], pc[3];
        project(a.position, pa); project(b.position, pb); project(c.position, pc);
        Emit(batch.verts, pa, a.uv, ToArgb(a.color), false);
        Emit(batch.verts, pb, b.uv, ToArgb(b.color), false);
        Emit(batch.verts, pc, c.uv, ToArgb(c.color), eol);
    };
    switch (mode) {
    case 4: // Triangles - each triangle is an independent PVR strip. Without
            // EOL per triangle the TA chains them into one strip and fills the
            // gaps between disconnected shapes (e.g. ship rigging ropes).
        for (std::size_t i = 0; i + 2 < count; i += 3)
            triangle(vertices[i], vertices[i + 1], vertices[i + 2], true);
        break;
    case 5: // TriangleStrip - one continuous strip, EOL only at the very end.
        for (std::size_t i = 0; i + 2 < count; ++i)
            triangle(vertices[i], vertices[i + 1], vertices[i + 2], i + 3 >= count);
        break;
    case 6: // TriangleFan
    case 9: // Polygon
        for (std::size_t i = 1; i + 1 < count; ++i)
            triangle(vertices[0], vertices[i], vertices[i + 1], true);
        break;
    case 7: // Quads
        for (std::size_t i = 0; i + 3 < count; i += 4) {
            triangle(vertices[i], vertices[i + 1], vertices[i + 2], true);
            triangle(vertices[i], vertices[i + 2], vertices[i + 3], true);
        }
        break;
    case 8: // QuadStrip
        for (std::size_t i = 0; i + 3 < count; i += 2) {
            triangle(vertices[i], vertices[i + 1], vertices[i + 3], true);
            triangle(vertices[i], vertices[i + 3], vertices[i + 2], true);
        }
        break;
    default: break;   // points/lines: nothing drawable on PVR TA yet
    }
    if (!batch.verts.empty())
        (translucent ? g_translucent : g_opaque).push_back(std::move(batch));
}
}} // namespace BufferedDraw::GL

// --- Mesh / skinning: no-ops until the 3D pass ---------------------------
#define MU_PVR_STUB(ret, name, params, args) ret name params { return ret{}; }
namespace MeshDraw { namespace GL { MU_MESH_DRAW_API(MU_PVR_STUB) } }
namespace Skinning { namespace GL { MU_SKINNING_API(MU_PVR_STUB) } }
#undef MU_PVR_STUB

// GL-direct profiling / terrain entry points excluded from the KOS build.
namespace RenderProfileGL {
void BeginFrame(int) {}
void SetPass(RenderProfile::Pass) {}
void EndFrame() {}
void Release() {}
std::uint64_t SkippedFrames() { return 0; }
std::uint64_t SkippedSegments() { return 0; }
bool Supported() { return false; }
void ObserveProgram(unsigned) {}
void ObserveTexture(unsigned, unsigned) {}
} // namespace RenderProfileGL
namespace TerrainRendererGL {
TerrainDraw::Result Draw(const TerrainDraw::FrameData&, const std::vector<TerrainDraw::Tile>&)
{
    TerrainDraw::Result result{};
    result.drawn = false;
    result.reason = "Terreno: sin backend PVR";
    return result;
}
bool BoundSourceTexture() { return false; }
bool SourceTexture(std::uint32_t) { return false; }
void Release() {}
} // namespace TerrainRendererGL
