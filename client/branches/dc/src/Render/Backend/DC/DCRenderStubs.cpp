// DCRenderStubs.cpp - scaffold backend. Empty definitions of every symbol the
// dispatch layer (Render/Backend/Dispatch.cpp) binds to the GL backend, so a
// KOS binary links before the PVR backend exists. Bodies are intentionally
// empty (missing returns are fine: this path is never meant to draw).
//
// DELETE THIS FILE once Render/Backend/DC draws through the PVR directly:
// the real backend will define the same symbols for the DC build.
//
// It also stubs the GL-*direct* entry points the shared code calls into
// (RenderProfileGL::*, TerrainRendererGL::*), which are normally defined in
// Render/Backend/GL/* and were excluded from the KOS build.

#include "Render/Backend/Api.h"
#include "Render/Backend/GfxDevice.h"
#include "Render/TerrainDraw.h"
#include "Render/RenderBackend.h"
#include "Render/RenderProfile.h"
#include "Render/Backend/GL/ProfileQueries.h"
#include "Render/Backend/GL/TerrainRenderer.h"

// void and non-void entries are handled by one template: non-void stubs return
// a zero value (Texture 0 == "invalid handle"; nullptr for strings; false for
// bools) so call sites that branch on truthiness behave like "asset missing"
// instead of reading garbage out of the return register. `return voidExpr` is
// legal C++, so the void entries work with the same macro.
namespace {
template <typename T> T dccpStubValue() { return T{}; }
template <> void dccpStubValue<void>() {}
template <> int dccpStubValue<int>() { return 1024; }              // MaxTextureSize / UnpackAlignment: benign non-zero
template <> const char* dccpStubValue<const char*>() { return "Dreamcast PVR (stub)"; }  // never null: printed with %s
} // namespace
#define MU_BACKEND_STUB(ret, name, params, args) ret name params { return dccpStubValue<ret>(); }

namespace GfxDevice { namespace GL { MU_GFX_DEVICE_API(MU_BACKEND_STUB) } }
namespace BufferedDraw { namespace GL {
    // Pretend to be ready so InitOpenGL proceeds; we draw nothing yet.
    bool Initialize() { return true; }
    void Release() {}
    void Submit(unsigned mode, const Vertex* vertices, std::size_t count) { (void)mode; (void)vertices; (void)count; }
    void Suspend() {}
    void Activate(unsigned shader, unsigned array, unsigned buffer) { (void)shader; (void)array; (void)buffer; }
    void DrawIndexed(unsigned array, unsigned buffer, const Vertex* vertices, std::size_t count, std::size_t indices) { (void)array; (void)buffer; (void)vertices; (void)count; (void)indices; }
} }
namespace MeshDraw { namespace GL { MU_MESH_DRAW_API(MU_BACKEND_STUB) } }
namespace Skinning { namespace GL { MU_SKINNING_API(MU_BACKEND_STUB) } }

#undef MU_BACKEND_STUB

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
    result.reason = "Terreno: sin backend (scaffold)";
    return result;
}
bool BoundSourceTexture() { return false; }
bool SourceTexture(std::uint32_t) { return false; }
void Release() {}
} // namespace TerrainRendererGL
