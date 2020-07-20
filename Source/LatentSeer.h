/*
 ==============================================================================
 
 LatentSeer.h
 Created: 26 Jun 2020 11:02:21am
 Author:  mhamilt7
 
 ==============================================================================
 */

#pragma once

#include <JuceHeader.h>

/*!
 @class LatentSeer
 @brief <#Quick Description#>
 @discussion <#Talk about what this does in more detail#>
 @namespace <#What is the Namespace if any#>
 @updated <#Date Edited#>
 */
class LatentSeer
{
public:
    //--------------------------------------------------------------------------
    /// Initialise relevant dsp variables
    void setup(double sampleRate, int samplesPerBlock, int numInputChannels);
    ///
    void getFromDelayBuffer (float mDelayVolume,AudioBuffer<float>& buffer,
                             int channel,
                             const int bufferLength,
                             const int delayBufferLength,
                             const float* bufferData,
                             const float* delayBufferData);
    ///
    void getMagnitudeOfDelayBuffer (const int delayBufferLength,
                                    const int readPosition,
                                    const int bufferLength);
    ///
    void getMaxOfXCorOfDelayBuffer (const int delayBufferLength,
                                    const int bufferLength,
                                    const int totalNumInputChannels);
    ///
    void checkIfThresholdExceeded (const int bufferLength);
    /// main DSP
    void process(int totalNumInputChannels, AudioBuffer<float>& buffer);
    ///
    void fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);
    /// discover if there has been a transient over the previous n milliseconds
    /// @param msLookBack milliseconds to look back
    /// @return true if transient false if not
    bool hadTransient(int msLookBack);
    //==========================================================================
    // Getters and Setters
    ///
    int getBpm();
    ///
    void setBpm(int bpm);
    ///
    int getBeatsPerBar();
    ///
    void setBeatsPerBar(int beatsPerBar);
    ///
    int getGain();
    ///
    void setGain(float gain);
    ///
    int getDelay();
    ///
    void setDelay(int delay);
    ///
    float getDelayVolume();
    ///
    void setDelayVolume(float volume);
    ///
    float getTransientThreshold();
    ///
    void setTransientThreshold(float threshold);
    ///
    float getReleaseThreshold();
    ///
    void setReleaseThreshold(float threshold);
    ///
    float getPitchThreshold();
    ///
    void setPitchThreshold(float threshold);
    ///
    float getLowestFreq();
    ///
    void setLowestFreq(float freq);
    
private:
    //--------------------------------------------------------------------------
    // <#private methods#>
private:
    //--------------------------------------------------------------------------
    ///
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
    ///
    bool mDelayBufferThresholdExceeded { false };
    ///
    int mTransientCounter { 0 };
    ///
    int mPitchTransientCounter { 0 };
    float mDelayBufferMagnitude { 0 };
    /// Threshold below which note is assumed to have released to allow future retriggering
    float mReleaseThreshInDelayed { -30.0 };
    /// Threshold below which note is assumed to have released to allow future retriggering
    int mLowestFrequency { 30 };
    ///
    int mReleaseCounter { 0 };
    /// true means no fade
    bool noFadeIn = true;
    /// Number of audio buffers that delay buffer plays for;
    int mNumBuffersForRelease = { 8 };
    /// Number of milliseconds that must be under release threshold for release
    float mReset_ms = { 20 };
    ///
    float mPitchChangeThreshold = { 0.75 };
    ///
    bool mNoFadeOut = { true };
    ///
    bool mNoFadeIn = { true };
    /// Maximum of the xcorrelation between signal and signal a few ms back in delay buffer
    float mNormalisedXCorMax = { 0 };
    /// mSampleRate*0.020 at 192 kHz for instance. Number of samples back in audio to compare across to get cross-correlation for pitch change detection. Calculated in prepare to play. Would need to be careful with size of mDelayBufferPitch if this is not to be changed during runtime
    int mNBack { 3840 };
    /// Counter for offseting lag in cross-correlation
    int mNLagBuffers { 0 };
    /// Counter for getting multiple pitch change detections in a rowv
    int mPitchDetectCounter { 0 };
    /// m means global member variable as opposed p for pointer or mp for member pointer
    AudioBuffer<float> mDelayBuffer;
    ///
    AudioBuffer<float> mXCorBuffer;
    /// Array for holding non-circular bit of delay buffer
    AudioBuffer<float> mDelayBufferPitch1;
    /// Array for holding non-circular bit of delay buffer
    AudioBuffer<float> mDelayBufferPitch2;
    /// write position initialised to 0 (not pointer);
    int mWritePosition { 0 };
    /// Default sample rate but overridden in the .c code
    int mSampleRate { 44100 };
    /// 0.75*mSampleRate/30.0 for 192kHz for instance
    int mPitchBufferLength { 4800 };
    //For getting bpm from a DAW https://forum.juce.com/t/getting-bpm-and-beats/13151/5
    ///
    //AudioPlayHead* playHead;
    ///
    //AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    // and then you need this in the .c file in the processorBlock
    //playHead = this->getPlayHead();
    //playHead->getCurrentPosition (currentPositionInfo);
    //then use currentPositionInfo.bpm
};
