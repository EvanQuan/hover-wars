#include "GameTime.h"

// Initialize Static Instance Variable
Time* Time::m_pInstance = nullptr;

/************\
 * Typedefs *
\************/
typedef std::chrono::high_resolution_clock Clock;

Time* Time::getInstance()
{
    if (nullptr == m_pInstance)
    {
        m_pInstance = new Time();
    }
    return m_pInstance;

}
/*
Initialize first tick as soon as the object is created.
*/
Time::Time()
{
    resetTimer();
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

No other class should call this method in order for all time values to be in sync
for every frame.
*/
void Time::updateTimeSinceLastFrame()
{
    m_pCurrentTick = Clock::now();
    m_pFrameTimeSinceLastFrame = m_pCurrentTick - m_pLastTick;    // get new frame time.
    m_pLastTick = m_pCurrentTick;                                 // Store new last tick
}

/*
The timer resets its start time and last tick time to now.

The Graphics Manager calls this once all the graphics have been initialized to
freshly start the time from scratch.
*/
void Time::resetTimer()
{
    m_pStartTick = Clock::now();
    m_pCurrentTick = m_pStartTick;
    m_pLastTick = m_pStartTick;
}
