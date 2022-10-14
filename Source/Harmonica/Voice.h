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

#define ORDER   384u

namespace WDYM {
    class Voice {
    public:
        Voice(SineTable& st, juce::ThreadPool& tp);
        ~Voice();

        void prepare(juce::dsp::ProcessSpec& _spec);
        void process(juce::dsp::AudioBlock<float>& audio);
        Voice operator=(Voice& v) { return *this; }

        std::array<std::vector<float>, 2>* readBack();

        bool isPlaying() { return enabled; }

        void sawTest(float freq);

        typedef struct Osc {
        public:
            float phase = 0;
            float phaseOffset = 0;
            float stepSize = 1;
            float amplitude = 0;
            bool en = false;
        };

    private:
        int numCpus;
        juce::dsp::ProcessSpec spec;
        juce::ThreadPool& threadPool;

        bool enabled;
        float baseFreq;
        float amp = 0.8;

        typedef std::array<Osc, ORDER> MonoVoice;
        struct {
            MonoVoice left;
            MonoVoice right;
        } voices;

        typedef struct StereoSample {
            float l;
            float r;
        };

        std::array<std::vector<float>, 2> localBuff;
        
        std::array<std::array<float, ORDER>, 2> oscOutputs;

        SineTable& sineTable;

        void setOscParams(Osc& osc, float freq, float amp);
        StereoSample getNextSample();
    };
}