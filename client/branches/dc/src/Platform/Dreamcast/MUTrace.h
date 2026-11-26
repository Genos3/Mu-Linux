#pragma once
// Real-time serial milestones for DC (DCLog -> stdout -> Flycast serial log).
#ifdef __DREAMCAST__
#include "Platform/Dreamcast/DCPlatform.h"
#define MU_TRACE(...) Platform::DCLog(__VA_ARGS__)
#else
#define MU_TRACE(...) ((void)0)
#endif
