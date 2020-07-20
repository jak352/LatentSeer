/*
 ==============================================================================
 
 StatusLed.cpp
 Created: 3 Jul 2020 11:03:10am
 Author:  mhamilt7
 
 ==============================================================================
 */

#include "StatusLed.h"

StatusLed::StatusLed() noexcept
{
    setOpaque(true);
    startTimerHz (refreshRate);
}

void StatusLed::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::black);
    
    if (stateSource)
    {
        g.setColour ((*stateSource) ? juce::Colours::green : juce::Colours::lightblue);
        g.fillEllipse(0, 0, getWidth(), getHeight());
    }
    
}
void StatusLed::resized ()
{
    
}
void StatusLed::visibilityChanged ()
{
    
}
void StatusLed::timerCallback ()
{
    if (stateSource)
    {
        if (prevState != *stateSource)
        {
            repaint();
            prevState = *stateSource;
        }
    }
    
}

void StatusLed::setSource(const bool *source)
{
    stateSource = source;
}
