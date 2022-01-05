/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "MeterComponent.h"

//==============================================================================
/**
*/
class AttachedSlider : public Slider
{
public:
    AttachedSlider ( AudioProcessorValueTreeState & stateToUse, const String & parameterID )
    :
    attachment (stateToUse, parameterID, *this)
    {
        
    }
    
    
    APVTS::SliderAttachment attachment;
    
private:
    
};

class PeakMeterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PeakMeterAudioProcessorEditor (PeakMeterAudioProcessor&);
    ~PeakMeterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void prepareToPlay (double sampleRate, int samplesPerBufferExpected);
    void processPreFader (const AudioBuffer<float>& buffer);
    void processPostFader (const AudioBuffer<float>& buffer);
    void releaseResources ();
    
private:
    
    MeterComponent inputMeterComponent;
    MeterComponent outputMeterComponent;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PeakMeterAudioProcessor& audioProcessor;
    
    AttachedSlider volumeSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PeakMeterAudioProcessorEditor)
};
