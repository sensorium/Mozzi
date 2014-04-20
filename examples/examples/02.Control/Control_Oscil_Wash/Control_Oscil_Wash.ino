/*  Plays a fluctuating ambient wash,
    using Mozzi sonification library.
  
    Demonstrates audio and control rate updates.
    There are 8 oscillators updated at control rate to set
    the volume of 8 audio oscillators.  Updating the volume
    at control rate saves processor time, incrementing the
    Oscils only 128 times per second rather than at the
    audio rate of 16384 Hz.
  
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
#include <mozzi_midi.h>

#define CONTROL_RATE 128

// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos8(COS8192_DATA);

// volume controls
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol8(COS8192_DATA);

// audio volumes updated each control interrupt and reused in audio till next control
char v1,v2,v3,v4,v5,v6,v7,v8;

void setup(){

  // set harmonic frequencies
  aCos1.setFreq(mtof(60));
  aCos2.setFreq(mtof(74));
  aCos3.setFreq(mtof(64));
  aCos4.setFreq(mtof(77));
  aCos5.setFreq(mtof(67));
  aCos6.setFreq(mtof(81));
  aCos7.setFreq(mtof(60));
  aCos8.setFreq(mtof(84));

// set volume change frequencies
  kVol1.setFreq(4.43f); // more of a pulse
  kVol2.setFreq(0.0245f);
  kVol3.setFreq(0.019f);
  kVol4.setFreq(0.07f);
  kVol5.setFreq(0.047f);
  kVol6.setFreq(0.031f);
  kVol7.setFreq(0.0717f);
  kVol8.setFreq(0.041f);

  v1=v2=v3=v4=v5=v6=v7=v8=127;
  
  startMozzi(CONTROL_RATE);
}

void loop(){
  audioHook();
}


void updateControl(){
   v1 = kVol1.next()>>1; // going at a higher freq, this creates zipper noise, so reduce the gain
   v2 = kVol2.next();
   v3 = kVol3.next();
   v4 = kVol4.next();
   v5 = kVol5.next();
   v6 = kVol6.next();
   v7 = kVol7.next();
   v8 = kVol8.next();
}

int updateAudio(){
  long asig = (long)
    aCos1.next()*v1 +
    aCos2.next()*v2 +
    aCos3.next()*v3 +
    aCos4.next()*v4 +
    aCos5.next()*v5 +
    aCos6.next()*v6 +
    aCos7.next()*v7 +
    aCos8.next()*v8;
  asig >>= 9;
  return (int) asig;
}
