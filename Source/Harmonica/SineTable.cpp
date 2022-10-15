/*
  ==============================================================================

    SineTable.cpp
    Created: 13 Oct 2022 8:29:43pm
    Author:  pwmag

  ==============================================================================
*/

#include "SineTable.h"

namespace WDYM {
    SineTable::SineTable() {
        // generate a sine wavetable
        for (int i = 0; i < wavetableSize; i++) {
            wavetable[i] = sin((juce::MathConstants<float>::twoPi * i) / (float)(wavetableSize - 1));
        }
    }

    SineTable::~SineTable() {

    }

    SineTable::SIMDFloat SineTable::getSample(SIMDFloat angle) {
        // Turn phase angles into valid wavetable indeces
        auto simdIndex0 = simdFmod(SIMDOp::truncate(angle.value), wavetable.size());
        auto simdIndex1 = simdFmod(SIMDOp::truncate(SIMDOp::add(angle.value, ((SIMDFloat)1).value)), wavetable.size());

        
        SIMDFloat v0 = 0, v1 = 0;
        
        // Run through each entry in a register
        for (size_t i = 0; i < SIMDFloat::size(); i++) {
            // Load wavetable index
            auto wtIndex0 = SIMDOp::get(simdIndex0.value, i);
            auto wtIndex1 = SIMDOp::get(simdIndex1.value, i);

            // Store wavetable entry into SIMD register
            v0.set(i, wavetable[wtIndex0]);
            v1.set(i, wavetable[wtIndex1]);
        }

        // use linterp instead of hermite for more speediness
        auto offset = angle - SIMDFloat::truncate(angle.value);
        auto slope = (v1 - v0) * 0.5f;
        return v0 + (offset * slope);
    }
}