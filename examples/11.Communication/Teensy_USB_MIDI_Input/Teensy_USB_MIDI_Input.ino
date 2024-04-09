/*  Example of a sound being triggered by MIDI input.

    Demonstrates playing notes with Mozzi in response to MIDI input,
    using the version of the Arduino MIDI library which
    works with Teensy boards (http://www.pjrc.com/teensy/td_midi.html).
    Tested on a Teensy2++, which can be used as a MIDI device
    without any extra parts.
    The sketch would be almost the same with the mainstream
    MIDI library (using MIDI as a prefix to calls instead of usbMIDI).
    This sketch won't compile on the Teensy2++ if you also
    have the mainstream MIDI library installed.

    Circuit: On the Teensy2++, audio output is on pin B5.  Midi input on usb.

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


// audio sinewave oscillator
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

// envelope generator
ADSR <MOZZI_CONTROL_RATE, MOZZI_AUDIO_RATE> envelope;

#define LED 6 // 6 on Teensy++ 2.0, 11 on Teensy 2.0, to see if MIDI is being recieved

void HandleNoteOn(byte channel, byte note, byte velocity) {
  //aSin.setFreq(mtof(note)); // simple but less accurate frequency
  aSin.setFreq_Q16n16(Q16n16_mtof(Q8n0_to_Q16n16(note))); // accurate frequency
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
  usbMIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  usbMIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function

  envelope.setADLevels(255,64);
  envelope.setTimes(50,200,10000,200); // 10000 is so the note will sustain 10 seconds unless a noteOff comes

  aSin.setFreq(440); // default frequency
  startMozzi();
}


void updateControl(){
  usbMIDI.read();
  envelope.update();
}


AudioOutput updateAudio(){
  return MonoOutput::from16Bit(envelope.next() * aSin.next());
}


void loop() {
  audioHook(); // required here
}
