---
layout: single
---

The interface between Mozzi and the Arduino environment consists of four main functions. These are `startMozzi()`, `updateAudio()`, `updateControl()` and `audioHook()`, visible in the “User space” section in the figure below. All four are required for a Mozzi sketch to compile.

![Mozzi system architecture](images/Mozzi-system.jpg)

`startMozzi(control_rate)` goes in Arduino's `setup()`. It starts a timer which regularly sends audio out from the audio output buffer, and calls updateControl() at the requested control rate given in Hz as a parameter, or defaulting to 64 Hz without a parameter.

`updateControl()` is where any analog or digital input sensing code should be placed and relatively slow changes such as LFO's or frequency changes can be performed.

`updateAudio()` is where audio synthesis code should be placed. This runs on average 16384 times per second, so code here needs to be lean. The only other strict requirement is that it returns an integer between -244 and 243 inclusive in STANDARD mode or -8192 to 8191 in HIFI mode.

`audioHook()` goes in Arduino's `loop()`. It wraps `updateAudio()` and takes care of filling the output buffer, hiding the details of this from user space.

Mozzi uses hardware interrupts on the processor which automatically call
interrupt service routines (ISR) at regular intervals. `startMozzi()` sets
up an interrupt for audio output at a sample rate of 16384 Hz. A counter in the audio output routine is used to call `updateControl`.  In earlier versions, a separate interrupt on Timer 0 was used for control.  

In `STANDARD_PLUS` (and old `STANDARD`) mode, the 16 bit Timer 1 is used by Mozzi on the ATmega processors for audio and control.  

`HIFI` mode additionally employs Timer 2 with Timer 1 for audio.

The output buffer has 256 cells which equates to a maximum latency of about 15 milliseconds, to give leeway for control operations without interrupting audio output. The buffer is emptied behind the scenes by the regular 16384 Hz audio interrupt.
