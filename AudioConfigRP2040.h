#ifndef AUDIOCONFIGRP2040_H
#define AUDIOCONFIGRP2040_H


// Sample frequency
#define AUDIO_RATE_PLATFORM_DEFAULT 32768

// this is driving the audio bias 
#define AUDIO_BITS 12
// Used internally to put the 0-biased generated audio into the centre of the output range (12 bits) -> Range from - 2048 to + 2048
#define AUDIO_BIAS ((uint16_t) 2048)

// The pwm counter is counting from 0 - 4095 
#define PWM_CYCLES ((AUDIO_BIAS * 2) -1)

// the PWM counter runs at clk_sys / div; The maximum clk_sys clock speed is 133MHz at normal core voltage
// PWM carrier frequency should be well out of hearing range, about 5 times the nyquist frequency if possible. 
//#define PWM_RATE 125000
//#define PWM_CLOCK_DIV 133000000.0 / PWM_CYCLES / PWM_RATE
// run at full speed
#define PWM_CLOCK_DIV 1.0

// Pins - Please make sure they are on the same channel!
#define AUDIO_CHANNEL_1_PIN 2
#define AUDIO_CHANNEL_2_PIN 3
#define AUDIO_CHANNEL_1_PIN_HIGH 3

// Overwrite Input Pins!
#if USE_AUDIO_INPUT == true
#ifdef AUDIO_INPUT_PIN
#undef AUDIO_INPUT_PIN 
#warning We use the AUDIO_INPUT_PIN defined in AudioConfigRP2040.h
#endif
// ADC input. 0...3 are GPIOs 26...29 respectively. Input 4 is the onboard temperature sensor.
#define AUDIO_INPUT_PIN 26
#define AUDIO_CHANNEL_IN 0
#endif


// We do not want to use int because it is 4 bytes -> we switch to 2 bytes instead!
#define AudioOutputStorage_t int16_t

#endif