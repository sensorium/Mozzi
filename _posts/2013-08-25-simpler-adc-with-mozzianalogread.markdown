---
layout: single
title: "Simpler adc with mozziAnalogRead"
date: 2013-08-25 21:20

categories: [ updates, adc ]
---

Version 2013-08-25 evolves the adc developments of the previous release, by
replacing the various ways of reading analog inputs (adcGetResult(),
adcReadAllChannels(), etc.) with mozziAnalogRead(). Use it the same as
analogRead(). It doesn't need any special setup.  

Audio input still uses getAudioInput(), not mozziAnalogRead().

Here's the Mozzi/extras/NEWS.txt entry of the changes in this release:

version 2013-08-25-18:38

- mozzi_analog.h - Much simpler, better analog input.  Now there's just mozziAnalogRead(pin).
	Sorry about breaking your sketches, again.
	mozziAnalogRead completely replaces the arduino analogRead, and analogRead is no longer able to be used with Mozzi.
	No setup is required for mozziAnalogRead, unless you want audio input, which is set in mozzi_config.h as before.
	You can still use setupFastAnalogRead(speed) if you want faster than mozzi's default, 
	which is automatically set if you don't do anything.  See the documentation if you want this.
	If you specially need analogRead for some reason (and you're not using audio input), you can comment out a 
	line in MozziGuts.cpp, in the startMozzi function, where it says setupMozziADC().
- All example sketches using analog reads are updated, or removed where no longer relevant.
- examples/04.Analog_Input changed to 04.Audio_Input
- examples/envelopes/ADSR_Envelope - now plays random envelopes as a more thorough test/demo.
- add examples/envelopes/ADSR_Envelope_x2 - shows an additive sound combining 2 oscillators with individual envelopes.
- Sample.h - added linear interpolation as optional template parameter, eg.:
	Sample <SAMPLE_NUM_CELLS, AUDIO_RATE, INTERP_LINEAR> aSample(SAMPLE_DATA);
	The default parameter is INTERP_NONE.
- removed examples/08.Samples/Sample_Offset too horrible
- added examples/08.Samples/Sample_Scrub...uses interpolation/smoothing at different scales
- Smooth.h - added operator method as an alternative to next() so you can use 
	mySmooth(input-value) instead of mySmooth.next(input-value), if you want.
- mozzi_midi.h, .cpp - changed return type from unsigned int to int for mtof(unsigned char midi_note), mtof(int midi_note),
	to suit Oscil::setFreq()
- added/corrected references to examples in docs
