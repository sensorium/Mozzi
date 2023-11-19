# Mozzi  

### sound synthesis library for Arduino  


Tim Barrass   

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
-    8 bit or 14 bit audio output modes on a bare Arduino, up to 16 bit using an external DAC.
-    Variable control rate from 64 Hz upwards.
-    Useful basic audio toolkit: oscillators, samples, lines, envelopes, scheduling, filtering.
-    Fast ADC and other cpu-efficient code utilities to help keep audio running smoothly.
-    Example sketches for easy modification.
-    Readymade wavetables and a script to convert your own soundfiles for Mozzi.
-    Usable on several platforms: Arduino, STM32, ESP, Teensy.
-    Mozzi is designed to be easy to use, open source and extendable.

***

## Installation  
Use the "code" button on Mozzi's Github page to download a ZIP file of the latest developing code.  Import this into Arduino, following the instructions from the [Arduino libraries guide](http://arduino.cc/en/Guide/Libraries).

*In the Arduino IDE, navigate to Sketch > Include Library > Add .ZIP Library. At the top of the drop down list, select the option to "Add .ZIP Library".*

(Note: the files in the "releases" section on Github are now legacy.  The development code is recommended.)

***

## Quick Start  
To hear Mozzi, connect a 3.5mm audio jack with the centre wire to the PWM output
on Digital Pin 9 on the Arduino, and the ground to the Ground on the Arduino.
Use this as a line out which you can plug into your computer and listen to with
a sound program like [Audacity](http://audacity.sourceforge.net/).
Try some examples from the __File > Examples > Mozzi__ menu.  

Below is a list of the Digital Pins used by Mozzi for STANDARD_PLUS mode PWM audio out on different boards.
Feedback about others is welcome.

Model | Pin | Tested
----- | --- | ------
Arduino Uno | 9	| yes
Arduino Uno R4 | A0 | yes
Arduino Duemilanove | 9	| yes
Arduino Nano | 9 | yes
Arduino Nano 33 Iot | A0 | yes
Arduino Pro Mini | 9 | yes
Arduino Leonardo | 9 | yes
Arduino Mega | 11 | yes
Arduino MBED (only the Giga has been tested, see "Hardware specific notes", below) | A13 | no
Arduino Giga | A13 (jack) | yes
Freetronics EtherMega | 11 | yes
Ardweeny | 9 | yes     
Boarduino | 9 | yes
Teensy | 14 | -
Teensy2 | B5 | yes  
Teensy2++ | B5(25) | yes
Teensy 3.0 3.1 LC 3.2 | DAC/D | yes
Teensy 3.4, 3.5 | DAC/D | -
Teensy 4.0 4.1 | A8 | yes
Gemma M0 | A0 | yes
Adafruit Playground Express | Built in Speaker | yes    
Sanguino | 13	| -  
STM32F1 maple core | PB8 | yes
STM32F1 STM core | PA8 | yes
STM32F4 STM core | PA8 | yes
ESP8266 *see details* | GPIO2 | yes
RP2040 | 0 | yes

For details about HIFI mode, read the [Mozzi core module documentation](http://sensorium.github.io/Mozzi/doc/html/group__core.html#gae99eb43cb29bb03d862ae829999916c4/).  

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

AudioOutput_t updateAudio() {
	// your audio code which returns a 0-centered integer, typically in the 8bit or 16-bit range
	return MonoOutput::from16Bit( [myvalue] );
}

void loop() {
	audioHook();
}
```

There's a detailed example explaining the different parts [here](http://sensorium.github.io/Mozzi/learn/a-simple-sketch/).

***

## Documentation

There's documentation in the doc folder in the Mozzi download and [online](http://sensorium.github.io/Mozzi/doc/html/index.html).  
There is practical help on the [learn](http://sensorium.github.io/Mozzi/learn/) page on the Mozzi site.  
Start or look up a topic on the [users forum](https://groups.google.com/forum/#!forum/mozzi-users/).  
Also, feel free to submit any issues on the [GitHub Mozzi site](https://github.com/sensorium/Mozzi/issues/).  
Look for code and usage changes [here](extras/NEWS.txt).  

***

## General caveats and Workarounds

* While Mozzi is running, calling `delay()`, `delayMicroseconds()`, or other functions which wait or cycle through loops can cause audio glitches.
Mozzi provides `EventDelay()` for scheduling instead of `delay`. In general, make sure to write non-blocking code!

* `analogRead()` is a time-consuming operation especially on the classic AVR boards. Mozzi provides `mozziAnalogRead()` as a drop-in replacement, which works in the
background instead of blocking the processor.

* Depending on the hardware and configured audio output mode, Mozzi will usually claim one or more hardware timers. This may affect, among other things, the ability to
use `analogWrite()`. Check the [Hardware Section of the Documentation](https://sensorium.github.io/Mozzi/doc/html/group__hardware.html) to options to configure ressource
usage.
  - If you need `analogWrite()`, you can do PWM output on any digital pins using the technique in *Mozzi>examples>11.Communication>Sinewave_PWM_pins_HIFI*.  

* As last resort, the timers can be made available with `stopMozzi()`, which stops audio interrupts, until you call `startMozzi()`.

***

## Tweaking Arduino for Faster Audio Code

If you need your synth to run faster on AVR architectures, Arduino versions above 1.5 can be tweaked to optimise compiled code for speed instead of small size.  

Find Arduino’s platform.txt (on OSX you can find it by searching in Users/your_name/Library/Arduino15). Search and replace -Os with -O2. Save.

It’s explained more thoroughly (for Windows) [here](http://www.instructables.com/id/Arduino-IDE-16x-compiler-optimisations-faster-code/?ALLSTEPS).

If you still need more speed, Arduino 1.0.5 produces slightly faster code.

Mozzi itself offers many helpers for producing faster code, such as Fixed-Point integer maths, fast replacements for `map()`, `random()`, `millis()`, and other functions.
Be sure to use these!

***

## Using external chips to produce the sound

External chips (DAC) can also be used on any platform which does not support natively the I2S protocol  using an user defined `audioOutput` function. This can allow a greater audio quality over the native ways to output the sound (PWM for AVR Arduinos and STM32 and 12 bit DAC for Teensy 3.*).
Examples are provided for the MCP492X DAC (12 bit on SPI) and for the (PT8211) 16 bit stereo DAC using SPI port to emulate the I2S protocol. The latter should be compatible with any DAC using I2S.

***

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


## Hardware compatibility and hardware specific notes

While originating on the 8 bit AVR based "classic Arduino" boards, Mozzi supports a wide variety of different MCUs, these days, including:
  - Classic AVR boards: Arduino Uno, Duemilanove, Nano, Nano 33, Pro Mini, Leonardo, Mega, many others, and countless clones, Teensy 1 and 2.x, ...
  - Various Teensy 3.x boards: Teensy 3.0/3.1/3.2/3.4/3.5/LC
  - Teensy 4.x boards: Teensy 4.0 and 4.1 are known to work, future boards stand good chances of working with Mozzi out of the box
  - STM32-based boards: Confusingly, several popular Arduino cores exist for these MCUs - and Mozzi supports several. Refer to the documentation
    linked below. Mozzi has been tested on the popule STM32F1 "blue pill" and STM32F4 "black pill" boards, but should support a very wide range
    of further boards out of the box.
  - SAMD21-based boards: Tested on the Arduino Circuitplayground M0, expect to work on others, too.
  - ESP8266: Should run on the whole range of boards, including the minimal ESP-01
  - ESP32: Should run on the wohle range of boards
  - Arduiono Giga/MBED: The Arduino Giga, and - in untested theory - the Arduion Portenta
  - RP2040: The Raspberry Pi Pico and other boards using RP2040
  - Arduino Uno R4: The new Uno, based on a 32-bit Renesas MCU

Since the hardware capatibilies of these boards are vastly different, the ports, too, differ in their internals and their capabilities. Importantly,
of course, they also differ in what pin(s) are used for audio output by default. Refer to the [Hardware Section of the Documentation](https://sensorium.github.io/Mozzi/doc/html/group__hardware.html) for the details
applicable to your hardware.
  

***

## Use and Remix
Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License, which is detailed in LICENSE.txt

Disclaimer: This is a human-readable summary of (and not a substitute for) the license.

You are free to:
 - Share — copy and redistribute the material in any medium or format
 - Adapt — remix, transform, and build upon the material

The licensor cannot revoke these freedoms as long as you follow the license terms.
Under the following terms:
 - Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
 - NonCommercial — You may not use the material for commercial purposes.
 - No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
