#ifndef AUDIOCONFIGSAMD51_H
#define AUDIOCONFIGSAMD51_H

/* SAMD51 has 12 bits DAC */
#define AUDIO_BITS 12

/** @ingroup core
*/
/* Used internally to put the 0-biased generated audio into the centre of the output range (12 bits) */
#define AUDIO_BIAS ((uint16_t) 1 << (AUDIO_BITS - 1))

#define AUDIO_CHANNEL_1_PIN DAC0

#endif        //  #ifndef AUDIOCONFIGSAMD51_H

