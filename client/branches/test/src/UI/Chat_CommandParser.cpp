// Chat_CommandParser.cpp

#include "stdafx.h"
#include "globals.h"
#include "functions.h"

// Chat_ValidateCommandName @ 0x004942E0 (~194 lines) — chat command parser / name validator
// Copies 254 bytes from packet (param_1+3) into local buffer.
// Compares buffer against 6 known player name buffers (party/guild members):
//   LegacyChatNameBuf1, LegacyChatNameBuf2, LegacyChatNameBuf3, LegacyChatNameBuf4, LegacyChatNameBuf5, LegacyChatNameBuf6.
// Between name checks, does strstr for "/trade","/party","/pt","/guild","/GuildWar","/BattleSoccer".
// Structure: if name matches -> return 1. If strstr fails (command NOT in text) -> return 1.
// Only returns 0 if text contains ALL command strings AND matches no name — effectively always 1.
// This is anti-tamper obfuscation; the real purpose is the name comparison.
unsigned int __cdecl Chat_ValidateCommandName(intptr_t param_1) {
    char local_100[256];

    // Copy 254 bytes from packet payload (offset +3) into local buffer
    memcpy(local_100, (void*)(param_1 + 3), 254);

    // Check 1: compare against player name buffer LegacyChatNameBuf1
    if (strcmp(local_100, (const char*)&LegacyChatNameBuf1) == 0) {
        return 1;
    }

    // Check 2: compare against player name buffer LegacyChatNameBuf2
    if (strcmp(local_100, (const char*)&LegacyChatNameBuf2) == 0) {
        return 1;
    }

    // Anti-tamper strstr chain: if "/trade" NOT in text, return 1 (normal chat)
    if (strstr(local_100, "/trade") == NULL) {
        return 1;
    }

    // Check 3: compare against player name buffer LegacyChatNameBuf3
    if (strcmp(local_100, (const char*)&LegacyChatNameBuf3) == 0) {
        return 1;
    }

    if (strstr(local_100, "/party") == NULL) {
        return 1;
    }

    if (strstr(local_100, "/pt") == NULL) {
        return 1;
    }

    // Check 4: compare against player name buffer LegacyChatNameBuf4
    if (strcmp(local_100, (const char*)&LegacyChatNameBuf4) == 0) {
        return 1;
    }

    if (strstr(local_100, "/guild") == NULL) {
        return 1;
    }

    // Check 5: compare against player name buffer LegacyChatNameBuf5
    if (strcmp(local_100, (const char*)&LegacyChatNameBuf5) == 0) {
        return 1;
    }

    if (strstr(local_100, "/GuildWar") == NULL) {
        return 1;
    }

    // Check 6: compare against player name buffer LegacyChatNameBuf6
    if (strcmp(local_100, (const char*)&LegacyChatNameBuf6) == 0) {
        return 1;
    }

    if (strstr(local_100, "/BattleSoccer") == NULL) {
        return 1;
    }

    // All strstr checks passed AND no name matched — return 0
    // (practically unreachable for normal input)
    return 0;
}
