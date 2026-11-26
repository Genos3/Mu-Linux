// DCInput.cpp - maple devices as a Win32-style 0..255 logical key table plus
// the InputEvent stream the game dispatches through Input_OnEvent.
//
// The game asks through IInput::KeyState(key), matching the SDL3 backend's
// contract: 0x8000 = held, low bit = "pressed since last query" (consumed).
// Absolute mouse position and clicks travel as InputEvents (MouseMove,
// LeftDown/Up, LeftDoubleClick, RightDown/Up, MiddleDown/Up, Text) emitted
// once per frame from DCWindow::ServiceEvents via DCInputPump().
//
// Controller (port A) and mouse share the maple bus. The mouse is relative
// (mouse_state_t::dx/dy); we accumulate an absolute cursor clamped to the
// 640x480 screen, exactly like the original g_MouseX/Y semantics.
//
// TODO(input):
//   * wheel: mouse_state_t carries no wheel; confirm the hardware field or
//     accept the missing scroll.
//   * controller navigation: d-pad/buttons are exposed only via KeyState;
//     map X/B/Y/A to UI actions if you want controller-first play.
//   * multiplayer: maple_enum_type(n>0, ...) for ports B,C,D.

#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/IInput.h"
#include <array>
#include <cstdint>
#include <cstdarg>
#include <cstring>

namespace Platform {
namespace {

constexpr uint32_t kBtnUp = 0x26, kBtnDown = 0x28, kBtnLeft = 0x25, kBtnRight = 0x27;
constexpr uint32_t kVkEnter = 0x0d, kVkEscape = 0x1b;

class DCInput final : public IInput {
    std::array<bool, 256> held{};
    std::array<bool, 256> pressed{};
    std::array<bool, 256> prevHeld{};
    int mouseX_ = 320, mouseY_ = 240;

public:
    // Refresh hardware into held[] and resolve mouse/keyboard event edges.
    // Emits through the bound sink (set by DCInputBind from the native bridge).
    void(*eventSink)(const InputEvent&) = nullptr;
    uint64_t lastLeftUpMs_ = 0;
    std::array<uint8_t, 6> prevKeys_{};
    int prevKeyCount_ = 0;

