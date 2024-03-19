/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class InstrumentPlayerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    InstrumentPlayerAudioProcessorEditor (InstrumentPlayerAudioProcessor&);
    ~InstrumentPlayerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // Declare MIDI keyboard state and component
    juce::MidiKeyboardState keyboardState;
    juce::MidiKeyboardComponent keyboardComponent;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    InstrumentPlayerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentPlayerAudioProcessorEditor)
};
