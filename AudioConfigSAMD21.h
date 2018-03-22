#ifndef AUDIOCONFIGSAMD21_H
#define AUDIOCONFIGSAMD21_H


/** @ingroup core
*/
/* Used internally to put the 0-biased generated audio into the centre of the output range (12 bits) */
#define AUDIO_BIAS ((uint16_t) 2048)


#define AUDIO_CHANNEL_1_PIN DAC0

#endif        //  #ifndef AUDIOCONFIGSAMD21_H

