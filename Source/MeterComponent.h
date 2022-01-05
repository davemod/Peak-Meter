/*
  ==============================================================================

    MeterComponent.h
    Created: 5 Jan 2022 9:53:26am
    Author:  David Hill

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#define PEAK_IN_DB -0.001f

class MeterChannelComponent : public Component, public Timer
{
public:
    
    enum ColourIDs {
        peakColourId,
        normalColourId
    };
    
    MeterChannelComponent (std::function<void()> onPeak);
    ~MeterChannelComponent ();
    
    void prepareToPlay (double sampleRate, int samplesPerBufferExpected);
    void process (const float * data, int numSamples);
    void releaseResources ();
    
    void resetPeaking () { peaking = false; lastPeak = false; }
    
    std::function<void()> onPeak;
    
private:
    
    Atomic<bool> peaking {false};
    Atomic<bool> lastPeak {false};
    
    const float decayInSeconds = 0.5f;
    int decayInSamples;
    
    Atomic<float> lastMaximum = 0.0f;
    Atomic<bool> valueChangedFlag {false};
    
    void resized () override;
    void paint (Graphics& g) override;
    
    void timerCallback () override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterChannelComponent)
    
};

class MeterComponent : public Component
{
public:
    
    MeterComponent (int numChannels = 2);
    
    void prepareToPlay (double sampleRate, int numSamplesExpected);
    void process (const AudioBuffer<float>& buffer);
    void releaseResources ();
    
private:
    
    Atomic<bool> peaking {false};
    
    OwnedArray<MeterChannelComponent> channels;
    
    int numChannels;

    void resized () override;
    void paintOverChildren (Graphics& g) override;
    void mouseDown (const MouseEvent &e) override;
    
    Rectangle<int> getMeterArea () { return getLocalBounds().reduced(0, 10); }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MeterComponent)

};
