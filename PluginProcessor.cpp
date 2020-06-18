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
    
    mSampleRate = sampleRate;//Get sample rate
    
    const int numInputChannels = getTotalNumInputChannels();
    const int delayBufferSize = ((6*60/40) * sampleRate) + samplesPerBlock; //6 beats of 40bpm plus a little bit
    
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
    
    //mPitchDetectCounter = 0;

    
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
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = mDelayBuffer.getNumSamples();
    
    //Check if buffer length has changed:
    if ( bufferLength != mXCorBuffer.getNumSamples() )
    {
        mXCorBuffer.setSize(totalNumInputChannels, bufferLength);
        mXCorBuffer.clear();
    }
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, bufferLength);

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    
    //delay in samples where mDelay_ms is negative for latency removal
    //Make detection happen some milliseconds ahead of intended playback to low amp start of trasient (used to be 6 buffers)
    float delaySamples = mSampleRate * ( mBeatsPerBar * ( 60.0 / mBPM ) + ( mDelay_ms - 10.0 ) / 1000.0 );//note mDelay_ms is negative
    const int readPosition = static_cast<int>( (delayBufferLength + mWritePosition) - delaySamples) % delayBufferLength; //first delayBufferLength is somewhat redundant as modulo delayBufferLength but saves modulo of negative argument confusions. Not a pointer

    
    //get information on delay buffer volume here across all channels
    //probably prefereable in case panned material is above threshold on one channel and just below threshold on other
    //Update mDelayBufferMagnitude:
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
    
    //Check if threshold is exceeded in delay buffer based on processing all channels:
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

//void LatentSeerAudioProcessor::findMinMaxDelayBuffer (int channel, const int bufferLength, //const int delayBufferLength, const float* bufferData, const float* delayBufferData)
//{
    //check
//}

void LatentSeerAudioProcessor::fillDelayBuffer (int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
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

void LatentSeerAudioProcessor::checkIfThresholdExceeded(const int bufferLength)
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
    else if ( ( mDelayBufferMagnitude > Decibels::decibelsToGain (mTransientThreshInDelayed) ) & ( mTransientCounter == 0 ) & ( ( mReleaseCounter > mNumBuffersForRelease ) || (
        mPitchDetectCounter == mNBack ) ) )
        //mNormalisedXCorMax < mPitchChangeThreshold ) ) )
    {
        mDelayBufferThresholdExceeded = true; //start transient playback (with mTransientCounter having been zero)
        mReleaseCounter = 0;
        mNoFadeIn = false;
        mTransientCounter = 1;
        //mPitchDetectCounter++;//Put change detect counter out of range so can only be retriggered when under threshold is detected.
    }
    else if ( mDelayBufferThresholdExceeded == true )
    {
        mTransientCounter++; // continue transient playback
        mReleaseCounter = 0;
    }
    else if ( (mReleaseCounter < mNumBuffersForRelease + 1) & ( mDelayBufferMagnitude < Decibels::decibelsToGain ( mReleaseThreshInDelayed ) ) )
    {
        mReleaseCounter++; // count release time while transient has recently turned off
    }
    //Check if we are not fading in or fading out the delay transient:
    mNoFadeOut = ( mTransientCounter != mNumBuffersForRelease + 1 );
    //mNoFadeIn = ( mTransientCounter != 0 );
}

void LatentSeerAudioProcessor::getFromDelayBuffer (float mDelayVolume, AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData)
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

void LatentSeerAudioProcessor::getMagnitudeOfDelayBuffer (const int delayBufferLength, const int readPosition, const int bufferLength)
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

void LatentSeerAudioProcessor::getMaxOfXCorOfDelayBuffer ( const int delayBufferLength, const int bufferLength, const int totalNumInputChannels )
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
