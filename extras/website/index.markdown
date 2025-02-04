---
layout: single
toc: true
---

Currently your Arduino can only beep like a microwave oven. Mozzi brings
your Arduino to life by allowing it to produce much more complex and interesting growls, sweeps and chorusing atmospherics. These sounds can be quickly and easily constructed from familiar synthesis units like oscillators, delays, filters and envelopes.

You can use Mozzi to generate algorithmic music for an installation or
performance, or make interactive sonifications of sensors, on a small, modular and super cheap Arduino, without the need for additional shields, message passing or external synths.

Here are some sounds of the example sketches which come with Mozzi:

***

Using a "Line" to sweep frequency:

<audio controls>
<source type="audio/ogg" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/02.Control/Line_Gliss_Double_32k_HIFI/Line_Gliss_Double_32k_HIFI.ogg?raw=true" preload="auto"></source>
<source type="audio/mp3" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/02.Control/Line_Gliss_Double_32k_HIFI/Line_Gliss_Double_32k_HIFI.mp3?raw=true" preload="auto"></source>
Your browser does not support the audio element. </audio>

***

Phase and amplitude modulation, with reverb:

<audio controls>
<source type="audio/ogg" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/09.Delays/ReverbTank_STANDARD/ReverbTank_STANDARD.ogg?raw=true" preload="auto"></source>
<source type="audio/mp3" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/09.Delays/ReverbTank_STANDARD/ReverbTank_STANDARD.mp3?raw=true" preload="auto"></source>
Your browser does not support the audio element. </audio>

***

A chordal wash of detuned oscillator pairs:

<audio  controls="controls">
<source type="audio/ogg" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/06.Synthesis/Detuned_Beats_Wash/Detuned_Beats_Wash.ogg?raw=true" preload="auto"></source>
<source type="audio/mp3" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/06.Synthesis/Detuned_Beats_Wash/Detuned_Beats_Wash.mp3?raw=true" preload="auto"></source>
Your browser does not support the audio element. </audio>

***

Waveshaping:

<audio  controls="controls">
<source type="audio/ogg" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/06.Synthesis/Waveshaper/Waveshaper.ogg?raw=true" preload="auto"></source>
<source type="audio/mp3" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/06.Synthesis/Waveshaper/Waveshaper.mp3?raw=true" preload="auto"></source>
Your browser does not support the audio element. </audio>

***

Samples sequenced on the fly:

<audio controls>
<source type="audio/ogg" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/08.Samples/Samples_Tables_Arrays/Samples_Tables_Arrays.ogg?raw=true" preload="auto"></source>
<source type="audio/mp3" src="https://github.com/sensorium/Mozzi/blob/gh-pages/examples/examples/08.Samples/Samples_Tables_Arrays/Samples_Tables_Arrays.mp3?raw=true" preload="auto"></source>
Your browser does not support the audio element. </audio>


## Features
-    Available for a wide and growing range of MCUs, with and without inbuilt DACs: Arduino Uno R3 and R4, STM32, Teensy, ESP8266, ESP32, Raspberry Pi Pico, and more.
-    Configurable sample rate, usually in powers of two (16384 Hz, or 32768 Hz).
-    Variable control rate from 64 Hz upwards.
-    Various inbuilt output modes, including 16 bit output to an external DAC.
-    Allows interfacing to custom output routines, with examples for playing audio on external DAC modules, and even bluetooth.
-    Useful basic audio toolkit: oscillators, samples, lines, envelopes, scheduling, filtering.
-    Fast ADC and other cpu-efficient code utilities to help keep audio running smoothly.
-    Example sketches for easy modification.
-    Readymade wavetables and a script to convert your own soundfiles for Mozzi.
-    Mozzi is designed to be easy to use, open source and extendable.

***

## Installation
The easiest installation option nowadays is to install Mozzi via the Library Manager in your Arduino application:
_Arduino➞Sketch➞Include Library➞Library Manager_ type "Mozzi" into the search field, then click "install".

