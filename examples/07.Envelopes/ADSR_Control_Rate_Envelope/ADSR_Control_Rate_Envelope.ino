/*  Example applying an ADSR envelope to an audio signal
    with Mozzi sonification library.  This shows
    internal updates as well as interpolation at CONTROL_RATE, using both
    update() and next()  in updateControl().
    This is less computationally intensive than doing interpolation with
    next() in updateAudio(), but can be less smooth, especially with fast envelopes.

    Demonstrates a simple ADSR object being controlled with
    noteOn() and noteOff() instructions.

    Tim Barrass 2013-14, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin8192_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 128 // faster than usual to help smooth CONTROL_RATE adsr interpolation (next())

Oscil <8192, AUDIO_RATE> aOscil(SIN8192_DATA);;

// for triggering the envelope
EventDelay noteDelay;

ADSR <CONTROL_RATE, CONTROL_RATE> envelope;

boolean note_is_on = true;
byte gain;

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
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
  gain = envelope.next(); // this is where it's different to an audio rate envelope
}


int updateAudio(){
  return (int) (gain * aOscil.next())>>8;
}


void loop(){
  audioHook(); // required here
}
