/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LatentSeerAudioProcessorEditor::LatentSeerAudioProcessorEditor (LatentSeerAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    
    mBPM_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mBPM_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mBPM_Slider.setRange (40u, 240u, 1u);
    //Starting value
    mBPM_Slider.setValue (processor.latentSeer.getBpm());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mBPM_Slider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mBPM_Slider);

    mBeatsPerBar_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mBeatsPerBar_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mBeatsPerBar_Slider.setRange (1.0f, 6.0f, 0.5f);
    //Starting value
    mBeatsPerBar_Slider.setValue (processor.latentSeer.getBeatsPerBar());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mBeatsPerBar_Slider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mBeatsPerBar_Slider);
    
    mGainSlider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mGainSlider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mGainSlider.setRange (0.0f, 1.0f, 0.01f);
    //Starting value
    mGainSlider.setValue (processor.latentSeer.getGain());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mGainSlider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mGainSlider);
    
    mDelay_ms_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mDelay_ms_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mDelay_ms_Slider.setRange (-100L, 0L, 1.0L);
    //Starting value
    mDelay_ms_Slider.setValue (processor.latentSeer.getDelay());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mDelay_ms_Slider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mDelay_ms_Slider);

    mDelayVolume_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mDelayVolume_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mDelayVolume_Slider.setRange (0.0f, 1.0f, 0.01f);
    //Starting value
    mDelayVolume_Slider.setValue (processor.latentSeer.getDelayVolume());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mDelayVolume_Slider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mDelayVolume_Slider);

    mTransientThreshInDelayed_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mTransientThreshInDelayed_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mTransientThreshInDelayed_Slider.setRange (-60.0f, 0.0f, 0.1f);
    //Starting value
    mTransientThreshInDelayed_Slider.setValue (processor.latentSeer.getTransientThreshold());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mTransientThreshInDelayed_Slider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mTransientThreshInDelayed_Slider);

//    mReleaseThreshInDelayed_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
//    //Style of text box (50 and 20 are size):
//    mReleaseThreshInDelayed_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
//    //Min, max, interval:
//    mReleaseThreshInDelayed_Slider.setRange (-60.0f, 0.0f, 0.1f);
//    //Starting value
//    mReleaseThreshInDelayed_Slider.setValue (processor.latentSeer.mReleaseThreshInDelayed);
//    //What's listening for changes in the slider? "this" is a pointer to this class
//    mReleaseThreshInDelayed_Slider.addListener (this);
//    //Make it a visible child of LatentSeerAudioProcessorEditor:
//    addAndMakeVisible (mReleaseThreshInDelayed_Slider);

    mPitchChangeThreshold_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mPitchChangeThreshold_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mPitchChangeThreshold_Slider.setRange (0.0f, 1.0f, 0.01f);
    //Starting value
    mPitchChangeThreshold_Slider.setValue (processor.latentSeer.getPitchThreshold());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mPitchChangeThreshold_Slider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mPitchChangeThreshold_Slider);
    
    mLowestFrequency_Slider.setSliderStyle (Slider::SliderStyle::LinearVertical);
    //Style of text box (50 and 20 are size):
    mLowestFrequency_Slider.setTextBoxStyle (Slider::TextBoxBelow, true, 50, 20);
    //Min, max, interval:
    mLowestFrequency_Slider.setRange (30.0f, 300.0f, 1.0f);
    //Starting value
    mLowestFrequency_Slider.setValue (processor.latentSeer.getLowestFreq());
    //What's listening for changes in the slider? "this" is a pointer to this class
    mLowestFrequency_Slider.addListener (this);
    //Make it a visible child of LatentSeerAudioProcessorEditor:
    addAndMakeVisible (mLowestFrequency_Slider);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (950, 400);
}

LatentSeerAudioProcessorEditor::~LatentSeerAudioProcessorEditor()
{
}

