// ----------------------------------------------------
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "Timer.h"


// L1: DONE 1: Fill Start(), Read(), ReadSec() methods
// they are simple, one line each!

Timer::Timer() : started(false) // Inicializa el indicador started
{
    Start();
}

void Timer::Start(uint32 startingMS)
{
    started = true;
    startTime = std::chrono::high_resolution_clock::now();
}

void Timer::Stop()
{
    started = false;
}

float Timer::Read() const
{
    return started ? std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - startTime).count() : 0;
}

uint32 Timer::CountDown(int total) const
{
    if (started) {
        total = total - std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - startTime).count();
        if (total <= 0) {
            total = 0;
        }
        return total;
    }
    else {
        return total;
    }
}

bool Timer::IsStarted() const
{
    return started;
}