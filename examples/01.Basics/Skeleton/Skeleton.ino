#include <MozziGuts.h>        // at the top of your sketch
#define CONTROL_RATE 64

void setup() {
  startMozzi(CONTROL_RATE);
}

void updateControl() {
  // your control code
}

AudioOutput_t updateAudio() {
  // For mono output, the return value of this function is really just a signed integer.
  // However, for best portability of your sketch to different boards and configurations,
  // pick one of the variants below, depending on the "natural" range of the audio values
  // you generate:
  return MonoOutput::from8Bit(0);  // if your signal is in 8 bit range
  /* OR */
  return MonoOutput::fromAlmostNBit(9, 0);  // if your signal is between -244 and 243 (_almost_ 9 bits is a special case on AVR boards)
  /* OR */
  return MonoOutput::fromAlmostNBit(9, 0).clip();  // To clip (instead of overflow), should a few stray values exceed the allowable range
  /* OR */
  return MonoOutput::from16Bit(0);  // if your signal is in 16 bit range, e.g. the product of two 8 bit numbers
  /* OR */
  return MonoOutput::fromNBit(21, 0);  // if your signal happens to be in 21 bit range
  // In case you are wondering:
  // In older sketches, you will find "int updateAudio()", returning a plain int value.
  // That will still work, but is not recommended.
}

void loop() {
  audioHook(); // fills the audio buffer
}
