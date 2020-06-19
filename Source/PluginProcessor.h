/*
 ==============================================================================
 
 This file was auto-generated!
 
 It contains the basic framework code for a JUCE plugin processor.
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
 */
class LatentSeerAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    LatentSeerAudioProcessor();
    ~LatentSeerAudioProcessor();
    
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    
#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif
    
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;
    
    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    
    //==============================================================================
    const String getName() const override;
    
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    
    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    void fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);
    void getFromDelayBuffer (float mDelayVolume,AudioBuffer<float>& buffer,int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);
    void getMagnitudeOfDelayBuffer (const int delayBufferLength, const int readPosition, const int bufferLength);
    void getMaxOfXCorOfDelayBuffer ( const int delayBufferLength, const int bufferLength, const int totalNumInputChannels );
    void checkIfThresholdExceeded(const int bufferLength);
    
    float mGain { 0.5 };
    /// Add this myself to make delay time in milliseconds
    int mDelay_ms { -40 };
    /// Add this myself to make amplitude of delay signal
    float mDelayVolume { 0.5 };
    /// Add this to make default BPM
    int mBPM { 120 };
    /// Add this to make default Beats Per Bar
    int mBeatsPerBar { 4 };
    /// Threshold over which note is detected in delayed signal
    float mTransientThreshInDelayed { -12.0 };
    bool mDelayBufferThresholdExceeded { false };
    int mTransientCounter { 0 };
    int mPitchTransientCounter { 0 };
    float mDelayBufferMagnitude { 0 };
    /// Threshold below which note is assumed to have released to allow future retriggering
    float mReleaseThreshInDelayed { -30.0 };
    /// Threshold below which note is assumed to have released to allow future retriggering
    int mLowestFrequency { 30 };
    int mReleaseCounter { 0 };
    ///  true means no fade
    bool noFadeIn = true;
    /// Number of audio buffers that delay buffer plays for;
    int mNumBuffersForRelease = { 8 };
    /// Number of milliseconds that must be under release threshold for release
    float mReset_ms = { 20 };
    float mPitchChangeThreshold = { 0.75 };
    bool mNoFadeOut = { true };
    bool mNoFadeIn = { true };
    /// Maximum of the xcorrelation between signal and signal a few ms back in delay buffer
    float mNormalisedXCorMax = { 0 };
    /** mSampleRate*0.020 at 192 kHz for instance. Number of samples back in audio to compare across to get cross-correlation for pitch change detection. Calculated in prepare to play. Would need to be careful with size of mDelayBufferPitch if this is not to be changed during runtime
     */
    int mNBack { 3840 };
    /// Counter for offseting lag in cross-correlation
    int mNLagBuffers { 0 };
    /// Counter for getting multiple pitch change detections in a row
    int mPitchDetectCounter { 0 };
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LatentSeerAudioProcessor)
    
    AudioBuffer<float> mDelayBuffer; //m means global member variable as opposed p for pointer or mp for member pointer
    AudioBuffer<float> mXCorBuffer;
    AudioBuffer<float> mDelayBufferPitch1; //Array for holding non-circular bit of delay buffer
    AudioBuffer<float> mDelayBufferPitch2; //Array for holding non-circular bit of delay buffer
    int mWritePosition { 0 }; //write position initialised to 0 (not pointer);
    int mSampleRate { 44100 }; //Default sample rate but overridden in the .c code
    int mPitchBufferLength { 4800 }; //0.75*mSampleRate/30.0 for 192kHz for instance
    
    //For getting bpm from a DAW https://forum.juce.com/t/getting-bpm-and-beats/13151/5
    //AudioPlayHead* playHead;
    //AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    //and then you need this in the .c file in the processorBlock
    //playHead = this->getPlayHead();
    //playHead->getCurrentPosition (currentPositionInfo);
    //then use currentPositionInfo.bpm
    
};
