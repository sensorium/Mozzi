/*  Example playing a sinewave at a set frequency,
    using Mozzi sonification library and an user-defined
    audioOutput() function.

    Demonstrates the use of audioOutput() using a R/2R DAC
    connected on a shift register 74HC595.

    Note: the overhead by putting a second one, and thus having a 16bits outputs
    would be minimal. The timing part here are the digitalWrite. One could use
    writing in the Arduino registers to speed it up.

    Circuit: 

    Qa      Qb      Qc      Qd      Qe      Qf      Qg      Qh   (from 74HC595)
     |       |       |       |       |       |       |       |
    2R      2R      2R      2R      2R      2R      2R      2R
     |---R---|---R---|---R---|---R---|---R---|---R---|---R---|---> (to phones)
    2R
     |
    GND

    74HC595  // Connect to:
    GND (08)      GND
    Vcc (16)      3.3/5V
    SER (14)      MOSI of Arduino
    OE  (13)      GND
    RCLK (12)     a digital Pin you have to define later by #define LATCH_PIN 31
    SRCLK (11)    SCK of Arduino
    SRCLR (10)    Vcc
    Qh' (9)       Not connected
    
    R=10kOhms or around, but all R at the same value
    
    For more details on the R/2R DAC see:
    https://hackaday.com/2015/11/05/logic-noise-digital-to-analog-with-an-r-2r-dac/

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2020-2024 T. Combriat and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include "MozziConfigValues.h"  // for named option values
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_TIMED
#define MOZZI_AUDIO_BITS 8
#define MOZZI_CONTROL_RATE 64 // Hz, powers of 2 are most reliable

#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include<SPI.h> // needed for the shift register

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

// External output parameters for this example
#define LATCH_PIN 31  // Number of stage of the resistance ladder = number of digits of the DAC

void audioOutput(const AudioOutput f) // f is a structure potentially containing both channels, scaled according to MOZZI_AUDIO_BITS
{
  int out = f.l() + MOZZI_AUDIO_BIAS;   // make the (zero centered) signal positive
  digitalWrite(LATCH_PIN, LOW);
  SPI.transfer(out);
  digitalWrite(LATCH_PIN, HIGH);
}                                                  
    

void setup() {
  pinMode(LATCH_PIN, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000000, MSBFIRST, SPI_MODE0));  // Qa is the last so we have to set as MSBFIRST
                                                                       // if you reverse the DAC you should put LSBFIRST
  startMozzi(); // :)
  aSin.setFreq(200); // set the frequency
}


void updateControl() {
  // put changing controls in here
}


AudioOutput updateAudio() {
  return MonoOutput::from8Bit(aSin.next()); // return an int signal centred around 0, 8bits wide
}                 
                      



void loop() {
  audioHook(); // required here
}
