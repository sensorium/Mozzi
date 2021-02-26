#ifndef AUDIOCONFIGSAMD21_H
#define AUDIOCONFIGSAMD21_H

/* Note: SAMD21 has 12 bits ADC, but only 10 bits DAC. See https://github.com/sensorium/Mozzi/issues/75 */
#define AUDIO_BITS 10

/** @ingroup core
*/
/* Used internally to put the 0-biased generated audio into the centre of the output range (10 bits) */
#define AUDIO_BIAS ((uint16_t) 1 << (AUDIO_BITS - 1))

#define AUDIO_CHANNEL_1_PIN DAC0

#endif        //  #ifndef AUDIOCONFIGSAMD21_H

