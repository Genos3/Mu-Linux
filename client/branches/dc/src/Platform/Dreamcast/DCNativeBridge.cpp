// DCNativeBridge.cpp - implements Platform/NativeBridge.h for KOS.
//
// Same job as SDLNativeBridge's Linux branch: route Platform events
// (Input_OnEvent) and socket readiness (Client_SocketEvent) into the game
// through the shared ISocket/IInput contracts. No HWND/HINSTANCE exist on DC;
// the returned sentinels only need to satisfy non-null checks downstream.

#include "Platform/NativeBridge.h"
#include "Platform/ISocket.h"
#include "Platform/Dreamcast/DCPlatform.h"

void Input_OnEvent(const Platform::InputEvent& event);
void Client_ClosePortable();
void Client_SocketEvent(uint16_t event, uint16_t error);

namespace Platform {
void BindNativeDispatch(HINSTANCE, WNDPROC)
{
    DCInputBind(Input_OnEvent, Client_ClosePortable);
    GetSocket().BindEvents(Client_SocketEvent);
}
// Identidades opacas: nunca se pasan a GDI (no existe en DC).
HWND NativeWindow() { return reinterpret_cast<HWND>(static_cast<uintptr_t>(0xDC1000FF)); }
HINSTANCE NativeInstance() { return nullptr; }
HDC NativeDC() { return reinterpret_cast<HDC>(static_cast<uintptr_t>(0xDC1000FF)); }
HGLRC NativeGLContext() { return nullptr; }
}
