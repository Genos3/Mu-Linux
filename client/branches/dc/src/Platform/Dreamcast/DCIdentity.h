#pragma once
#include "Platform/IIdentity.h"
namespace Platform {
// HWID from the BroadBand Adapter MAC, persisted on VMU so it survives reboots.
class DCIdentity final : public IIdentity {
public:
    DCIdentity() = default;
    DCIdentity(const char* vmuPath) : vmuPath_(vmuPath) {}
    bool Read(char (&output)[36], const char*& error) override;
private:
    const char* vmuPath_ = "/vmu/a1/mu_hwid.bin";   // figure out exact vmu path at integration time
};
} // namespace Platform
