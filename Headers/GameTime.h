#pragma once
#include <chrono>

using namespace std::chrono;

// Typedef for 1/60th of a second: (60 fps)
typedef duration<long, std::ratio<1, 60>> sixtieths_of_a_sec;

/***********************************************************
 * Time Class: Maintains the Time for the game logic.
 * Written by: James Cote
\************************************************************/
class Time
{
public:
    // Constructor and Destructor
    Time();
    ~Time();

    // Gets the current FrameTime
    duration<double> getFrameTime() const { return m_pFrameTime;  }

private:
    // Time Recording Variables
    time_point<steady_clock> m_pLastTick;
    duration<double> m_pFrameTime; 

    // Private update function for Graphics Manager to call.
    void updateTime();

    // Only allows Graphics Manager to update the timer.
    friend class GameManager;
};

