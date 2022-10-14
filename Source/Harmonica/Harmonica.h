/*
  ==============================================================================

    Harmonica.h
    Created: 10 Oct 2022 3:31:30pm
    Author:  pwmag

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Common/FixedWidthBuffer.h"
#include "../Common/RingBuffer.h"
#include "MidiQueue.h"
#include "Voice.h"
#include "../Common/Squeeze.h"

#define NUM_VOICES (size_t)8

namespace WDYM {
    class Harmonica : public juce::Component {
    public:
        Harmonica(juce::ThreadPool& tp);
        ~Harmonica();
        void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
        void process(juce::dsp::AudioBlock<float>& audioBlock, juce::MidiBuffer& midi);
        void pushNextSampleIntoFifo(float sample) noexcept;

    private:
        SineTable sineTable;

        void scan(float *s, int index, int channelIndex);
        void panic() { noteRR.panic(); }

        midiQueue_t noteRR;

        float samplesPerMs = 44.1f;
        float sampleRate = 44100.f;
        float frameLength = 10;     // length of frame in ms

        static constexpr auto order = 10;
        static constexpr auto fftSize = 1 << order;
        std::array<float, fftSize * 2> fftData;
        std::array<float, fftSize> fifo;
        int fifoIndex = 0;
        bool nextFFTBlockReady = false;

        std::array<float, fftSize> fourierFreqs;
        xynth::RingBuffer rb[2];

        juce::dsp::FFT fourier;

        std::array<Voice*, NUM_VOICES> voices;
        juce::ThreadPool& threadPool;
    };
}