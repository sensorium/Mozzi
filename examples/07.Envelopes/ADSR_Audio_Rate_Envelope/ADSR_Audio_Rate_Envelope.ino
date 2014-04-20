/*  Example applying an ADSR envelope to an audio signal
    with Mozzi sonification library.  This shows 
    internal updates at CONTROL_RATE, using update() in updateControl(),
    with interpolation and output using next() at AUDIO_RATE in updateAudio().
    This is the "ordinary" way to use ADSR for smooth amplitude transitions while
    maintaining reasonable efficiency by updating internal states in updateControl().
    
    Demonstrates a simple ADSR object being controlled with
    noteOn() and noteOff() instructions.
  
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

Oscil <8192, AUDIO_RATE> aOscil(SIN8192_DATA);; 

// for triggering the envelope
EventDelay noteDelay;

ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

boolean note_is_on = true;

void setup(){
  Serial.begin(115200);
  randSeed(); // fresh random
  noteDelay.set(2000); // 2 second countdown
  startMozzi(CONTROL_RATE);
}


unsigned int duration, attack, decay, sustain, release_ms;

void updateControl(){
  if(noteDelay.ready()){
    
      // choose envelope levels
      byte attack_level = rand(128)+127;
      byte decay_level = rand(255);
      envelope.setADLevels(attack_level,decay_level);

      // generate a random new adsr parameter value in milliseconds
      int r = rand(1000)-rand(1000);
      unsigned int new_value = abs(r);
      
     // randomly choose one of the adsr parameters and set the new value
     switch (rand(4)){
       case 0:
       attack = new_value;
       break;
       
       case 1:
       decay = new_value;
       break;
       
       case 2:
       sustain = new_value;
       break;
       
       case 3:
       release_ms = new_value;
       break;
     }
     envelope.setTimes(attack,decay,sustain,release_ms);    
     envelope.noteOn();

     byte midi_note = rand(107)+20;
     aOscil.setFreq((int)mtof(midi_note));
    

     // print to screen
     Serial.print("midi_note\t"); Serial.println(midi_note);
     Serial.print("attack_level\t"); Serial.println(attack_level);
     Serial.print("decay_level\t"); Serial.println(decay_level);
     Serial.print("attack\t\t"); Serial.println(attack);
     Serial.print("decay\t\t"); Serial.println(decay);
     Serial.print("sustain\t\t"); Serial.println(sustain);
     Serial.print("release\t\t"); Serial.println(release_ms);
     Serial.println();

     noteDelay.start(attack+decay+sustain+release_ms);
     
   }
  envelope.update();
} 


int updateAudio(){
  return (int) (envelope.next() * aOscil.next())>>8;
}


void loop(){
  audioHook(); // required here
}






