/*
  ==============================================================================

    Harmonica.cpp
    Created: 10 Oct 2022 3:31:30pm
    Author:  pwmag

  ==============================================================================
*/

#include "Harmonica.h"

namespace WDYM {
    Harmonica::Harmonica() : forwardFFT(fftOrder) {
        
    }

    void Harmonica::init(float sampleRate, int samplesPerBlock) {

    }

    void Harmonica::process(juce::dsp::AudioBlock<float>& audioBlock, juce::MidiBuffer& midi) {
        for (const auto midiIterator : midi) {
            auto midiMessage = midiIterator.getMessage();
            if (midiMessage.getMessage().isNoteOn()) {

            }

            if (midiMessage.getMessage().isNoteOff())
        }

        if (audioBlock.getNumChannels() > 2) {
            auto* channelData = audioBlock.getChannelPointer(0);

            for (auto i = 0; i < audioBlock.getNumSamples(); ++i) {
                pushNextSampleIntoFifo(channelData[i]);

                if (nextFFTBlockReady) {
                    forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());

                    int fundIndex = 0;
                    float maxAmp = 0;
                    for (int j = 0; j < fftSize; j++) {
                        if (fftData[j] > maxAmp) {
                            maxAmp = fftData[j];
                            fundIndex = j;
                        }



                    }
                }
            }
        }
    }

    void Harmonica::pushNextSampleIntoFifo(float sample)
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