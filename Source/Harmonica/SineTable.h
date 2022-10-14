/*
  ==============================================================================

    SineTable.h
    Created: 13 Oct 2022 8:29:43pm
    Author:  pwmag

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#define NYQUIST_FR 200000.f

namespace WDYM {
    class SineTable {
    public:
        SineTable();
        ~SineTable();

        unsigned int getWidth() { return wavetableSize; }
        float getSample(float angle);

    private:
        static constexpr int wavetableSize = 4096u;
        std::array<float, wavetableSize> wavetable;
    };
}