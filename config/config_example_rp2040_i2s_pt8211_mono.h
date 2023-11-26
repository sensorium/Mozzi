/* Configuration example

This example is targetted at the RP2040 (raspberry Pi pico) platform only!

Configure the Raspberry Pico to output sound in mono on a I2S DAC on LSBJ format (like the PT8211). */


#include "MozziConfigValues.h"  // for named option values

#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
#define MOZZI_I2S_FORMAT MOZZI_I2S_FORMAT_LSBJ // PT8211 is on LSBJ format

// all of these are the defaults on RP2040 outputting on I2S, anyway, thus commented
#define MOZZI_AUDIO_BITS 16
#define MOZZI_I2S_PIN_BCK 20
#define MOZZI_I2S_PIN_WS (MOZZI_I2S_PIN_BCK+1) // CANNOT BE CHANGED, HAS TO BE NEXT TO pBCLK, i.e. default is 21
#define MOZZI_I2S_PIN_DATA 22

