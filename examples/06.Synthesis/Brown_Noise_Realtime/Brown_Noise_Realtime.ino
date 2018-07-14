/*  Example generating brownian noise in real time,
    using Mozzi sonification library.
  
    Demonstrates rand(), a fast random generator,
    a filter used as a "leaky integrator" 
    (based on an implementation by Barry Dorr:
    https://www.edn.com/design/systems-design/4320010/A-simple-software-lowpass-filter-suits-embedded-system-applications)
    and the use of Oscil to modulate amplitude.
    
    Circuit: Audio output on digital pin 9 on a Uno or similar.
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 20118, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <mozzi_rand.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please

#define FILTER_SHIFT 6 // 5 or 6 work well - the spectrum of 6 looks a bit more linear, like the generated brown noise in Audacity

void setup()
{
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
  aSin.setFreq(0.05f); // set the frequency
}


void updateControl()
{
  // put changing controls in here
}


int updateAudio()
{
  static int filtered;

  char whitenoise = rand((byte)255) - 128;
  filtered = filtered - (filtered>>FILTER_SHIFT) + whitenoise;

  int asig = filtered>>3; // shift to 8 bit range (trial and error)
  return ((int)asig * aSin.next())>>8;
}


void loop(){
  audioHook(); // required here
}



