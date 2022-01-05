/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PeakMeterAudioProcessor::PeakMeterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
tree(*this, nullptr, {"PARAMS"}, getParameterLayout())
#endif
{
}

PeakMeterAudioProcessor::~PeakMeterAudioProcessor()
{
}

//==============================================================================
const juce::String PeakMeterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PeakMeterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PeakMeterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PeakMeterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PeakMeterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PeakMeterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PeakMeterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PeakMeterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PeakMeterAudioProcessor::getProgramName (int index)
{
    return {};
}

void PeakMeterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PeakMeterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    if (auto editor = getActiveEditor())
    {
        static_cast<PeakMeterAudioProcessorEditor*> (editor)->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void PeakMeterAudioProcessor::releaseResources()
{
    if (auto editor = getActiveEditor())
    {
        static_cast<PeakMeterAudioProcessorEditor*> (editor)->releaseResources();
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PeakMeterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PeakMeterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float gain = 1.0f;
    if (auto value = getApvts().getRawParameterValue(VOLUME_PARAMETER_ID))
        gain = value->load();
    
    /** This is not my style as this locks inside getActiveEditor and is not completely safe when opening / closing the editor – with some time I rather would create a lockfree queue for audio samples so that the editor can poll the most recent data from the Processor */
    if (auto editor = getActiveEditor())
    {
        static_cast<PeakMeterAudioProcessorEditor*> (editor)->processPreFader (buffer);
    }

    buffer.applyGain(gain);
    
    if (auto editor = getActiveEditor())
    {
        static_cast<PeakMeterAudioProcessorEditor*> (editor)->processPostFader (buffer);
    }
}

//==============================================================================
bool PeakMeterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PeakMeterAudioProcessor::createEditor()
{
    return new PeakMeterAudioProcessorEditor (*this);
}

//==============================================================================
void PeakMeterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PeakMeterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PeakMeterAudioProcessor();
}

APVTS::ParameterLayout PeakMeterAudioProcessor::getParameterLayout ()
{
    return {
        std::make_unique<AudioParameterFloat> (VOLUME_PARAMETER_ID, VOLUME_PARAMETER_ID, NormalisableRange<float> (0.0f, 1.0f), 1.0f)
    };
    
}
