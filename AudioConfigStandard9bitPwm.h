#ifndef AUDIOCONFIGSTANDARD9BITPWM_H
#define AUDIOCONFIGSTANDARD9BITPWM_H


/** Original Mozzi output configuration.
Almost 9 bit sound at 16384 Hz and 16384 kHz pwm rate
Timer 1: outputs samples at AUDIO_RATE 16384 Hz, by setting Timer 1 pwm level 
Output on Timer1, Pin 9.
*/


/** @ingroup core
This is the dynamic range of Mozzi's audio output in STANDARD mode.
It is equal to Timer1.pwmPeriod calculated for interrupt rate 16384.  
*/
#define AUDIO_PWM_RESOLUTION 488

/* Used internally to put the 0-biased generated audio into the right range for PWM output.*/
#define AUDIO_BIAS ((unsigned char) 244)

// Used internally.  If there was a channel 2, it would be OCR1B.
#define AUDIO_CHANNEL_1_OUTPUT_REGISTER OCR1A

#define AUDIO_CHANNEL_1_PIN TIMER1_A_PIN // defined in TimerOne/config/known_16bit_timers.h

#endif        //  #ifndef AUDIOCONFIGSTANDARD9BITPWM_H

