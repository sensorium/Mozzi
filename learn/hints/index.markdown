---
layout: single
---

## Efficient code for smooth audio

* Writing speed-efficient code can be tricky, because the Arduino IDE sets
the compiler for small code size rather than speed!  Here's a great read ; ) [Atmel AVR4027: Tips and Tricks to Optimize. Your C Code for 8-bit AVR Microcontrollers](https://ww1.microchip.com/downloads/en/AppNotes/doc8453.pdf)

* Pre-calculate where possible. Do as much as you can in `setup()` and
`updateControl()`, and keep `updateAudio()` lean.

* Avoid division! Use powers of two for numbers where you can, and use
[bit-shifting arithmetic](https://arduino.cc/en/Reference/Bitshift) instead of multiplication and division where possible.  Shifting by 8 or 16 is faster than other amounts.

---

## Number types

* For speed, use the smallest data types you can, and unsigned where possible.

* **Choose variables or casts which provide room for intermediate results!**

* Take care declaring and casting variables - this can affect speed and also be **a source of unexpected sounds**.

* Use integer and fixed point maths, and avoid floating point, particularly in `updateAudio()`. Mozzi has a collection of fixed-point number types and conversion functions in __mozzi_fixmath.h__. These are often much faster than floating point, but they can over - or under - flow if you're not careful.

---

## Debugging

* Printing to the Arduino Serial monitor is a useful tool for debugging, but beware of trying to print too much or your whole computer might freeze! It's mostly OK to print from `updateControl()` with a `CONTROL_RATE` of 64, and it works well with a baud rate of 9600, ie. `Serial.begin(9600)`. Remember to set it in the monitor window too. Notice that using Serial adds size to your sketch and can often disrupt audio, so check if you've got print statements interfering if you're hearing audio clicks.

* To print audio rate debugging info, don't use the Arduino serial monitor, try [CoolTerm](https://freeware.the-meiers.org/), it's a great application which performs much better and can capture incoming data to a file.

* An oscilloscope is useful for timing your code. Mozzi has fast code to flip digital pin 13 without interfering much with the audio timing. Put `#include mozzi_utils.h` in your sketch and `setPin13Out()` in `setup()`, then use `setPin13High()` and `setPin13Low()` around the code you want to time.

---

## Cool code

* In `updateAudio()` you can store "submixes" in variables, to eventually make the return line easier to read and troubleshoot. (Thanks to Charles Peck for this tip).

___

## Listening and analysing

* A computer line-in works well for listening, and some good tools to record or examine your sounds are [Audacity](https://www.audacityteam.org/download/),
[Baudline](https://www.baudline.com/) signal analyser, and
[SignalScope](https://www.faberacoustical.com/products/signalscope/).

* In Audacity, set the Project Rate to Mozzi's audio rate of 16384 Hz before recording. This will make the PWM artifacts disappear and the waves will look like normal, non-PWM sound. You need to adjust the recording level by ear, because the sound distorts at a lower level than expected even though the levels in the window look OK.

* If you're stuck with a fixed recording rate and you need to see what's
happening in your synthesis, filtering makes a big difference. There's a Pure Data patch in Mozzi/pd with 4 4000Hz filters in series which can be used with a utility like Jack or Soundflower to filter the audio before it gets to your sound program. This allows you to check what's happening in real time.

---

## Keep up to date!
* Mozzi is actively developing, so make sure you've got the latest version, and the [users' forum](https://groups.google.com/forum/#!forum/mozzi-users/) is there to help.
