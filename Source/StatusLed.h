/*
  ==============================================================================

    StatusLed.h
    Created: 3 Jul 2020 11:03:10am
    Author:  mhamilt7

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

/*!
 @class StatusLed
 @brief Track a boolean value and display
 @discussion <#Talk about what this does in more detail#>
 @namespace <#What is the Namespace if any#>
 @updated <#Date Edited#>
 */
class StatusLed : public juce::Component, private juce::Timer
{
public:
    StatusLed() noexcept;
    //--------------------------------------------------------------------------
    void paint (juce::Graphics&) override;
    void resized () override;
    void visibilityChanged () override;
    void timerCallback () override;
    void setSource(const bool*);
protected:
    //--------------------------------------------------------------------------
    //    <#member functions#>
private:
    //--------------------------------------------------------------------------
    const bool* stateSource = nullptr;
    bool prevState;
    bool ledTurningOn = false;
    float colourInterp = 0.0;
    float colourStep = 0.25;
    int refreshRate = 30;
    
    juce::Colour onColour  = juce::Colour(0xFF33FF33);
    juce::Colour offColour = juce::Colour(0xFF000033);
};
