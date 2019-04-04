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

    Tim Barrass 2013, CC by-nc-sa.
*/

#include <MIDI.h>
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <Line.h> // for envelope
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_midi.h>
#include <ADSR.h>
#include <mozzi_fixmath.h>
#include <Portamento.h>

MIDI_CREATE_DEFAULT_INSTANCE();

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 128 // Hz, powers of 2 are most reliable

// audio sinewave oscillator
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// envelope generator
ADSR <CONTROL_RATE> envelope;

Portamento <CONTROL_RATE, AUDIO_RATE>aPortamento;

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

  startMozzi(CONTROL_RATE);
}


void updateControl(){
  MIDI.read();
  envelope.update();
  aSin.setFreq_Q16n16(aPortamento.next());
}


int updateAudio(){
  return (int) (envelope.next() * aSin.next())>>8;
}


void loop() {
  audioHook(); // required here
}
