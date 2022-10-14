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

    float SineTable::getSample(float angle) {
        auto v0 = wavetable[((int)angle + 0) % wavetable.size()];
        auto v1 = wavetable[((int)angle + 1) % wavetable.size()];
        auto v2 = wavetable[((int)angle + 2) % wavetable.size()];
        auto v3 = wavetable[((int)angle + 3) % wavetable.size()];
        auto offset = angle - floor(angle);


        float slope0 = (v2 - v0) * 0.5f;
        float slope1 = (v3 - v1) * 0.5f;
        float v = v1 - v2;
        float w = slope0 + v;
        float a = w + v + slope1;
        float b_neg = w + a;
        float stage1 = a * offset - b_neg;
        float stage2 = stage1 * offset + slope0;
        return stage2 * offset + v1;
    }
}