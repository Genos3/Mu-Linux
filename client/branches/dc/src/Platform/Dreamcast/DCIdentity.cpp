// DCIdentity.cpp - 35-char hardware ID for the DC build.
//
// Strategy:
//   1. If a valid HWID was persisted on the VMU (32 hex + 4 dashes layout),
//      reuse it - stable across reboots even if the BBA changes MAC.
//   2. Otherwise derive 128 bits from the BBA MAC (bba_get_mac) folded with a
//      constant tag, hash-mix them (FNV-1a x4), persist to the VMU, and return.
//   3. If the VMU write fails (no card / full / wrong path), still return the
//      derived HWID and log a warning; HWID only feeds the server-side
//      blacklist check, it does not pair with saves.
//
// Persisted file layout: exactly 32 lowercase hex bytes (no dashes), matching
// the Posix backend's validation routine.

#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/Dreamcast/DCIdentity.h"
#include <cstdio>
#include <cstring>
#include <cstdint>

namespace Platform {
namespace {
constexpr uint64_t kTag = 0x4d554f4e494c4531ULL;   // "MUONILE1"

uint64_t fnv1a(const uint8_t* data, size_t size, uint64_t seed)
{
    uint64_t hash = seed ? seed : 1469598103934665603ull;
    for (size_t i = 0; i < size; ++i) { hash ^= data[i]; hash *= 1099511628211ull; }
    return hash;
}

void hex8(uint64_t value, char* out)
{
    for (int i = 0; i < 8; ++i) {
        out[i] = static_cast<char>((value & 0xfull) < 10 ? '0' + (value & 0xfull)
                                                         : 'a' + ((value & 0xfull) - 10));
        value >>= 4;
    }
}

bool validHex32(const char* bytes)
{
    for (int i = 0; i < 32; ++i) {
        char c = bytes[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
    }
    bool nonzero = false;
    for (int i = 0; i < 32; ++i) nonzero |= bytes[i] != '0';
    return nonzero;
}

bool readFromVmu(const char* path, char (&output)[36])
{
    std::FILE* file = std::fopen(path, "rb");
    if (!file) return false;
    char raw[64] = {};
    size_t count = std::fread(raw, 1, sizeof(raw) - 1, file);
    std::fclose(file);
    if (count != 32 || !validHex32(raw)) return false;
    int position = 0;
    for (int i = 0; i < 32; ++i) {
        if (i && i % 8 == 0) output[position++] = '-';
        output[position++] = raw[i];
    }
    output[position] = 0;
    return true;
}

bool writeToVmu(const char* path, const char (&hex32)[32])
{
    std::FILE* file = std::fopen(path, "wb");
    if (!file) return false;
    size_t written = std::fwrite(hex32, 1, 32, file);
    std::fclose(file);
    return written == 32;
}
} // namespace

bool DCIdentity::Read(char (&output)[36], const char*& error)
{
    output[0] = 0; error = nullptr;
    if (readFromVmu(vmuPath_, output)) return true;

    uint8_t mac[6] = {};
    bba_get_mac(mac);
    // Mix MAC + tag twice for 128 bits of stable material.
    uint64_t h1 = fnv1a(mac, sizeof(mac), kTag);
    uint64_t h2 = fnv1a(mac, sizeof(mac), fnv1a(mac, sizeof(mac), kTag ^ 0xffffffffULL));
    char hex32[32];
    hex8(h1, hex32);
    hex8(h2, hex32 + 8);
    hex8(h1 ^ h2, hex32 + 16);
    hex8(h1 + h2, hex32 + 24);

    if (!writeToVmu(vmuPath_, hex32))
        DCLog("HWID: no se pudo persistir en VMU (%s); usando HWID derivado de la MAC", vmuPath_);

    int position = 0;
    for (int i = 0; i < 32; ++i) {
        if (i && i % 8 == 0) output[position++] = '-';
        output[position++] = hex32[i];
    }
    output[position] = 0;
    return true;
}
} // namespace Platform
