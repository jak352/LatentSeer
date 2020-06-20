# LatentSeer
**by Jonathan Kemp**

- [LatentSeer](#latentseer)
	- [About](#about)
	- [ToDo](#todo)
	- [Instructions](#instructions)
		- [Install](#install)
		- [Use](#use)
	- [License](#license)

## About

A plugin to alleviate audio latency or provide percussive effects through mixing in tempo-matched delayed transients or predictive audio. This program was initiated by Dr Jonathan A. Kemp of Kemp Strings and the University of St Andrews.

Users of the plugin can specify a BPM and number of beats per bar. The software detects transients based in the audio coming into the audio buffer based on a combination of amplitude triggering and pitch change detection. The output features transients from a bar previously which are inserted into the raw audio. A negative delay time can be specified so that the transients play early of the beat. This is designed to help with remote jamming (as often latencies of around 40 ms can cause players to slow down and fall out of sync when using remote jamming over the internet). The effect is also useful for generated a glitchy percussive accompaniment during music making in general.

## ToDo

- [ ] A level meter next to the amplitude threshold control
- [ ] Installers for mac and windows
- [ ] The ability to remember/automate settings in the DAW
- [ ] A light or indicator showing that a transient is being played back.
- [ ] Fade ins and fade outs over certain number of ms instead of the whole buffer

## Instructions

Play along to a metronome (say by switching on the metronome in the DAW/host). Currently the BPM on the plugin should be set manually to match the BPM of the metronome. When the sound level coming into the effect changes from a quiet level to a level over the "Attach thresh." decibel value then that transient attack will be played back one bar later based on the Beats Per Minute ("BPM") and number of "Beats per Bar" with a time offset determined by the "Delay Time (ms)" control. When the "Delay Time" is equal to -40 milliseconds, the transient sounds 40 milliseconds earlier than it was played in the previous bar. Similarly, when the musical pitch changes, the start of the new pitch is played back one bar later with the timing offset by the value in the "Delay Time (ms)" setting. Pitch change detection is based on the cross-correlation of the signal with a delayed version of the signal (with a normalisation applied in the manner of Philip McLeod and Geoff Wyvill's paper "A SMARTER WAY TO FIND PITCH"). Increasing the pitch sensitivity means more frequency pitch change detections (at the risk of false pitch change detections). The "Min. Freq. (Hz)" control should be increased for detection of pitch changes on higher pitch instruments (as this controls the length of the time that the normalised cross-correlation is required to be under the "Pitch Sensitivity" value before a pitch change is detected). The volume of the "Dry" and "Delay" signals can be independently controlled with the original volumes achieved with both set to 1.

### Install

The current software can be used by installing JUCE and using the Projucer software. The file LatentSeer.jucer and source code can be used within an effect plugin. Open in IDE (XCode on mac or Virtual Studio 2019 on windows) then choose build. This generates standalone app and VST3 plugins (and AU plugin .component file). The plugins are visible to the host on Mac and may be found in the Builds folder for windows. In windows the VST3 can be manually copied into C:/Program Files/Common FIles/VST3 and a DAW such as REAPER opened. On Mac the plugin is installed in the user's library so it is detected when the DAW is opened. 

### Use

Add the effect VST or AU plugin onto a track within the DAW/host to process the sound within the track. For standalone operation the windows .exe or Mac OSX app can be opened and the menu used to select inputs and outputs.

Currently, if using online jamming software such as JamKazam or soundjack.eu then the plugin or standalone app should have its output fed into the input of the jamming software using some kind of digital or analogue loopback, or the software could be used at the receiving end. The ideas is for transients to hit the beat (with the negative "Delay Time (ms)" in LatentSeer approximately matching the latency over for internet communication) with the sustain of the pitches played following immediately after.

The effect can also be used as a quirky delay effect in general.

## License

The code is provided open source under an MIT License and can be compiled using JUCE to create plugins such as VST3 and AU as well as a standalone application. [See License](./LICENSE)
