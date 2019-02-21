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

    /*
    @return the difference in time between the last updateTime() call
            and the second last updateTime() call. If updateTIme() has
            only been called once, compare it to when this Time object
            has been instantiated.

            Unit: seconds
    */
    duration<double> getFrameTime() const { return m_pFrameTime;  }
    void resetTimer();

private:
    // Time Recording Variables
    time_point<steady_clock> m_pLastTick;
    duration<double> m_pFrameTime; 

    // Private update function for Graphics Manager to call.
    void updateTime();

    // Only allows Graphics Manager to update the timer.
    friend class GameManager;
};

