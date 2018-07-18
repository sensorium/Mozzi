---
layout: single
---

<a id = "top"> </a>  
[1. Everything sounds glitchy, including the examples.](#q1)   
[2. Everything sounds too quiet.](#q2)  
[3. My patch was working and now it sounds horrible!](#q3)  
[4. How can I optimise my code to get the most out of Mozzi?](#q4)  
[5. How can I extend Mozzi? ](#q5)  
[6. Can I contribute developments?  How?](#q6)  

---

<a id="q1"></a>
### Everything sounds glitchy, including the examples.

Maybe you're listening to a `STANDARD` or `STANDARD_PLUS` sketch, but `AUDIO_MODE` is set to `HIFI`.
Check the file __Mozzi/mozzi_config.h__.  Look for:
``` c
//#define AUDIO_MODE STANDARD
#define AUDIO_MODE STANDARD_PLUS
//#define AUDIO_MODE HIFI
```
For `STANDARD_PLUS` mode, the lines should appear as above,
with the `STANDARD` and `HIFI` lines commented out.  Most of the examples work in
`STANDARD_PLUS` mode, unless they say otherwise.

[(go to top)](#top)   

---

<a id="q2"></a>
### Everything sounds too quiet.

Check if you're listening to a `HIFI` sketch in `STANDARD` or `STANDARD_PLUS` mode.  See the previous answer.

[(go to top)](#top)  

---

<a id="q3"></a>
### My patch was working and now it sounds horrible!

* Has the `AUDIO_MODE` changed in __Mozzi/mozzi_config.h__?  
* Is the processor having trouble keeping up with the amount of calculations it's doing?  
* Has a signed or unsigned variable been cast to the wrong sign?  
* Is there a data type overflowing somewhere?  
* Are there `Serial.print()` statements slowing things down?  

[(go to top)](#top)  

---

<a id="q4"></a>
### How can I optimise my code to get the most out of Mozzi?

Read this: [Atmel AVR4027: Tips and Tricks to Optimize. Your C Code for 8-bit AVR Microcontrollers](http://ww1.microchip.com/downloads/en/AppNotes/doc8453.pdf).  
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
It might make use of `AUDIO_RATE` or `CONTROL_RATE`.
It seems self-explanatory to me, but then I haven't ever explained it to anyone.

[(go to top)](#top)  

---

<a id="q6"></a>
### Can I contribute developments?  How?

[Fork Mozzi on GitHub](https://github.com/sensorium/Mozzi), and your changes will become part of the "development graph", or just post to the [users' list](https://groups.google.com/forum/#!forum/mozzi-users/).

[(go to top)](#top)  

---

To be continued...
