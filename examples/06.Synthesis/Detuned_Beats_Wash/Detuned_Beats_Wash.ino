/*  Plays a fluctuating ambient wash using pairs
    of slightly detuned oscillators, following an example
    from Miller Puckette's Pure Data manual.
  
    The detune frequencies are modified by chance in
    updateControl(), and the outputs of 14 audio
    oscillators are summed in updateAudio().
  
    Demonstrates the use of fixed-point Q16n16
    format numbers, mtof() for converting midi note
    values to frequency, and xorshift96() for random numbers.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>
#include <mozzi_fixmath.h> // for Q16n16 fixed-point fractional number type

// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA);

// duplicates but slightly off frequency for adding to originals
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6b(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7b(COS8192_DATA);

// base pitch frequencies in 24n8 fixed int format (for speed later)
Q16n16 f1,f2,f3,f4,f5,f6,f7;


Q16n16 variation() {
  // 32 random bits & with 524287 (b111 1111 1111 1111 1111)
  // gives between 0-8 in Q16n16 format
  return  (Q16n16) (xorshift96() & 524287UL);
}


void setup(){
  startMozzi(64); // a literal control rate here

  // select base frequencies using mtof (midi to freq) and fixed-point numbers
  f1 = Q16n16_mtof(Q16n0_to_Q16n16(48));
  f2 = Q16n16_mtof(Q16n0_to_Q16n16(74));
  f3 = Q16n16_mtof(Q16n0_to_Q16n16(64));
  f4 = Q16n16_mtof(Q16n0_to_Q16n16(77));
  f5 = Q16n16_mtof(Q16n0_to_Q16n16(67));
  f6 = Q16n16_mtof(Q16n0_to_Q16n16(57));
  f7 = Q16n16_mtof(Q16n0_to_Q16n16(60));

  // set Oscils with chosen frequencies
  aCos1.setFreq_Q16n16(f1);
  aCos2.setFreq_Q16n16(f2);
  aCos3.setFreq_Q16n16(f3);
  aCos4.setFreq_Q16n16(f4);
  aCos5.setFreq_Q16n16(f5);
  aCos6.setFreq_Q16n16(f6);
  aCos7.setFreq_Q16n16(f7);

  // set frequencies of duplicate oscillators
  aCos1b.setFreq_Q16n16(f1+variation());
  aCos2b.setFreq_Q16n16(f2+variation());
  aCos3b.setFreq_Q16n16(f3+variation());
  aCos4b.setFreq_Q16n16(f4+variation());
  aCos5b.setFreq_Q16n16(f5+variation());
  aCos6b.setFreq_Q16n16(f6+variation());
  aCos7b.setFreq_Q16n16(f7+variation());
}


void loop(){
  audioHook();
}


void updateControl(){
  // todo: choose a nice scale or progression and make a table for it
  // or add a very slow gliss for f1-f7, like shephard tones

  // change frequencies of the b oscillators
  switch (lowByte(xorshift96()) & 7){ // 7 is 0111

    case 0:
      aCos1b.setFreq_Q16n16(f1+variation());
    break;

    case 1:
       aCos2b.setFreq_Q16n16(f2+variation());
    break;

    case 2:
       aCos3b.setFreq_Q16n16(f3+variation());
    break;

    case 3:
       aCos4b.setFreq_Q16n16(f4+variation());
    break;

    case 4:
       aCos5b.setFreq_Q16n16(f5+variation());
    break;

    case 5:
       aCos6b.setFreq_Q16n16(f6+variation());
    break;

    case 6:
       aCos7b.setFreq_Q16n16(f7+variation());
    break;
  }
}


int updateAudio(){

  int asig =
    aCos1.next() + aCos1b.next() +
    aCos2.next() + aCos2b.next() +
    aCos3.next() + aCos3b.next() +
    aCos4.next() + aCos4b.next() +
    aCos5.next() + aCos5b.next() +
    aCos6.next() + aCos6b.next() +
    aCos7.next() + aCos7b.next();

  return asig >> 3;
}
