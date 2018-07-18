---
layout: single
title: "Teensy 3.0/3.1 test release"
date: 2014-08-09 21:21
comments: false
categories: [ ports, Teensy, releases ]
---


This release is a test of the recent work on the Mozzi Teensy 3 port in response to the donation campaign reaching the grand goal of $1000.
Thankyou to all the dozen or so donors, especially my brother and sister, who donated more than half!  

It should run as usual on the Arduinos already supported, and now also Teensy 3.0 and 3.1 (instructions for Teensies below).
I expect some mistakes and bugs so please let me know on the forum and I'll try to sort them out asap.  

Extra libraries required for use withTeensy 3.0/3.1:

- [Timer library for Teensy 3.0](https://github.com/loglow/IntervalTimer) by Daniel Gilbert

- [ADC library](https://github.com/pedvide/ADC) by Pedro Villanueva 


Some of the differences for Teensy 3.0/3.1 which will affect users include:

- Audio output is on pin A14/DAC, in STANDARD or STANDARD_PLUS audio modes,  These modes are identical on Teensy 3.0/3.1, as
	the output is via DAC rather than PWM.
	
- Output is 12 bits in STANDARD and STANDARD_PLUS modes, up from nearly 9 bits for Atmel based boards.
	HIFI audio, which works by summing two output pins, is not available on Teensy 3.0/3.1.
	
- \#include <ADC.h> is required at the top of every Teensy 3.0/3.1 sketch.  
	The examples come with this commented out, for Arduino compatibility.

- Serial baud rate for monitoring in the IDE needs to be set to 9600 to work with Teensy 3.0/3.1.
	This slow rate can cause audio glitches.
	
- [twi_nonblock](https://github.com/sensestage/Mozzi) code by Marije Baalman for non-blocking I2C is not compatible with Teensy 3.0/3.1.  


Various other changes which affect the library in general:

- Changed license from GPL3 to Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.

- ADSR.h 

	- next() now returns unsigned char instead of unsigned int
	
	- added functions to set ADSR times in update steps, eg. setAttackUpdateSteps(unsigned int steps), etc., 
	for finer-grained control

- SampleHuffman.h - now plays correctly from beginning when start() used before finished playing previous cycle.

- Re-included RCpoll.h and examples>03.Sensors>RCpoll, for capacitive sensing.  
	They have been included before, but disappeared somewhere along the way.
	
- Added PDResonant.h, and examples>06.Synthesis>PDResonant.  PDResonant is a simple midi intrument which demonstrates 
	Phase Distortion used to create a sweeping resonant filter effect, based on 
	https://en.wikipedia.org/wiki/Phase_distortion_synthesis.
	
- mozzi_fixmath.h - added Q15n16_to_Q8n0()

- Fixed broken link in Sensor examples to https://sensorium.github.io/Mozzi/learn/introductory-tutorial/

- Moved Arduino timer libraries into "utility" folder and added conditional compilation to hide them from Teensy 3.0/3.1 compilation.

 - Removed cpu speed warning from MozziGuts.h
