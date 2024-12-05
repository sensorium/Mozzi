/*  Example of a sound being triggered by MIDI input.

    Demonstrates playing notes with Mozzi in response to MIDI input,
    using  Arduino MIDI library v4.2
    (https://github.com/FortySevenEffects/arduino_midi_library/releases/tag/4.2)

    Circuit:
      MIDI input circuit as per http://arduino.cc/en/Tutorial/Midi
      Note: midi input on rx pin, not tx as in the illustration on the above page.
      Midi has to be disconnected from rx for sketch to upload.
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
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_midi.h>
#include <ADSR.h>


MIDI_CREATE_DEFAULT_INSTANCE();

// audio sinewave oscillator
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

// envelope generator
ADSR <MOZZI_CONTROL_RATE, MOZZI_AUDIO_RATE> envelope;

#define LED 13 // shows if MIDI is being recieved

void HandleNoteOn(byte channel, byte note, byte velocity) {
  aSin.setFreq(mtof(float(note)));
  envelope.noteOn();
  digitalWrite(LED,HIGH);
}

void HandleNoteOff(byte channel, byte note, byte velocity) {
  envelope.noteOff();
  digitalWrite(LED,LOW);
}

void setup() {
  pinMode(LED, OUTPUT);

  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  MIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function
  // Initiate MIDI communications, listen to all channels (not needed with Teensy usbMIDI)
  MIDI.begin(MIDI_CHANNEL_OMNI);

  envelope.setADLevels(255,64);
  envelope.setTimes(50,200,10000,200); // 10000 is so the note will sustain 10 seconds unless a noteOff comes

  aSin.setFreq(440); // default frequency
  startMozzi();
}


void updateControl(){
  MIDI.read();
  envelope.update();
}


AudioOutput updateAudio(){
  return MonoOutput::from16Bit(envelope.next() * aSin.next());
}


void loop() {
  audioHook(); // required here
}
