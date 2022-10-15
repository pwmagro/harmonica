/*
  ==============================================================================

    Voice.cpp
    Created: 13 Oct 2022 8:29:57pm
    Author:  pwmag

  ==============================================================================
*/

#include "Voice.h"

namespace WDYM {
    Voice::Voice(SineTable& st, juce::ThreadPool& tp) : sineTable(st), threadPool(tp) {
        enabled = false;
    }

    Voice::~Voice() {
    }

    void Voice::prepare(juce::dsp::ProcessSpec& _spec) {
        spec = _spec;
        localBuff[0].reserve(spec.maximumBlockSize);
        localBuff[1].reserve(spec.maximumBlockSize);
    }


    void Voice::process(juce::dsp::AudioBlock<float>& audio) {
        auto lch = audio.getChannelPointer(0);
        auto rch = audio.getChannelPointer(1);

        localBuff[0].clear();
        localBuff[1].clear();

        for (int s = 0; s < audio.getNumSamples(); s++) {
            Voice::StereoSample sample = getNextSample();

            localBuff[0].push_back(sample.l * amp);
            localBuff[1].push_back(sample.r * amp);
        }

    }

    std::array<std::vector<float>, 2>* Voice::readBack() {
        return &localBuff;
    }

    void Voice::sawTest(float freq) {
        enabled = true;
        amp = 0.125;

        // get a square on left, saw on right
        for (int i = 0; i < SIMD_CNT; i++) {
            for (int j = 0; j < simdSize; j++) {
                setOscParams(voices.left[i], freq * ((i * simdSize) + j + 1), (((i * simdSize) + j + 1) % 2) / (float)(2 * ((i * simdSize) + j + 1)), j);
                setOscParams(voices.right[i], freq * ((i * simdSize) + j + 1), 1 / (float)(2 * ((i * simdSize) + j + 1)), j);
            }
        }
    }

    void Voice::sinTest(float freq) {
        enabled = true;

        setOscParams(voices.left[0], freq, 1, 0);
        setOscParams(voices.right[0], freq, 1, 0);
    }

    void Voice::setOscParams(Osc& osc, float freq, float amp, int simdIndex) {
        auto stepSize = sineTable.getWidth() * freq / (float)spec.sampleRate;
        osc.stepSize.set(simdIndex, stepSize);
        osc.amplitude.set(simdIndex, std::min(amp, 1.f));
        osc.en.set(simdIndex, freq <= NYQUIST_FR && amp > 0);
    }

    Voice::StereoSample Voice::getNextSample() {

        // lambda here for potential future parallelization
        // Find each oscillator's current value
        auto getSample = [&](int i) {
            auto& losc = voices.left[i];
            auto& rosc = voices.right[i];

            auto lSampleChunk = (sineTable.getSample(losc.phase + losc.phaseOffset) * losc.amplitude).sum();
            auto rSampleChunk = (sineTable.getSample(rosc.phase + rosc.phaseOffset) * rosc.amplitude).sum();

            oscOutputs[0][i] = lSampleChunk;
            oscOutputs[1][i] = rSampleChunk;

            losc.phase = SineTable::simdFmod((losc.phase + losc.stepSize), sineTable.getWidth());
            rosc.phase = SineTable::simdFmod((rosc.phase + rosc.stepSize), sineTable.getWidth());
        };

        Voice::StereoSample retVal;
        retVal.l = 0.f;
        retVal.r = 0.f;

        // Could be parallel
        for (int i = 0; i < SIMD_CNT; i++) {
            getSample(i);
        }

        // Must be serial
        // Sum all oscillator values for each channel
        /*for (int p = 0; p < ORDER; p++) {
            retVal.l += oscOutputs[0][p];
            retVal.r += oscOutputs[1][p];
        }*/

        

        retVal.l = std::accumulate(oscOutputs[0].begin(), oscOutputs[0].end(), retVal.l);
        retVal.r = std::accumulate(oscOutputs[1].begin(), oscOutputs[1].end(), retVal.r);

        return retVal;
    }
}