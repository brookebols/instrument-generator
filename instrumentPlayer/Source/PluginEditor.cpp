/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
InstrumentPlayerAudioProcessorEditor::InstrumentPlayerAudioProcessorEditor (InstrumentPlayerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), keyboardComponent(keyboardState, MidiKeyboardComponent::horizontalKeyboard)
{
    // Set the size of the keyboard component
    keyboardComponent.setKeyWidth(30);
    keyboardComponent.setAvailableRange(21, 108); // A0 to C8
    addAndMakeVisible(keyboardComponent);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

InstrumentPlayerAudioProcessorEditor::~InstrumentPlayerAudioProcessorEditor()
{
}

//==============================================================================
void InstrumentPlayerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void InstrumentPlayerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    // Set the bounds of the keyboard component
    keyboardComponent.setBounds(10, 40, getWidth() - 20, 100);
}
