---
layout: single
toc: true
---

# Mozzi hints for smooth audio

Most MCUs do not have a whole lot of processing power, and producing samples at audio rate in real time is no mean feat. This page shows some tips and tricks for writing
efficient code. The most important thing however is to avoid some common pitfalls. So let's start by looking at that.

# Common pitfalls to avoid

## Avoid blocking code

Perhaps the single most important advice is to avoid calls to `delay()` or `delayMicroseconds()`. During these calls, the MCU just sits idle, doing nothing, while the
audio buffer is running low. Mozzi provides the `EventDelay` as a non-blocking replacement for common use cases (see __File > Examples > Mozzi > 02.Control > EventDelay__).

Blocking code may also linger in place where you do not expect it, however. For instance, you should be aware of blocking in any kind of data transmission. This includes
`Serial.print()`, for instance: Serial generally writes to a hardware buffer (the size depending on your platform), but once that fills up, subsequent calls to `Serial.print()`
will block waiting until enough room is free in the buffer, again. (As a rule of thumb, it will often be ok to have one or two `Serial.print()` statements in `updateControl()`,
and that can be very useful for debugging. Having `Serial.print()` called at audio rate, is asking for trouble, however. See also "Debugging", below.)

Similar problems affect SPI and I2C transmissions, although the usually higher data rate helps reduce the problem. Beware that convenient functions in device drivers often hide blocking code.
This is especially true for functions that return data. For the AVR port, the `twi_nonblock.h` interface has non-blocking alternatives to I2C communication.

## Analog reads

Converting analog input to a digital value (ADC) is a relatively slow operation, and `analogRead()` is - again - spending a lot of time simply doing nothing, while waiting
for the ADC to complete. Placing one or two `analogRead()`s into `updateControl()` may just work in some cases, but it's much more efficient to make analog reads asynchronous.
Fear not, as that is actually quite simple with `mozziAnalogRead()`, which can be used as a drop-in replacement in most cases. Essentially, this will return the lastest value
read on a pin, while the actual ADC is done asynchronously, and the result will be available in one of the next iterations. (This does not work when multiplexing analog
inputs, externally.)

## Floating point numbers

Many MCUs, and the classic Arduino, in particular, are very bad at floating point calculations (i.e. involving data types `float` or even `double`). This will be very slow,
and add a lot to code size. It may not be avoidable in all cases, but is in many. Take at look at the classes `SFixMath` and `UFixMath` for efficient integer-based fixed
point arithmetic (or the older __mozzi_fixmath.h__).

# Further optimizations

## Genral advice

* Here's a great read, written for (but not only relevant to) 8-bit AVR MCUs ; ) [Atmel AVR4027: Tips and Tricks to Optimize. Your C Code for 8-bit AVR Microcontrollers](https://ww1.microchip.com/downloads/en/AppNotes/doc8453.pdf)

* Pre-calculate where possible. Do as much as you can in `setup()` and
`updateControl()`, and keep `updateAudio()` lean.

* Avoid division! Use powers of two for numbers where you can, and use
[bit-shifting arithmetic](https://arduino.cc/en/Reference/Bitshift) instead of multiplication and division where possible.  Shifting by 8 or 16 is faster than other amounts.

* -O2 optimization flag is the default in most arduino cores, todo. Note, that depending on your compiler, you may be able to tweak some using `#pragma GCC optimize("O3")`.
  (But in some cases, the effect may be negative).

## Number types

* For speed, use the smallest data types you can, and unsigned where possible.

* **Choose variables or casts which provide room for intermediate results!**

* Take care declaring and casting variables - this can affect speed and also be **a source of unexpected sounds**.

# Optimizing for smaller code size

Running out of program flash?

* See if you can avoid using `Serial`, floating point arithmetic, and use of the `new` operator. The first use of each may pull in significant portions of code.

* Disabling analog reads (`#define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_NONE`) will save some bytes.

* Obviously, wavetables consume a lot of flash. Perhaps a smaller one will be good enough?

* `#pragma GCC optimize("Os")` at the top may give yield some minor flash savings.

# Debugging

* Printing to the Arduino Serial monitor is a useful tool for debugging, but beware of trying to print too much or your whole computer might freeze! It's mostly OK to print from `updateControl()` with the default `MOZZI_CONTROL_RATE` of 64, and it works well with a baud rate of 9600, ie. `Serial.begin(9600)`. Remember to set it in the monitor window too. Notice that using Serial adds size to your sketch and can often disrupt audio, so check if you've got print statements interfering if you're hearing audio clicks.

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

* In Audacity, set the Project Rate to Mozzi's audio rate of 16384 Hz before recording (at least if you are using Mozzi on a classic Arduino at default settings; most other platforms
deafult to a `MOZZI_AUDIO_RATE` of 32768). This will make the PWM artifacts disappear and the waves will look like normal, non-PWM sound. You need to adjust the recording level by ear,
because the sound distorts at a lower level than expected even though the levels in the window look OK.

* If you're stuck with a fixed recording rate and you need to see what's
happening in your synthesis, filtering makes a big difference. There's a Pure Data patch in Mozzi/pd with 4 4000Hz filters in series which can be used with a utility like Jack or Soundflower to filter the audio before it gets to your sound program. This allows you to check what's happening in real time.

---

## Keep up to date!
* Mozzi is actively developing, so make sure you've got the latest version, and the [users' forum](https://groups.google.com/forum/#!forum/mozzi-users/) is there to help.
