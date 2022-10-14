/*
  ==============================================================================

    Harmonica.cpp
    Created: 10 Oct 2022 3:31:30pm
    Author:  pwmag

  ==============================================================================
*/

#include "Harmonica.h"

namespace WDYM {
    Harmonica::Harmonica(juce::ThreadPool& tp) : fourier(order), fftData(), fifo(), fourierFreqs(), threadPool(tp), sineTable() {
        for (int i = 0; i < voices.size(); i++) {
            voices[i] = new Voice(sineTable, tp);
        }

    }

    Harmonica::~Harmonica() {
        for (int i = 0; i < voices.size(); i++) {
            delete voices[i];
        }
    }
    
    void Harmonica::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
        samplesPerMs = sampleRate / 1000.f;
        this->sampleRate = sampleRate;

        for (int i = 0; i < fftSize; i++) {
            fourierFreqs[i] = juce::roundToInt((sampleRate * i) / fftSize);
        }

        juce::dsp::ProcessSpec spec;
        spec.maximumBlockSize = samplesPerBlockExpected;
        spec.numChannels = 2;
        spec.sampleRate = sampleRate;

        rb[0].prepare(spec);
        rb[1].prepare(spec);
        
        for (int i = 0; i < voices.size(); i++) {
            voices[i]->prepare(spec);
        }

        for (int i = 0; i < NUM_VOICES; i++) {
            voices[i]->sawTest((i + 1) * 110.f);
        }
    }

    void Harmonica::process(juce::dsp::AudioBlock<float>& audioBlock, juce::MidiBuffer& midi) {
        for (const auto midiIterator : midi) {
            auto midiMessage = midiIterator.getMessage();
            if (midiMessage.isNoteOn()) {
                noteRR.note_on(midiMessage);
            }

            if (midiMessage.isNoteOff()) {
                noteRR.note_off(midiMessage);
            }
        }

        if (audioBlock.getNumChannels() != 2) {
            jassertfalse;
        }

        audioBlock.clear();

        juce::AudioBuffer<float> workingBuffer(2, audioBlock.getNumSamples());
        workingBuffer.copyFrom(0, 0, audioBlock.getChannelPointer(0), audioBlock.getNumSamples());
        workingBuffer.copyFrom(1, 0, audioBlock.getChannelPointer(1), audioBlock.getNumSamples());
        auto workingBlock = juce::dsp::AudioBlock<float>(workingBuffer);

        auto ctx = juce::dsp::ProcessContextReplacing<float>(workingBlock);

        auto voiceProcess = [&](int i) {
            if (voices[i]->isPlaying()) {
                voices[i]->process(audioBlock);
            }
        };

        squeeze::ParallelFor(threadPool, (size_t)0, NUM_VOICES, voiceProcess);

        for (int i = 0; i < NUM_VOICES; i++) {
            if (voices[i]->isPlaying()) {
                auto b = *(voices[i]->readBack());
                juce::FloatVectorOperations::add(audioBlock.getChannelPointer(0), b[0].data(), audioBlock.getNumSamples());
                juce::FloatVectorOperations::add(audioBlock.getChannelPointer(1), b[1].data(), audioBlock.getNumSamples());
            }
        }

        //audioBlock.copyFrom(ctx.getOutputBlock());

        //for (int s = 0; s < audioBlock.getNumSamples(); s++) {
        //    for (int c = 0; c < 1/*audioBlock.getNumChannels()*/; c++) {
        //        auto ch = audioBlock.getChannelPointer(c);
        //        pushNextSampleIntoFifo(ch[s]);
        //        if (nextFFTBlockReady) {
        //            fourier.performRealOnlyForwardTransform(fftData.data());
        //            nextFFTBlockReady = false;

        //            if (noteRR.midiQ.empty()) continue;

        //            auto max = juce::FloatVectorOperations::findMaximum(fftData.data(), fftSize / 64);
        //            auto maxIndex = std::distance(fftData.begin(), std::find(fftData.begin(), fftData.end(), max));
        //            fftData[maxIndex] = 0;

        //            auto targetFreq = noteRR.midiQ.front().hz;
        //            auto targetIndex = std::distance(fourierFreqs.begin(), std::find(fourierFreqs.begin(), fourierFreqs.end(), juce::roundToInt(targetFreq)));
        //            fftData[targetIndex] = max;

        //            fourier.performRealOnlyInverseTransform(fftData.data());

        //            float* data = fftData.data();
        //            float** datadata = &data;
        //            juce::dsp::AudioBlock<float> temp(datadata, 1, fftSize);
        //            rb[c].writeSamples(temp);
        //        }
        //        if (rb[c].sampleAvailable())
        //            ch[s] = rb[c].readNextSample();
        //        else
        //            ch[s] = 0;
        //    }
        //}
    }

    void Harmonica::pushNextSampleIntoFifo(float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next line should now be rendered..
        if (fifoIndex == fftSize)       // [8]
        {
            if (!nextFFTBlockReady)    // [9]
            {
                std::fill(fftData.begin(), fftData.end(), 0.0f);
                std::copy(fifo.begin(), fifo.end(), fftData.begin());
                nextFFTBlockReady = true;
            }

            fifoIndex = 0;
        }

        fifo[(size_t)fifoIndex++] = sample; // [9]
    }
}