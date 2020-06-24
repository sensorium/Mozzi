/*  Example playing a sinewave at a set frequency,
    using Mozzi sonification library and an user-defined
    audioOutput() function.

    #define EXTERNAL_AUDIO_OUTPUT true should be uncommented
    in mozzi_config.h.

    Demonstrates the use of audioOutput() using a R/2R DAC
    connected on 6 digital pins of an Arduino.

    NOTE: this is for demonstration purpose, this particular way of
    outputting sound, using digitalWrite() and a R/2R DAC is less 
    efficient than the native method. However, this method could be
    substantially improved by using registers
    (see: https://www.instructables.com/id/Fast-digitalRead-digitalWrite-for-Arduino/)
    or even better, using continous registers to update all digital pins
    at the same time. One could also use Serial to Parallel converters.
    

    Circuit: 

    D0/LSB   D1      D2      D3      D4     D5/MSB
     |       |       |       |       |       |
    2R      2R      2R      2R      2R      2R
     |---R---|---R---|---R---|---R---|---R---|-----(to phones)
    2R
     |
    GND
    
    R=10kOhms or around, but all R at the same value
    
    For more details on the R/2R DAC see:
    https://hackaday.com/2015/11/05/logic-noise-digital-to-analog-with-an-r-2r-dac/

    Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2012, CC by-nc-sa.
    T. Combriat 2020, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable



// External output parameters for this example
#define R2R_N_PIN 6  // Number of stage of the resistance ladder = number of digits of the DAC
const int r2r_pin[R2R_N_PIN] = {30, 31, 32, 33, 34, 35}; // pins to the resistance ladder, in order,
                                                         // starting with LSB (pin closer to GND)
                                                         // so D0, D1, etc.

#define AUDIO_BIAS 32    // we are at 6 bits so we have to bias the signal of 2^(6-1)=32

void audioOutput(int l, int r) // l is the sample we want to output, it is zero-centered
                               // r is used when using STEREO_HACK (see mozzi_config.h)
{
  l += AUDIO_BIAS;   // make the signal positive
  int mask = 0b00000001;   // mask for outputting only 1 bit (one per pin)
    for (int i = 0; i < R2R_N_PIN; i++)
    {
      digitalWrite(r2r_pin[i], bool((l >> i) & mask));  // write on digital Pins:
    }                                                   // shift the value to the right
}                                                       // apply the mask to get only the last bit
                                                        // use that value in digitalWrite()
    

void setup() {
  for (int i = 0; i < R2R_N_PIN; i++) pinMode(r2r_pin[i], OUTPUT);
  startMozzi(CONTROL_RATE); // :)
  aSin.setFreq(200); // set the frequency
}


void updateControl() {
  // put changing controls in here
}


int updateAudio() {
  return aSin.next()>>2; // return an int signal centred around 0
}                        // as we are using only 6 bits, and aSin 
                         // is returning of 8 we have to get rid 
                         // of two



void loop() {
  audioHook(); // required here
}
