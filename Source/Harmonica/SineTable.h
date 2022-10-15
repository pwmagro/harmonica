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
        using SIMDFloat = juce::dsp::SIMDRegister<float>;
        using SIMDOp = juce::dsp::SIMDNativeOps<float>;
        SineTable();
        ~SineTable();

        unsigned int getWidth() { return wavetableSize; }
        juce::dsp::SIMDRegister<float> getSample(juce::dsp::SIMDRegister<float> angle);
        
        static SIMDFloat simdFmod(SIMDFloat x, SIMDFloat y) { return x - SIMDOp::mul(SIMDOp::truncate(_mm_div_ps(x.value, y.value)), y.value); };


    private:
        static constexpr int wavetableSize = 4096u;
        std::array<float, wavetableSize> wavetable;
    };
}