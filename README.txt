README for Cuttlefish, a sound synthesis library for Arduino.
Tim Barrass 2010-12


Cuttlefish is a sound synthesis library for Arduino.

Cuttlefish extends the acoustic range of your bare Arduino from a simple "beep" tone to a wide vocabulary of organic gurgles, glides and sweeps. These more complex and engaging can be quickly and easily constructed from familiar synthesis units like oscillators, delays, filters and envelopes.

This allows you to use Cuttlefish to generate algorithmic music in an installation or performance, or interactive sonifications of sensors, without the need for additional shields, external synthesisers, and message passing protocols.  As a side benefit Cuttlefish also radically speeds up data acquisition from sensors.

Listen to some example recordings in the Cuttlefish/examples_recordings folder.

It is designed to be easy to use, open source. and extendable.
If you enjoy using Cuttlefish for a Project, or have extended it, I would be pleased to hear about it and provide support wherever possible. Contribute suggestions, improvements and bug fixes to the Cuttlefish forum on Github, or Fork it to contribute directly to future developments.

FEATURES
- 	16384 Hz, 8-bit audio output.  (Actually, nearly 9 bit resolution, -244 to 243).
- 	Variable control rate from 64 Hz upwards.
- 	Useful basic audio toolkit: oscillators, samples, lines, envelopes, scheduling, filtering.
-	Fast ADC and other cpu-efficient code utilities to help keep audio running smoothly.
-	Example sketches for easy modification.


INSTALLATION
Put Cuttlefish into your libraries folder.  Restart Arduino so it can find Cuttlefish.

CAVEAT
This code has been tested on Arduino Dumemilanove ATmega328.
Feedback about performance on other Arduinos welcome.


QUICK START
To hear Cuttlefish connect a 3.5mm audio jack with the centre wire to the PWM output on Digital pin 9 on Arduino, and the black ground to the Ground on the Arduino.
Try some examples.  They are in your File>Examples>Cuttlefish menu.


USING CUTTLEFISH
You could paste this into an empty sketch as a template:

#include <Cuttlefish.h> // at the top of your sketch
#define CONTROL_RATE 64 // or some other power of 2

void setup() {
	StartCuttlefish(CONTROL_RATE);
}

void loop() {
	audioHook();
}

int updateAudio(){
	// your audio code which returns an int between -244 and 243
	// actually, a char is fine
}

void updateControl(){
	// your control code
}


AUDIO OUTPUT

HINTS AND TIPS
Cuttlefish doesn't play nice with  Arduino time functions millis(), micros(), delay(), delayMicroseconds().
Instead, you can use TimeBomb for scheduling.  See the EventDelay example.

A computer line-in works well for listening, and you can record and examine your
sounds with Audacity (http://audacity.sourceforge.net/) and
Baudline signal analyser (http://www.baudline.com/).


CONTRIBUTIONS / INCLUDED DEPENDENCIES
Cuttlefish makes use of the following code:
Under GPL3:
TimerOne library - See Google Code project http://code.google.com/p/arduino-timerone/ for latest
TimerTwo library - Downloaded from https://bitbucket.org/johnmccombs, 4/2/2012, but unable to find original author.  Please contact us if this is yours!
Under GPL2:
Ead - adapted from ead~.c puredata external (creb library) Copyright (c) 2000-2003 by Tom Schouten
