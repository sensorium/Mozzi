/**
* We demonstrate how we can send the output to Serial so that we can verify it in the Arduino Serial Plotter
*/
#include <Mozzi.h>
#include <Phasor.h> // for controlling panning position
#include <Oscil.h> // oscil for audio sig
#include <tables/pinknoise8192_int8.h> // table for oscillator
#include "MozziOut.h"

#if CHANNELS != 2 
#error("Please set the channels to 2 for this example")
#endif

// use: Oscil <table_size, update_rate> oscilName (wavetable)
Oscil <PINKNOISE8192_NUM_CELLS, AUDIO_RATE> aNoise(PINKNOISE8192_DATA);
Phasor <CONTROL_RATE> kPan; // outputs an unsigned long 0-max 32 bit positive number
unsigned int pan; // convey pan from updateControl() to updateAudioStereo();

// output to Serial
AudioStream mozzi;
const int buffer_size = 100;
AudioOutputStorage_t buffer[buffer_size];
const int sample_size = sizeof(AudioOutputStorage_t);


void setup(){
  aNoise.setFreq(2.111f); // set the frequency with an unsigned int or a float
  kPan.setFreq(0.25f); // take 4 seconds to move left-right
  mozzi.start(); // :)
  Serial.begin(115200);
}


void updateControlN(int channels){
  pan = kPan.next()>>16;
  Serial.println(pan);
}


void updateAudioN(int channels, MultiChannelOutput &out ) {
  int asig = aNoise.next();
  out[0] = (((long)pan*asig)>>16);
  out[1] = ((((long)65535-pan)*asig)>>16);
}


void loop(){
    size_t bytes = mozzi.readBytes((uint8_t*)buffer, buffer_size/sample_size);
    size_t samples = bytes / sample_size;
    for (int j=0;j<samples;j++){
      if (j % CHANNELS == 0)
        Serial.println();
      else
        Serial.print(", ");
      Serial.print(buffer[j]);
    }
}

