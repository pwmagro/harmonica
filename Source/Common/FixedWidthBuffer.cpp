/*
  ==============================================================================

    FixedWidthBuffer.cpp
    Created: 26 Sep 2022 3:48:05pm
    Author:  pwmag

  ==============================================================================
*/

#include "FixedWidthBuffer.h"

namespace WDYM {
    FixedWidthBuffer::FixedWidthBuffer() {
        workingBuffer = new std::vector<float>;
        fullBuffer = new std::vector<float>;

        workingBuffer->resize(width);
        fullBuffer->resize(width);
    }

    FixedWidthBuffer::FixedWidthBuffer(int maxWidth) {
        width = maxWidth;
        this->maxWidth = maxWidth;

        workingBuffer = new std::vector<float>;
        fullBuffer = new std::vector<float>;

        workingBuffer->resize(width);
        fullBuffer->resize(width);
    }

    FixedWidthBuffer::~FixedWidthBuffer() {
        delete workingBuffer;
        delete fullBuffer;
    }

    void FixedWidthBuffer::process(juce::AudioBuffer<float>& buffer) {
        if (buffer.getNumChannels() != 2) {
            jassertfalse;
        }

        auto leftBuff = buffer.getReadPointer(0);
        auto rightBuff = buffer.getReadPointer(1);

        for (int i = 0; i < buffer.getNumSamples(); i++) {
            writeSample((leftBuff[i] + rightBuff[i]) * 0.5);
        }
    }

    void FixedWidthBuffer::writeSample(float sample) {

        if (workingIndex < width) {
            workingBuffer->push_back(sample);
            workingIndex++;
        }

        else {
            std::vector<float>* temp = workingBuffer;
            workingBuffer = fullBuffer;
            fullBuffer = temp;
            workingBuffer->clear();
            workingIndex = 0;
            fullBufferIsValid = true;
        }
    }

    float FixedWidthBuffer::getSample(int i) {
        if (fullBuffer->size() != width) return 0.0; // the buffer isn't valid. oops

        if (i < 0 || i >= width) {
            jassertfalse;
        }

        return (*fullBuffer)[i];
    }

    std::vector<float>* FixedWidthBuffer::getBuffer() {
        if (fullBufferIsValid) {
            fullBufferIsValid = false;
            return fullBuffer;
        }
        else return nullptr;
    }

    float FixedWidthBuffer::operator[](int i) {
        return getSample(i);
    }

    void FixedWidthBuffer::resize(int newWidth) {
        if (newWidth <= maxWidth && newWidth > 0) {
            width = newWidth;
        }
    }

}