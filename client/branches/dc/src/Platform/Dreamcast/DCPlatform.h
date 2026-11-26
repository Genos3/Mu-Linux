#pragma once
// KallistiOS umbrella header for the Dreamcast platform backend.
//
// Only translation units that are part of the KOS build may include this.
// Shared code never sees maple/pvr types: it still goes through the
// Platform::I* interfaces (IWindow, ISocket, IInput, ...) and the Win32
// type shim in Platform/Posix/WinTypes.h (reused; it is already LP32).

#if !defined(__DREAMCAST__)
#error "Platform/Dreamcast is only for KOS builds. Note: -D__DREAMCAST__ comes from the KOS toolchain environment."
#endif

#include <kos.h>          // arch: pulls in dc/pvr.h, dc/maple.h, timer, ...
#include <kos/thread.h>   // thd_sleep
#include <dc/net/broadband_adapter.h>  // bba_get_mac()

#include "Platform/Posix/WinTypes.h"   // BOOL/BYTE/WORD/..., opaque HWND/HDC, LP32 asserts
#include "Platform/IInput.h"   // InputEvent (pump de eventos DC)
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <cstdint>

namespace Platform {
// Single log path for the backend: printf (DC serial/gdb-stub/emulator console)
// and dbglog in debug builds.
void DCLog(const char* fmt, ...);
// Input pump (DCInput.cpp): called once per frame from DCWindow::ServiceEvents.
// Binds the game's event sinks and converts maple state into InputEvents.
void DCInputBind(void (*inputSink)(const InputEvent&), void (*closeSink)());
void DCInputPump();
}

// Mark an unimplemented hook so the first boot attempt fails loudly in the log
// instead of silently returning 0/false.
#define DC_NYI(what) do { Platform::DCLog("[DC] NYI: %s (%s:%d)", (what), __FILE__, __LINE__); } while (0)
