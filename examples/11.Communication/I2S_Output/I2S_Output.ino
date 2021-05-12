/**
* We demonstrate how we can send the output to an externa DAC with the help of the Arduino I2S interace using the I2S library.
* This library allows you to use the I2S protocol on SAMD21 based boards (i.e Arduino or Genuino Zero, MKRZero or MKR1000 Board).
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include "I2S.h"
#include "MozziStreams.h"

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // Hz, powers of 2 are most reliable

AudioStream mozzi;
const int bufferSize=1024;
uint8_t buffer[bufferSize];

void setup() {
  Serial.begin(115200);
  while(!Serial);
  Serial.println("setup...");
  mozzi.start(CONTROL_RATE); // :)
  aSin.setFreq(440); // set the frequency

  I2S.begin(I2S_PHILIPS_MODE, AUDIO_RATE, 16); 
  I2S.setBufferSize(bufferSize);
  Serial.println("playing sound...");
}

void updateControl(){
  // put changing controls in here
}

AudioOutput_t updateAudio(){
  return MonoOutput::from8Bit(aSin.next()); // return an int signal centred around 0
}

void loop(){
  int size = I2S.availableForWrite();
  size = mozzi.read(buffer, size);
  I2S.write(buffer, size);
}
