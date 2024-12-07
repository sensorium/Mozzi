
/*  Example of simple stereo synthesis,

    using Mozzi sonification library and an external dual DAC PT8211 (inspired by: https://sparklogic.ru/code-snipplets/i2s-example-code.html)
    using an user-defined audioOutput() function. I2S, the protocol used by this DAC, is here emulated in synced way using SPI.

    Circuit:

    PT8211   //  Connect to:
    -------       -----------
    Vdd           V+
    BCK           SCK (PB13 here, for SPI2)
    WS            any digital pin (defined by WS_pin here, PB12)
    DIN           MOSI (PB15 here, for SPI2)
    GND           GND
    L/R           to audio outputs

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2020-2024 T. Combriat and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/
#include "MozziConfigValues.h"  // for named option values
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_TIMED
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO
#define MOZZI_CONTROL_RATE 256 // Hz, powers of 2 are most reliable

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <SPI.h>

// Synthesis part
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCos1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCos2(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kEnv1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kEnv2(COS2048_DATA);




// External audio output parameters
#define WS_pin PB12   // channel select pin for the DAC

SPIClass mySPI(2);    // declaration of SPI for using SPI2 and thus freeing all ADC pins





void audioOutput(const AudioOutput f) // f is a structure containing both channels
{
  digitalWrite(WS_pin, LOW);  //select Right channel
  mySPI.transfer16(f.r());    // Note: This DAC works on 0-centered samples, no need to add MOZZI_AUDIO_BIAS

  digitalWrite(WS_pin, HIGH);  // select Left channel
  mySPI.transfer16(f.l());
}



void setup() {
  pinMode(WS_pin, OUTPUT);

  
  mySPI.begin();
  mySPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0)); //MSB first, according to the DAC spec


  aCos1.setFreq(440.f);
  aCos2.setFreq(220.f);
  kEnv1.setFreq(0.25f);
  kEnv2.setFreq(0.30f);

  startMozzi();
}



// Carry enveloppes
int env1, env2;

void updateControl() {
  env1 = kEnv1.next();
  env2 = kEnv2.next();
}


AudioOutput updateAudio() {
  return StereoOutput::from16Bit(aCos1.next() * env1, aCos2.next() * env2);
}


void loop() {
  audioHook();

}

