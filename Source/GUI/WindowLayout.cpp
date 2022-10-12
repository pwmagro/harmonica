/*
  ==============================================================================

    WindowLayout.cpp
    Created: 5 Sep 2022 3:26:44pm
    Author:  thesp

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WindowLayout.h"

WindowLayout::WindowLayout(xynth::GuiData& g) : guiData(g), aboutOverlay(g)
{
    auto& treeState = g.audioProcessor.treeState;

    // Add and make visible
    guiData.showAbout = [this]() { aboutOverlay.setVisible(true); };
}

WindowLayout::~WindowLayout()
{
}

void WindowLayout::paint (juce::Graphics& g)
{

}

void WindowLayout::resized()
{
    const int spacing = 5;

    auto rect = getLocalBounds();
    rect.reduce(spacing, spacing);
}
