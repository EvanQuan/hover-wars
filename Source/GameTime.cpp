#include "GameTime.h"

/************\
 * Typedefs *
\************/
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

The Graphics Manager calls this every frame update to determine the real world
time difference between this frame update and the last frame update in order to
update the in game environment correctly.
*/
void Time::updateTime()
{
    time_point<steady_clock> pCurrTick = Clock::now();
    m_pFrameTime = pCurrTick - m_pLastTick;    // get new frame time.
    m_pLastTick = pCurrTick;                   // Store new last tick
}

/*
The timer resets its last tick to now.

The Graphics Manager calls this once all the graphics have been initialized to
freshly start the time from scratch.
*/
void Time::resetTimer()
{
    m_pLastTick = Clock::now();
}
