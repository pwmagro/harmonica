/*
  ==============================================================================

    Voice.h
    Created: 13 Oct 2022 8:29:57pm
    Author:  pwmag

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SineTable.h"
//#include "../Common/Squeeze.h"
#include <omp.h>

#define ORDER   512u
#define SIMD_CNT    (ORDER / simdSize)

namespace WDYM {
    class Voice {
    public:
        using SIMDFloat = juce::dsp::SIMDRegister<float>;
        using SIMDOp = juce::dsp::SIMDNativeOps<float>;
        Voice(SineTable& st, juce::ThreadPool& tp);
        ~Voice();

        void prepare(juce::dsp::ProcessSpec& _spec);
        void process(juce::dsp::AudioBlock<float>& audio);
        Voice operator=(Voice& v) { return *this; }

        std::array<std::vector<float>, 2>* readBack();

        bool isPlaying() { return enabled; }

        void setMidiNote(int id, float amplitude) {
            amp = amplitude;
            midiNote = id;
            sinTest(juce::MidiMessage::getMidiNoteInHertz(id)); }
        int getMidiNote() { return midiNote; }

        void sawTest(float freq);
        void sinTest(float freq);

        void disable() { enabled = false; };

        // each Osc struct contains multiple actual oscillators
        typedef struct Osc {
        public:
            alignas (32) SIMDFloat phase = 0;
            alignas (32) SIMDFloat phaseOffset = 0;
            alignas (32) SIMDFloat stepSize = 1;
            alignas (32) SIMDFloat amplitude = 0;
            alignas (32) SIMDFloat en = false;
        };

    private:
        
        int numCpus;
        juce::dsp::ProcessSpec spec;
        juce::ThreadPool& threadPool;

        bool enabled;
        float baseFreq;
        int midiNote;
        float amp = 0.8;

        static constexpr size_t simdSize = SIMDFloat::size();
        typedef std::array<Osc, SIMD_CNT> MonoVoice;
        struct {
            MonoVoice left;
            MonoVoice right;
        } voices;

        typedef struct StereoSample {
            float l = 0.f;
            float r = 0.f;
        };

        std::array<std::vector<float>, 2> localBuff;
        
        std::array<std::array<float, SIMD_CNT>, 2> oscOutputs;

        SineTable& sineTable;

        void setOscParams(Osc& osc, float freq, float amp, int simdIndex);
        StereoSample getNextSample();
    };
}