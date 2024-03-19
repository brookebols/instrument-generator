/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
InstrumentPlayerAudioProcessor::InstrumentPlayerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    keyboardState.addListener(this);
    formatManager.registerBasicFormats();
    loadSamples();
}

InstrumentPlayerAudioProcessor::~InstrumentPlayerAudioProcessor()
{
    keyboardState.removeListener(this);
}

std::pair<const char*, int> getSampleDataAndSize(int midiNote) {
    switch (midiNote) {
        case 48: return { BinaryData::_48_wav, BinaryData::_48_wavSize };
        case 49: return { BinaryData::_49_wav, BinaryData::_49_wavSize };
        case 50: return { BinaryData::_50_wav, BinaryData::_50_wavSize };
        case 51: return { BinaryData::_51_wav, BinaryData::_51_wavSize };
        case 52: return { BinaryData::_52_wav, BinaryData::_52_wavSize };
        case 53: return { BinaryData::_53_wav, BinaryData::_53_wavSize };
        case 54: return { BinaryData::_54_wav, BinaryData::_54_wavSize };
        case 55: return { BinaryData::_55_wav, BinaryData::_55_wavSize };
        case 56: return { BinaryData::_56_wav, BinaryData::_56_wavSize };
        case 57: return { BinaryData::_57_wav, BinaryData::_57_wavSize };
        case 58: return { BinaryData::_58_wav, BinaryData::_58_wavSize };
        case 59: return { BinaryData::_59_wav, BinaryData::_59_wavSize };
        case 60: return { BinaryData::_60_wav, BinaryData::_60_wavSize };
        default: return { nullptr, 0 };
    }
}

//==============================================================================
void InstrumentPlayerAudioProcessor::loadSamples()
{
    
    for (int midiNote = 48; midiNote <= 60; ++midiNote) // Adjust range as needed
    {
        auto [resourceData, resourceSize] = getSampleDataAndSize(midiNote);

        if (resourceData != nullptr && resourceSize > 0)
        {
            DBG ("Found sample: " + String(midiNote));
            // Create a MemoryInputStream from the binary data
            std::unique_ptr<juce::MemoryInputStream> stream(new juce::MemoryInputStream(resourceData, static_cast<size_t>(resourceSize), false));

            // Create an AudioFormatReader from the stream
            std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(stream)));

            if (reader != nullptr)
            {
                DBG ("Sample loaded: " + String(midiNote));
                // Store the reader in your map, associated with the MIDI note
                noteSamples[midiNote] = std::make_pair(juce::File(), std::make_unique<AudioFormatReaderSource>(reader.release(), true));
            }
			else
			{
				DBG ("Failed to load sample: " + String(midiNote));
			}
        }
        else {
            DBG("Sample not found: " + String(midiNote));
		}
    }

    /*
    File sampleDirectory("C:/Users/brook/PycharmProjects/InstrumentGenerator/synthesized_audio"); // Update this path to where your samples are located
    RangedDirectoryIterator iter(sampleDirectory, false, "*.wav");

    for (auto& file : iter) {
        DBG("Found sample: " + file.getFile().getFullPathName());
        auto* reader = formatManager.createReaderFor(file.getFile());
        if (reader != nullptr) {
            int midiNote = file.getFile().getFileNameWithoutExtension().getIntValue();
            noteSamples[midiNote] = std::make_pair(file.getFile(), std::make_unique<AudioFormatReaderSource>(reader, true));
        }
    }
    */
    
}

File InstrumentPlayerAudioProcessor::getSampleFile(int midiNote) {
	auto it = noteSamples.find(midiNote);
	if (it != noteSamples.end()) {
		return it->second.first;
	}
	return {};
}

void InstrumentPlayerAudioProcessor::handleNoteOn(juce::MidiKeyboardState*, int midiChannel, int midiNote, float velocity) {
    startNote(midiNote);
}