    void Refresh()
    {
        prevHeld = held;
        held.fill(false);

        // --- controller ---
        if (maple_device_t* dev = maple_enum_type(0, MAPLE_FUNC_CONTROLLER)) {
            if (const cont_state_t* st = static_cast<const cont_state_t*>(maple_dev_status(dev))) {
                held[kBtnUp] = (st->buttons & CONT_DPAD_UP) != 0;
                held[kBtnDown] = (st->buttons & CONT_DPAD_DOWN) != 0;
                held[kBtnLeft] = (st->buttons & CONT_DPAD_LEFT) != 0;
                held[kBtnRight] = (st->buttons & CONT_DPAD_RIGHT) != 0;
                if (held[kBtnUp] || held[kBtnDown] || held[kBtnLeft] || held[kBtnRight]) {
                    static int s_dpadLog = 0;
                    if (s_dpadLog < 8) {
                        ++s_dpadLog;
                        Platform::DCLog("DPAD: up=%d down=%d left=%d right=%d",
                                        held[kBtnUp], held[kBtnDown], held[kBtnLeft], held[kBtnRight]);
                    }
                }
                held[kVkEnter] = (st->buttons & CONT_A) != 0;
                held[kVkEscape] = (st->buttons & CONT_B) != 0;
                held[0x10] = (st->buttons & CONT_C) != 0;    // VK_SHIFT
                held[0x09] = (st->buttons & CONT_START) != 0;// VK_TAB
                held[0x20] = (st->buttons & CONT_Y) != 0;    // VK_SPACE
                held[0x78] = (st->buttons & CONT_Z) != 0;    // VK_F9
            }
        }

        // --- mouse (any maple port: A=0 ... D=3) ---
        for (int port = 0; port < 4; ++port) {
            maple_device_t* dev = maple_enum_type(port, MAPLE_FUNC_MOUSE);
            if (!dev) continue;
            if (const mouse_state_t* st = static_cast<const mouse_state_t*>(maple_dev_status(dev))) {
                mouseX_ += st->dx; mouseY_ += st->dy;
                if (mouseX_ < 0) mouseX_ = 0; if (mouseX_ > 639) mouseX_ = 639;
                if (mouseY_ < 0) mouseY_ = 0; if (mouseY_ > 479) mouseY_ = 479;
                held[0x01] = (st->buttons & MOUSE_LEFTBUTTON) != 0;   // VK_LBUTTON
                held[0x02] = (st->buttons & MOUSE_RIGHTBUTTON) != 0;  // VK_RBUTTON
                held[0x04] = (st->buttons & MOUSE_MIDDLEBUTTON) != 0; // VK_MBUTTON
            }
            break;
        }

        // --- keyboard (any maple port) ---
        for (int port = 0; port < 4; ++port) {
            maple_device_t* dev = maple_enum_type(port, MAPLE_FUNC_KEYBOARD);
            if (!dev) continue;
            if (const kbd_state_t* st = static_cast<const kbd_state_t*>(maple_dev_status(dev))) {
                bool shift = (st->cond.modifiers.raw & KBD_MOD_SHIFT) != 0;
                held[0x10] = held[0x10] || shift;                       // VK_SHIFT
                held[0x11] = (st->cond.modifiers.raw & KBD_MOD_CTRL) != 0;   // VK_CONTROL
                held[0x12] = (st->cond.modifiers.raw & KBD_MOD_ALT) != 0;    // VK_MENU
                for (int i = 0; i < KBD_MAX_PRESSED_KEYS; ++i) {
                    kbd_key_t key = st->cond.keys[i];
                    if (key == KBD_KEY_NONE || key == KBD_KEY_ERROR) continue;
                    if (key == KBD_KEY_ENTER || key == KBD_KEY_PAD_ENTER) held[kVkEnter] = true;
                    if (key == KBD_KEY_ESCAPE) held[kVkEscape] = true;
                    // Arrow keys on the DC keyboard drive the same VKs as the
                    // D-pad, so host-keyboard input can move the test camera.
                    if (key == KBD_KEY_UP)    held[kBtnUp] = true;
                    if (key == KBD_KEY_DOWN)  held[kBtnDown] = true;
                    if (key == KBD_KEY_LEFT)  held[kBtnLeft] = true;
                    if (key == KBD_KEY_RIGHT) held[kBtnRight] = true;
                }
            }
            break;
        }

        // Rising edges into pressed[].
        for (int i = 0; i < 256; ++i)
            if (held[i] && !prevHeld[i]) pressed[i] = true;
    }

    void Emit(InputEventType type, int x, int y, uint32_t character)
    {
        if (!eventSink) return;
        InputEvent event{};
        event.type = type; event.x = x; event.y = y; event.wheel = 0; event.character = character;
        eventSink(event);
    }

    bool HasImeModeIndicator() const override { return false; }

    // NOTE: Refresh() is called exactly once per frame from DCInputPump()
    // (via DCWindow::ServiceEvents/ServiceLoadingEvents). KeyState is a pure
    // read of the last sample; calling Refresh here would re-sample maple and
    // double-count relative mouse motion.
    int16_t KeyState(int key) override
    {
        if (key < 0 || key >= 256) return 0;
        const auto result = static_cast<int16_t>((held[key] ? 0x8000 : 0) | (pressed[key] ? 1 : 0));
        pressed[key] = false;
        return result;
    }

    void CaptureMouse(bool capture) override { (void)capture; }   // absolute cursor, nothing to capture

    ImeState ReadImeState(bool clearActive) override
    {
        (void)clearActive;
        return ImeState{0, 0};   // no IME on DC
    }

    void PositionIme(int x, int y, int width, int height) override { (void)x; (void)y; (void)width; (void)height; }
    std::string ClipboardText() override { return {}; }           // no clipboard on DC

