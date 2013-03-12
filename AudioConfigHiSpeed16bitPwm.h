#ifndef AUDIOCONFIGHISPEED10BITPWM_H
#define AUDIOCONFIGHISPEED10BITPWM_H


/** 
10 bit sound at 16384 Hz and 250kHz pwm rate
Timer 1: PWM 250kHz 
Timer 2: called at AUDIO_RATE 16384 Hz, setting Timer1 pwm levels
Output on Timer1, low byte on Pin 10, and high byte on Pin 9 (on 328 based Arduino boards)
Add signals through a 3.9k resistor on high byte pin and 1M resistor on low byte pin.
Use 1% or better resistors.
As discussed on http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
Also, higher quality output circuits are on the site.
Lower pwm rates allow greater bit depths, but because the
*/

#define AUDIO_RATE 16384
//#define AUDIO_RATE 32768

/* Used internally for audio-rate optimisation.*/
#define AUDIO_RATE_AS_LSHIFT 14
//#define AUDIO_RATE_AS_LSHIFT 15

// pins defined in TimerOne/config/known_16bit_timers.h
#define AUDIO_CHANNEL_1_HIGHBYTE_PIN TIMER1_A_PIN // 3.9k resistor
#define AUDIO_CHANNEL_1_LOWBYTE_PIN TIMER1_B_PIN // 1 M resistor
#define AUDIO_CHANNEL_1_HIGHBYTE_REGISTER OCR1AL
#define AUDIO_CHANNEL_1_LOWBYTE_REGISTER OCR1BL

/* Used internally to put the 0-biased generated audio into the right range for PWM output.*/
// 16 bit
//#define AUDIO_BIAS 0x8000
// 14 bit
//#define AUDIO_BIAS ((unsigned int) 0x2000) // get period or count midpoint for the power of 2 pwm rates?, won't be so tidy
// 12 bit
#define AUDIO_BIAS 0x800
// 10 bit
//#define AUDIO_BIAS 0x200


#endif        //  #ifndef AUDIOCONFIGHISPEED10BITPWM_H

