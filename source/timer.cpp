#include "timer.hpp"

#include <chrono>

using namespace love;

TickCounter Timer::counter {};

Timer::Timer()
{
    osTickCounterStart(&counter);
    this->previousFpsUpdate = this->currentTime = Timer::GetTime();
}

double Timer::GetTime()
{
    counter.elapsed = svcGetSystemTick() - counter.reference;
    return osTickCounterRead(&counter) / 1000.0;
}

void Timer::Sleep(double seconds) const
{
    if (seconds >= 0)
    {
        auto time = std::chrono::duration<double>(seconds);
        svcSleepThread(std::chrono::duration<double, std::nano>(time).count());
    }
}

double Timer::Step()
{
    this->frames++;

    this->previousTime = this->currentTime;
    this->currentTime  = Timer::GetTime();

    this->delta = this->currentTime - this->previousTime;

    if (this->delta < 0)
        this->delta = 0;

    double timeSinceLast = (this->currentTime - this->previousFpsUpdate);

    if (timeSinceLast > this->fpsUpdateFrequency)
    {
        this->fps               = int((this->frames / timeSinceLast) + 0.5);
        this->averageDelta      = timeSinceLast / frames;
        this->previousFpsUpdate = this->currentTime;
        this->frames            = 0;
    }

    return this->delta;
}
