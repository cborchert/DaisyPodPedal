// LEARNING: Only included once
#pragma once

#include "daisy_pod.h" // we need System
#include <cstdint>     // we need uint32_t

using namespace daisy;
// LEARNING: you can bring in specific names from a namespace rather than the whole thing this can help avoid name collisions
using std::uint32_t;

class Timer
{
    // LEARNING: member variables (private by default)
    uint32_t interval_ms;
    uint32_t last_time_ms;

    // LEARNING: public members
public:
    // LEARNING: constructor with parameter; the initializer list sets member variables before executing the body
    //           note that we could have initialized the member variables inside the body, but it's less efficient
    Timer(uint32_t interval) : interval_ms(interval), last_time_ms(0) {}

    // Returns true if the timer has reached the interval
    bool Ready()
    {
        uint32_t now_ms = System::GetNow();
        uint32_t elapsed_ms = now_ms - last_time_ms;
        if (elapsed_ms < interval_ms)
        {
            return false;
        }

        last_time_ms = now_ms;
        return true;
    }
}; // LEARNING: semicolon required after class / struct definition (BUT NOT AFTER functions!)
