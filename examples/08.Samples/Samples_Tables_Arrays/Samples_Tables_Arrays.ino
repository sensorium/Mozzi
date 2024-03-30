/*  Example of a polyphonic sketch in which
    11 sound tables are shared between 3 voices,
    using Mozzi sonification library.

    Demonstrates use of Sample() objects as players,
    using setTable() to share many sound tables between
    a few players, to minimise processing in updateAudio().
    Shows how to use Samples and sound tables in arrays,
    EventDelay() for scheduling, and rand() to select
    sound tables and vary the gain of each player.

    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.io/Mozzi/

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>
#include <Sample.h>
#include <EventDelay.h>
#include <mozzi_rand.h> // for rand()
#include <samples/bamboo/bamboo_00_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_01_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_02_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_03_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_04_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_05_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_06_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_07_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_08_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_09_2048_int8.h> // wavetable data
#include <samples/bamboo/bamboo_10_2048_int8.h> // wavetable data

// for scheduling samples to play
EventDelay kTriggerDelay;
EventDelay kTriggerSlowDelay;

byte ms_per_note = 111; // subject to MOZZI_CONTROL_RATE

const byte NUM_PLAYERS = 3; // 3 seems to be enough
const byte NUM_TABLES = 11;

Sample <BAMBOO_00_2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSample[NUM_PLAYERS] ={
  Sample <BAMBOO_00_2048_NUM_CELLS, MOZZI_AUDIO_RATE> (BAMBOO_00_2048_DATA),
  Sample <BAMBOO_01_2048_NUM_CELLS, MOZZI_AUDIO_RATE> (BAMBOO_01_2048_DATA),
  Sample <BAMBOO_02_2048_NUM_CELLS, MOZZI_AUDIO_RATE> (BAMBOO_02_2048_DATA),
};

// watch out - tables are const (but you can choose which ones you reference)
const int8_t * tables[NUM_TABLES] ={
  BAMBOO_00_2048_DATA,
  BAMBOO_01_2048_DATA,
  BAMBOO_02_2048_DATA,
  BAMBOO_03_2048_DATA,
  BAMBOO_04_2048_DATA,
  BAMBOO_05_2048_DATA,
  BAMBOO_06_2048_DATA,
  BAMBOO_07_2048_DATA,
  BAMBOO_08_2048_DATA,
  BAMBOO_09_2048_DATA,
  BAMBOO_10_2048_DATA
};

// gains for each sample player
byte gain[NUM_PLAYERS];


void setup(){
  for (int i=0;i<NUM_PLAYERS;i++){  // play at the speed they're sampled at
    (aSample[i]).setFreq((float) BAMBOO_00_2048_SAMPLERATE / (float) BAMBOO_00_2048_NUM_CELLS);
  }
  kTriggerDelay.set(ms_per_note); // countdown ms, within resolution of MOZZI_CONTROL_RATE
  kTriggerSlowDelay.set(ms_per_note*6); // resolution-dependent inaccuracy leads to polyrhythm :)

  startMozzi();
}


byte randomGain(){
  return rand((byte)80,(byte)255);
}


void updateControl(){
  static byte player =0;

  if(kTriggerDelay.ready()){
    gain[player] = randomGain();
    (aSample[player]).setTable(tables[rand(NUM_TABLES)]);
    (aSample[player]).start();
    player++;
    if(player==NUM_PLAYERS) player = 0;
    kTriggerDelay.start();
  }

  if(kTriggerSlowDelay.ready()){
    gain[player] = randomGain();
    (aSample[player]).setTable(tables[rand(NUM_TABLES)]);
    (aSample[player]).start();
    player++;
    if(player==NUM_PLAYERS) player = 0;
    kTriggerSlowDelay.start();
  }
}


AudioOutput updateAudio(){
  long asig=0;
  for (byte i=0;i<NUM_PLAYERS;i++){
    asig += (int) (aSample[i]).next() * gain[i];
  }
  //clip any stray peaks to max output range
  return MonoOutput::fromAlmostNBit(15, asig).clip();
}


void loop(){
  audioHook();
}
