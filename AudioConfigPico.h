#ifndef AUDIOCONFIGPICO_H
#define AUDIOCONFIGPICO_H

#include "hardware/pwm.h"

#define AUDIO_BITS 12
#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))

#define AUDIO_CHANNEL_1_PIN 2
#define AUDIO_CHANNEL_2_PIN 3

#warning pi pico is experimental


#endif        //  #ifndef AUDIOCONFIGPICO_H
