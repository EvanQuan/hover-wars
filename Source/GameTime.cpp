#include "GameTime.h"

typedef std::chrono::high_resolution_clock Clock;

// Initialize first Tick
Time::Time()
{
    m_pLastTick = Clock::now();
}

// Nothing to Destruct
Time::~Time()
{

}

// Updates the duration since last update and returns the time between frames.
void Time::updateTime()
{
    time_point<steady_clock> pCurrTick = Clock::now();
    m_pFrameTime = pCurrTick - m_pLastTick;    // get new frame time.
    m_pLastTick = pCurrTick;    // Store new last tick
}
