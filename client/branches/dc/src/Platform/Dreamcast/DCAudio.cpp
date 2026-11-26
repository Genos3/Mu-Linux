// DCAudio.cpp - AICA/SFX backend for the DC build (implements Platform::IAudio).
//
// Strategy (simple and honest):
//   * SFX: KOS sfxmgr loads the game's 8/16-bit PCM WAVs (<= 65534 samples per
//     sfxmgr's documented limit; longer files fail CreateBuffer and the caller
//     treats them as not loaded, same as a missing file on PC).
//   * Music: stubbed at this layer on purpose - the AICA streaming path
//     (dc/sound/stream.h + libtremor/libmp3) belongs to the music milestone;
//     CSound already no-ops it on DC (SoundManager.cpp).
//   * 3D: x/z map to AICA pan (0..255); listener height/fog are ignored.
//   * Volume: IAudio takes hundredths of a dB (<=0); mapped to 0..255 linear-ish
//     via 10^(dB/20) * 255.
//   * Buffers are cached per (slot, path) inside sfxmgr; handles are shared.

#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/IAudio.h"
#include <dc/sound/sound.h>
#include <strings.h>
#include <algorithm>
#include <cmath>
#include <cstring>

namespace Platform {
namespace {
class DCAudio final : public IAudio {
    sfxhnd_t handles_[420] = {};   // 0 == SFXHND_INVALID
    int channels_[420] = {};       // channels requested per slot
    int volume_[420][4] = {};      // stored hundredths of dB per slot/channel
    int pan_[420][4] = {};         // stored pan 0..255 per slot/channel (128 = center)
    int slotChannel_[420] = {};    // channel passed to the last Play (for Stop)
    bool spatial_[420] = {};
public:
    DCAudio() { std::memset(volume_, 0, sizeof(volume_)); std::memset(pan_, 0, sizeof(pan_)); }

    int32_t Initialize(void* window) override
    {
        (void)window;   // AICA is up via KOS_INIT_FLAGS(INIT_DEFAULT), but the
                        // sfxmgr/sound-memory layer needs an explicit snd_init()
                        // before any snd_sfx_load(), or KOS asserts in snd_mem.c.
        int result = snd_init();
        if (result != 0) {
            DCLog("[DC] DCAudio: snd_init failed (%d)", result);
            return -1;
        }
        DCLog("[DC] DCAudio: AICA ready (sfxmgr)");
        return 0;
    }

    int32_t CreateBuffer(int slot, const char* path, int channels, bool spatial) override
    {
        if (slot < 0 || slot >= 420 || !path) return -1;
        // KOS sfxmgr uses plain fopen(): it does NOT go through
        // Platform::GetFileSystem(), so backslashes and the /cd mount point
        // have to be normalized here (same folding as DCFileSystem::Fold).
        char loadPath[1024];
        ::snprintf(loadPath, sizeof(loadPath), "%s", path);
        for (char& c : loadPath) if (c == '\\') c = '/';
        if (loadPath[0] != '/') {
            char absolute[1024];
            ::snprintf(absolute, sizeof(absolute), "/cd/%.1019s", loadPath);
            ::snprintf(loadPath, sizeof(loadPath), "%s", absolute);
        }
        // Prefer the offline-converted 4-bit ADPCM WAV: sfxmgr supports it,
        // and it is far smaller than the original PCM. Fall back to the
        // original path while a given asset has not been converted yet.
        sfxhnd_t handle = SFXHND_INVALID;
        const std::size_t len = std::strlen(loadPath);
        if (len > 4 && ::strcasecmp(loadPath + len - 4, ".wav") == 0) {
            char adpcm[1024 + 8];
            std::memcpy(adpcm, loadPath, len - 4);
            ::strcpy(adpcm + len - 4, ".adpcm.wav");
            handle = snd_sfx_load(adpcm);
        }
        if (handle == SFXHND_INVALID)
            handle = snd_sfx_load(loadPath);
        if (handle == SFXHND_INVALID) {
            std::string lower(loadPath);
            for (char& c : lower) if (c >= 'A' && c <= 'Z') c += 32;
            ::snprintf(loadPath, sizeof(loadPath), "%s", lower.c_str());
            handle = snd_sfx_load(loadPath);
        }
        if (handle == SFXHND_INVALID) {
            DCLog("[DC] DCAudio: no se pudo cargar '%s' (!= 65534 muestras o WAV?)", path);
            return -2;
        }
        handles_[slot] = handle;
        channels_[slot] = channels;
        spatial_[slot] = spatial;
        return 0;
    }

    int32_t Play(int slot, uint32_t channel, bool loop) override
    {
        if (slot < 0 || slot >= 420 || handles_[slot] == SFXHND_INVALID) return -1;
        const int ch = channel < 32 ? static_cast<int>(channel) : 0;
        slotChannel_[slot] = ch;
        const int vol = VolumeAt(slot, ch);
        const int pan = pan_[slot][ch & 3];
        const int played = snd_sfx_play_chn(ch, handles_[slot], vol, pan);
        (void)loop;   // sfxmgr: loop is a play flag; sfx slots here are one-shot
        return played >= 0 ? 0 : -3;
    }

    void Stop(int slot) override
    {
        if (slot < 0 || slot >= 420) return;
        snd_sfx_stop(slotChannel_[slot]);
    }

    int32_t ReleaseBuffer(int slot) override
    {
        if (slot < 0 || slot >= 420) return -1;
        if (handles_[slot] != SFXHND_INVALID) {
            snd_sfx_unload(handles_[slot]);
            handles_[slot] = SFXHND_INVALID;
        }
        return 0;
    }

    void SetVolume(int slot, int channel, int32_t hundredthsDb) override
    {
        if (slot < 0 || slot >= 420 || channel < 0 || channel >= 4) return;
        volume_[slot][channel] = hundredthsDb;
    }

    void SetPosition(int slot, int channel, float x, float y, float z) override
    {
        (void)y; (void)z;   // listener is a point; only lateral pan is meaningful on AICA
        if (slot < 0 || slot >= 420 || channel < 0 || channel >= 4) return;
        // +x to the right -> pan right. Distance attenuation is done by the
        // caller through SetVolume; keep it simple here.
        const float p = 128.0f + x * 64.0f;
        pan_[slot][channel] = static_cast<int>(p < 0.0f ? 0.0f : (p > 255.0f ? 255.0f : p));
    }

    void Shutdown() override { snd_sfx_unload_all(); }

private:
    int VolumeAt(int slot, int channel) const
    {
        const int hundredths = volume_[slot][channel & 3];
        if (hundredths <= -10000) return 0;             // muted
        const double gain = std::pow(10.0, hundredths / 2000.0);   // 1/100 dB -> ratio (20*log10)
        const int vol = static_cast<int>(gain * 255.0);
        return std::clamp(vol, 0, 255);
    }
};
} // namespace

IAudio& GetAudio()
{
    static DCAudio audio;
    return audio;
}
} // namespace Platform
