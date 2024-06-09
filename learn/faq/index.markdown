---
layout: single
toc: true
---

<a id="top"></a>

---

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

### My patch was working and now it sounds horrible!

* Have you done any changes to the `MOZZI_AUDIO_MODE`?  
* Is the processor having trouble keeping up with the amount of calculations it's doing?  
* Has a signed or unsigned variable been cast to the wrong sign?  
* Is there a data type overflowing somewhere?  
* Are there `Serial.print()` statements slowing things down?  

[(go to top)](#top)  

---

### How can I optimise my code to get the most out of Mozzi?

Read this: [Atmel AVR4027: Tips and Tricks to Optimize. Your C Code for 8-bit AVR Microcontrollers](https://ww1.microchip.com/downloads/en/AppNotes/doc8453.pdf).  
And there are a few hints [here](../learn/hints/) on the Mozzi site.

Also, to make your compiled code run slightly faster, Arduino versions above 1.5 can be tweaked to optimise for speed instead of small size.

Find Arduino’s platform.txt (on OSX you can find it by searching in Users/your_name/Library/Arduino15). Search and replace -Os with -O2. Save.

It’s explained more thoroughly (for Windows) [here](https://www.instructables.com/id/Arduino-IDE-16x-compiler-optimisations-faster-code/?ALLSTEPS).

If you _still_ need more speed, Arduino 1.0.5 compiles code that runs slightly faster.


[(go to top)](#top)  

---

### I added a second .cpp-file to my project, and now I get "duplicate definition" errors while linking!

To allow direct configuration of Mozzi, while also optimizing some computations, aggressively, most of Mozzi is actually compiled in the same "compilation unit"
as your sketch. However, when including ```Mozzi.h``` from both you ```.ino```-file, and a separate ```.cpp``` file, this will essentially cause Mozzi to
be included twice, resulting in a linker error. Instead, include ```Mozzi.h``` in only one of the files, and, instead, include ```MozziHeadersOnly.h``` in the
other. (Alternatively concentrate all calls to Mozzi-code in either of the files, and don't include Mozzi-headers at all, in the other.)

[(go to top)](#top)  

---

### I'm trying to use multiplexing of analog input, but it doesn't work in Mozzi!

Depending on your board, trying to use analog input multiplexing libraries such as PotMux may either fail to compile, or fail to produce meaningful readings.
To fix this, set Mozzi's analog input to disabled at the top of your sketch: ```#define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE```. Keep in mind, however,
that analog read implementations are usually synchronous, and thus slow, especially on the classic AVR-based Arduinos. It will generally be a good idea to
limit the number of analog reads per ```updateControl()```, e.g. by only taking every other (or even every 10th) reading per iteration.

[(go to top)](#top)  

---

### Somehow my configuration options do not seem to work

Be sure to always adhere the following pattern
```
// No other Mozzi include above this line
#include <MozziConfigValues.h>
#define MOZZI_SOME_OPTION MOZZI_SOME_VALUE
[...]
#include <Mozzi.h>
```
Also, in case you are including Mozzi from more than one own source file, be sure to set **identic** options in both files.
See [Configuring Mozzi](../configuration/) for some additional hints.

[(go to top)](#top)  

---

### My sketch used to work, and now it will no longer compile!

We try to keep Mozzi backwards compatible with your earlier sketches as much as possible, but sometimes that will not be 100% possible. If that is the case,
adjusting your sketch to work with the latest and greatest Mozzi is generally very simple, however. See [Porting](../porting_sketches/) for specific
information on what has changed, and how to adjust.

[(go to top)](#top)  

---

### How can I extend Mozzi?

It's easy!  I usually copy the simplest class.h file I can find in the Mozzi folder and base the next thing on that.
Usually it'll have a `next()` method, maybe a couple of `set()` methods and whetever else you decide on.
It might make use of `MOZZI_AUDIO_RATE` or `MOZZI_CONTROL_RATE`.
It seems self-explanatory to me, but then I haven't ever explained it to anyone.

[(go to top)](#top)  

---

### How can I use Mozzi on my all new hardware?

If your CPU is not yet supported by Mozzi, things are a bit more involved. However, by now, ports to many very
different MCU exist, and so you can go by example to quite some degreee.

The whole process is outlined in some detail in [internal/MOzziGuts_impl_template.cpp](https://github.com/sensorium/Mozzi/blob/master/internal/MozziGuts_impl_template.hpp).

[(go to top)](#top)  

---

### Can I contribute developments? How?

[Fork Mozzi on GitHub](https://github.com/sensorium/Mozzi), and your changes will become part of the "development graph", or just post to the [users' list](https://groups.google.com/forum/#!forum/mozzi-users/).

[(go to top)](#top)  

---

To be continued...
