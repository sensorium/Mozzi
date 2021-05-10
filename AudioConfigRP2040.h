#pragma once

/* Used internally to put the 0-biased generated audio into the centre of the output range (12 bits on Teensy 3) */
#define AUDIO_BIAS ((uint16_t) 2048)
#define AUDIO_BITS 12

#define PWM_RATE 32768
// The pwm counter is counting from 0 - 4095 
#define PWM_CYCLES ((AUDIO_BIAS * 2) -1)

// the PWM counter runs at clk_sys / div; The maximum clk_sys clock speed is 133MHz at normal core voltage
#define PWM_CLOCK_DIV 133000000.0 / PWM_CYCLES / PWM_RATE

// Pins - Please make sure they are on the same channel!
#define AUDIO_CHANNEL_1_PIN 14
#define AUDIO_CHANNEL_2_PIN 15
#define AUDIO_CHANNEL_1_PIN_HIGH 15

// Timer settings
#define ALARM_POOL_HARDWARE_ALARM_NUM 2
#define ALARM_POOL_HARDWARE_ALARM_COUNT 1

// Bypass buffering -> only false is supported
#define BYPASS_MOZZI_OUTPUT_BUFFER false
#define LOG_OUTPUT Serial