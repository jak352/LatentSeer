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
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    if (stateSource)
    {
        if (ledTurningOn)
        {
            colourInterp += colourStep;
            if (colourInterp >= 1.0)
            {
                colourInterp = 1.01;
                ledTurningOn = false;
            }
        }
        else
        {
            colourInterp -= colourStep;
            if (colourInterp < 0.0)
            {
                colourInterp = 0.0;
            }
        }
        g.setColour (offColour.interpolatedWith(onColour, colourInterp));        
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
            ledTurningOn = true;
            prevState = *stateSource;
        }
         repaint();
    }
    
}

void StatusLed::setSource(const bool *source)
{
    stateSource = source;
}
