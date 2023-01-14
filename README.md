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
Arduino Duemilanove | 9	| yes
Arduino Nano | 9 | yes
Arduino Pro Mini | 9 | yes
Arduino Leonardo | 9 | yes
Arduino Mega | 11 | yes
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
STM32duino (see "Hardware specific notes", below) | PB8 | yes
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

int updateAudio(){
	// your audio code which returns an int between -244 and 243
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

## Caveats and Workarounds

#### AVR

* While Mozzi is running, calling `delay()`, `delayMicroseconds()`, or other functions which wait or cycle through loops can cause audio glitches.
Mozzi provides `EventDelay()` for scheduling instead of `delay`.

* `analogRead()` is replaced by `mozziAnalogRead()`, which works in the background instead of blocking the processor.  

* Mozzi interferes with `analogWrite()`.  In `STANDARD` and `STANDARD_PLUS` audio modes, Mozzi takes over Timer1 (pins 9 and 10), but you can use the Timer2 pins, 3 and 11 (your board may differ).  In `HIFI` mode, Mozzi uses Timer1 (or Timer4 on some boards), and Timer2, so pins 3 and 11 are also out.  
If you need `analogWrite()`, you can do PWM output on any digital pins using the technique in *Mozzi>examples>11.Communication>Sinewave_PWM_pins_HIFI*.  


#### Last Resort
The timers can be made available with `stopMozzi()`, which stops audio interrupts, until you call `startMozzi()`.  

***

## Tweaking Arduino for Faster Audio Code

If you need your synth to run faster on AVR architectures, Arduino versions above 1.5 can be tweaked to optimise compiled code for speed instead of small size.  

Find Arduino’s platform.txt (on OSX you can find it by searching in Users/your_name/Library/Arduino15). Search and replace -Os with -O2. Save.

It’s explained more thoroughly (for Windows) [here](http://www.instructables.com/id/Arduino-IDE-16x-compiler-optimisations-faster-code/?ALLSTEPS).

If you still need more speed, Arduino 1.0.5 produces slightly faster code.

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


## Hardware specific notes

### STM32(duino)
port by Thomas Friedrichsmeier

Compiles for and runs on a STM32F103C8T6 blue pill board, with a bunch of caveats (see below), i.e. on a board _without_ a
real DAC. Should probably run on any other board supported by [STM32duino](https://github.com/rogerclarkmelbourne/Arduino_STM32) (STM32F4 is __not__ supported for now).

- You will need a very recent checkout of the Arduino_STM32 repository, otherwise compilation will fail.
- Audio output is to pin PB8, by default (HIFI-mode: PB8 and PB9)
- If you want to use MIDI, be sure to replace "MIDI_CREATE_DEFAULT_INSTANCE()" with "MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI)" (or Serial2)
- Timers 4 (PWM output), and 2 (audio rate) are used by default.
- The STM32-specific options (pins, timers) can be configured in AudioConfigSTM32.h
- Default audio resolution is currently set to 10 bits, which yields 70khZ PWM frequency on a 72MHz CPU. HIFI mode is 2*7bits at up to 560Khz (but limited to 5 times audio rate)
- HIFI_MODE did not get much testing
- STEREO not yet implemented (although that should not be too hard to do)
- AUDIO_INPUT is completely untested (but implemented in theory)
- Note that AUDIO_INPUT and mozziAnalogRead() return values in the STM32's full ADC resolution of 0-4095 rather than AVR's 0-1023.
- twi_nonblock is not ported

### Teensy 3.0/3.1/3.2/3.4/3.5/LC

This port is working with the latest version of Teensyduino (1.8.5)
Extra libraries required for use with Teensy 3.*:
These are included in the standard Teensyduino install unless you explicitly disable them
- [Timer library](https://github.com/loglow/IntervalTimer) for Teensy 3.* by Daniel Gilbert
- [ADC library](http://github.com/pedvide/ADC) by Pedro Villanueva

Some of the differences for Teensy 3.* which will affect users include:

- On Teeensy 3.0/3.1/3.2/Audio output is on pin A14/DAC, in STANDARD or STANDARD_PLUS audio modes.
    These modes are identical on Teensy 3.0/3.1/3.2, as the output is via DAC rather than PWM.
- Output is 12 bits in STANDARD and STANDARD_PLUS modes, up from nearly 9 bits for Atmel based boards. HIFI audio, which works by summing two output pins, is not available on Teensy 3.0/3.1.
- twi_nonblock code by Marije Baalman for non-blocking I2C is not compatible with Teensy 3.0/3.1/3.2.

### Teensy 4.0/4.1
port by Thomas Combriat

This port is working with the latest version of Teensyduino (1.8.5)
Extra libraries required for use with Teensy 4.*:
These are included in the standard Teensyduino install unless you explicitly disable them
- [Timer library](https://github.com/loglow/IntervalTimer) for Teensy 3.* by Daniel Gilbert
- [ADC library](http://github.com/pedvide/ADC) by Pedro Villanueva

Some of the differences for Teensy 4.*:

- Contrary to the Teensy 3, the Teensy 4 do not have any DAC. The output is done on pin A8 (PWM) by default (editable in `AudioConfigTeensy4.h`

### ESP8266
port by Thomas Friedrichsmeier

- Since flash memory is not built into the ESP8266, but connected, externally, it is much too slow for keeping wave tables, audio samples, etc. Instead, these are kept in RAM on this platform.
- Asynchronous analog reads are not implemented. `mozziAnalogRead()` relays to `analogRead()`.
- AUDIO_INPUT is not implemented.
- twi_nonblock is not ported
- Several audio output modes exist, the default being PDM_VIA_SERIAL (configurable in AudioConfigESP.h):
  - PDM_VIA_SERIAL: Output is coded using pulse density modulation, and sent via GPIO2 (Serial1 TX).
    - This output mode uses timer1 for queuing audio sample, so that timer is not available for other uses.
    - Note that this mode has slightly lower effective analog output range than PDM_VIA_I2S, due to start/stop bits being added to the output stream.
  - PDM_VIA_I2S: Output is coded using pulse density modulation, and sent via the I2S pins. The I2S data out pin (GPIO3, which is also "RX") will have the output,
  but *all* I2S output pins (RX, GPIO2 and GPIO15) will be affected. Mozzi tries to set GPIO2 and GPIO15 to input mode, and *at the time of this writing*, this allows
  I2S output on RX even on boards such as the ESP01 (where GPIO15 is tied to Gnd). However, it seems safest to assume that this mode may not be useable on boards where
  GPIO2 or GPIO15 are not available as output pins.
  - EXTERNAL_DAC_VIA_I2S: Output is sent to an external DAC (such as a PT8211), digitally coded. This is the only mode that supports STEREO. It also needs the least processing power.
- There is no "HIFI_MODE" in addition to the above output options. For high quality output, either use an external DAC, or increase the PDM_RESOLUTION value.
- Note that the ESP8266 pins can output less current than the other supported CPUs. The maximum is 12mA, with a recommendation to stay below 6mA.
  - WHEN CONNECTING A HEADPHONE, DIRECTLY, USE APPROPRIATE CURRENT LIMITING RESISTORS (>= 500Ohms).
- _Any_ WiFi-activity can cause severe spikes in power consumption. This can cause audible "ticking" artifacts, long before any other symptoms.
  - If you do not require WiFi in your sketch, you should turn it off, _explicitly_, using `WiFi.mode(WIFI_OFF)`.
  - A juicy enough, well regulated power supply, and a stabilizing capacitor between VCC and Gnd can help a lot.
  - As the (PDM) output signal is digital, a single transistor can be used to amplify it to an independent voltage level.
- The audio output resolution is always 16 bits on this platform, _internally_. Thus, in updateAudio(), you should scale your output samples to a full 16 bit range. The effective number of output bits cannot easily
  be quantified, due to PDM coding.
- audioHook() calls `yield()` once for every audio sample generated. Thus, as long as your audio output buffer does not run empty, you should not need any additional `yield()`s inside `loop()`.

### ESP32
port by Dieter Vandoren and Thomas Friedrichsmeier

- Since flash memory is not built into the ESP32, but connected, externally, it is much too slow for keeping wave tables, audio samples, etc. Instead, these are kept in RAM on this platform.
- Asynchronous analog reads are not implemented. `mozziAnalogRead()` relays to `analogRead()`.
- AUDIO_INPUT is not implemented.
- twi_nonblock is not ported
- Currently, three audio output modes exist, the default being INTERNAL_DAC (configurable in AudioConfigESP32.h). *The configuration details may still be subject to change; please be prepared to make some minimal adjustments to your code, when upgrading Mozzi*:
  - INTERNAL_DAC: Output using the built-in DAC on GPIO pins 25 and 26.
    - 8 bits resolution, mono (on both pins) or stereo
    - For simplicity of code, both pins are always used, even in mono output mode
  - PT8211_DAC: Output is sent via I2S in a format suitable for the PT8211 external EXTERNAL_DAC
    - 16 bits resolution, mono or stereo. Remember to shift your audio accordingly.
    - Output pins can be configured in AudioConfigESP32.h. Default is BCK: 26, WS: 15, DATA: 33
  - PDM_VIA_I2S: Output is converted using pulse density modulation, sent to the I2S data pin. No external hardware needed.
    - 16 bits resolution. Remember to shift your audio accordingly.
    - Output (DATA) pin can be configured in AudioConfigESP32.h. Default 33. Note that the BCK and WS pins are also used in this mode.
    - The PDM_RESOLUTION parameter can be used to reduce noise at the cost of more CPU power.
    - Mono, only.
- "HIFI_MODE" is not currently implemented, but could conceivably be realized for the INTERNAL_DAC mode. Patches welcome.
- WIFI-activity not yet tested, but likely the same notes as for ESP8266 apply. Consider turning off WIFI.
- The implementation of audioTicks() may be slightly inaccurate on this platform.

### RP2040 (Raspberry Pi Pico)
port by Thomas Friedrichsmeier

Compiles and runs using [this core](https://github.com/earlephilhower/arduino-pico). Can probably be ported to the Mbed core for RP2040, relatively easily, as it relies mostly
on the RP2040 SDK API. Tested on a Pi Pico.

- This is a recent addition, implementation details may still change (currently just PWM driven by a timer; this may be worth changing to a DMA driven output)
- Wavetables and samples are not kept in progmem on this platform. While apparently speed (of the external flash) is not much of an issue, the data always seems to be copied into RAM, anyway.
- Currently, two audio output modes exist (configurable in AudioConfigRP2040.h) in addition to using an user-defined `audioOutput` function, with the default being PWM_VIA_BARE_CHIP:
  - PWM_VIA_BARE_CHIP: PWM audio output on pin 0, by default, with 11 bits default output resolution
    - One hardware timer interrupt and one DMA channel are claimed (number not hardcoded), a non-exclusive handler is installed on DMA_IRQ_0.
    - HIFI_MODE not yet implemented (although that should not be too hard to do).
  - EXTERNAL_DAC_VIA_I2S: I2S output to be connected to an external DAC
    - 16 bits resolution by default (configurable in AudioConfigRP2040.h), 8, 16, 24 (left aligned) and 32 resolution are available.
    - Both plain I2S and LSBJ_FORMAT (for the PT8211 for instance) are available (configurable in AudioConfigRP2040.h), default is LSBJ.
    - Outputs pins can be configured in AudioConfigRP2040.h. Default is BCK: 20, WS: 21, DATA: 22.
    - Two DMA channels are claimed (numbers not hardcoded), non-exclusive handlers are installed on DMA_IRQ_0.
    - At the time of writing, LSBJ is only available with github arduino-pico core.
- Note that AUDIO_INPUT and mozziAnalogRead() return values in the RP2040's full ADC resolution of 0-4095 rather than AVR's 0-1023.
- twi_nonblock is not ported
- Code uses only one CPU core


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
