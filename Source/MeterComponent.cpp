/*
  ==============================================================================

    MeterComponent.cpp
    Created: 5 Jan 2022 9:53:26am
    Author:  David Hill

  ==============================================================================
*/

#include "MeterComponent.h"


// ================================================================
// ================================================================

MeterChannelComponent::MeterChannelComponent (std::function<void()> onPeak)
:
onPeak (onPeak)
{
    setColour (peakColourId, Colours::darkorange);
    setColour (normalColourId, Colours::green);
    
    startTimerHz(60);
}

MeterChannelComponent::~MeterChannelComponent ()
{
    
}

void MeterChannelComponent::prepareToPlay (double sampleRate, int samplesPerBufferExpected)
{
    decayInSamples = roundToInt (decayInSeconds * sampleRate);
}

void MeterChannelComponent::process (const float * data, int numSamples)
{
    auto minMax = FloatVectorOperations::findMinAndMax(data, numSamples);
    auto max = jmax (abs (minMax.getStart()), abs (minMax.getEnd()));
    
    auto k = (float)decayInSamples / (decayInSamples + 1);
    
    if (max <= lastMaximum.get())
    {
        max = max + k * (lastMaximum.get() - max);
    }
        
    if (lastMaximum.get() != max)
    {
        lastMaximum = max;
        valueChangedFlag = true;
        
        if (juce::Decibels::gainToDecibels(max) >= PEAK_IN_DB)
            peaking = true;
    }
}

void MeterChannelComponent::releaseResources ()
{
    
}

void MeterChannelComponent::resized ()
{}

void MeterChannelComponent::paint (Graphics& g)
{
    auto dezibels = juce::Decibels::gainToDecibels (lastMaximum.get(),-60.0f);
   
    g.setColour(peaking.get() ? findColour (peakColourId) : findColour (normalColourId));
    g.fillRect(getLocalBounds().removeFromBottom((dezibels + 60) / 60 * getHeight()));
}

void MeterChannelComponent::mouseDown (const MouseEvent& e)
{
    if (auto parent = getParentComponent())
        parent->mouseDown (e);
}

void MeterChannelComponent::timerCallback ()
{
    if (peaking.get() && !lastPeak.get() && onPeak)
    {
        onPeak ();
    }

    lastPeak = peaking.get();
    
    if (! valueChangedFlag.get())
        return;
    
    repaint ();
    
    valueChangedFlag = false;
}

MeterComponent::MeterComponent (int numChans)
:
numChannels (numChans)
{
    for (int i = 0; i < numChannels; i++)
    {
        addAndMakeVisible (channels.add (new MeterChannelComponent ([&](){
            peaking = true;
            repaint ();
        })));
    }
}

void MeterComponent::prepareToPlay(double sampleRate, int numSamplesExpected)
{
    for (auto c : channels)
        c->prepareToPlay(sampleRate, numSamplesExpected);
}

void MeterComponent::process(const AudioBuffer<float> &buffer)
{
    int size = jmin (buffer.getNumChannels(), numChannels);
    
    for (int i = 0; i < size; i++)
    {
        channels.getUnchecked(i)->process(buffer.getReadPointer(i),    buffer.getNumSamples());
    }
}

void MeterComponent::releaseResources()
{
    for (auto c : channels)
    {
        c->releaseResources();
    }
}

void MeterComponent::resized ()
{
    auto bounds = getMeterArea();
    
    int widthPerChannel = bounds.getWidth () / (numChannels);
    
    for (auto c : channels)
        c->setBounds(bounds.removeFromLeft(widthPerChannel).reduced(1, 0));
}

void MeterComponent::paintOverChildren (Graphics& g)
{
    g.setColour(Colours::red);
    
    if (peaking.get())
        g.fillRect(getLocalBounds().removeFromTop(10).reduced(0,1));
    
    g.setColour (Colours::white);
    g.setFont (9);
    g.drawRect(getLocalBounds());
    
    auto meterArea = getMeterArea().toFloat();
    
    for (int i = 0; i >= -60;  i -= 6)
    {
        auto pos = meterArea.getRelativePoint(0.5f, (float)i / -60.0f);
        
        g.drawText ((String)i, meterArea.withHeight(10.0f).withCentre(pos), Justification::centred);
    }
}

void MeterComponent::mouseDown (const MouseEvent& e)
{
    peaking = false;
    
    for (auto c : channels)
        c->resetPeaking();
    
    repaint();
}
