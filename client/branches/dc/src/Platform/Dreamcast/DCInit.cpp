// DCInit.cpp - startup flags and the backend log funnel.
//
// Exactly one TU must define KOS_INIT_FLAGS; keep it out of ClientEntry.cpp so
// that file stays byte-identical with the other backends.
//   INIT_DEFAULT: video + sound (AICA) + timer + maple
//   INIT_NET:     native TCP/IP stack (BBA / LAN / W5500 modem)
// If the network hardware is not required for a given build, drop INIT_NET and
// the client stays offline (Net_Connect fails cleanly, login screen still runs).

#include "Platform/Dreamcast/DCPlatform.h"
#include <kos/irq.h>

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_NET);

// Log the faulting instruction before KOS reboots. Layout-sensitive crashes
// otherwise look like a silent reset with no clue where they happened.
static void DCExceptionHandler(irq_t code, irq_context_t *ctx, void *) {
    const unsigned c = static_cast<unsigned>(code);
    // POST/IRQ codes are normal interrupts (e.g. TMU0 timer) - let KOS handle.
    const bool fatal = (c >= 0x0040 && c <= 0x01a0) || c == 0x0780 || c == 0x07e0;
    if (!fatal) return;
    std::printf("[MU] EXCEPTION code=%04x pc=%08x pr=%08x sp=%08x\n",
                c, CONTEXT_PC(*ctx), ctx->pr, CONTEXT_SP(*ctx));
    for (;;) { __asm__ volatile("nop"); }   // freeze so the log can be read
}
static const bool s_exceptionHandlerInstalled = [] {
    irq_set_global_handler(DCExceptionHandler, nullptr);
    return true;
}();

namespace Platform {
void DCLog(const char* fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    ::vsnprintf(buffer, sizeof(buffer), fmt, args);   // newlib: ::vsnprintf no existe aquí
    va_end(args);
    // Single channel with an [MU] tag so host-side tooling can split game
    // messages out of Flycast's serial log into game.log. No dbglog duplicate.
    std::printf("[MU] %s\n", buffer);
}
}
