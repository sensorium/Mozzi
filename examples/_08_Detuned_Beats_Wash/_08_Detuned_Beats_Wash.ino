/*  Plays a fluctuating ambient wash using pairs
 *  of slightly detuned oscillators, following an example
 *  from Miller Puckette's Pure Data manual.
 *
 *  The detune frequencies are modified by chance in
 *  updateControl(), and the outputs of 14 audio
 *  oscillators are summed in updateAudio().
 *  
 *  Circuit: Audio output on digital pin 9.
 *
 *  sweatsonics@sweatsonics.com 2012.
 *  This example code is in the public domain.
 */
 
#include <CuttlefishGuts.h>
#include <Oscil.h> 
#include <tables/cos8192_int8.h>
#include <utils.c> // for mtof and xorshift96() fastish random number generator

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
unsigned long f1,f2,f3,f4,f5,f6,f7;


void setup(){
  startCuttlefish(64); // a literal control rate here
  
  // select harmonic frequencies
  f1 = (unsigned long) (mtof(48.f) * 256);
  f2 = (unsigned long) (mtof(74.f) * 256);
  f3 = (unsigned long) (mtof(64.f) * 256);
  f4 = (unsigned long) (mtof(77.f) * 256);
  f5 = (unsigned long) (mtof(67.f) * 256);
  f6 = (unsigned long) (mtof(57.f) * 256);
  f7 = (unsigned long) (mtof(60.f) * 256);
  
  // set harmonic frequencies
  aCos1.setFreq_n8(f1);
  aCos2.setFreq_n8(f2);
  aCos3.setFreq_n8(f3);
  aCos4.setFreq_n8(f4);
  aCos5.setFreq_n8(f5);
  aCos6.setFreq_n8(f6);
  aCos7.setFreq_n8(f7);

  // set frequencies of duplicate oscillators
  aCos1b.setFreq_n8(f1+variation());
  aCos2b.setFreq_n8(f2+variation());
  aCos3b.setFreq_n8(f3+variation());
  aCos4b.setFreq_n8(f4+variation());
  aCos5b.setFreq_n8(f5+variation());
  aCos6b.setFreq_n8(f6+variation());
  aCos7b.setFreq_n8(f7+variation());
}


void loop(){
  audioHook();
}


unsigned int variation() {
  return  (unsigned int) (xorshift96() & 2047UL); // between 0-8 in 8n8 format
}


void updateControl(){ 
  // todo: choose a nice scale or progression and make a table for it
  // or add a very slow gliss for f1-f7, like shephard tones
  
  // change frequencies of the b oscillators
  switch (lowByte(xorshift96()) & 7){ // 7 is 0111

    case 0:
      aCos1b.setFreq_n8(f1+variation());
    break;
    
    case 1:
       aCos2b.setFreq_n8(f2+variation());
    break;
    
    case 2:
       aCos3b.setFreq_n8(f3+variation());
    break;
    
    case 3:
       aCos4b.setFreq_n8(f4+variation());
    break;
    
    case 4:
       aCos5b.setFreq_n8(f5+variation());
    break;
    
    case 5:
       aCos6b.setFreq_n8(f6+variation());
    break;
    
    case 6:
       aCos7b.setFreq_n8(f7+variation());
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
