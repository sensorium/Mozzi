/* Phase distortion used to simulate resonant filter, based on
    https://en.wikipedia.org/wiki/Phase_distortion_synthesis
    
    Demonstrates PDResonant, a class which can be used as a
    simple midi instrument.  The class shows how the Mozzi Phasor class
    can be used to generate an index into a wavetable, and an ADSR
    is used to modulate the effect by modifying the Phasor frequency and sync.
    More complex phase distortion effects could be developed by using
    precalculated tables, or calcuating tables on the fly using a double buffer,
    or a line-breakpoint model, a sort of hybridPhasor-Line object.
    
    This version can only play a single voice at a time, it might 
    be possible to optimise it enough for more simultaneous voices.
 */



//#include <MIDI.h>

//#include <ADC.h>  // Teensy 3.0/3.1 uncomment this line and install http://github.com/pedvide/ADC
#include <MozziGuts.h>

// for fake midi
#include <EventDelay.h>
#include <mozzi_rand.h>

#include <mozzi_midi.h>
#include <mozzi_analog.h>
#include <IntMap.h>

#include "PDResonant.h"
// wavetable for oscillator:
#include <tables/sin2048_int8.h>


// analog joystick for controlling speed of modulation: assigned to attack, decay times and sustain level
#define X A0
#define Y A1
unsigned int x_axis = 512; // give a static value for test without midi
unsigned int y_axis = 512;
const IntMap kmapX(0, 1023, 0, 1000); // A
const IntMap kmapY(0, 1023, 0, 3000); //D

PDResonant voice;

// for fake midi
EventDelay startNote; 
EventDelay endNote;

void setup(){
  randSeed(); // fresh random for fake midi

  pinMode(13, OUTPUT); // for midi feedback
  // initialize midi channel:
  //MIDI.begin(MIDI_CHANNEL_OMNI);

  //MIDI.setHandleNoteOn(HandleNoteOn);
  //MIDI.setHandleNoteOff(HandleNoteOff);

  // open MOZZI (: 
  startMozzi(CONTROL_RATE);

}


void HandleNoteOn(byte channel, byte pitch, byte velocity){
  if (velocity > 0)
  {
    voice.noteOn(channel, pitch, velocity);
    unsigned int attack = kmapX(x_axis);
    unsigned int decay = kmapY(y_axis);
    voice.setPDEnv(attack,decay);
    digitalWrite(13,HIGH);
  }
  else{
    stopNote(channel,  pitch,  velocity);
  }
}


void HandleNoteOff(byte channel, byte pitch, byte velocity){
  stopNote(channel,  pitch,  velocity);
}

void stopNote(byte channel, byte pitch, byte velocity){
  voice.noteOff(channel, pitch, velocity);
  digitalWrite(13,LOW);
}


void fakeMidiRead(){
  static char curr_note;
  if(startNote.ready()){
    curr_note = 20+rand(40);
    HandleNoteOn(1,curr_note,127);
    startNote.set(2000);
    startNote.start();
    endNote.set(1000);
    endNote.start();
  }
  if(endNote.ready()){
    HandleNoteOff(1,curr_note,0);
  }
}


void updateControl(){
  // check MIDI : 
  //MIDI.read();

  // analog joystick for controlling speed of modulation: assigned to attack, decay times and sustain level
  //x_axis = mozziAnalogRead(X);
  //y_axis = mozziAnalogRead(Y);

  // for testing/demo without external input
  fakeMidiRead();
  x_axis = 512; //mozziAnalogRead(X);
  y_axis = 512; // mozziAnalogRead(Y);

  voice.update();
}


int updateAudio(){
  return voice.next();
}


void loop(){
  audioHook(); // required here
}















