/*
 ==============================================================================
 
 LatentSeer.cpp
 Created: 26 Jun 2020 11:02:21am
 Author:  mhamilt7
 
 ==============================================================================
 */

#include "LatentSeer.h"


void LatentSeer::setup(double sampleRate, int samplesPerBlock, int numInputChannels)
{
    mSampleRate = sampleRate; // Get sample rate
    
    const int delayBufferSize = ((6*60/40) * sampleRate) + samplesPerBlock; // 6 beats of 40bpm plus a little bit
    
    //Number of buffers back to look back on to assess whether pitch has changed:
    mNBack = static_cast<int>( sampleRate * 0.020 / samplesPerBlock );
    if ( mNBack == 0 )
    {
        mNBack = 1;
    }
    //Counter for varying bit of delay buffer used for cross-correlation:
    //mNLagBuffers = 0;
    
    //Number of periods of min frequency turned in samples for pitch analysis buffer size:
    //Assume 30 Hz minimum?
    mPitchBufferLength = samplesPerBlock * static_cast<int>( sampleRate / ( mLowestFrequency  * samplesPerBlock ) );
    
    mDelayBuffer.setSize(numInputChannels, delayBufferSize);
    //zero the contents as otherwise we can get noise on decreasing BPM:
    mDelayBuffer.clear();
    
    
    mXCorBuffer.setSize(numInputChannels, samplesPerBlock);
    mXCorBuffer.clear();
    
    mDelayBufferPitch1.setSize( numInputChannels, mPitchBufferLength );
    mDelayBufferPitch1.clear();
    mDelayBufferPitch2.setSize( numInputChannels, mPitchBufferLength );
    mDelayBufferPitch2.clear();
    
    //mPitchDetectCounter = 0
}
//------------------------------------------------------------------------------
void LatentSeer::getFromDelayBuffer (float mDelayVolume, AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    //delay in samples. Make playback happen earlier than detection by some buffers
    float delaySamples = mSampleRate * (mBeatsPerBar*(60.0/mBPM) + ( mDelay_ms - 9.0 ) /1000.0); //Was 3 buffers
    //Position to look in delay buffer to find delay from approriate time:
    const int readPosition = static_cast<int>( (delayBufferLength + mWritePosition) - delaySamples) % delayBufferLength; //first delayBufferLength is somewhat redundant as modulo delayBufferLength. Not a pointer
    
    if ( mDelayBufferThresholdExceeded )
    {
        if (delayBufferLength > bufferLength + readPosition)
        {
            //inputs are destination channel, destination start poition, pointer to delay buffer source position, number of samples, gain
            buffer.addFromWithRamp ( channel, 0, delayBufferData + readPosition, bufferLength, mNoFadeIn * mDelayVolume, mNoFadeOut * mDelayVolume ); //if we're not running off the end of the delay buffer then add in the delayed signal to (output) buffer
        }
        else
        {
            const int bufferRemaining = delayBufferLength - readPosition;
            const float wrapGain = ( ( 1 - mNoFadeIn ) * ( bufferRemaining/bufferLength ) + ( 1 - mNoFadeOut ) * ( 1 - ( bufferRemaining/bufferLength ) ) + mNoFadeOut );
            // Scale fade in and fade out according to (bufferRemaining/bufferLength) ratio
            buffer.addFromWithRamp ( channel, 0, delayBufferData + readPosition, bufferRemaining, mNoFadeIn * mDelayVolume, wrapGain * mDelayVolume );
            buffer.addFromWithRamp ( channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining, wrapGain * mDelayVolume, mDelayVolume );//with correction to video 41 by Alessandro to start at start of delayBufferdata when wrapping back
        }
    }
    //buffer.applyGain( mNormalisedXCorMax ); //test
}
//------------------------------------------------------------------------------
void LatentSeer::getMagnitudeOfDelayBuffer (const int delayBufferLength, const int readPosition, const int bufferLength)
{
    //mDelayBufferMagnitude = 0;
    if (delayBufferLength > bufferLength + readPosition)
    {
        mDelayBufferMagnitude = mDelayBuffer.getMagnitude (readPosition, bufferLength);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - readPosition;
        float delayBufferMagnitude_1;
        float delayBufferMagnitude_2;
        //Find magnitude at end of delay buffer, then at start after wrap around
        delayBufferMagnitude_1 = mDelayBuffer.getMagnitude(readPosition, bufferRemaining);
        delayBufferMagnitude_2 = mDelayBuffer.getMagnitude(0, bufferLength - bufferRemaining);
        mDelayBufferMagnitude = std::max(delayBufferMagnitude_1,delayBufferMagnitude_2);
    }
}
//------------------------------------------------------------------------------
void LatentSeer::getMaxOfXCorOfDelayBuffer ( const int delayBufferLength, const int bufferLength, const int totalNumInputChannels )
{
    //            Pitch Change Detector
    //Check if cross correlation between nearby buffers is over threshold?
    
    //delay in samples. Make playback happen earlier than detection by some buffers
    const int delaySamplesInt = static_cast<int>( mSampleRate * ( mBeatsPerBar * ( 60.0 / mBPM ) + ( mDelay_ms - 9.0 ) / 1000.0 ) );
    
    //Temporary variables:
    //float tmp1;
    //float tmp2;
    float xCor;
    float sumSq;
    //int offset;
    mNormalisedXCorMax = 0;
    //Make integer numner of samples to add to delay to make sure we pick up pitch changes at correct time as changes happen within buffers of finite length:
    const int pitchDetectOffset = static_cast<int>(0.36*mPitchBufferLength);
    //const int pitchDetectOffset = 0;
    
    //Number of buffers back to look back on to assess whether pitch has changed:
    mNBack = static_cast<int>( mSampleRate * 0.020 / bufferLength );
    if ( mNBack == 0 )
    {
        mNBack = 1;
    }
    
    //Counter for varying bit of delay buffer used for cross-correlation:
    mNLagBuffers = (mNLagBuffers + 1) % mNBack;
    
    //Relative postion of delay in pitch buffer relative to main delay buffer
    int delayBufferPitchPos1 = delayBufferLength + mWritePosition - delaySamplesInt - ( mNBack + mNLagBuffers ) * bufferLength - pitchDetectOffset;
    int delayBufferPitchPos2 = delayBufferLength + mWritePosition - delaySamplesInt - pitchDetectOffset;
    
    //MATLAB (n is buffer number so not needed here, m is separation to loop over):
    //tmp1 = y_no_win(1:N_buffer-m,n-N_back);
    //tmp2 = y_no_win(m+1:N_buffer,n);
    //y_cor(m,n) = (tmp1')*(tmp2);
    //m_t(m,n) = tmp1'*tmp1 + tmp2'*tmp2;
    //n_t = 2*y_cor./m_t;
    //maximum delay used is delaySamplesInt + mNBack*bufferLength
    //minimum delay used is delaySamplesInt - (mPitchBufferLength - 1)
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        //Get pointer to now in the delay buffer:
        auto* delayBufferData = mDelayBuffer.getReadPointer ( channel );
        auto* xCorBufferWrite = mXCorBuffer.getWritePointer ( channel );
        auto* delayBufferPitchWrite1 = mDelayBufferPitch1.getWritePointer ( channel );
        auto* delayBufferPitchWrite2 = mDelayBufferPitch2.getWritePointer ( channel );
        //fill new buffer so we don't need mod command within loop over m:
        for (int sample = 0; sample < mPitchBufferLength; ++sample)// + mNBack + ( mNLagBuffers * bufferLength
        {
            delayBufferPitchWrite1 [ sample ] = delayBufferData [ ( delayBufferPitchPos1 + sample ) % delayBufferLength ];
            delayBufferPitchWrite2 [ sample ] = delayBufferData [ ( delayBufferPitchPos2 + sample ) % delayBufferLength ];
        }
        //Get pointer to now in the new samller buffer with circularity removed:
        auto* delayBufferPitchData1 = mDelayBufferPitch1.getReadPointer(channel);
        auto* delayBufferPitchData2 = mDelayBufferPitch2.getReadPointer(channel);
        for (int m = 0; m < bufferLength; ++m)//Just loop over bufferLength
        {
            xCor = 0;//Reset cross correlation for new overlap distance
            sumSq = 0;//Reset sum of squares for new overlap distance
            for (int sample = 0; sample < mPitchBufferLength - m; ++sample)
            {
                //tmp1 = delayBufferPitchData1 [ sample ];
                //tmp2 = delayBufferPitchData2 [ sample + m ];
                //Cross-correlation Type II (tapering) no windowing:
                //xCor += tmp1 * tmp2;
                //Sum of Squares
                //sumSq += (tmp1 * tmp1) + (tmp2 * tmp2);
                xCor += delayBufferPitchData1 [ sample ] * delayBufferPitchData2 [ sample + m ];
                sumSq += ( delayBufferPitchData1 [ sample ] * delayBufferPitchData1 [ sample ] ) + ( delayBufferPitchData2 [ sample + m ] * delayBufferPitchData2 [ sample + m ] );
            }//End of loop through samples in entire pitch buffer
            if ( sumSq != 0 )
            {
                xCorBufferWrite [ m ] = 2*xCor/sumSq;
            }
            else
            {
                xCorBufferWrite [ m ] = 1;
            }
        }//End of loop through lags
    }//End of loop through channels
    mNormalisedXCorMax = mXCorBuffer.getMagnitude (0, bufferLength);//Max absolute value in all channels
    //Check if new amplitude is under release threshold
    if ( mDelayBufferPitch2.getMagnitude (mPitchBufferLength - bufferLength, bufferLength )  < mReleaseThreshInDelayed )
    {
        mNormalisedXCorMax = 1;
    }
    
}
//------------------------------------------------------------------------------
void LatentSeer::checkIfThresholdExceeded(const int bufferLength)
{
    //number of buffers before release from transient detection where mDelay_ms is negative for latency removal:
    float sustain_ms = - mDelay_ms + 9.0;
    if (sustain_ms < 20 )
    {
        sustain_ms = 20;
    }
    else if (sustain_ms > 40 )
    {
        sustain_ms = 40;
    }
    mNumBuffersForRelease = static_cast<int>( ( mSampleRate * ( sustain_ms /1000.0 ) )/bufferLength );
    
    //Assume no fade in or out unless set otherwise later:
    mNoFadeIn = true;
    
    //We need mNormalisedXCorMax < mPitchChangeThreshold to happen mNBack times in a row to confirm a pitch change detection
    if (mNormalisedXCorMax < mPitchChangeThreshold)
    {
        if ( mPitchDetectCounter < mNBack )
        {
            mPitchDetectCounter++;//note will trigger when mPitchDetectCounter gets to mNBack
        }
        //        else if ( mPitchDetectCounter < mNBack + 2 )
        //        {
        //            mPitchDetectCounter++; // can't retrigger immediately on continuous pitch change detection
        //        }
        //        else
        //        {
        //            mPitchDetectCounter = 0;//We need mNBack in a row to get detection
        //        }
    }
    else
    {
        mPitchDetectCounter = 0;//We need mNBack in a row to get detection
    }
    
    // Check if threshold exceeded
    if ( mTransientCounter > mNumBuffersForRelease )
    {
        mDelayBufferThresholdExceeded = false; // stop transient playback
        mTransientCounter = 0;
        mReleaseCounter = 0;
        mPitchDetectCounter = 0;//Stops note off causing mPitchDetectCounter to get stuck high
    }
    else if ( ( mDelayBufferMagnitude > Decibels::decibelsToGain (mTransientThreshInDelayed) ) && ( mTransientCounter == 0 ) && ( ( mReleaseCounter > mNumBuffersForRelease ) || (
                                                                                                                                                                                mPitchDetectCounter == mNBack ) ) )
        //mNormalisedXCorMax < mPitchChangeThreshold ) ) )
    {
        mDelayBufferThresholdExceeded = true; //start transient playback (with mTransientCounter having been zero)
        mReleaseCounter = 0;
        mNoFadeIn = false;
        mTransientCounter = 1;
        //mPitchDetectCounter++;//Put change detect counter out of range so can only be retriggered when under threshold is detected.
    }
    else if (mDelayBufferThresholdExceeded)
    {
        mTransientCounter++; // continue transient playback
        mReleaseCounter = 0;
    }
    else if ( (mReleaseCounter < (mNumBuffersForRelease + 1)) && ( mDelayBufferMagnitude < Decibels::decibelsToGain ( mReleaseThreshInDelayed ) ) )
    {
        mReleaseCounter++; // count release time while transient has recently turned off
    }
    //Check if we are not fading in or fading out the delay transient:
    mNoFadeOut = ( mTransientCounter != (mNumBuffersForRelease + 1));
    //mNoFadeIn = ( mTransientCounter != 0 );
}
//------------------------------------------------------------------------------
void LatentSeer::fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
{
    
    //copy data from main buffer to delay buffer
    if (delayBufferLength > bufferLength + mWritePosition) //if the position we are writing to in the delay buffer is not off the end of the delay buffer
    {
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferLength,1.0, 1.0);
    }
    else //else we have to wrap round as we would have to write off the end of the delay buffer otherwise
    {
        const int bufferRemaining = delayBufferLength - mWritePosition;
        mDelayBuffer.copyFromWithRamp(channel, mWritePosition, bufferData, bufferRemaining, 1.0, 1.0);
        mDelayBuffer.copyFromWithRamp(channel, 0, bufferData + bufferRemaining, bufferLength - bufferRemaining, 1.0, 1.0); // Note correction by ffAudio
    }
    
}
//------------------------------------------------------------------------------
void LatentSeer::process(int totalNumInputChannels, AudioBuffer<float>& buffer)
{
    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = mDelayBuffer.getNumSamples();
    
    //Check if buffer length has changed:
    if ( bufferLength != mXCorBuffer.getNumSamples() )
    {
        mXCorBuffer.setSize(totalNumInputChannels, bufferLength);
        mXCorBuffer.clear();
    }
    float delaySamples = mSampleRate * ( mBeatsPerBar * ( 60.0 / mBPM ) + ( mDelay_ms - 10.0 ) / 1000.0 );//note mDelay_ms is negative
    const int readPosition = static_cast<int>( (delayBufferLength + mWritePosition) - delaySamples) % delayBufferLength; //first
    getMagnitudeOfDelayBuffer (delayBufferLength, readPosition, bufferLength);
    if ( mPitchChangeThreshold < 1 )//Check if pitch change theshold is non-trivial
    {
        //Check maximum of cross correlation between buffer and buffer some milliseconds before
        //This is low if the pitch is changing or note is stopping:
        getMaxOfXCorOfDelayBuffer ( delayBufferLength, bufferLength, totalNumInputChannels  );
    }
    else
    {
        mNormalisedXCorMax = 1;
    }
    checkIfThresholdExceeded ( bufferLength );
    
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        
        //pointers to the current starting read positions in the buffers:
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = mDelayBuffer.getReadPointer(channel);
        
        //Use function below to put main buffer into circular buffer:
        fillDelayBuffer (channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        
        //pointer to the current write position in buffer
        //auto* channelData = buffer.getWritePointer (channel);
        //for (int sample = 0; sample < bufferLength; ++sample)
        //{
        //Change data in buffer:
        //channelData [sample] = channelData [sample] * mGain; //Gain
        //}
        
        //Apply gain to existing buffer contents
        if ( mDelayBufferThresholdExceeded == false )
        {//if no transient being played from delay buffer use normal buffer with (mGain applied)
            buffer.applyGainRamp(channel, 0, bufferLength, mGain, mGain);
        }
        else // use mNoFadeOut which is false if delay transient is fading out
        {// use mNoFadeIn which is false if delay transient is fading in
            buffer.applyGainRamp(channel, 0, bufferLength, (1 - mNoFadeIn) * mGain, (1 - mNoFadeOut) * mGain);
        }
        
        //Use function below to add data from delayBuffer into the main buffer write position
        getFromDelayBuffer(mDelayVolume, buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        
    }//End of through channels
    
    //Move write position in delay buffer on by audio buffer length compared to last audio buffer;
    mWritePosition += bufferLength;
    //modulo to wrap to start of buffer when needed
    mWritePosition %= delayBufferLength;
}

bool LatentSeer::hadTransient(int msLookBack)
{
    return (mTransientCounter > 0);
}
//==============================================================================
// Getters and Setters
///
int LatentSeer::getBpm()
{
    return mBPM;
}
///
void LatentSeer::setBpm(int bpm)
{
}
///
int LatentSeer::getBeatsPerBar()
{
    return mBeatsPerBar;
}

///
void LatentSeer::setBeatsPerBar(int beatsPerBar)
{
}
///
int LatentSeer::getGain()
{
    return mGain;
}
///
void LatentSeer::setGain(float gain)
{
}
///
int LatentSeer::getDelay()
{
    return mDelay_ms;
}
///
void LatentSeer::setDelay(int delay)
{
}
///
float LatentSeer::getDelayVolume()
{
    return mDelayVolume;
}
///
void LatentSeer::setDelayVolume(float volume)
{
}
///
float LatentSeer::getTransientThreshold()
{
    return mTransientThreshInDelayed;
}
///
void LatentSeer::setTransientThreshold(float threshold)
{
}
///
float LatentSeer::getReleaseThreshold()
{
    return mReleaseThreshInDelayed;
}
///
void LatentSeer::setReleaseThreshold(float threshold)
{
}
///
float LatentSeer::getPitchThreshold()
{
    return mPitchChangeThreshold;
}
///
void LatentSeer::setPitchThreshold(float threshold)
{
}
///
float LatentSeer::getLowestFreq()
{
    return mLowestFrequency;
}
///
void LatentSeer::setLowestFreq(float freq)
{
}


