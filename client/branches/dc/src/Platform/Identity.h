#pragma once
#include "Platform/IIdentity.h"
#ifdef _WIN32
#include "Platform/Win32/Win32Identity.h"
#elif defined(__DREAMCAST__)
#include "Platform/Dreamcast/DCIdentity.h"
#else
#include "Platform/Posix/PosixIdentity.h"
#endif
namespace Platform {
inline IIdentity& GetIdentity() {
#ifdef _WIN32
    static Win32Identity identity;
#elif defined(__DREAMCAST__)
    static DCIdentity identity;
#else
    static PosixIdentity identity;
#endif
    return identity;
}
}
