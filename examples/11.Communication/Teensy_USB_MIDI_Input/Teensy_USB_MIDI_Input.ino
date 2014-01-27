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
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2013.
    This example code is in the public domain.
*/


#include <MIDI.h>
#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <Line.h> // for envelope
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <mozzi_midi.h>
#include <ADSR.h>
#include <mozzi_fixmath.h>


// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 128 // powers of 2 please

// audio sinewave oscillator
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// envelope generator
ADSR <CONTROL_RATE> envelope;

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

  // Initiate MIDI communications, listen to all channels (not needed with Teensy usbMIDI)
  //MIDI.begin(MIDI_CHANNEL_OMNI);    

  // Connect the HandleNoteOn function to the library, so it is called upon reception of a NoteOn.
  usbMIDI.setHandleNoteOn(HandleNoteOn);  // Put only the name of the function
  usbMIDI.setHandleNoteOff(HandleNoteOff);  // Put only the name of the function

  envelope.setADLevels(255,64);
  envelope.setTimes(50,200,10000,200); // 10000 is so the note will sustain 10 seconds unless a noteOff comes

  aSin.setFreq(440); // default frequency
  startMozzi(CONTROL_RATE); 
}


void updateControl(){
  usbMIDI.read();
  envelope.update();
}


int updateAudio(){
  return (int) (envelope.next() * aSin.next())>>8;
}


void loop() {
  audioHook(); // required here
} 



