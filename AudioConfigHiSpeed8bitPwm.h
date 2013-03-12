#ifndef AUDIOCONFIGHISPEED8BITPWM_H
#define AUDIOCONFIGHISPEED8BITPWM_H


/** 8 bit sound at 16384 Hz and 62.5kHz pwm rate
Timer 1: outputs samples at AUDIO_RATE 16384 Hz, by setting Timer 2 pwm level 
Timer 2: PWM 62.5 kHz
Output on Timer2, Pin 3.
*/
#define AUDIO_RATE 16384

/* Used internally for audio-rate optimisation.*/
#define AUDIO_RATE_AS_LSHIFT 14

#define AUDIO_CHANNEL_1_HIGHBYTE_PIN TIMER1_A_PIN // defined in TimerOne/config/known_16bit_timers.h
#define AUDIO_CHANNEL_1_LOWBYTE_PIN TIMER1_B_PIN
#define AUDIO_CHANNEL_1_HIGHBYTE_REGISTER OCR1AL
#define AUDIO_CHANNEL_1_LOWBYTE_REGISTER OCR1BL

#define AUDIO_PWM_RESOLUTION 65536
/* Used internally to put the 0-biased generated audio into the right range for PWM output.*/
#define AUDIO_BIAS 32768

#endif        //  #ifndef AUDIOCONFIGHISPEED8BITPWM_H

