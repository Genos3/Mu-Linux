// DCPresent.cpp - frame submission / swap for the PVR backend.
//
// Implements Platform::IGLContext so the shared Core/Window.cpp + Scene_* code
// (which calls GetGLContext().Present() unconditionally) works unchanged.
// There is no GL context on DC; the interface name stays for compatibility.
//
// TODO(render-backend): once Render\Backend\PVR exists, the frame bracket
// (pvr_scene_begin / pvr_scene_finish) belongs to the render path, not to
// Present(). Today the call order is: draws submit TA primitives into the
// current pass, Present() forces the tile accelerator to drain.

#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/IGLContext.h"

// Defined in Render/Backend/DC/DCPVR.cpp (same translation unit set).
void DCPVR_SceneFinish();

namespace Platform {
namespace {
class DCPresent final : public IGLContext {
    int identityTag_ = 1;
    bool interval_ = false;
public:
    // Opaque, per-context token for resource caches. Distinct and stable.
    void* CurrentIdentity() const override { return const_cast<int*>(&identityTag_); }

    bool Initialize() override
    {
        DCLog("DCPresent::Initialize");
        return true;
    }

    void Release() override {}

    void Present() override
    {
        DCPVR_SceneFinish();
        pvr_wait_ready();
    }

    bool SetSwapInterval(int interval) override
    {
        interval_ = interval != 0;
        // PVR has no double buffered GL interval toggle; vblank pacing happens
        // in pvr_scene_finish. Return true so the config path is happy.
        return true;
    }
};
} // namespace

IGLContext& GetGLContext()
{
    static DCPresent present;
    return present;
}
} // namespace Platform
