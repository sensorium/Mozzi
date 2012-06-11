README for Cuttlefish, a sound synthesis library for Arduino.
Tim Barrass 2010-12


Currently your Arduino can only beep like a microwave oven. Cuttlefish brings
your Arduino to life by allowing it to produce much more complex and interesting
gurgles, sweeps and phasing distortions. These sounds can be quickly and easily
constructed from familiar synthesis units like oscillators, delays, filters and
envelopes.

You can use Cuttlefish to generate algorithmic music for an installation or
performance, or make interactive sonifications of sensors, on a small, modular
and super cheap Arduino, without the need for additional shields, message
passing or external synths.

Cuttlefish is designed to be easy to use, open source and extendable.

If you enjoy using Cuttlefish for a project, or have extended it, we would be
pleased to hear about it and provide support wherever possible. Contribute
suggestions, improvements and bug fixes to the Cuttlefish forum on Github, or
Fork it to contribute directly to future developments.

Cuttlefish is a development of research into Mobile Sonification in the
SweatSonics Project. (http://stephenbarrass.wordpress.com/tag/sweatsonics/)

FEATURES
-    16384 Hz, 8-bit audio output. (Actually, nearly 9 bit resolution, -244 to 243).
-    Variable control rate from 64 Hz upwards.
-    Useful basic audio toolkit: oscillators, samples, lines, envelopes, scheduling, filtering.
-    Fast ADC and other cpu-efficient code utilities to help keep audio running smoothly.
-    Example sketches for easy modification.
-    Readymade wavetables and a script to convert your own soundfiles for Cuttlefish.


INSTALLATION
Download Cuttlefish and unzip it. It will probably have a name like
"fairyland-Cuttlefish-2bee818". Rename the unzipped folder "Cuttlefish". Then,
following the instructions from the Arduino libraries page: Open your Arduino
sketchbook folder. If there is already a folder there called libraries, place
the library folder in there. If not, create a folder called libraries in the
sketchbook folder, and drop the library folder in there. Then re-start the
Arduino programming environment, and you should see your new library in the
Sketch > Import Library menu.

QUICK START
To hear Cuttlefish, connect a 3.5mm audio jack with the centre wire to the PWM
output on Digital pin 9 on Arduino, and the black ground to the Ground on the
Arduino. Use this as a line out which you can plug into your computer and listen
to with a sound program like Audacity. Try some examples. They are in your
File > Examples > Cuttlefish menu.

USING CUTTLEFISH
Here's a minimal template for an empty Cuttlefish sketch:

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


DOCUMENTATION
There's documentation in the doc folder in the Cuttlefish download
and online. (http://fairyland.github.com/Cuttlefish/doc/html/index.html) There
are hints and tips and more help on the Cuttlefish wiki.
(https://github.com/fairyland/Cuttlefish/wiki/Hints-and-Tips) Also, feel free to
submit any issues on the GitHub Cuttlefish site
(https://github.com/fairyland/Cuttlefish/issues)

CAVEATS
This code has been tested on Arduino Dumemilanove ATmega328. Feedback
about performance on other Arduinos welcome. Cuttlefish doesn't play nice with
Arduino time functions millis(), micros(), delay(), delayMicroseconds().
Instead, you can use Cuttlefish's EventDelay() for scheduling.

CONTRIBUTIONS / INCLUDED DEPENDENCIES
Cuttlefish makes use of the following code:

Under GPL3:
TimerOne library - http://code.google.com/p/arduino-timerone/ for latest
TimerTwo library - Downloaded from https://bitbucket.org/johnmccombs, 4/2/2012, but unable to find
original author. Please contact us if this is yours!

Cuttlefish has also drawn on and been influenced by:
ead~.c puredata external (creb library) Copyright (c) 2000-2003 by Tom Schouten (GPL2)
AF_precision_synthesis.pde by Adrian Freed, 2009.
(http://adrianfreed.com/content/arduino-sketch-high-frequency-precision-sine-wave-tone-sound-synthesis)
