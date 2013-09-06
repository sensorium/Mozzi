/*  Example of a polyphonic sketch in which 
    11 sound tables are shared between 3 voices,
    using Mozzi sonification library.
  
    Demonstrates use of Sample() objects as players, 
    using setTable() to share many sound tables between 
    a few players, to minimise processing in updateAudio().
    Shows how to use Samples and sound tables in arrays,
    EventDelay() for scheduling, and rand() to select 
    sound tables and vary the gain of each player.
  
    Circuit: Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
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


#define CONTROL_RATE 64

// for scheduling samples to play
EventDelay kTriggerDelay;
EventDelay kTriggerSlowDelay;

byte ms_per_note = 111; // subject to CONTROL_RATE

const byte NUM_PLAYERS = 3; // 3 seems to be enough
const byte NUM_TABLES = 11;

Sample <BAMBOO_00_2048_NUM_CELLS, AUDIO_RATE> aSample[NUM_PLAYERS] ={
  Sample <BAMBOO_00_2048_NUM_CELLS, AUDIO_RATE> (BAMBOO_00_2048_DATA),
  Sample <BAMBOO_01_2048_NUM_CELLS, AUDIO_RATE> (BAMBOO_01_2048_DATA),
  Sample <BAMBOO_02_2048_NUM_CELLS, AUDIO_RATE> (BAMBOO_02_2048_DATA),
};

// watch out - tables are const (but you can choose which ones you reference)
const char * tables[NUM_TABLES] ={
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
  kTriggerDelay.set(ms_per_note); // countdown ms, within resolution of CONTROL_RATE
  kTriggerSlowDelay.set(ms_per_note*6); // resolution-dependent inaccuracy leads to polyrhythm :)

  startMozzi(CONTROL_RATE);
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


int updateAudio(){
  long asig=0;
  for (byte i=0;i<NUM_PLAYERS;i++){
    asig += (int) (aSample[i]).next() * gain[i];
  }
  asig >>= 6; // shift into usable range
  //clip any stray peaks to max output range
  return (int)constrain((int)asig,-244,243);
}


void loop(){
  audioHook();
}




















