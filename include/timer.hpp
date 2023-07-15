#pragma once

#include <3ds.h>

namespace love
{
    class Timer
    {
      public:
        Timer();

        double Step();

        void Sleep(double seconds) const;

        static double GetTime();

        static Timer& Instance()
        {
            static Timer instance;
            return instance;
        }

        double GetDelta() const
        {
            return this->delta;
        }

        double GetAverageDelta() const
        {
            return this->averageDelta;
        }

        int GetFPS() const
        {
            return this->fps;
        }

      private:
        double currentTime;
        double previousFpsUpdate;

        double previousTime;

        int fps;
        double averageDelta;
        double fpsUpdateFrequency;

        int frames;
        double delta;

        static TickCounter counter;
    };
} // namespace love