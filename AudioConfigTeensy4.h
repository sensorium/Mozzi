#ifndef AUDIOCONFIGTEENSY4_H
#define AUDIOCONFIGTEENSY4_H

#warning If you get a compilation error you should probably update Teensyduino to its latest version

/** @ingroup core
*/
/* Used internally to put the 0-biased generated audio into the centre of the output range (10 bits on Teensy 4 using PWM) */
#define AUDIO_BIAS ((uint16_t) 512)
#define AUDIO_BITS 10

#define AUDIO_CHANNEL_1_PIN A8
#define AUDIO_CHANNEL_2_PIN A9


#endif        //  #ifndef AUDIOCONFIGTEENSY4_H

