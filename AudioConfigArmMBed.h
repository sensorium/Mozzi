#ifndef AUDIOCONFIGMBED_H
#define AUDIOCONFIGMBED_H

/**
 * @brief with a PWM Rate of 40000 we can output around 39000 samples on 2 channels. So to be save side I am setting
 * the Audio Rate to 22000. If you exheed the limit the processor starts to blink with morse code...
 */


// Number of output / input bits
#define AUDIO_BITS 12
// Used internally to put the 0-biased generated audio into the centre of the output range (12 bits) = (2 ** AUDIO_BITS) / 2
#define AUDIO_BIAS ((uint16_t) 2048)

// Basic PWM frequency
#define PWM_RATE 40000

// Sample frequency
#define AUDIO_RATE 22000

// Pins - We just define the first 2 pins - Pins for additional channels can be defined via the API
#define AUDIO_CHANNEL_1_PIN 2
#define AUDIO_CHANNEL_2_PIN 3
#define AUDIO_CHANNEL_1_PIN_HIGH 3

// Analog input - only active when USE_AUDIO_INPUT = true in mozzi_config.h
#define AUDIO_INPUT_PIN A0

// We do not want to use the default which is int because it is 4 bytes -> we switch to 2 bytes instead!
#define AudioOutputStorage_t int16_t

// Default ouput implementation class
#define PLATFORM_OUTPUT_CLASS MozziMBED

#endif