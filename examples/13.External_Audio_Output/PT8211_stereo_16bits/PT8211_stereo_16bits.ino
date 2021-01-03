/*  Example of simple panning and stereo,
    using Mozzi sonification library and an external dual DAC PT8211 (inspired by: https://sparklogic.ru/code-snipplets/i2s-example-code.html)
    using an user-defined audioOutput() function. I2S, the protocol used by this DAC, is here emulated in synced way using SPI.


    #define EXTERNAL_AUDIO_OUTPUT true should be uncommented in mozzi_config.h.
    #define STEREO_HACK true should be set in mozzi_config.h


    Circuit: 
    
    PT8211   //  Connect to:
    -------       -----------
    Vdd           V+
    BCK           SCK 
    WS            any digital pin (defined by WS_pin here, pin 5)
    DIN           MOSI
    GND           GND
    L/R           to audio outputs

    Mozzi documentation/API
    https://sensorium.github.io/Mozzi/doc/html/index.html

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
    T. Combriat 2020, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <SPI.h>

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable


// Synthesis part
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos2(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kEnv1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kEnv2(COS2048_DATA);




// External audio output parameters
#define WS_pin 5   // channel select pin for the DAC
#define AUDIO_BIAS 0  // this DAC works on 0-centered signals





void audioOutput(int l, int r)
{

/* Note:
 *  the digital writes here can be optimised using portWrite if more speed is needed
 */

  digitalWrite(WS_pin, LOW);  //select Right channel
  SPI.transfer16(r);

  digitalWrite(WS_pin, HIGH);  // select Left channel
  SPI.transfer16(l);
}



void setup() {
  pinMode(WS_pin, OUTPUT);

  // Initialising the SPI connection on default port
  SPI.begin();
  SPI.beginTransaction(SPISettings(200000000, MSBFIRST, SPI_MODE0));  //MSB first, according to the DAC spec


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


// needed for stereo output
int audio_out_1, audio_out_2;

void updateAudio() {
  audio_out_1 = (aCos1.next() * env1);
  audio_out_2 = (aCos2.next() * env2) ;
}


void loop() {
  audioHook();
}
