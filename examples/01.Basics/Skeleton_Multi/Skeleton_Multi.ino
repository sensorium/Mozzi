/* This example shows how to set up a sketch where Mozzi-related functions are called
   from more than one .cpp source file (which will be compiled, separately).

   Unless you have good reason to do this, it is recommended to base your sketch on the
   single-file "Skeleton" example, instead.

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2012-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <Mozzi.h>        // at the top of your sketch

void setup() {
  startMozzi();
}

void updateControl() {
  // your control code
}

void loop() {
  audioHook(); // fills the audio buffer
}
