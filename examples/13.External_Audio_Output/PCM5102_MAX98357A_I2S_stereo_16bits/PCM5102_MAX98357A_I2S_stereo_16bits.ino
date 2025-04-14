
/*  Example of simple and stereo synthesis via I2S for the RP2040. Tested with a Pi Pico.

    using Mozzi sonification library and an external dual DAC PT8211 (inspired by: https://sparklogic.ru/code-snipplets/i2s-example-code.html)
    using an user-defined audioOutput() function.

    This example has been tested with the PCM5102 from Texas Instruments as found on boards configured as shown here: https://github.com/dwhinham/mt32-pi/wiki/GY-PCM5102-DAC-module

    This example also works unmodified with Max98357A I2S amplifier boards.


    Circuit: 
    
    I2S DAC   //  Connect to:
    -------       -----------
    Vdd           V+
    BCK           20 
    WS/LCK        21           
    DIN           22
    GND           GND
    L/R           to audio outputs

    Mozzi documentation/API
    https://sensorium.github.io/Mozzi/doc/html/index.html

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
    T. Combriat 2020, CC by-nc-sa.
*/

// before including Mozzi.h, configure external audio output mode:
#include "MozziConfigValues.h"  // for named option values
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO 
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
// all of these are the defaults on RP2040 outputting on I2S, anyway, thus commented
#define MOZZI_AUDIO_BITS 16
#define MOZZI_I2S_PIN_BCK 20
#define MOZZI_I2S_PIN_WS (MOZZI_I2S_PIN_BCK+1) // CANNOT BE CHANGED, HAS TO BE NEXT TO pBCLK, i.e. default is 21
#define MOZZI_I2S_PIN_DATA 22

// End I2S related config

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play

// Synthesis part
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos2(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kEnv1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kEnv2(COS2048_DATA);

void setup() {
  aCos1.setFreq(440.f);
  aCos2.setFreq(220.f);
  kEnv1.setFreq(0.25f);
  kEnv2.setFreq(0.30f);
  startMozzi(CONTROL_RATE);
}

// Carry enveloppes
int env1, env2;
void updateControl() {
  env1 = kEnv1.next();
  env2 = kEnv2.next();
}

AudioOutput_t updateAudio() {
  return StereoOutput::from16Bit(aCos1.next() * env1,aCos2.next() * env2);
}

void loop() {
  audioHook();
}
