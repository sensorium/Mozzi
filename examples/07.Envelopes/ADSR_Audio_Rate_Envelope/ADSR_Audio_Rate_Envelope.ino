/*  Example applying an ADSR envelope to an audio signal
    with Mozzi sonification library.  This shows
    how to use an ADSR which updates at MOZZI_AUDIO_RATE, in updateAudio(),
    and output using next() at MOZZI_AUDIO_RATE in updateAudio().

    Another example in this folder shows an ADSR updating at MOZZI_CONTROL_RATE,
    which is more efficient, but MOZZI_AUDIO_RATE updates shown in this example
    enable faster envelope transitions.

    Demonstrates a simple ADSR object being controlled with
    noteOn() and noteOff() instructions.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <tables/sin8192_int8.h>
#include <mozzi_rand.h>
#include <mozzi_midi.h>

Oscil <8192, MOZZI_AUDIO_RATE> aOscil(SIN8192_DATA);;

// for triggering the envelope
EventDelay noteDelay;

ADSR <MOZZI_AUDIO_RATE, MOZZI_AUDIO_RATE> envelope;

boolean note_is_on = true;

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  randSeed(); // fresh random
  noteDelay.set(2000); // 2 second countdown
  startMozzi();
}


unsigned int duration, attack, decay, sustain, release_ms;

void updateControl(){
  if(noteDelay.ready()){

      // choose envelope levels
      byte attack_level = rand(128)+127;
      byte decay_level = rand(255);
      envelope.setADLevels(attack_level,decay_level);

    // generate a random new adsr time parameter value in milliseconds
     unsigned int new_value = rand(300) +100;
     Serial.println(new_value);
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

/*
     // print to screen
     Serial.print("midi_note\t"); Serial.println(midi_note);
     Serial.print("attack_level\t"); Serial.println(attack_level);
     Serial.print("decay_level\t"); Serial.println(decay_level);
     Serial.print("attack\t\t"); Serial.println(attack);
     Serial.print("decay\t\t"); Serial.println(decay);
     Serial.print("sustain\t\t"); Serial.println(sustain);
     Serial.print("release\t\t"); Serial.println(release_ms);
     Serial.println();
*/
     noteDelay.start(attack+decay+sustain+release_ms);
   }
}


AudioOutput updateAudio(){
  envelope.update();
  return MonoOutput::from16Bit((int) (envelope.next() * aOscil.next()));
}


void loop(){
  audioHook(); // required here
}
