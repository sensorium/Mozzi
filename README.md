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
-    Available for a wide and growing range of MCUs, with and without inbuilt DACs: Arduino Uno R3 and R4, STM32, Teensy, ESP8266, ESP32, Raspberry Pi Pico, and more.
-    Configurable sample rate, usually in powers of two (16384 Hz, 32768 Hz, 64536 Ht).
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

## Quick Start
To hear Mozzi, wire a 3.5mm audio jack with the centre to the audio out pin for your Arduino as shown in the table below, and the shield to GND on the Arduino.
Plug into your computer and listen with a sound program like [Audacity](https://audacity.sourceforge.net/). (Or connect any other high-impedance input, like an active speaker)
Try some examples from the __File > Examples > Mozzi__ menu.

For more about audio output, including high quality output modes [Mozzi Output tutorial](https://sensorium.github.io/Mozzi/learn/output/).  


## Supported boards, output modes and default pins
Table is not necessarily complete. *Abbreviations explained below the table.* The default output mode is framed with a border in each row.
If stereo is supported, in a mode, the cell has a red/blue background, and (where applicable) the second pin number is given in parentheses (+X).
Check the [hardware section of the API-documentation](https://sensorium.github.io/Mozzi/doc/html/hardware.html) for platform specific notes and (pin) configuration options.

<style>
td[data-mo] { background-color: rgba(0,0,255,.5); }
td[data-st] { background: linear-gradient(-45deg,rgba(0,0,255,.5) 50%, rgba(255,0,0,.5) 50%); }
td[data-md] { border: 2px solid; background-color: rgba(0,0,255,.5); }
td[data-sd] { border: 2px solid; background: linear-gradient(-45deg,rgba(0,0,255,.5) 50%, rgba(255,0,0,.5) 50%); }
</style>

<table border="0">
<thead>
<tr><td><b>Board or family</b>  / <b>Output mode</b>                                            </td><td> PWM-1          </td><td> PWM-2             </td><td> PDM        </td><td>inbuilt DAC </td><td>I2S DAC (native)</td></tr>
</thead>
<tbody>
<tr style="border-top: 1px solid">
    <td><i>ATmega328/168</i>: Uno (R3), Nano, Pro Mini, Duemilanove, Leonardo, etc.             </td><td data-sd>9 (+10) </td><td data-mo>9, 10      </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ATmega32U4</i>: Teensy, Teensy2, 2++ <i>B5/B6 correspond to pins 14/15 in Arduino</i></td><td data-sd>B5 (+B6)</td><td data-mo>B5, B6     </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ATmega2560</i>: Arduino Mega, Freetronics EtherMega, etc.                            </td><td data-sd>11 (+12)</td><td data-mo>11, 12     </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ATmega1284</i>: Sanguino                                                             </td><td data-sd>13 (+12)</td><td data-mo>13, 12     </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td>Teensy3.x - <i>note: DAC Pin number depends on model: A14, A12, or A21</i>              </td><td> -              </td><td> -                 </td><td> -          </td><td data-md>DAC </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td>Teensy4.x                                                                               </td><td data-sd>A8 (+A9)</td><td> -                 </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>LGT8F328P</i>: "Register clone" of the ATmega328, uses the same code in Mozzi        </td><td data-sd>9 (+10) </td><td data-mo>9, 10      </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>SAMD</i>: Arduino Nano 33 Iot, Adafruit Playground Express, Gemma M0 etc.            </td><td> -              </td><td> -                 </td><td> -          </td><td data-md>A0/speaker</td><td> -   </td></tr>
<tr style="border-top: 1px solid">
    <td><i>Renesas Arduino Core</i>: Arduino Uno R4                                             </td><td> -              </td><td> -                 </td><td> -          </td><td data-md>A0  </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>Arduino MBED Core</i>: Arduino Giga (only model tested so far in this family)        </td><td> -              </td><td> -                 </td><td data-mo>SERIAL2TX</td><td data-sd>A13 (+A12)</td><td> -    </td></tr>
<tr style="border-top: 1px solid">
    <td><i>STM32 maple core</i>: Various STM32F1 and STM32F4 boards, "Blue/Black Pill"          </td><td data-sd>PB8 (+PB9)</td><td data-mo>PB8, PB9 </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>STM32duino (STM official) core</i>: Huge range of STM32Fx boards                     </td><td data-sd>PA8 (+PA8)</td><td data-mo>PA8, PA9 </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ESP8266</i>: ESP-01, Wemos D1 mini, etc. <i>note: Beware of erratic pin labels</i>   </td><td> -              </td><td> -                 </td><td data-md>GPIO2</td><td> -         </td><td data-st>yes </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ESP32</i>: <i>note: Beware of vastly different pin labels across board variants</i>  </td><td> -              </td><td> -                 </td><td data-mo>yes </td><td data-sd>GPIO25 (+GPIO26)</td><td data-st>yes</td></tr>
<tr style="border-top: 1px solid">
    <td><i>RP2040</i>: Raspberry Pi Pico and friends                                            </td><td data-sd>0 (+1)  </td><td data-mo>0, 1       </td><td> -          </td><td> -          </td><td data-st>yes </td></tr>
</tbody>
</table>

<span style="font-size: .6em">
 - PWM-1: 1-pin PWM mode (`MOZZI_OUTPUT_PWM`)
 - PWM-2: 2-pin PWM mode (`MOZZI_OUTPUT_2PIN_PWM`)
 - PDM: Pulse density modulation, may be either `MOZZI_OUTPUT_PDM_VIA_SERIAL` or `MOZZI_OUTPUT_PDM_VIA_I2S`
 - inbuilt DAC: `MOZZI_OUTPUT_INTERNAL_DAC`
 - I2S DAC (native): native support for externally connected I2S DACs (`MOZZI_OUTPUT_I2S_DAC`). Since this requires several, often
   configurable pins, and is never the default option, no pin numbers are shown in this table.
 - **All** platforms also support "external" output modes (`MOZZI_OUTPUT_EXTERNAL_TIMED` or `MOZZI_OUTPUT_EXTERNAL_CUSTOM`), which allow
   for connecting DACs or other external circuitry.
</span>

***

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
  

***

## Use and Remix
Mozzi is licensed under a the LGPL version 2.1 or (at your option) any later version of the license.

Disclaimer: This is a human-readable summary of (and not a substitute for) the license.
- You may copy, distribute and modify the Mozzi library itself provided that you state modifications and license them under LGPL-2.1.

- You may distribute *your own* source code which merely *uses* the Mozzi-API under any licence that you wish.

- Regarding distribution of *binaries* (also inside a hardware project) that include Mozzi, the Arduino FAQ sums up the situation as follows:
  "Using the Arduino core and libraries for the firmware of a commercial product does not require you to release the source code for the firmware. The LGPL does, however, require you to make available object files that allow for the relinking of the firmware against updated versions of the Arduino core and libraries. Any modifications to the core and libraries must be released under the LGPL."

- Note that using third-party libraries/code - including as shown in some of the Mozzi examples - may introduce additional restrictions.
