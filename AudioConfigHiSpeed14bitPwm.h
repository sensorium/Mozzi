#ifndef AUDIOCONFIGHISPEED14BITPWM_H
#define AUDIOCONFIGHISPEED14BITPWM_H

/*
14 bit sound at 16384 Hz and 125kHz pwm rate
Timer 1: PWM 125kHz 
Timer 2: called at AUDIO_RATE 16384 Hz, setting Timer1 pwm levels
Output on Timer1, low byte on Pin 10, and high byte on Pin 9 (on 328 based Arduino boards)
Add signals through a 3.9k resistor on high byte pin and 499k resistor on low byte pin.
Use 0.5% resistors or select the most accurate from a batch.
As discussed on http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
Also, there are higher quality output circuits are on the site.

Boards, pins and resistor positions are documented in MozziGuts.h
*/

/* PWM carrier frequency, for HIFI this should be well out of hearing range, about 5 times the nyquist frequency if possible. */
#define PWM_RATE 125000
// following doesn't play nice
//#define PWM_RATE 65536 // count will be 244 (7+ bits) on each pin = 14+ bits


// pins defined in TimerOne/config/known_16bit_timers.h
#define AUDIO_CHANNEL_1_HIGHBYTE_PIN TIMER1_A_PIN // 3.9k resistor
#define AUDIO_CHANNEL_1_LOWBYTE_PIN TIMER1_B_PIN // 1 M resistor
#define AUDIO_CHANNEL_1_HIGHBYTE_REGISTER OCR1AL
#define AUDIO_CHANNEL_1_LOWBYTE_REGISTER OCR1BL

/* Used internally to put the 0-biased generated audio into the right range for PWM output.*/
// 14 bit
#define AUDIO_BIAS ((unsigned int) 0x2000)


#endif        //  #ifndef AUDIOCONFIGHISPEED14BITPWM_H

