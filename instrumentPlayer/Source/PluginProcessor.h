/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <map>
#include <vector>
#include <JuceHeader.h>

struct SampleVoice {
    std::unique_ptr<juce::AudioFormatReaderSource> source;
    bool isPlaying = false;
    int midiNote = -1;
};

//==============================================================================
/**
*/
class InstrumentPlayerAudioProcessor  : public juce::AudioProcessor,
    public juce::MidiKeyboardStateListener
{
public:
    //==============================================================================
    InstrumentPlayerAudioProcessor();
    ~InstrumentPlayerAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void loadSamples();
    File getSampleFile(int midiNote);
    void handleNoteOn(MidiKeyboardState*, int midiChannel, int midiNote, float velocity) override;
    void handleNoteOff(MidiKeyboardState*, int midiChannel, int midiNote, float velocity) override;
    void startNote(int midiNote);
    void stopNote(int midiNote);
    void stopAllNotes();

    MidiKeyboardState& getKeyboardState() { return keyboardState; }

private:
    MidiKeyboardState keyboardState;
    std::map<int, std::pair<File, std::unique_ptr<AudioFormatReaderSource>>> noteSamples;
    std::vector<SampleVoice> voices;
    AudioFormatManager formatManager;
    AudioSampleBuffer voiceBuffer;
    int maxVoices = 16;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InstrumentPlayerAudioProcessor)
};
