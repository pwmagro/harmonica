/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GUI/WindowLayout.h"
#include "GUI/Utils/GuiData.h"

//==============================================================================
/**
*/
class HarmonicaAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    HarmonicaAudioProcessorEditor (HarmonicaAudioProcessor&);
    ~HarmonicaAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::SharedResourcePointer<xynth::DefaultLookAndFeel> defaultLnf;

private:
    juce::ApplicationProperties properties;
    double localWidth = WIDTH;
    double localHeight = HEIGHT;
    double scale = 1; // unused but guiData wants it i guess
    xynth::GuiData guiData;
    WindowLayout windowLayout;

    HarmonicaAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicaAudioProcessorEditor)
};
