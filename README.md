Mozzi
=====
sound synthesis library for Arduino
------------------------------------

Version 0.01.1v
Tim Barrass 2010-13


Currently your Arduino can only beep like a microwave oven. Mozzi brings
your Arduino to life by allowing it to produce much more complex and interesting
growls, sweeps and chorusing atmospherics. These sounds can be quickly and easily
constructed from familiar synthesis units like oscillators, delays, filters and
envelopes.

You can use Mozzi to generate algorithmic music for an installation or
performance, or make interactive sonifications of sensors, on a small, modular
and super cheap Arduino, without the need for additional shields, message
passing or external synths.

Mozzi is designed to be easy to use, open source and extendable.

If you enjoy using Mozzi for a project, or have extended it, we would be
pleased to hear about it and provide support wherever possible. Contribute
suggestions, improvements and bug fixes to the Mozzi wiki on Github, or
Fork it to contribute directly to future developments.

Mozzi is a development of research into Mobile Sonification in the
[SweatSonics](http://stephenbarrass.wordpress.com/tag/sweatsonics/) project.



### Features

-    16384 Hz sample rate, with 8 bit or 14 bit audio output modes. 
-    Variable control rate from 64 Hz upwards.
-    Useful basic audio toolkit: oscillators, samples, lines, envelopes, scheduling, filtering.
-    Fast ADC and other cpu-efficient code utilities to help keep audio running smoothly.
-    Example sketches for easy modification.
-    Readymade wavetables and a script to convert your own soundfiles for Mozzi.


### Installation

Download Mozzi and unzip it. It will probably have a name like
"sensorium-Mozzi-2bee818". Rename the unzipped folder "Mozzi".
Then, following the instructions from the Arduino libraries page:
Open your Arduino sketchbook folder. If there is already a folder there called
libraries, place the library folder in there. If not, create a folder called
libraries in the sketchbook folder, and drop the Mozzi folder in there. Then
re-start the Arduino programming environment, and you should see Mozzi in the
Sketch > Import Library menu.


### Quick Start

To hear Mozzi, connect a 3.5mm audio jack with the centre wire to the PWM
output on the Digital Pin for your board according to the table below. 
Use this as a line out which you can plug into your computer and listen
to with a sound program like Audacity. Try some examples from the
File > Examples > Mozzi menu.

Below is a list of the Digital Pins used by Mozzi for audio out on different boards.
Those which have been tested and reported to work have an x.
Feedback about others is welcome.

x	 9	Arduino Uno  
x	 9	Arduino Duemilanove  
x	 9	Arduino Nano  
x	 9	Arduino Leonardo  
x	 9  Ardweeny  
x	11  Freetronics EtherMega  
..14	Teensy  
x	B5  Teensy2  
x	B5(25) Teensy2++  
x	11	Arduino Mega  
..13	Sanguino  
x	 9  Boarduino  


### Using Mozzi

Here's a template for an empty Mozzi sketch:

	#include <MozziGuts.h>         // at the top of your sketch
	#define CONTROL_RATE 64 // or some other power of 2

	void setup() {
		startMozzi(CONTROL_RATE);
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


### Documentation

There's documentation in the doc folder in the Mozzi download
and [online] (http://sensorium.github.com/Mozzi/doc/html/index.html).  
There are hints and tips and more help on the Mozzi [wiki]
(https://github.com/sensorium/Mozzi/wiki/Hints-and-Tips).  
Start or look up a topic on the Mozzi [users forum]
(https://groups.google.com/forum/#!forum/mozzi-users).  
Also, feel free to submit any issues on the [GitHub Mozzi site]
(https://github.com/sensorium/Mozzi/issues).  
Share [code snippets]
(https://github.com/sensorium/Mozzi/wiki/Code-Snippets-to-Share).  
Check for code and usage changes in NEWS.txt.


### Caveats

Mozzi disables Arduino time functions millis(), micros(), delay(), and
delayMicroseconds(). Instead, you can use Mozzi's EventDelay() for scheduling.


###Contributions / Included Dependencies

Mozzi makes use of the following code:

[TimerOne library] (http://www.pjrc.com/teensy/td_libs_TimerOne.html)
[FrequencyTimer2 library] (http://www.pjrc.com/teensy/td_libs_FrequencyTimer2.html) 
[xorshift] (http://www.jstatsoft.org/v08/i14/xorshift.pdf) random number generator, George Marsaglia, (2003)


Mozzi has also drawn on and been influenced by (among many others):

ead~.c puredata external (creb library) Copyright (c) 2000-2003 by Tom Schouten (GPL2)  
[AF_precision_synthesis](http://adrianfreed.com/content/arduino-sketch-high-frequency-precision-sine-wave-tone-sound-synthesis)
by Adrian Freed, 2009.  
[Resonant filter](http://www.musicdsp.org/archive.php?classid=3#259) posted to musicdsp.org by Paul Kellett,
and fixed point version of the filter on [dave's blog of art and programming]
(http://www.pawfal.org/dave/blog/2011/09/)
State Variable filter pseudocode at http://www.musicdsp.org/showone.php?id=23 and http://www.musicdsp.org/showone.php?id=142.
