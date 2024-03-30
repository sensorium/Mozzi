/* Example using Metronome to playing samples encoded with Huffman compression.

   Demonstrates Metronome start, stop and ready, and the the SampleHuffman class.

   Circuit:
   Audio output on digital pin 9 on a Uno or similar, or
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
#include <Metronome.h>
#include <SampleHuffman.h>
#include <mozzi_rand.h>

#include <samples/thumbpiano_huffman/thumbpiano0.h>
#include <samples/thumbpiano_huffman/thumbpiano1.h>
#include <samples/thumbpiano_huffman/thumbpiano2.h>
#include <samples/thumbpiano_huffman/thumbpiano3.h>
#include <samples/thumbpiano_huffman/thumbpiano4.h>

SampleHuffman thumb0(THUMB0_SOUNDDATA,THUMB0_HUFFMAN,THUMB0_SOUNDDATA_BITS);
SampleHuffman thumb1(THUMB1_SOUNDDATA,THUMB1_HUFFMAN,THUMB1_SOUNDDATA_BITS);
SampleHuffman thumb2(THUMB2_SOUNDDATA,THUMB2_HUFFMAN,THUMB2_SOUNDDATA_BITS);
SampleHuffman thumb3(THUMB3_SOUNDDATA,THUMB3_HUFFMAN,THUMB3_SOUNDDATA_BITS);
SampleHuffman thumb4(THUMB4_SOUNDDATA,THUMB4_HUFFMAN,THUMB4_SOUNDDATA_BITS);

const char NUM_SAMPLES = 5;

Metronome kMetro(800); // enough delay so samples don't overlap, because the load would be too much

void setup() {
  startMozzi();
}



void updateControl(){
  static unsigned int counter;
  counter++;
  if(counter==177)kMetro.stop();
  if(counter==203){kMetro.start();counter = 0;}
  if(kMetro.ready()){
    switch(rand(NUM_SAMPLES)){
      case 0:
      thumb0.start();
      break;

      case 1:
      thumb1.start();
      break;

      case 2:
      thumb2.start();
      break;

      case 3:
      thumb3.start();
      break;

      case 4:
      thumb4.start();
      break;
    }
  }
}


AudioOutput updateAudio(){
  int asig = (int)
  thumb0.next() +
  thumb1.next() +
  thumb2.next() +
  thumb3.next() +
  thumb4.next();
  // Note: Samples don't overlap, here, therefore this the sum is still only 8 bits range
  return MonoOutput::from8Bit(asig);
}


void loop() {
  audioHook();
}
