/*  Example applying ADSR envelopes to 2 audio oscillators
     with Mozzi sonification library.

     Demonstrates ADSR, EventDelay and rand().

     Created by Tim Barrass 2013.
     This example code is in the public domain.
 */

#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin8192_int8.h> 
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 64

Oscil <8192, AUDIO_RATE> aOscil0(SIN8192_DATA);
Oscil <8192, AUDIO_RATE> aOscil1(SIN8192_DATA);

// for triggering the envelope
EventDelay noteDelay;

// ADSR update_rate, interpolation_rte
ADSR <CONTROL_RATE,AUDIO_RATE> envelope0;
ADSR <CONTROL_RATE,AUDIO_RATE> envelope1;

boolean note_is_on = true;

void setup(){
  Serial.begin(115200);
  randSeed(); // fresh random
  envelope0.setTimes(rand(300),rand(300),rand(300),rand(300));  
  envelope1.setTimes(rand(300),rand(300),rand(300),rand(300));  
  noteDelay.set(2000); // 2 second countdown
  startMozzi(CONTROL_RATE);
}


unsigned int attack0, decay0, sustain0, release_ms0;
unsigned int attack1, decay1, sustain1, release_ms1;

void updateControl(){
  if(noteDelay.ready()){

    // change adsr 0

    // choose envelope levels
    byte attack0_level = rand(128)+127;
    byte decay0_level = rand(255);
    envelope0.setADLevels(attack0_level,decay0_level);

    // generate a random new adsr parameter value in milliseconds
    int r = rand(300)-rand(300);
    unsigned int new_value = abs(r)+100;

    // randomly choose one of the adsr parameters and set the new value
    switch (rand(4)){
    case 0:
      attack0 = new_value;
      break;

    case 1:
      decay0 = new_value;
      break;

    case 2:
      sustain0 = new_value;
      break;

    case 3:
      release_ms0 = new_value;
      break;
    }
    envelope0.setTimes(attack0,decay0,sustain0,release_ms0);    
    envelope0.noteOn();



    // change adsr 1 

    // choose envelope levels
    byte attack1_level = rand(128)+127;
    byte decay1_level = rand(255);
    envelope1.setADLevels(attack1_level,decay1_level);

    // generate a random new adsr parameter value in milliseconds

    r = rand(300)-rand(300);
    new_value = abs(r)+100;

    // randomly choose one of the adsr parameters and set the new value
    switch (rand(4)){
    case 0:
      attack1 = new_value;
      break;

    case 1:
      decay1 = new_value;
      break;

    case 2:
      sustain1 = new_value;
      break;

    case 3:
      release_ms1 = new_value;
      break;
    }
    envelope1.setTimes(attack1,decay1,sustain1,release_ms1);    
    envelope1.noteOn();

    // change the pitch
    byte midi_note = rand(107)+20;
    int freq = (int)mtof(midi_note);
    aOscil0.setFreq(freq);
    aOscil1.setFreq(freq*2);   

    // print to screen
    Serial.print("midi_note\t"); 
    Serial.println(midi_note);
    Serial.print("attack0_level\t"); 
    Serial.print(attack0_level);
    Serial.print("\t attack1_level\t"); 
    Serial.println(attack1_level);
    Serial.print("decay0_level\t"); 
    Serial.print(decay0_level);
    Serial.print("\t decay1_level\t"); 
    Serial.println(decay1_level);
    Serial.print("attack0\t\t"); 
    Serial.print(attack0);
    Serial.print("\t attack1\t"); 
    Serial.println(attack1);
    Serial.print("decay0\t\t"); 
    Serial.print(decay0);
    Serial.print("\t decay1\t\t"); 
    Serial.println(decay1);
    Serial.print("sustain0\t"); 
    Serial.print(sustain0);
    Serial.print("\t sustain1\t"); 
    Serial.println(sustain1);
    Serial.print("release0\t"); 
    Serial.print(release_ms0);
    Serial.print("\t release1\t"); 
    Serial.println(release_ms1);
    Serial.println();

    // set duration to longest adsr of the 2
    int dur0 = attack0+decay0+sustain0+release_ms0;
    int dur1 = attack1+decay1+sustain1+release_ms1;
    if(dur0>dur1){
      noteDelay.start(dur0);
    }
    else{
      noteDelay.start(dur1);
    }

  }
  envelope0.update();
  envelope1.update();
} 


int updateAudio(){
  return ((long)envelope0.next() * aOscil0.next()) +
    ((int)envelope1.next() * aOscil1.next())
    >>9;
}


void loop(){
  audioHook(); // required here
}








