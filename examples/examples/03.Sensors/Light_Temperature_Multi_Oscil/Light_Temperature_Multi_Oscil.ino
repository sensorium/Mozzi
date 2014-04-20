/*  
  Plays a fluctuating ambient wash in response to light and temperature sensors,
  using Mozzi sonification library.
  
  8 control rate oscillators are used to set the volume of 8 audio oscillators.
  Temperature readings from a thermistor are used to set the notes
  being played, and light readings from a light dependent resistor are
  mapped to the pulse rates of the volume control oscillators.
  
  Circuit:
    Audio output on digital pin 9 (on a Uno or similar), or 
    check the README or http://sensorium.github.com/Mozzi/
    
    Temperature dependent resistor (Thermistor) and 5.1k resistor on analog pin 1:
      Thermistor from analog pin to +5V
      5.1k resistor from analog pin to ground
    
    Light dependent resistor (LDR) and 5.1k resistor on analog pin 2:
      LDR from analog pin to +5V
      5.1k resistor from analog pin to ground
  
  Mozzi help/discussion/announcements:
  https://groups.google.com/forum/#!forum/mozzi-users
  
  Tim Barrass 2013.
  This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 256

#define THERMISTOR_PIN 1
#define LDR_PIN 2
#define NUM_VOICES 8
#define THRESHOLD 10

// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos0(COS8192_DATA);

// volume controls
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol0(COS8192_DATA);

// audio volumes updated each control interrupt and reused in audio till next control
char v1,v2,v3,v4,v5,v6,v7,v0;

// notes to play depending on whether temperature reading increases or decreases
float upnotes[NUM_VOICES] = {
  mtof(62.f),mtof(64.f),mtof(70.f),mtof(72.f),mtof(79.f),mtof(81.f), mtof(86.f), mtof(88.f)};

float downnotes[NUM_VOICES] = {
  mtof(64.f),mtof(65.f),mtof(88.f),mtof(72.f),mtof(79.f),mtof(84.f),mtof(86.f),mtof(89.f)};
  

void setup(){
  startMozzi(CONTROL_RATE);
}


// returns freq
int temperatureToFreq(char oscil_num, int temperature){
  static int previous_temperature;
  int freq;
  if (temperature>previous_temperature){
    freq = upnotes[oscil_num];
  } else {
     freq = downnotes[oscil_num];
  }
  previous_temperature = temperature;
  return freq;
}


void updateControl(){
  static float previous_pulse_freq;
    
  // read analog inputs
  int temperature = mozziAnalogRead(THERMISTOR_PIN); // not calibrated to degrees!
  int light = mozziAnalogRead(LDR_PIN);

  // map light reading to volume pulse frequency
  float pulse_freq = (float)light/256;
  previous_pulse_freq = pulse_freq;

  v0 = kVol0.next();
  v1 = kVol1.next();
  v2 = kVol2.next();
  v3 = kVol3.next();
  v4 = kVol4.next();
  v5 = kVol5.next();
  v6 = kVol6.next();
  v7 = kVol7.next();
  
  // set one note oscillator frequency each time (if it's volume is close to 0)
  static char whoseTurn;
  switch(whoseTurn){  
  case 0:
    kVol0.setFreq(pulse_freq);
    if(abs(v0)<THRESHOLD) aCos0.setFreq(temperatureToFreq(0,temperature));
    break;

  case 1:
    kVol1.setFreq(pulse_freq);
    if(abs(v1)<THRESHOLD) aCos1.setFreq(temperatureToFreq(1,temperature));
    break;

  case 2:
    kVol2.setFreq(pulse_freq);
    if(abs(v2)<THRESHOLD) aCos2.setFreq(temperatureToFreq(2,temperature));
    break;

  case 3:
    kVol3.setFreq(pulse_freq);
    if(abs(v3)<THRESHOLD) aCos3.setFreq(temperatureToFreq(3,temperature));
    break;

  case 4:
    kVol4.setFreq(pulse_freq);
    if(abs(v4)<THRESHOLD) aCos4.setFreq(temperatureToFreq(4,temperature));
    break;

  case 5:
    kVol5.setFreq(pulse_freq);
    if(abs(v5)<THRESHOLD) aCos5.setFreq(temperatureToFreq(5,temperature));
    break;

  case 6:
    kVol6.setFreq(pulse_freq);
    if(abs(v6)<THRESHOLD) aCos6.setFreq(temperatureToFreq(6,temperature));
    break;

  case 7:
    kVol7.setFreq(pulse_freq);
    if(abs(v7)<THRESHOLD) aCos7.setFreq(temperatureToFreq(7,temperature));
    break;

  }

  if(++whoseTurn>=NUM_VOICES) whoseTurn = 0;
}



int updateAudio(){
  long asig = (long)
    aCos0.next()*v0 +
      aCos1.next()*v1 +
      aCos2.next()*v2 +
      aCos3.next()*v3 +
      aCos4.next()*v4 +
      aCos5.next()*v5 +
      aCos6.next()*v6 +
      aCos7.next()*v7;
  asig >>= 9; // shift back to audio output range
  return (int) asig;
}



void loop(){
  audioHook();
}

