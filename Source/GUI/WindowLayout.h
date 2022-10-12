/*
  ==============================================================================

    WindowLayout.h
    Created: 5 Sep 2022 3:26:44pm
    Author:  thesp

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utils/GuiData.h"
#include "About/AboutOverlay.h"
#include "Misc/Logo.h"
#include "CustomLooks/Styles.h"

class WindowLayout : public juce::Component
{
public:
    WindowLayout(xynth::GuiData&);
    ~WindowLayout() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    float x = 0;
private:
    xynth::GuiData& guiData;

    // About overlay
    AboutOverlay aboutOverlay;

    // Miscellaneous items
    //Logo logo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WindowLayout)
};
