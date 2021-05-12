/**
* We demonstrate how we can use the AudioStream as Aduio Data Source.
* We just print the data to the Serial port where you could analyze it in the Serial Plotter
*/

#include <MozziOut.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

AudioStream mozzi;
const int bufferSize=100;
int16_t buffer[bufferSize];

void setup() {
  Serial.begin(115200);
  while(!Serial);
  
  Serial.println("setup...");
  mozzi.start(CONTROL_RATE); // :)
  aSin.setFreq(440); // set the frequency

  Serial.println("playing sound...");
}

void updateControl(){
  // put changing controls in here
}

AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSin.next()); // return an int signal centred around 0
}

void loop(){
  int byteCount = sizeof(int16_t);

  // we just copy the generated sound data from mozzi to I2S
  size_t size = mozzi.readBytes((uint8_t*)buffer, bufferSize*byteCount);
  for (int16_t j=0;j<size/byteCount;j++){
    Serial.println(buffer[j]);
  }