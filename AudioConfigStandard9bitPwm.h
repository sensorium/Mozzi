#ifndef AUDIOCONFIGSTANDARD9BITPWM_H
#define AUDIOCONFIGSTANDARD9BITPWM_H


/** @ingroup core
This is the dynamic range of Mozzi's audio output in STANDARD mode.
It is equal to Timer1.pwmPeriod calculated for interrupt rate 16384.
It's included in the documentation because it's a slightly unusual number and useful to know 
about when you're writing sketches.
*/
#define STANDARD_PWM_RESOLUTION 488

/* PWM carrier frequency, for standard mode this will be the same as the audio rate. */
#define PWM_RATE 16384

/* Used internally to put the 0-biased generated audio into the right range for PWM output.*/
#define AUDIO_BIAS ((uint8_t) 244)

// Used internally.  If there was a channel 2, it would be OCR1B.
#define AUDIO_CHANNEL_1_OUTPUT_REGISTER OCR1A

#define AUDIO_CHANNEL_1_PIN TIMER1_A_PIN // defined in TimerOne/config/known_16bit_timers.h

// Default ouput implementation class
#define PLATFORM_OUTPUT_CLASS MozziAVR


#endif        //  #ifndef AUDIOCONFIGSTANDARD9BITPWM_H

