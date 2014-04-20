#Mozzi  

###sound synthesis library for Arduino  


Tim Barrass  
version 2014-04-20-15:06  

Currently your Arduino can only beep like a microwave oven. Mozzi brings
your Arduino to life by allowing it to produce much more complex and interesting
growls, sweeps and chorusing atmospherics. These sounds can be quickly and easily
constructed from familiar synthesis units like oscillators, delays, filters and
envelopes.

You can use Mozzi to generate algorithmic music for an installation or
performance, or make interactive sonifications of sensors, on a small, modular
and super cheap Arduino, without the need for additional shields, message
passing or external synths.

***

## Features  
-    16384 Hz sample rate, or experimental 32768 Hz rate.
-    8 bit or 14 bit audio output modes.
-    Variable control rate from 64 Hz upwards.
-    Useful basic audio toolkit: oscillators, samples, lines, envelopes, scheduling, filtering.
-    Fast ADC and other cpu-efficient code utilities to help keep audio running smoothly.
-    Example sketches for easy modification.
-    Readymade wavetables and a script to convert your own soundfiles for Mozzi.  
-    Mozzi is designed to be easy to use, open source and extendable.

***

## Installation  
Download Mozzi from the top of this page and unzip it. It will probably have a
name like "sensorium-Mozzi-2bee818". Rename the unzipped folder "Mozzi". Then,
following the instructions from the [Arduino libraries guide](http://arduino.cc/en/Guide/Libraries):  

In the Arduino IDE, navigate to __Sketch > Import Library__. At the top of the drop
down list, select the option to __Add Library__. Return to the __Sketch > Import Library__ menu. 
You should now see the library at the bottom of the drop-down
menu. It is ready to be used in your sketch.

***

## Quick Start  
To hear Mozzi, connect a 3.5mm audio jack with the centre wire to the PWM output
on Digital Pin 9\* on Arduino, and the ground to the Ground on the Arduino.
Use this as a line out which you can plug into your computer and listen to with
a sound program like [Audacity](http://audacity.sourceforge.net/). 
Try some examples from the __File > Examples > Mozzi__ menu.  

Below is a list of the Digital Pins used by Mozzi for STANDARD mode PWM audio out on different boards.
Those which have been tested and reported to work have an x.
Feedback about others is welcome.

x	 9	Arduino Uno  
x	 9	Arduino Duemilanove  
x	 9	Arduino Nano  
x	 9	Arduino Pro Mini  
x	 9	Arduino Leonardo
x	11	Arduino Mega  
x	11  Freetronics EtherMega  
x	 9  Ardweeny  
x	 9  Boarduino
..14	Teensy  
x	B5  Teensy2  
x	B5(25) Teensy2++  
..13	Sanguino  

For details about HIFI mode, read the [Mozzi core module documentation](http://sensorium.github.com/Mozzi/doc/html/group__core.html#gae99eb43cb29bb03d862ae829999916c4/).  

***

## Using Mozzi  
Here's a template for an empty Mozzi sketch:  

```
#include <MozziGuts.h>   // at the top of your sketch

void setup() {
	startMozzi();
}

void updateControl(){
	// your control code
}

int updateAudio(){
	// your audio code which returns an int between -244 and 243
}

void loop() {
	audioHook();
}
```

There's a detailed example explaining the different parts [here](http://sensorium.github.com/Mozzi/learn/a-simple-sketch/).

***

## Documentation

There's documentation in the doc folder in the Mozzi download and [online](http://sensorium.github.com/Mozzi/doc/html/index.html).  
There is practical help on the [learn](http://sensorium.github.com/Mozzi/learn/) page on the Mozzi site.  
Start or look up a topic on the [users forum](https://groups.google.com/forum/#!forum/mozzi-users/).  
Also, feel free to submit any issues on the [GitHub Mozzi site](https://github.com/sensorium/Mozzi/issues/).  
Look for code and usage changes [here](extras/NEWS.txt).  

***

## Caveats and Workarounds

* While Mozzi is running, the Arduino time functions `millis()`, `micros()`, `delay()`, and
`delayMicroseconds()` are disabled.  

Mozzi provides `EventDelay()` for scheduling instead of `delay`, and `mozziMicros()` for timing, with 61us resolution (in `STANDARD` mode).  

* Mozzi interferes with `analogWrite()`.  In `STANDARD` audio mode, Mozzi takes over Timer0 (pins 5 and 6) and 
Timer1 (pins 9 and 10), but you can use the Timer2 pins, 3 and 11 (your board may differ).  In `HIFI` mode, 
Mozzi uses Timer0, Timer1 (or Timer4 on some boards), and Timer2, so pins 3 and 11 are also out.  

If you need PWM output (`analogWrite()`), you can do it on any digital pins using the technique in 
Mozzi>examples>11.Communication>Sinewave_PWM_pins_HIFI.  

* `analogRead()` is replaced by `mozziAnalogRead()`, which works in the background instead of blocking the processor.  

####Last Resort
The timers can be made available with `pauseMozzi()`, which suspends audio and 
control interrupts until you call `unpauseMozzi()`.  

***

If you enjoy using Mozzi for a project, or have extended it, we would be
pleased to hear about it and provide support wherever possible. Contribute
suggestions, improvements and bug fixes to the Mozzi wiki on Github, or
Fork it to contribute directly to future developments.

Mozzi is a development of research into Mobile Sonification in the
[SweatSonics](http://stephenbarrass.wordpress.com/tag/sweatsonics/) project. 

***

##Contributions / Included Dependencies  
Modified versions of the following libraries are included in the Mozzi download:  

[TimerOne library](http://www.pjrc.com/teensy/td_libs_TimerOne.html)  
[FrequencyTimer2 library](http://www.pjrc.com/teensy/td_libs_FrequencyTimer2.html) - now a [fork with support for ATmega32u4 processors](https://github.com/sensorium/FrequencyTimer2/)   

Mozzi has also drawn on and been influenced by (among many others):  

[xorshift] (http://www.jstatsoft.org/v08/i14/xorshift.pdf) random number generator, George Marsaglia, (2003)  
ead~.c puredata external (creb library) Copyright (c) 2000-2003 by Tom Schouten (GPL2)  
[AF_precision_synthesis](http://adrianfreed.com/content/arduino-sketch-high-frequency-precision-sine-wave-tone-sound-synthesis)
by Adrian Freed, 2009  
[Resonant filter](http://www.musicdsp.org/archive.php?classid=3#259) posted to musicdsp.org by Paul Kellett,
and fixed point version of the filter on [dave's blog of art and programming] (http://www.pawfal.org/dave/blog/2011/09/)  
State Variable filter pseudocode at [musicdsp.org] (http://www.musicdsp.org/showone.php?id=23 and http://www.musicdsp.org/showone.php?id=142)  
Various examples from [Pure Data](http://puredata.info/) by Miller Puckette  
[Practical synthesis tutorials](http://www.obiwannabe.co.uk/) by Andy Farnell  
