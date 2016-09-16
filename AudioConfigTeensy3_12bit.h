#ifndef AUDIOCONFIGTEENSY3_12BIT_H
#define AUDIOCONFIGTEENSY3_12BIT_H


/** @ingroup core
*/
/* Used internally to put the 0-biased generated audio into the centre of the output range (12 bits on Teensy 3) */
#define AUDIO_BIAS ((uint16_t) 2048)

#define AUDIO_CHANNEL_1_PIN A14

#endif        //  #ifndef AUDIOCONFIGTEENSY3_12BIT_H

