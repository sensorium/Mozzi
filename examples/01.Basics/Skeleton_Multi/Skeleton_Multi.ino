/** This example shows how to set up a sketch where Mozzi-related functions are called
 *  from more than one .cpp source file (which will be compiled, separately).
 *
 *  Unless you have good reason to do this, it is recommended to base your sketch on the
 *  single-file "Skeleton" example, instead. */

#include <Mozzi.h>        // at the top of your sketch

void setup() {
  startMozzi(64);
}

void updateControl() {
  // your control code
}

void loop() {
  audioHook(); // fills the audio buffer
}
