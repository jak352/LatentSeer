/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LatentSeerAudioProcessor::LatentSeerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

LatentSeerAudioProcessor::~LatentSeerAudioProcessor()
{
}

//==============================================================================
const String LatentSeerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool LatentSeerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool LatentSeerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool LatentSeerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double LatentSeerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int LatentSeerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int LatentSeerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void LatentSeerAudioProcessor::setCurrentProgram (int index)
{
}

const String LatentSeerAudioProcessor::getProgramName (int index)
{
    return {};
}

void LatentSeerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void LatentSeerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    latentSeer.setup(sampleRate, samplesPerBlock, getTotalNumInputChannels());
}

void LatentSeerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LatentSeerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void LatentSeerAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    transientState = latentSeer.hadTransient(40);
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int bufferLength = buffer.getNumSamples();
    
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, bufferLength);

    latentSeer.process(totalNumInputChannels, buffer);
}

//void LatentSeerAudioProcessor::findMinMaxDelayBuffer (int channel, const int bufferLength, //const int delayBufferLength, const float* bufferData, const float* delayBufferData)
//{
    //check
//}

//==============================================================================
bool LatentSeerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* LatentSeerAudioProcessor::createEditor()
{
    return new LatentSeerAudioProcessorEditor (*this);
}

//==============================================================================
void LatentSeerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void LatentSeerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LatentSeerAudioProcessor();
}

const bool* LatentSeerAudioProcessor::getTransientStatePointer()
{
    return (const bool*)&transientState;
}

