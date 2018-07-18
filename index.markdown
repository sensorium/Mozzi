---
layout: home
toc: true
---

Currently your Arduino can only beep like a microwave oven. Mozzi brings
your Arduino to life by allowing it to produce much more complex and interesting
growls, sweeps and chorusing atmospherics. These sounds can be quickly and easily
constructed from familiar synthesis units like oscillators, delays, filters and
envelopes.

You can use Mozzi to generate algorithmic music for an installation or
performance, or make interactive sonifications of sensors, on a small, modular
and super cheap Arduino, without the need for additional shields, message
passing or external synths.

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
-    16384 Hz sample rate or experimental 32768 Hz rate.
-    8 bit or 14 bit audio output modes (12 bits on Teensy 3.x).
-    PWM audio output on Atmel-based boards, or true analog out on Teensy 3.x.
-    Variable control rate.
-    Useful basic audio toolkit: oscillators, samples, lines, envelopes, scheduling, filtering.
-    Fast ADC and other cpu-efficient code utilities to help keep audio running smoothly.
-    Example sketches for easy modification.
-    Readymade wavetables and a script to convert your own soundfiles for Mozzi.
-	   Designed to be easy to use, open source (CC-nc-sa) and extendable.


## Installation
Download the most recent Mozzi from the link at the top of this page.
Then, follow the instructions from the [Arduino libraries guide](https://arduino.cc/en/Guide/Libraries).


## Quick Start
To hear Mozzi, wire a 3.5mm audio jack with the centre to the audio out pin for your Arduino as shown in the table below, and the shield to GND on the Arduino.
Plug into your computer and listen with a sound program like [Audacity](https://audacity.sourceforge.net/).
Try some examples from the __File > Examples > Mozzi__ menu.

{% include boards-tested.markdown %}

You could try a [Mozzibyte](https://www.kickstarter.com/projects/mozzibyte/mozzibyte-mozzi-arduino-mini-sensor-sonification-s), a little board for Mozzi with an amp and sockets for headphone/line and speaker.

For more about audio output, including HIFI mode, read the [Mozzi core module documentation](https://sensorium.github.com/Mozzi/doc/html/group__core.html#gae99eb43cb29bb03d862ae829999916c4/).


## Using Mozzi
Here's a template for an empty Mozzi sketch:

{% highlight c++ %}
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
{% endhighlight %}

There's a detailed example explaining the different parts [here](learn/a-simple-sketch/).


## Documentation

There's documentation in the extras/doc folder in the Mozzi download and [online](https://sensorium.github.com/Mozzi/doc/html/index.html).
There is practical help on the [learn](learn/) page on the Mozzi site.
Start or look up a topic on the [users forum](https://groups.google.com/forum/#!forum/mozzi-users/).
Also, feel free to submit any issues on the [GitHub Mozzi site](https://github.com/sensorium/Mozzi/issues/).
Look for code and usage changes [here](https://github.com/sensorium/Mozzi/blob/gh-pages/examples/extras/NEWS.txt).


## Wierd things

#### AVR

* While Mozzi is running, calling `delay()`, `delayMicroseconds()`, or other functions which wait or cycle through loops can cause audio glitches.
Mozzi provides `EventDelay()` for scheduling instead of `delay()`.

* `analogRead()` is replaced by `mozziAnalogRead()`, which works in the background instead of blocking the processor.  

* Mozzi interferes with `analogWrite()`.  In `STANDARD` and `STANDARD_PLUS` audio modes, Mozzi takes over Timer1 (pins 9 and 10), but you can use the Timer2 pins, 3 and 11 (your board may differ).  In `HIFI` mode,
Mozzi uses Timer1 (or Timer4 on some boards), and Timer2, so pins 3 and 11 are also out.  
If you need `analogWrite()`, you can do PWM output on any digital pins using the technique in
*Mozzi>examples>11.Communication>Sinewave_PWM_pins_HIFI*.  

* Mozzi provides`mozziMicros()` as an alternative to 'millis()' and 'micros()'.  

* The timers can be made available with `stopMozzi()`, which stops audio interrupts, until you call `startMozzi()`.  


## Contributions / Included Dependencies  
Modified versions of the following libraries are included in the Mozzi download:  

[TimerOne library](https://www.pjrc.com/teensy/td_libs_TimerOne.html)  
[FrequencyTimer2 library](https://www.pjrc.com/teensy/td_libs_FrequencyTimer2.html) - now a [fork with support for ATmega32u4 processors](https://github.com/sensorium/FrequencyTimer2/)   

Mozzi has also drawn on and been influenced by (among many others):  

[xorshift](https://school.anhb.uwa.edu.au/personalpages/kwessen/shared/Marsaglia03.html) random number generator, George Marsaglia, (2003)  
ead~.c puredata external (creb library) Copyright (c) 2000-2003 by Tom Schouten (GPL2)  
[AF_precision_synthesis](https://adrianfreed.com/content/arduino-sketch-high-frequency-precision-sine-wave-tone-sound-synthesis)
by Adrian Freed, 2009  
[Resonant filter](https://www.musicdsp.org/archive.php?classid=3#259) posted to musicdsp.org by Paul Kellett,
and fixed point version of the filter on [dave's blog of art and programming](https://www.pawfal.org/dave/blog/2011/09/)  
State Variable filter pseudocode at musicdsp.org [here](https://www.musicdsp.org/showone.php?id=23) and [here](https://www.musicdsp.org/showone.php?id=142)  
Various examples from [Pure Data](https://puredata.info/) by Miller Puckette  
[Practical synthesis tutorials](https://www.moz.ac.at/sem/lehre/lib/pd-sounddesign/index.html) by Andy Farnell  


## Hardware specific notes

### STM32(duino)
port by Thomas Friedrichsmeier

Compiles for and runs on a STM32F103C8T6 blue pill board, with a bunch of caveats (see below), i.e. on a board _without_ a
real DAC. Should probably run on any other board supported by [STM32duino](https://github.com/rogerclarkmelbourne/Arduino_STM32).

- You will need a very recent (12/2017) checkout of the Arduino_STM32 repository, otherwise compilation will fail.
- Audio output is to pin PB8, by default (HIFI-mode: PB8 and PB9)
- If you want to use MIDI, be sure to replace "MIDI_CREATE_DEFAULT_INSTANCE()" with "MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI)" (or Serial2)
- Timers 4 (PWM output), and 2 (audio rate) are used by default.
- The STM32-specific options (pins, timers) can be configured in AudioConfigSTM32.h
- Default audio resolution is currently set to 10 bits, which yields 70khZ PWM frequency on a 72MHz CPU. HIFI mode is 2*7bits at up to 560Khz (but limited to 5 times audio rate)
- HIFI_MODE did not get much testing
- STEREO_HACK not yet implemented (although that should not be too hard to do)
- AUDIO_INPUT is completely untested (but implemented in theory)
- Note that AUDIO_INPUT and mozziAnalogRead() return values in the STM32's full ADC resolution of 0-4095 rather than AVR's 0-1023.
- twi_nonblock is not ported

### Teensy 3.0/3.1/3.2/3.4/3.5/LC

Extra libraries required for use withTeensy 3.*:
These are included in the standard Teensyduino install unless you explicitly disable them
- [Timer library](https://github.com/loglow/IntervalTimer) for Teensy 3.* by Daniel Gilbert
- [ADC library](https://github.com/pedvide/ADC) by Pedro Villanueva

Some of the differences for Teensy 3.0# which will affect users include:

- On Teeensy 3.0/3.1/3.2/Audio output is on pin A14/DAC, in STANDARD or STANDARD_PLUS audio modes.
    These modes are identical on Teensy 3.0/3.1/3.2, as the output is via DAC rather than PWM.
- Output is 12 bits in STANDARD and STANDARD_PLUS modes, up from nearly 9 bits for Atmel based boards. HIFI audio, which works by summing two output pins, is not available on Teensy 3.0/3.1.
- #include <ADC.h> is required at the top of every Teensy 3 sketch.
- The examples come with this commented out, for Arduino compatibility.
- Serial baud rate for monitoring in the IDE needs to be set to 9600 to work with Teensy 3.0/3.1/3.2. This slow rate can cause audio glitches.
- twi_nonblock code by Marije Baalman for non-blocking I2C is not compatible with Teensy 3.0/3.1/3.2.

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
  - PDM_VIA_I2S: Output is coded using pulse density modulation, and sent via the I2S pins. The I2S data out pin (which is also "RX") will have the output, but *all* I2S output pins (RX, GPIO2 and GPIO15) will be
  affected. Mozzi tries to set GPIO2 and GPIO15 to input mode, and *at the time of this writing*, this allows I2S output on RX even on boards such as the ESP01 (where GPIO15 is tied to Gnd). However, it seems safest to
  assume that this mode may not be useable on boards where GPIO2 or GPIO15 are not available as output pins.
  - EXTERNAL_DAC_VIA_I2S: Output is sent to an external DAC (such as a PT8211), digitally coded. This is the only mode that supports STEREO_HACK. It also need the least processing power.
    - TODO: Untested!
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

***

## Contact   
If you enjoy using Mozzi for a project, or have extended it, we would be
pleased to hear about it and provide support wherever possible. Contribute
suggestions, improvements and bug fixes to the [users forum](https://groups.google.com/forum/#!forum/mozzi-users/)
or [GitHub Mozzi site](https://github.com/sensorium/Mozzi/issues/). Fork it to contribute directly to future developments.

Mozzi began as a development of research into Mobile Sonification in the
[SweatSonics](https://stephenbarrass.wordpress.com/tag/sweatsonics/) project.

***
