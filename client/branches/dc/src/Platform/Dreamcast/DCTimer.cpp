// DCTimer.cpp - high resolution timer + wall clock for the DC backend.
//
// QueryPerformanceCounter semantics are preserved: CounterFrequency() == 1 MHz,
// ReadCounter() returns microseconds since boot (via KOS's nanosecond math).

#include "Platform/Dreamcast/DCPlatform.h"
#include "Platform/ITimer.h"
#include <ctime>
#include <cmath>

namespace Platform {
namespace {
class DCTimer final : public ITimer {
public:
    int64_t CounterFrequency() override { return 1000000; }   // microseconds

    bool ReadCounter(int64_t& counter) override
    {
        counter = static_cast<int64_t>(timer_us_gettime64());
        return true;
    }

    LocalDateTime LocalTime() override
    {
        std::time_t now = std::time(nullptr);
        std::tm tmNow{};
#if defined(_WIN32)
        localtime_s(&tmNow, &now);
#else
        tmNow = *std::localtime(&now);   // KOS/newlib: single-threaded enough here
#endif
        LocalDateTime result{};
        result.wYear = static_cast<uint16_t>(tmNow.tm_year + 1900);
        result.wMonth = static_cast<uint16_t>(tmNow.tm_mon + 1);
        result.wDayOfWeek = static_cast<uint16_t>(tmNow.tm_wday);
        result.wDay = static_cast<uint16_t>(tmNow.tm_mday);
        result.wHour = static_cast<uint16_t>(tmNow.tm_hour);
        result.wMinute = static_cast<uint16_t>(tmNow.tm_min);
        result.wSecond = static_cast<uint16_t>(tmNow.tm_sec);
        result.wMilliseconds = static_cast<uint16_t>(::timer_ms_gettime64() % 1000);
        return result;
    }

    uint32_t Ticks() override { return static_cast<uint32_t>(timer_ms_gettime64()); }
    uint32_t MultimediaTicks() override { return Ticks(); }

    void SleepMilliseconds(uint32_t milliseconds) override
    {
        thd_sleep(milliseconds);
    }

    TimerStart Start() override
    {
        TimerStart start{};
        ReadCounter(start.counter);
        start.ticks = Ticks();
        return start;
    }

    double Wait(const TimerStart& start, double minimumMilliseconds) override
    {
        const uint64_t now = timer_us_gettime64();
        const double elapsed = static_cast<double>(
            now - static_cast<uint64_t>(start.counter < 0 ? 0 : start.counter)) / 1000.0;
        if (elapsed < minimumMilliseconds)
            SleepMilliseconds(static_cast<uint32_t>(minimumMilliseconds - elapsed));
        const uint64_t after = timer_us_gettime64();
        return static_cast<double>(
            after - static_cast<uint64_t>(start.counter < 0 ? 0 : start.counter)) / 1000.0;
    }

    void Shutdown() override {}
};
} // namespace

ITimer& GetTimer()
{
    static DCTimer timer;
    return timer;
}
} // namespace Platform
