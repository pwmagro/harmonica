/*
  ==============================================================================

    Harmonica.h
    Created: 10 Oct 2022 3:31:30pm
    Author:  pwmag

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../GUI/Utils/GuiData.h"

namespace WDYM {
    class Harmonica : public juce::Component {
    public:
        Harmonica();
        void init(float sampleRate, int samplesPerBlock);
        void process(juce::dsp::AudioBlock<float>& audioBlock, juce::MidiBuffer& midi);

    private:
        void pushNextSampleIntoFifo(float sample);

        static constexpr auto fftOrder = 12;
        static constexpr auto fftSize = 1 << fftOrder;
        static constexpr auto numVoices = 16;

        std::array<float, numVoices> inFreqs;
        std::array<bool, numVoices> voiceActive;

        juce::dsp::FFT forwardFFT;
        std::array<float, fftSize> fifo;
        std::array<float, fftSize * 2> fftData;
        int fifoIndex = 0;
        bool nextFFTBlockReady = false;
    };
}