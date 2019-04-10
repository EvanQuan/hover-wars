#pragma once
#include <chrono>

using namespace std::chrono;

/*
Typedef for 1/60th of a second: (60 fps)
This is used to determine the frame rate for rendering updates.
*/
typedef duration<long, std::ratio<1, 60>> sixtieth_of_a_sec;

/***********************************************************
 * GameTime Class: Maintains the Time for the game logic.
 * Written by: James Cote, Evan Quan
\************************************************************/
class GameTime final
{
public:

    // Constructor and Destructor
    GameTime();
    ~GameTime();

    /*
    Get the difference in time between the last updateTimeSinceLastFrame()
    call and the second last updateTimeSinceLastFrame() call. If updateTIme()
    has only been called once, compare it to when this GameTime object has been
    instantiated.

    @return time in seconds
    */
    duration<double> getFrameTimeSinceLastFrame() const { return m_pFrameTimeSinceLastFrame;  }

private:
    // Time Recording Variables
    /*
    The time for the current frame since last update. This is used to determine
    the real world time betweeen frames as well as for ability cool downs.
    */
    time_point<steady_clock> m_pCurrentTick;
    /*
    Last frame's m_pCurrentTick value. This is used to determine the real
    world time between frames.
    */
    time_point<steady_clock> m_pLastTick;
    /*
    The difference in time between this frame and the last.
    */
    duration<double> m_pFrameTimeSinceLastFrame;

    /*
    Private update function for Graphics Manager to call.
    */
    void updateTimeSinceLastFrame();

    void resetTimer();

    /*
    Only allows Graphics Manager to update the timer.
    */
    friend class GameManager;
};