//==============================================================================
void LatentSeerAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("LatentSeer v1.0.1 by Jonathan A. Kemp", getWidth() / 2 - 60, getHeight() / 2 - 190, 120, 40, Justification::centredTop, 2);
    
    //Text boxes above sliders  Text then int x, int y, int width, int height,
    g.drawFittedText ("BPM", getWidth() / 2 - 400, getHeight() / 2 - 190, 100, 20, Justification::centred, 1);
    g.drawFittedText ("Beats Per Bar", getWidth() / 2 - 300, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    g.drawFittedText ("Delay Time (ms)", getWidth() / 2 - 200, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    g.drawFittedText ("Dry Volume", getWidth() / 2 - 100, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    g.drawFittedText ("Delay Volume", getWidth() / 2 + 0, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    g.drawFittedText ("Atack Thresh.", getWidth() / 2 + 100, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    //g.drawFittedText ("Release Thresh.", getWidth() / 2 + 200, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    g.drawFittedText ("Pitch Sensitivity", getWidth() / 2 + 200, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    g.drawFittedText ("Min. Freq. (Hz)", getWidth() / 2 + 300, getHeight() / 2 - 90, 100, 20, Justification::centred, 1);
    //g.setColour(Colours::saddlebrown);
    //g.setColour(Colours::orange);//need to use mDelayBufferThresholdExceeded somehow
    //g.fillEllipse(getWidth() / 2 + 250 - 20/2, getHeight() / 2 - 120, 20, 20);
    //StatusLight(<#StatusLight *light#>);
}

void LatentSeerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    mBPM_Slider.setBounds(getWidth() / 2 - 400, getHeight() / 2 - 175, 100, 365); // position and size
    mBeatsPerBar_Slider.setBounds(getWidth() / 2 - 300, getHeight() / 2 - 75, 100, 150); // position and size
    mDelay_ms_Slider.setBounds(getWidth() / 2 - 200, getHeight() / 2 - 75, 100, 150); // position and size
    mGainSlider.setBounds(getWidth() / 2 - 100 , getHeight() / 2 - 75, 100, 150); // position and size
    mDelayVolume_Slider.setBounds(getWidth() / 2 + 0, getHeight() / 2 - 75, 100, 150); // position and size
    mTransientThreshInDelayed_Slider.setBounds(getWidth() / 2 + 100, getHeight() / 2 - 75, 100, 150); // position and size
    //mReleaseThreshInDelayed_Slider.setBounds(getWidth() / 2 + 200, getHeight() / 2 - 75, 100, 150); // position and size
    mPitchChangeThreshold_Slider.setBounds(getWidth() / 2 + 200, getHeight() / 2 - 75, 100, 150); // position and size
    mLowestFrequency_Slider.setBounds(getWidth() / 2 + 300, getHeight() / 2 - 75, 100, 150); // position and size
}

void LatentSeerAudioProcessorEditor::sliderValueChanged (Slider *slider)
{
    if (slider == &mGainSlider)
    {
        processor.latentSeer.setGain(mGainSlider.getValue());
    }
    if (slider == &mDelay_ms_Slider)
    {
    processor.latentSeer.setDelay(mDelay_ms_Slider.getValue());
    }
    if (slider == &mDelayVolume_Slider)
    {
        processor.latentSeer.setDelayVolume(mDelayVolume_Slider.getValue());
    }
    if (slider == &mBPM_Slider)
    {
        processor.latentSeer.setBpm(mBPM_Slider.getValue());
    }
    if (slider == &mBeatsPerBar_Slider)
    {
        processor.latentSeer.setBeatsPerBar(mBeatsPerBar_Slider.getValue());
    }
    if (slider == &mTransientThreshInDelayed_Slider)
    {
        processor.latentSeer.setTransientThreshold(mTransientThreshInDelayed_Slider.getValue());
        processor.latentSeer.setReleaseThreshold(processor.latentSeer.getTransientThreshold() - 18.0);
    }
//    if (slider == &mReleaseThreshInDelayed_Slider)
//    {
//        processor.latentSeer.mReleaseThreshInDelayed = mReleaseThreshInDelayed_Slider.getValue();
//    }
    if (slider == &mPitchChangeThreshold_Slider)
    {
        processor.latentSeer.setPitchThreshold(mPitchChangeThreshold_Slider.getValue());
    }
    if (slider == &mLowestFrequency_Slider)
    {
        processor.latentSeer.setLowestFreq(mLowestFrequency_Slider.getValue());
    }
}
