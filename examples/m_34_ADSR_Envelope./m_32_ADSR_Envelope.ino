/*  Example applying an ADSR envelope to an audio signal
 *  with Mozzi sonification library.
 *
 *  Demonstrates a simple ADSR object being controlled with
 *  noteOn() and noteOff() instructions.
 *
 *  Created by Tim Barrass 2013.
 *  This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin512_int8.h> 

#define CONTROL_RATE 64

Oscil <512, AUDIO_RATE> aOscil(SIN512_DATA);; 

// for triggering the envelope
EventDelay <CONTROL_RATE>  boringDelay;

ADSR <CONTROL_RATE> envelope;

boolean note_is_on = true;

void setup(){
  boringDelay.set(2000); // 2 second countdown
  aOscil.setFreq(440.f);
  
  // set attack and decay levels
  envelope.setADLevels(255,100);

  // Set a long sustain time if you intend to finish the note on the fly with noteOff().
  // You can also provide a sustain time if you want it to finish in its own time without a noteOff().
  // make sure the release time is long enough in relation to CONTROL_RATE for a few control steps
  envelope.setTimes(200u,300u,10000u,500u);  // "u" means unsigned int

  startMozzi(CONTROL_RATE);
}


void updateControl(){
  if(boringDelay.ready()){
    if (note_is_on){
      note_is_on = false;
      envelope.noteOff();
    }
    else{
      note_is_on = true;
      envelope.noteOn();
    }
    boringDelay.start();
  }
  envelope.update();
} 


int updateAudio(){
  return (int) (envelope.next() * aOscil.next())>>8;
}


void loop(){
  audioHook(); // required here
}






