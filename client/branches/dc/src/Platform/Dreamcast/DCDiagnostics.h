#pragma once
#include "Platform/IDiagnostics.h"
#include <cstdio>
#include <cstdint>
namespace Platform {
// Console-backed diagnostics for the KOS build.
// No signal-handler/backtrace plumbing (KOS has no sigaltstack); a crash shows
// in the emulator/console. debug.log is attempted in the CWD (writable roots
// only - VMU/SD), failures are non-fatal.
class DCDiagnostics final : public IDiagnostics {
    std::FILE* logFile_ = nullptr;
public:
    ~DCDiagnostics() override { if (logFile_) std::fclose(logFile_); }
    std::int64_t MeasureClockRate(std::uint32_t) override { return 0; }   // not used on this path
    bool Install(DiagnosticLog log) override
    {
        if (logFile_) return true;
        if (std::FILE* file = std::fopen("debug.log", "a")) logFile_ = file;
        if (log) log("[DC] diagnostics installed (console + debug.log best-effort)");
        return true;
    }
};
} // namespace Platform
