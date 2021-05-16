#ifndef AUDIOCONFIGRP2040_H
#define AUDIOCONFIGRP2040_H

/* Used internally to put the 0-biased generated audio into the centre of the output range (12 bits) */
#define AUDIO_BIAS ((uint16_t) 2048)
#define AUDIO_BITS 12

#define PWM_RATE 32768
// The pwm counter is counting from 0 - 4095 
#define PWM_CYCLES ((AUDIO_BIAS * 2) -1)

// the PWM counter runs at clk_sys / div; The maximum clk_sys clock speed is 133MHz at normal core voltage
#define PWM_CLOCK_DIV 133000000.0 / PWM_CYCLES / PWM_RATE

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

// Timer settings
#define ALARM_POOL_HARDWARE_ALARM_NUM 2
#define ALARM_POOL_HARDWARE_ALARM_COUNT 1

// We do not want to use int because it is 4 bytes -> we switch to 2 bytes instead!
#define AudioOutputStorage_t int16_t

#endif