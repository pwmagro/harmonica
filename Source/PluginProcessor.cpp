/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HarmonicaAudioProcessor::HarmonicaAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
    treeState (*this, nullptr, "Parameters", createParameters()),
    harmonica()
#endif
{

}

HarmonicaAudioProcessor::~HarmonicaAudioProcessor()
{
}

//==============================================================================
const juce::String HarmonicaAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HarmonicaAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HarmonicaAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HarmonicaAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HarmonicaAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HarmonicaAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HarmonicaAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HarmonicaAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HarmonicaAudioProcessor::getProgramName (int index)
{
    return {};
}

void HarmonicaAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HarmonicaAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Start diode sim
    harmonica.init(sampleRate, samplesPerBlock);
}

void HarmonicaAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HarmonicaAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HarmonicaAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);
    harmonica.process(block, midiMessages);
}

//==============================================================================
bool HarmonicaAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HarmonicaAudioProcessor::createEditor()
{
    return new HarmonicaAudioProcessorEditor(*this);
}

//==============================================================================
void HarmonicaAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = treeState.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HarmonicaAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    // 'If' statements are for error checking
    if (xmlState != nullptr) {
        if (xmlState->hasTagName(treeState.state.getType())) {
            treeState.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HarmonicaAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout HarmonicaAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(GAIN_ID, GAIN_NAME,         1.f,  20.f,   5.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(SAT_ID,  SAT_NAME,          0.f,   0.5f,  0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(VF_ID,   VF_NAME,           0.f,   5.f,   0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(VB_ID,   VB_NAME,         -20.f,   0.f, -12.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(TRR_ID,  TRR_NAME,          0.f,  20.f,   2.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(TRR_MAG_ID, TRR_MAG_NAME,   0.f,   2.f,   1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(TRR_SKEW_ID, TRR_SKEW_NAME, 0.f,   0.7f,  0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(MIX_ID,  MIX_NAME,          0.f,   2.f,   1.f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(HUE_ID, HUE_NAME, 0.f, 360.f, 25.f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(DIODE_1_ID, DIODE_1_NAME, true ));
    params.push_back(std::make_unique<juce::AudioParameterBool>(DIODE_2_ID, DIODE_2_NAME, false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(DC_OFF_ID, DC_OFF_NAME, true));

    return { params.begin(), params.end() };
}