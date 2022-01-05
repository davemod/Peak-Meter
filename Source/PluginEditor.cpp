/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PeakMeterAudioProcessorEditor::PeakMeterAudioProcessorEditor (PeakMeterAudioProcessor& p)
:
AudioProcessorEditor (&p),
audioProcessor (p),
volumeSlider (p.getApvts(), VOLUME_PARAMETER_ID)
{
    volumeSlider.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    
    addAndMakeVisible (inputMeterComponent);
    addAndMakeVisible (volumeSlider);
    addAndMakeVisible (outputMeterComponent);
    
    prepareToPlay (p.getBlockSize(), p.getSampleRate());
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PeakMeterAudioProcessorEditor::~PeakMeterAudioProcessorEditor()
{
}

//==============================================================================
void PeakMeterAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void PeakMeterAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds ().reduced(10);
    inputMeterComponent.setBounds(bounds.removeFromLeft (50));
    
    outputMeterComponent.setBounds (bounds.removeFromRight (50));

    volumeSlider.setBounds(bounds.reduced(5, 0));
}


void PeakMeterAudioProcessorEditor::prepareToPlay (double sampleRate, int samplesPerBufferExpected)
{
    inputMeterComponent.prepareToPlay(sampleRate, samplesPerBufferExpected);
    outputMeterComponent.prepareToPlay(sampleRate, samplesPerBufferExpected);
}

void PeakMeterAudioProcessorEditor::processPreFader (const AudioBuffer<float>& buffer)
{
    inputMeterComponent.process(buffer);
}

void PeakMeterAudioProcessorEditor::processPostFader (const AudioBuffer<float>& buffer)
{
    outputMeterComponent.process(buffer);
}

void PeakMeterAudioProcessorEditor::releaseResources ()
{
    inputMeterComponent.releaseResources();
    outputMeterComponent.releaseResources();
}