For other installation methods (e.g. the development version), see the [Download page](https://sensorium.github.io/Mozzi/download).

### Dependencies
Starting in version 2.0, Mozzi requires [FixMath](https://github.com/tomcombriat/FixMath) to be installed. If you install Mozzi via the Library Manager of Arduino, FixMath should be automatically installed. 

## Quick Start
To hear Mozzi, wire a 3.5mm audio jack with the centre to the audio out pin for your Arduino as shown in the table below, and the shield to GND on the Arduino.
Plug into your computer and listen with a sound program like [Audacity](https://audacity.sourceforge.net/). (Or connect any other high-impedance input, like an active speaker)
Try some examples from the __File > Examples > Mozzi__ menu.

For more about audio output, including high quality output modes [Mozzi Output tutorial](https://sensorium.github.io/Mozzi/learn/output/).  

### A note for users coming from Mozzi 1.x
Mozzi 2.0 brings a lot of changes under the hood, and is not 100% source compatible with earlier versions of Mozzi. Most sketches should continue to compile, but with
a lot of warnings. A few others will no longer work. See [Porting to Mozzi 2.0](https://sensorium.github.io/Mozzi/learn/porting/) for what to change.
If desparate, there is still a "Mozzi_1" branch in the git repository which contains the last stable version of Mozzi 1.x and packages for this version can be found on [Github](https://github.com/sensorium/Mozzi/releases/tag/1.1.2), but this will not get any new development. Users still on these versions are encouraged to migrate to Mozzi 2.


{% include boards-tested.markdown %}


## Using Mozzi
Here's a template for an empty Mozzi sketch:

{% highlight c++ %}
#include <Mozzi.h>   // at the top of your sketch

void setup() {
	startMozzi();
}

void updateControl(){
	// your control code
}

AudioOutput_t updateAudio(){
	MonoOutput::from16Bit( [my_cool_sample] );
}

void loop() {
	audioHook();
}
{% endhighlight %}

There's a detailed example explaining the different parts [here](https://sensorium.github.io/Mozzi/learn/a-simple-sketch/).


## Documentation

For getting started, browse the practical help on the [learn](https://sensorium.github.io/Mozzi/learn/) page on the Mozzi site.  
API reference documentation is available in the doc folder in the Mozzi download and [online](http://sensorium.github.io/Mozzi/doc/html/index.html).  
Start or look up a topic on the [users forum](https://groups.google.com/forum/#!forum/mozzi-users/).  
Also, feel free to submit any issues on the [GitHub Mozzi site](https://github.com/sensorium/Mozzi/issues/).  
Look for code and usage changes [here](extras/NEWS.txt).  
For hardware specific details, including supported features, caveats, and hardware-dependent configuration options,
refer to the [Hardware Section of the API-Documentation](https://sensorium.github.io/Mozzi/doc/html/hardware.html).

## Compatibility issues

* In most setups, Mozzi claims one or two hardware timers. This may result in incompatibilities with certain libraries, and / or the ability to use timer-based functions such as `analogWrite()`. As the details on this
  differ a lot between the supported boards, read up on the details - and available workarounds - in the [Hardware Section of the API-Documentation](https://sensorium.github.io/Mozzi/doc/html/hardware.html).

* There is also an example on emulating `analogWrite()` on any digitial pin in *Mozzi>examples>11.Communication>Sinewave_PWM_pins_HIFI*.

* The timers can be made available with `stopMozzi()`, which stops audio interrupts, until you call `startMozzi()`.

* Note that it is of utmost importance to write non-blocking code, such that the
  audio buffer never runs low. Hints on how to do this, including why, and how you
  should avoid using `delay()`, `analogRead()`, and how to make your code run faster,
  can be found at [on the learn pages](https://sensorium.github.io/Mozzi/learn/hints/).


## Extending Mozzi

### Using external chips to produce the sound

External chips (DAC) can also be used on any platform which does not support natively the I2S protocol  using an user defined `audioOutput` function. This can allow a greater audio quality over the native ways to output the sound (PWM for AVR Arduinos and STM32 and 12 bit DAC for Teensy 3.*).
Examples are provided for the MCP492X DAC (12 bit on SPI) and for the (PT8211) 16 bit stereo DAC using SPI port to emulate the I2S protocol. The latter should be compatible with any DAC using I2S.

### Extending the library itself

If you enjoy using Mozzi for a project, or have extended it, we would be
pleased to hear about it and provide support wherever possible. Contribute
suggestions, improvements and bug fixes to the Mozzi wiki on Github, or
Fork it to contribute directly to future developments.

Mozzi is a development of research into Mobile Sonification in the
[SweatSonics](http://stephenbarrass.wordpress.com/tag/sweatsonics/) project.

***

## Contributions / Included Dependencies  
Modified versions of the following libraries are included in the Mozzi download:  

[TimerOne library](http://www.pjrc.com/teensy/td_libs_TimerOne.html)  
[FrequencyTimer2 library](http://www.pjrc.com/teensy/td_libs_FrequencyTimer2.html) - now a [fork with support for ATmega32u4 processors](https://github.com/sensorium/FrequencyTimer2/)   

Mozzi has also drawn on and been influenced by (among many others):  

[xorshift](http://www.jstatsoft.org/v08/i14/xorshift.pdf) random number generator, George Marsaglia, (2003)  
ead~.c puredata external (creb library) Copyright (c) 2000-2003 by Tom Schouten (GPL2)  
[AF_precision_synthesis](http://adrianfreed.com/content/arduino-sketch-high-frequency-precision-sine-wave-tone-sound-synthesis)
by Adrian Freed, 2009  
[Resonant filter](http://www.musicdsp.org/archive.php?classid=3#259) posted to musicdsp.org by Paul Kellett,
and fixed point version of the filter on [dave's blog of art and programming](http://www.pawfal.org/dave/blog/2011/09/)  
State Variable filter pseudocode at [musicdsp.org](http://www.musicdsp.org/showone.php?id=23) and [here](http://www.musicdsp.org/showone.php?id=142)  
Various examples from [Pure Data](http://puredata.info/) by Miller Puckette  
[Practical synthesis tutorials](http://www.obiwannabe.co.uk/) by Andy Farnell  
  

***

## Use and Remix
Mozzi is licensed under a the LGPL version 2.1 or (at your option) any later version of the license.

Disclaimer: This is a human-readable summary of (and not a substitute for) the license.
- You may copy, distribute and modify the Mozzi library itself provided that you state modifications and license them under LGPL-2.1.

- You may distribute *your own* source code which merely *uses* the Mozzi-API under any licence that you wish.

- Regarding distribution of *binaries* (also inside a hardware project) that include Mozzi, the Arduino FAQ sums up the situation as follows:
  "Using the Arduino core and libraries for the firmware of a commercial product does not require you to release the source code for the firmware. The LGPL does, however, require you to make available object files that allow for the relinking of the firmware against updated versions of the Arduino core and libraries. Any modifications to the core and libraries must be released under the LGPL."

- Note that using third-party libraries/code - including as shown in some of the Mozzi examples - may introduce additional restrictions.
