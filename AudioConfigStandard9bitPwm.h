#ifndef AUDIOCONFIGSTANDARD9BITPWM_H
#define AUDIOCONFIGSTANDARD9BITPWM_H


/** Original Mozzi output configuration.
Almost 9 bit sound at 16384 Hz and 16384 kHz pwm rate
Timer 1: outputs samples at AUDIO_RATE 16384 Hz, by setting Timer 1 pwm level 
Output on Timer1, Pin 9.
*/
//#define STANDARD_9_BIT_PWM 1

/** @ingroup core
AUDIO_RATE is fixed at 16384 Hz for now. 
This is a compromise between the sample rate (interrupt rate) and sample
bitdepth (pwm width), which are interdependent due to the way pulse wave
modulation is used to generate the sound output. With the AUDIO_RATE at 16384,
the sample resolution is 488, which provides some headroom above the 8bit table
resolution currently used by the oscillators. You can look at the TimerOne
library for more info about how interrupt rate and pwm resolution relate. 
@todo Possible option for output to R/2R DAC circuit, like
http://blog.makezine.com/2008/05/29/makeit-protodac-shield-fo/ This would limit
dynamic range to 8 bit, but would remove the 16384Hz pwm carrier frequency noise
which can be a problem in some applications, requiring filtering to remove (see
the Mozzi wiki for filter schematics).
*/

#define AUDIO_RATE 16384

/* Used internally for audio-rate optimisation.*/
#define AUDIO_RATE_AS_LSHIFT 14

/** @ingroup core
This is the maximum sample resolution. 8 bit (ie. 256) is usually fine but
there might be times where the extra headroom is useful. Use powers of two in
audio calculations and use right shifts for divisions where possible. Also, in
tests while writing Mozzi, shifts on byte boundaries seem to work fastest.
For example, >> 8 is faster than >> 4. Test your timings where possible, using
an oscilloscope.  To test timing, include utils.h in your sketch, put SET_PIN13_OUT
in setup(), then SET_PIN13_HIGH and SET_PIN13_LOW around your test code
(see <a href="group__util.html">Mozzi utility functions</a>).
*/


/** @ingroup core
This is the dynamic range of Mozzi's audio output, equal to Timer1.pwmPeriod calculated for interrupt rate 16384.  
*/
#define AUDIO_PWM_RESOLUTION 488
/* Used internally to put the 0-biased generated audio into the right range for PWM output.*/
#define AUDIO_BIAS ((unsigned char) 244)

// Used internally.  If there was a channel 2, it would be OCR1B.
#define AUDIO_CHANNEL_1_OUTPUT_REGISTER OCR1A

#define AUDIO_CHANNEL_1_PIN TIMER1_A_PIN // defined in TimerOne/config/known_16bit_timers.h

#endif        //  #ifndef AUDIOCONFIGSTANDARD9BITPWM_H

