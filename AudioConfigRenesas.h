#ifndef AUDIOCONFIGRENESAS_H
#define AUDIOCONFIGRENESAS_H

#if not IS_RENESAS()
#error This header should be included for Arduino FSB board (Uno R4/Renesa) family, only
#endif




#define AUDIO_CHANNEL_1_PIN A0
#if (AUDIO_CHANNELS > 1)
#define AUDIO_CHANNEL_2_PIN 10
#endif

#define AUDIO_BITS 12 // outputting resolution of the on-bard DAC. Other values are *NOT* expected to work.

#define AUDIO_BITS_PER_CHANNEL AUDIO_BITS

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))

#define BYPASS_MOZZI_OUTPUT_BUFFER true  // Mozzi initial buffer are not of the good type, so we bypass it and create our own

#endif        //  #ifndef AUDIOCONFIGRENESAS_H

