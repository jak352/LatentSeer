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

### Install

### Use

## License

The code is provided open source under an MIT License and can be compiled using JUCE to create plugins such as VST3 and AU as well as a standalone application. [See License](./LICENSE)
