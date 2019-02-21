#include "GameTime.h"

typedef std::chrono::high_resolution_clock Clock;

/*
Initialize first tick as soon as the object is created.
*/
Time::Time()
{
    m_pLastTick = Clock::now();
}

/*
Nothing to Destruct
*/
Time::~Time()
{

}

/*
Updates the duration since last update.
*/
void Time::updateTime()
{
    time_point<steady_clock> pCurrTick = Clock::now();
    m_pFrameTime = pCurrTick - m_pLastTick;    // get new frame time.
    m_pLastTick = pCurrTick;    // Store new last tick
}

/*
The timer resets its last tick to now.
*/
void Time::resetTimer()
{
    m_pLastTick = Clock::now();
}
