#ifndef AUDIOCONFIGRP2040_H
#define AUDIOCONFIGRP2040_H

#if not IS_RP2040()
#error This header should be included for RP2040, only
#endif


// AUDIO output modes
#define PWM_VIA_BARE_CHIP 1  // output using one of the gpio of the board
#define EXTERNAL_DAC_VIA_I2S 2  // output via external DAC connected to I2S (PT8211 or similar)

//******* BEGIN: These are the defines you may want to change. Best not to touch anything outside this range. ************/
#define RP2040_AUDIO_OUT_MODE PWM_VIA_BARE_CHIP
//******* END: These are the defines you may want to change. Best not to touch anything outside this range. ************/


#if (RP2040_AUDIO_OUT_MODE == PWM_VIA_BARE_CHIP)  
#define AUDIO_CHANNEL_1_PIN 0
#if (AUDIO_CHANNELS > 1)
// Audio channel pins for stereo or HIFI must be on the same PWM slice (which is the case for the pairs (0,1), (2,3), (4,5), etc.
#define AUDIO_CHANNEL_2_PIN 1
#endif

// The more audio bits you use, the slower the carrier frequency of the PWM signal. 11 bits yields ~ 60kHz on a 133Mhz CPU (which appears to be a reasonable compromise)
#define AUDIO_BITS 11
#endif

#if (RP2040_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
// ****** BEGIN: These are define you may want to change. Best not to touch anything outside this range. ************/
#define BCLK_PIN 20
#define WS_PIN (pBCLK+1)  // CANNOT BE CHANGED, HAS TO BE NEXT TO pBCLK
#define DOUT_PIN 22
#define LSBJ_FORMAT false   // some DAC, like the PT8211, use a variant of I2S data format called LSBJ
                           // set this to true to use this kind of DAC or false for plain I2S.
#define AUDIO_BITS 16   // available values are 8, 16, 24 (LEFT ALIGN in 32 bits type!!) and 32 bits
// ****** END: These are define you may want to change. Best not to touch anything outside this range. ************/

#define BYPASS_MOZZI_OUTPUT_BUFFER true

// Configuration of the I2S port, especially DMA. Set in stone here as default of the library when this was written.
// Probably do not change if you are not sure of what you are doing
#define BUFFERS 8       // number of DMA buffers used
#define BUFFER_SIZE 256  // total size of the buffer, in samples
#endif


#define AUDIO_BITS_PER_CHANNEL AUDIO_BITS

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))

#endif        //  #ifndef AUDIOCONFIGRP2040_H

