/*  Example of a sound being triggered by MIDI input, with portamento.

    Demonstrates playing notes with Mozzi in response to MIDI input.
    Also demonstrates Portamento(), a traditional portamento with a
    duration which can be set.

    Uses Arduino MIDI library v4.2
    (https://github.com/FortySevenEffects/arduino_midi_library/releases/tag/4.2)

    Circuit:
      MIDI input circuit as per http://arduino.cc/en/Tutorial/Midi
      (midi has to be disconnected from rx for sketch to upload)
      Audio output on digital pin 9 on a Uno or similar.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2013-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <MIDI.h>
// use #define for MOZZI_CONTROL_RATE, not a constant
#define MOZZI_CONTROL_RATE 128 // Hz, powers of 2 are most reliable
#include <Mozzi.h>
#include <Oscil.h> // oscillator template
#include <Line.h> // for envelope
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_midi.h>
#include <ADSR.h>
#include <mozzi_fixmath.h>
#include <Portamento.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// audio sinewave oscillator
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

// envelope generator
ADSR <MOZZI_CONTROL_RATE, MOZZI_AUDIO_RATE> envelope;

Portamento <MOZZI_CONTROL_RATE>aPortamento;

#define LED 13


void HandleNoteOn(byte channel, byte note, byte velocity) {
  aPortamento.start(note);
  envelope.noteOn();
  digitalWrite(LED,HIGH);
}



void HandleNoteOff(byte channel, byte note, byte velocity) {
  envelope.noteOff();
  digitalWrite(LED,LOW);
}



void setup() {
  pinMode(LED, OUTPUT);

  // Initiate MIDI communications, listen to all channels
  MIDI.begin(MIDI_CHANNEL_OMNI);

  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function

  envelope.setADLevels(255,64);
  envelope.setTimes(50,200,10000,300); // 10000 is so the note will sustain 10 seconds unless a noteOff comes

  aPortamento.setTime(300u);

  startMozzi();
}


void updateControl(){
  MIDI.read();
  envelope.update();
  aSin.setFreq_Q16n16(aPortamento.next());
}


AudioOutput updateAudio(){
  return MonoOutput::from16Bit((int) (envelope.next() * aSin.next()));
}


void loop() {
  audioHook(); // required here
}
