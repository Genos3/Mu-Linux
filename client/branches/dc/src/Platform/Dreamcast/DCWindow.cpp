// DCWindow.cpp - video mode, PVR init and the (trivial) event pump.
//
// Dreamcast has no window system: "ServiceEvents" only needs to keep the input
// devices sampled once per frame. The maple devices are polled lazily by
// IInput::KeyState, which makes this a no-op for the scaffold; the game loop in
// Platform/ClientEntry.cpp calls it every iteration before Scene_Dispatch.
//
// TODO(perf): cache maple state once per frame here (maple_enum_type +
// maple_dev_status) instead of polling in KeyState; move the mouse cursor
// integration and the "absolute mouse position" clamp in at the same time.

#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/IWindow.h"

namespace Platform {
namespace {
class DCWindow final : public IWindow {
    bool created_ = false;
    bool closeRequested_ = false;
    uint32_t width_ = 640, height_ = 480;

    void InitPvrOnce()
    {
        static bool done = false;
        if (done) return;
        done = true;
        pvr_init_params_t params = {
            { PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },
            1024 * 1024, 0, 0, 0, 0, 0
        };
        pvr_init(&params);
        pvr_set_bg_color(0, 0, 0);   // black, repainted every frame anyway
        DCLog("pvr_init done (bins: 16 opaque, %uKB VB)", 1024u);
    }
public:
    ~DCWindow() override { Shutdown(); }

    void ServiceLoadingEvents(unsigned maximum) override
    {
        // Keep input sampled during loading screens too.
        (void)maximum;
        DCInputPump();
    }

    void ServiceEvents() override { DCInputPump(); }
    bool UsesPlatformPump() const override { return true; }
    bool CloseRequested() const override { return closeRequested_; }

    void Shutdown() override
    {
        if (!created_) return;
        created_ = false;
        DCLog("Shutdown");
    }

    void ShowError(const char* text, const char* title, bool attach, bool errorIcon) override
    {
        (void)attach; (void)errorIcon;
        // Console fallback: DC debug builds expose serial/netconsole. A real
        // on-screen error panel is TODO(mu-ui) once the 2D path is up.
        DCLog("ERROR %s: %s", title, text);
    }

    bool Create(const WindowSettings& settings) override
    {
        width_ = settings.width ? settings.width : 640;
        height_ = settings.height ? settings.height : 480;
        // Dreamcast: fixed 640x480, 16bpp. Hardware cannot do windowed mode or
        // MSAA; honor only the fullscreen flag for future use.
        // The GD-ROM iso9660 driver mounts itself on /cd; make that the cwd so
        // any bare relative path the game hands to fopen()/std::filesystem
        // (e.g. "Data\\...") resolves against the disc.
        chdir("/cd");
        vid_set_mode(DM_640x480, PM_RGB565);
        InitPvrOnce();
        created_ = true;
        DCLog("Create(%ux%u) -> DM_640x480 RGB565", width_, height_);
        return true;
    }

    bool IsCreated() const override { return created_; }

    void ChangeMode(const WindowSettings& settings) override
    {
        // Video mode is fixed on DC; switching means re-init PVR, which the
        // client never does today. Log and keep running.
        (void)settings;
        DCLog("ChangeMode ignored: DC video mode is fixed 640x480 RGB565");
    }

    void RestoreDisplay() override {}
    void SetTitle(const char* title) override { DCLog("title: %s", title); }   // no window title bar on DC
    void ShowAndFocus() override {}
    void QuitLoop() override { closeRequested_ = true; }
    void CloseWindow() override { closeRequested_ = true; }
    void RequestCloseApplication() override { closeRequested_ = true; }
};
} // namespace

IWindow& GetWindow()
{
    static DCWindow window;
    return window;
}
} // namespace Platform