void InstrumentPlayerAudioProcessor::handleNoteOff(juce::MidiKeyboardState*, int midiChannel, int midiNote, float velocity) {
    stopNote(midiNote);
}

void InstrumentPlayerAudioProcessor::startNote(int midiNote) {
    auto [resourceData, resourceSize] = getSampleDataAndSize(midiNote);

    if (resourceData != nullptr && resourceSize > 0) {
        // Create a MemoryInputStream from the binary data
        std::unique_ptr<juce::MemoryInputStream> stream(new juce::MemoryInputStream(resourceData, static_cast<size_t>(resourceSize), false));

        // Create an AudioFormatReader from the stream
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(stream)));

        if (reader) {
            SampleVoice voice;
            voice.source = std::make_unique<AudioFormatReaderSource>(reader.release(), true);
            voice.isPlaying = true;
            voice.midiNote = midiNote;

            // Prepare the source to be played
            //voice.source->prepareToPlay(samplesPerBlockExpected, sampleRate);

            voices.push_back(std::move(voice));
        }
    }
    
    /*
    File sampleFile = getSampleFile(midiNote);
    if (sampleFile.exists()) {
        auto* reader = formatManager.createReaderFor(sampleFile);
        if (reader) {
            SampleVoice voice;
            voice.source = std::make_unique<AudioFormatReaderSource>(reader, true);
            voice.isPlaying = true;
            voice.midiNote = midiNote;
            voices.push_back(std::move(voice));
        }
    }
    */
}

void InstrumentPlayerAudioProcessor::stopNote(int midiNote) {
    for (auto& voice : voices) {
        if (voice.midiNote == midiNote) {
            voice.isPlaying = false;
            voice.midiNote = -1;
            break; // Assuming one voice per note for simplicity
        }
    }
}

void InstrumentPlayerAudioProcessor::stopAllNotes() {
	for (auto& voice : voices) {
		voice.isPlaying = false;
		voice.midiNote = -1;
	}
}

//==============================================================================

const juce::String InstrumentPlayerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool InstrumentPlayerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool InstrumentPlayerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool InstrumentPlayerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double InstrumentPlayerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int InstrumentPlayerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int InstrumentPlayerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void InstrumentPlayerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String InstrumentPlayerAudioProcessor::getProgramName (int index)
{
    return {};
}

void InstrumentPlayerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void InstrumentPlayerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void InstrumentPlayerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool InstrumentPlayerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void InstrumentPlayerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    /*
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
    */

    // Forward incoming MIDI messages to the keyboard state
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    MidiMessage message;
    int time;
    for (const auto metadata : midiMessages) {
        message = metadata.getMessage();
        if (message.isNoteOn()) {
            DBG("Note on: " + String(message.getNoteNumber()));
            startNote(message.getNoteNumber());
        }
        else if (message.isNoteOff()) {
            DBG("Note off: " + String(message.getNoteNumber()));
            stopNote(message.getNoteNumber());
        }
    }

    for (auto& voice : voices) {
        if (voice.isPlaying) {
            voiceBuffer.setSize(buffer.getNumChannels(), buffer.getNumSamples(), false, false, true);
            voiceBuffer.clear();
            voice.source->getNextAudioBlock(AudioSourceChannelInfo(voiceBuffer));
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
                buffer.addFrom(channel, 0, voiceBuffer, channel, 0, buffer.getNumSamples());
            }
        }
    }

    // Remove voices that are no longer playing
    voices.erase(std::remove_if(voices.begin(), voices.end(), [](const SampleVoice& voice) { return !voice.isPlaying; }), voices.end());
}

//==============================================================================
bool InstrumentPlayerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* InstrumentPlayerAudioProcessor::createEditor()
{
    return new InstrumentPlayerAudioProcessorEditor (*this);
}

//==============================================================================
void InstrumentPlayerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void InstrumentPlayerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new InstrumentPlayerAudioProcessor();
}
