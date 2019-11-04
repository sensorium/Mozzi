#ifndef AUDIOCONFIGSAMD21_H
#define AUDIOCONFIGSAMD21_H


/** This may need adjusting: SAMD21-boards seem to come with different resolution DACs. The Adafruit M0 has 10 bits.
 *  see https://github.com/sensorium/Mozzi/issues/75 */
#if defined(ADAFRUIT_FEATHER_M0)
#define AUDIO_BITS 10
#else
#define AUDIO_BITS 12
#endif

/** @ingroup core
*/
/* Used internally to put the 0-biased generated audio into the centre of the output range */
#define AUDIO_BIAS ((uint16_t) 1 << (AUDIO_BITS - 1))


#define AUDIO_CHANNEL_1_PIN DAC0

#endif        //  #ifndef AUDIOCONFIGSAMD21_H

