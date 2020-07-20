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
    bool a = true;
    int refreshRate = 30;
};
