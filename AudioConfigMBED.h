#ifndef AUDIOCONFIGMBED_H
#define AUDIOCONFIGMBED_H

/* Used internally to put the 0-biased generated audio into the centre of the output range (12 bits) */
#define AUDIO_BIAS ((uint16_t) 2048)
#define AUDIO_BITS 12

#define PWM_RATE 32768

// Pins - Please make sure they are on the same channel!
#define AUDIO_CHANNEL_1_PIN 14
#define AUDIO_CHANNEL_2_PIN 15
#define AUDIO_CHANNEL_1_PIN_HIGH 15
// Analog input
#define AUDIO_CHANNEL_1_PIN A0

// Bypass buffering -> only false is supported
#define LOG_OUTPUT Serial

#endif