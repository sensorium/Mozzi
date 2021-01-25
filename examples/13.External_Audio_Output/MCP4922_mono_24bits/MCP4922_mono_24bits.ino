/*  Example of using the two channels of a DAC to have high fidelity output using the technique described here : http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/index.html
    using Mozzi sonification library and an external dual DAC MCP4922 (original library by Thomas Backman - https://github.com/exscape/electronics/tree/master/Arduino/Libraries/DAC_MCP49xx)
    using an user-defined audioOutput() function.

    WARNING: YOU CANNOT ACHEIVE MORE THAN 16BITS ON AN AVR ARDUINO, THIS EXAMPLE WON'T WORK AS IT IS.

    #define EXTERNAL_AUDIO_OUTPUT true should be uncommented in mozzi_config.h.
    #define EXTERNAL_AUDIO_BITS 24 should be set in mozzi_config.h

    Circuit: (see the DAC library README for details)

    MCP4921   //  Connect to:
    -------       -----------
    Vdd           V+
    CS            any digital pin defined by SS_PIN (see after), or pin 7 on UNO / 38 on Mega if you are using Portwrite
    SCK           SCK of Arduino
    SDI           MOSI of Arduino
    VoutA/B       (see after)
    Vss           to GND
    VrefA/B       to V+ or a clean tension ref between V+ and GND
    LDAC          to GND


    Dual DAC electrical connections
    -------------------------------

    VoutA -------- R1 -----------> To headphones/loudspeaker or anything else
                            |
    VoutB -------- R2 -------

    R2 = 2^n * R1  with n the number of bits per stage.
    This should be precise, use a trimmer on R1 to adjust it precisely.
    Recommended values: R1 around 1k
                        R2 around 4M

    Notes: - int type in Arduino is 16 bits. So you cannot have 24 bits on an Arduino using audioOutput() function. But this does work on 32bits platforms (STM32 for instance)
           - 24 bits is a lot. To acheive real 24 bits you need to be very careful on your electronics (good precision on R, buffer after the summing junction)
           - this is certainly over-kill, however this technique can prove useful for outputting 16 bits, by combining 2 * 8 bits DACs
           - the communication with the DAC takes the same time for 8/10 and 12bits DAC, so 2*8bits should be as fast as 2*1 bits.



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





// External audio output parameters and DAC declaration
#define SS_PIN 38  // if you are on AVR and using PortWrite you need still need to put the pin you are actually using: 7 on Uno, 38 on Mega
//#define AUDIO_BIAS 8388608  // we are at 24 bits, so we have to bias the signal of 2^(24-1) = 8388608 (not needed since PR #98)
#define BITS_PER_CHANNEL 12  // each channel of the DAC is outputting 12 bits
DAC_MCP49xx dac(DAC_MCP49xx::MCP4922, SS_PIN);



void audioOutput(const AudioOutput f) // f is a structure containing both channels
{

  int out = AUDIO_BIAS + f.l();

  unsigned short lowBits = (unsigned short) out;  //
  unsigned short highBits =  out >> BITS_PER_CHANNEL;

  dac.output2(highBits, lowBits);  // outputs the two channels in one call.
}



void setup() {
  aCos1.setFreq(440.f);
  aCos2.setFreq(220.f);
  kEnv1.setFreq(0.1f);


  dac.init();   // start SPI communications

  //dac.setPortWrite(true);  //comment this line if you do not want to use PortWrite (for non-AVR platforms)

  startMozzi(CONTROL_RATE);
}



// Carry enveloppes
int env1 = 0;

void updateControl() {
  env1 = kEnv1.next();
}




AudioOutput_t updateAudio() {
  return MonoOutput::fromNBit(24, (int32_t)aCos1.next() * aCos2.next() * env1) ; // specify that the audio we are sending here is 24 bits.
}

void loop() {
  audioHook();
}