    int CursorX() const { return mouseX_; }
    int CursorY() const { return mouseY_; }
};

DCInput* inputInstance = nullptr;

} // namespace

// --- binding helpers used by DCWindow (pump) and DCNativeBridge (bind) ---
void DCInputBind(void (*inputSink)(const InputEvent&), void (*closeSink)())
{
    (void)closeSink;   // no OS-level close on DC; the console is off
    if (inputInstance) inputInstance->eventSink = inputSink;
}

void DCInputPump()
{
    if (!inputInstance) return;
    DCInput& input = *inputInstance;
    input.Refresh();

    // Motion (emit only when it changed; MouseMove is pure state).
    static int lastX = -1, lastY = -1;
    if (input.CursorX() != lastX || input.CursorY() != lastY) {
        input.Emit(InputEventType::MouseMove, input.CursorX(), input.CursorY(), 0);
        lastX = input.CursorX(); lastY = input.CursorY();
    }

    // Button transitions for mouse.
    static const struct { uint16_t mask; InputEventType down, up; } kButtons[] = {
        { MOUSE_LEFTBUTTON,   InputEventType::LeftDown,  InputEventType::LeftUp },
        { MOUSE_RIGHTBUTTON,  InputEventType::RightDown, InputEventType::RightUp },
        { MOUSE_MIDDLEBUTTON, InputEventType::MiddleDown,InputEventType::MiddleUp },
    };
    // Mouse button state: find the first mouse on ANY maple port (A..D).
    maple_device_t* mouseDev = nullptr;
    for (int port = 0; port < 4 && !mouseDev; ++port)
        mouseDev = maple_enum_type(port, MAPLE_FUNC_MOUSE);
    if (mouseDev) {
        if (const mouse_state_t* st = static_cast<const mouse_state_t*>(maple_dev_status(mouseDev))) {
            static int prev = 0;
            const int cur = st->buttons;
            for (const auto& b : kButtons) {
                const bool now = (cur & b.mask) != 0, was = (prev & b.mask) != 0;
                if (now && !was) {
                    input.Emit(b.down, input.CursorX(), input.CursorY(), 0);
                    if (b.mask == MOUSE_LEFTBUTTON) {
                        const uint64_t nowMs = timer_ms_gettime64();
                        if (input.lastLeftUpMs_ && nowMs - input.lastLeftUpMs_ < 400)
                            input.Emit(InputEventType::LeftDoubleClick, input.CursorX(), input.CursorY(), 0);
                    }
                } else if (!now && was) {
                    input.Emit(b.up, input.CursorX(), input.CursorY(), 0);
                    if (b.mask == MOUSE_LEFTBUTTON) input.lastLeftUpMs_ = timer_ms_gettime64();
                }
            }
            prev = cur;
        }
    }

    // Keyboard text events (printable ASCII only) - any maple port.
    maple_device_t* kbdDev = nullptr;
    for (int port = 0; port < 4 && !kbdDev; ++port)
        kbdDev = maple_enum_type(port, MAPLE_FUNC_KEYBOARD);
    if (kbdDev) {
        if (const kbd_state_t* st = static_cast<const kbd_state_t*>(maple_dev_status(kbdDev))) {
            for (int i = 0; i < KBD_MAX_PRESSED_KEYS; ++i) {
                const kbd_key_t key = st->cond.keys[i];
                if (key == KBD_KEY_NONE || key == KBD_KEY_ERROR) continue;
                bool seen = false;
                for (int j = 0; j < input.prevKeyCount_; ++j)
                    if (input.prevKeys_[j] == static_cast<uint8_t>(key)) { seen = true; break; }
                if (seen) continue;
                const kbd_region_t region = static_cast<kbd_region_t>(st->region);
                const char c = kbd_key_to_ascii(key, region, st->cond.modifiers, st->cond.leds);
                if (c >= 0x20)
                    input.Emit(InputEventType::Text, input.CursorX(), input.CursorY(), static_cast<uint32_t>(static_cast<unsigned char>(c)));
            }
            input.prevKeyCount_ = 0;
            for (int i = 0; i < KBD_MAX_PRESSED_KEYS && input.prevKeyCount_ < 6; ++i)
                if (st->cond.keys[i] != KBD_KEY_NONE) input.prevKeys_[input.prevKeyCount_++] = static_cast<uint8_t>(st->cond.keys[i]);
        }
    }
}

IInput& GetInput()
{
    static DCInput input;
    inputInstance = &input;
    return input;
}

} // namespace Platform
