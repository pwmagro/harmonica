/*
  ==============================================================================

    Harmonica.cpp
    Created: 10 Oct 2022 3:31:30pm
    Author:  pwmag

  ==============================================================================
*/

#include "Harmonica.h"

namespace WDYM {
    Harmonica::Harmonica() : fourier(order), fftData(), fifo(), fourierFreqs() {
        setAudioChannels(2, 2);
    }

    Harmonica::~Harmonica() {
        shutdownAudio();
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

        for (int c = 0; c < audioBlock.getNumChannels(); c++) {
            auto ch = audioBlock.getChannelPointer(c);
            for (int s = 0; s < audioBlock.getNumSamples(); s++) {
                pushNextSampleIntoFifo(ch[s]);
                if (nextFFTBlockReady) {
                    fourier.performRealOnlyForwardTransform(fftData.data());
                    nextFFTBlockReady = false;

                    if (noteRR.midiQ.empty()) continue;

                    auto max = juce::FloatVectorOperations::findMaximum(fftData.data(), fftSize / 64);
                    auto maxIndex = std::distance(fftData.begin(), std::find(fftData.begin(), fftData.end(), max));
                    fftData[maxIndex] = 0;

                    auto targetFreq = noteRR.midiQ.front().hz;
                    auto targetIndex = std::distance(fourierFreqs.begin(), std::find(fourierFreqs.begin(), fourierFreqs.end(), juce::roundToInt(targetFreq)));
                    fftData[targetIndex] = max;

                    fourier.performRealOnlyInverseTransform(fftData.data());

                    float* data = fftData.data();
                    float** datadata = &data;
                    juce::dsp::AudioBlock<float> temp(datadata, 1, fftSize);
                    rb[c].writeSamples(temp);
                }
            }
        }
        audioBlock.clear();
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

    void Harmonica::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
        auto n = bufferToFill.numSamples;
        for (int i = 0; i < 2; i++) {
            auto ch = bufferToFill.buffer->getWritePointer(i);
            for (int j = 0; j < n; j++) {
                ch[j] = rb[i].readNextSample();
            }
        }
    }
}