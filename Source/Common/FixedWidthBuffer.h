/*
  ==============================================================================

    FixedWidthBuffer.h
    Created: 26 Sep 2022 3:48:05pm
    Author:  pwmag

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace WDYM {
    class FixedWidthBuffer {
    public:
        FixedWidthBuffer();
        FixedWidthBuffer(int maxWidth);
        ~FixedWidthBuffer();

        void process(juce::AudioBuffer<float>& buffer);
        void resize(int newWidth);
        float getSample(int i);
        float operator[](int i);
        int getWidth() { return width; }
        void writeSample(float sample);
        std::vector<float>* getBuffer();
        bool isValid() { return fullBufferIsValid; }

    private:
        int width = 11025;
        int maxWidth = width;

        std::vector<float>* workingBuffer;
        std::vector<float>* fullBuffer;
        bool fullBufferIsValid = false;
        float workingIndex = 0;
    };
}