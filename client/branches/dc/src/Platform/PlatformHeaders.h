#pragma once
#include "Platform/CompilerCompat.h"

// Único selector de cabeceras nativas para el código compartido.
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif
#elif defined(__DREAMCAST__)
// KOS: ningún header de Win32/DC vive en el camino compartido; tipos LP32 vía shim.
#include <kos.h>
#include "Platform/Posix/WinTypes.h"
#else
#include "Platform/Posix/WinTypes.h"
#endif
