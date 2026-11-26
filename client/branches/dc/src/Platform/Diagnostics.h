#pragma once
#include "Platform/IDiagnostics.h"
#ifdef _WIN32
#include "Platform/Win32/Win32Diagnostics.h"
#elif defined(__DREAMCAST__)
#include "Platform/Dreamcast/DCDiagnostics.h"
#else
#include "Platform/Posix/PosixDiagnostics.h"
#endif
namespace Platform {
inline IDiagnostics& GetDiagnostics() {
#ifdef _WIN32
    static Win32Diagnostics diagnostics;
#elif defined(__DREAMCAST__)
    static DCDiagnostics diagnostics;
#else
    static PosixDiagnostics diagnostics;
#endif
    return diagnostics;
}
}
