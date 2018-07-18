---
layout: single
title: "Audio and Control ADC together"
date: 2013-07-29 18:24

categories: [updates, adc]
---

Version 2013-07-29-18:09 enables audio and control rate analog inputs to work at the same time.
Audio input is on analog pin A0.  Each time an audio sample is output, the audio input is sampled.
For each audio input sample, one control input is also converted, by changing the input channel and
triggering two conversions in a row using the ADC interrupt, keeping the second reading because the first
is unreliable, then switching the channel back to 0 again for audio input and starting another audio conversion, which
is also discarded.  Then the cycle goes again with the next audio output triggering a second, usable, audio input sample.
Each time, one control input is stored, until the list of analog channels waiting to be sampled is empty.
The audio samples continue alone until the next control interrupt, when a sketch has adcReadAllChannels() in updateControl().  

You can test it with examples/04.Analog_Input/Audio_and_Control_Input and examples/04.Analog_Input/Audio_Input_with_Knob_Filter.  

Also, now the Mozzi library downloads as Mozzi.zip and unzips as Mozzi, instead of sensorium-Mozzi-r4783rth43t or whatever.  


The NEWS.txt entry for this release is:  

- IMPORTANT: renamed RecentAverage.h to RollingAverage.h.  Update sketches accordingly.  
- mozzi_analog.h    
	- added the ability to sample audio and control rate analog inputs at the same time  
	- added options for setupFastAnalogRead(), default is FAST (adc clock div main clock by 16, same as previously),
		and setupFastAnalogRead(FASTER) and FASTEST for divide by 8 and divide by 4.  These faster rates risk being less accurate 
	(see docs for setupFastAnalogRead), but FASTER (or FASTEST) is required for audio input with control inputs at the same time.
- added examples/04.Analog_Input/Audio_and_Control_Input  
- added examples/04.Analog_Input/Audio_Input_with_Knob_Filter  
- removed examples/04.Analog_Input/Audio_Input_with_Filter_Control  
- RollingAverage.h - change unsigned char index to unsigned int and same for num_readings_as_rshift, so more than 255 items can be averaged
- added audioTicks(), returns how many audio samples have been played since the program started - this will overflow less often than mozziMicros().  
- mozziMicros() was wrong, audioticks/us-per-tick... fixed, now audioticks*us-per-tick  
- added Stack.h, a simple stack class used internally for reading analog control inputs via adcReadAllChannels().  
- added RollingStat.h, which calculates an approximation of the variance and standard deviation for a window of recent inputs, 
	for doing things with sensor inputs.  
