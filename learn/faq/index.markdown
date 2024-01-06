---
layout: single
---

<a id = "top"> </a>  
[1. Everything sounds glitchy, including the examples.](#q1)   
[2. Everything sounds too quiet.](#q2)  
[3. My patch was working and now it sounds horrible!](#q3)  
[4. How can I optimise my code to get the most out of Mozzi?](#q4)  
[5. How can I extend Mozzi? ](#q5)  
[6. How can I use Mozzi on my all new hardware? ](#q6)  
[7. Can I contribute developments?  How?](#q7)  

---

<a id="q1"></a>
### Everything sounds glitchy, including the examples.

Check that you output circuit matches the output mode (`MOZZI_AUDIO_MODE`) you have configured.
For instance, `MOZZI_OUTPUT_MODE_2PIN_PWM` (aka "HIFI") needs an entirely different hardware
setup than `MOZZI_OUTPUT_MODE_PWM`.

Most of the examples use the default output mode, but some do not. Be sure to read the advice
given at the top of each example, and - especially if using hardware other than the classic
Arduino AVR boards - make sure you understand what pins to expect the audio output on.

TODO: Link to hardware and config sections in the API docs.

[(go to top)](#top)   

---

<a id="q2"></a>
### Everything sounds too quiet (or too loud and is clipping).

You may have miscalculated the sample range in your `updateAudio()` function, and need to
scale from a different number of bits (e.g. feeding only 14 bits into `MonoOutput::from16Bit(sample)`
will sound too quiet, and you'd need `MonoOutput::fromNBit(14, sample)`, instead).

When trying some older example found on the internet, it's even quite possible that the
`updateAudio()` function does not perform any scaling at all, and simply returns a plain integer value
that was appropriate for whatever hardware and configuration they used. In this case, try to understand
the range produced in those examples, and conver to an appropriate `MonoOutput::fromXY()` scaling,
instead.

[(go to top)](#top)  

---

<a id="q3"></a>
### My patch was working and now it sounds horrible!

* Have you done any changes to the `MOZZI_AUDIO_MODE`?  
* Is the processor having trouble keeping up with the amount of calculations it's doing?  
* Has a signed or unsigned variable been cast to the wrong sign?  
* Is there a data type overflowing somewhere?  
* Are there `Serial.print()` statements slowing things down?  

[(go to top)](#top)  

---

<a id="q4"></a>
### How can I optimise my code to get the most out of Mozzi?

Read this: [Atmel AVR4027: Tips and Tricks to Optimize. Your C Code for 8-bit AVR Microcontrollers](https://ww1.microchip.com/downloads/en/AppNotes/doc8453.pdf).  
And there are a few hints [here](../learn/hints/) on the Mozzi site.

Also, to make your compiled code run slightly faster, Arduino versions above 1.5 can be tweaked to optimise for speed instead of small size.

Find Arduino’s platform.txt (on OSX you can find it by searching in Users/your_name/Library/Arduino15). Search and replace -Os with -O2. Save.

It’s explained more thoroughly (for Windows) [here](https://www.instructables.com/id/Arduino-IDE-16x-compiler-optimisations-faster-code/?ALLSTEPS).

If you _still_ need more speed, Arduino 1.0.5 compiles code that runs slightly faster.


[(go to top)](#top)  

---

<a id="q5"></a>
### How can I extend Mozzi?

It's easy!  I usually copy the simplest class.h file I can find in the Mozzi folder and base the next thing on that.
Usually it'll have a `next()` method, maybe a couple of `set()` methods and whetever else you decide on.
It might make use of `MOZZI_AUDIO_RATE` or `MOZZI_CONTROL_RATE`.
It seems self-explanatory to me, but then I haven't ever explained it to anyone.

[(go to top)](#top)  

---

<a id="q6"></a>
### How can I use Mozzi on my all new hardware?

If your CPU is not yet supported by Mozzi, things are a bit more involved. However, by now, ports to many very
different MCU exist, and so you can go by example to quite some degreee.

The whole process is outlined in some detail in [internal/MOzziGuts_impl_template.cpp](https://github.com/sensorium/Mozzi/blob/master/internal/MozziGuts_impl_template.hpp).

[(go to top)](#top)  

---

<a id="q7"></a>
### Can I contribute developments?  How?

[Fork Mozzi on GitHub](https://github.com/sensorium/Mozzi), and your changes will become part of the "development graph", or just post to the [users' list](https://groups.google.com/forum/#!forum/mozzi-users/).

[(go to top)](#top)  

---

To be continued...
