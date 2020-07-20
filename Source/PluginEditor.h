/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "StatusLed.h"

//==============================================================================
/**
*/
class LatentSeerAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener
                                        //public StatusLight
{
public:
    LatentSeerAudioProcessorEditor (LatentSeerAudioProcessor&);
    ~LatentSeerAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (Slider* slider) override;
    
    //virtual bool StatusLight (StatusLight* light);
    
private:
    Slider mGainSlider;
    Slider mDelay_ms_Slider;
    Slider mDelayVolume_Slider;
    Slider mBPM_Slider;
    Slider mBeatsPerBar_Slider;
    Slider mTransientThreshInDelayed_Slider;
    //Slider mReleaseThreshInDelayed_Slider;
    Slider mPitchChangeThreshold_Slider;
    Slider mLowestFrequency_Slider;
    StatusLed led;
    LatentSeerAudioProcessor& processor;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LatentSeerAudioProcessorEditor)
};
