/*  Example of simple panning and stereo,
    using Mozzi sonification library and an external dual DAC MCP4922 (original library by Thomas Backman - https://github.com/exscape/electronics/tree/master/Arduino/Libraries/DAC_MCP49xx)
    using an user-defined audioOutput() function.


    #define EXTERNAL_AUDIO_OUTPUT true should be uncommented in mozzi_config.h.
    #define AUDIO_CHANNELS STEREO should be set in mozzi_config.h



    Circuit: (see the DAC library README for details)

    MCP4921   //  Connect to:
    -------       -----------
    Vdd           V+
    CS            any digital pin defined by SS_PIN (see after), or pin 7 on UNO / 38 on Mega if you are using Portwrite
    SCK           SCK of Arduino
    SDI           MOSI of Arduino
    VoutA/B       to headphones/loudspeaker (right and left channels)
    Vss           to GND
    VrefA/B       to V+ or a clean tension ref between V+ and GND
    LDAC          to GND


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
#include <DAC_MCP49xx.h>  // https://github.com/tomcombriat/DAC_MCP49XX 
                          // which is an adapted fork from https://github.com/exscape/electronics/tree/master/Arduino/Libraries/DAC_MCP49xx  (Thomas Backman)

#define CONTROL_RATE 256 // Hz, powers of 2 are most reliable


// Synthesis part
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, AUDIO_RATE> aCos2(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kEnv1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, CONTROL_RATE> kEnv2(COS2048_DATA);




// External audio output parameters and DAC declaration
#define SS_PIN 7  // if you are on AVR and using PortWrite you need still need to put the pin you are actually using: 7 on Uno, 38 on Mega
//#define AUDIO_BIAS 2048  // we are at 12 bits, so we have to bias the signal of 2^(12-1) = 2048
DAC_MCP49xx dac(DAC_MCP49xx::MCP4922, SS_PIN);



void audioOutput(const AudioOutput f) // f is a structure containing both channels
{
  int out_l = f.l() + AUDIO_BIAS;  // the DAC wants positive signals only, so we need to add a bias.
  int out_r = f.r() + AUDIO_BIAS;

  dac.output2(out_l, out_r);  // outputs the two channels in one call.

}



void setup() {

  aCos1.setFreq(440.f);
  aCos2.setFreq(220.f);
  kEnv1.setFreq(0.25f);
  kEnv2.setFreq(4.f);

  dac.init();   // start SPI communications

  dac.setPortWrite(true);  //comment this line if you do not want to use PortWrite (for non-AVR platforms)

  startMozzi(CONTROL_RATE);
}



// Carry enveloppes
int env1, env2;

void updateControl() {
  env1 = kEnv1.next();
  env2 = kEnv2.next();
}


AudioOutput_t updateAudio() {
  return StereoOutput::fromNBit(12, aCos1.next() * env1, aCos2.next() * env2);
}


void loop() {
  audioHook();
}
