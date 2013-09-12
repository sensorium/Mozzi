/*
 Example playing samples encoded with Huffman compression.
 
 Demonstrates the SampleHuffman class.
 SampleHuffman, most of this explanation, and the audio2huff.py script are adapted from "audioout",
 an Arduino sketch by Thomas Grill, 2011 http//grrrr.org.
 
 Huffman decoding is used on sample differentials,
 saving 50-70% of space for 8 bit data, depending on the sample rate.
 
 This implementation just plays back one sample each time next() is called, with no
 speed or other adjustments.  It's slow, so it's likely you will only be able to play one sound at a time.

 Audio data, Huffman decoder table, sample rate and bit depth are defined
 in a sounddata.h header file.  This file can be generated for a sound file with the 
 accompanying Python script audio2huff.py, in Mozzi/extras/python/

 Invoke with:
 python audio2huff.py --sndfile=arduinosnd.wav --hdrfile=sounddata.h --bits=8 --name=soundtablename

 You can resample and dither your audio file with SOX, 
 e.g. to 8 bits depth @ Mozzi's 16384 Hz  sample rate:
 sox fullglory.wav -b 8 -r 16384 arduinosnd.wav

 Alternatively you can export a sound from Audacity, which seems to have less noticeable or no dithering, 
 using Project Rate 16384 Hz and these output options:
 Other uncompressed files, Header: WAV(Microsoft), Encoding: Unsigned 8 bit PCM

 The header file contains two lengthy arrays:
 One is "SOUNDDATA" which must fit into Flash RAM (available in total: 32k for ATMega328)
 The other is "HUFFMAN" which must also fit into Flash RAM

 Circuit:
  Audio output on digital pin 9 (on a Uno or similar), or 
  check the README or http://sensorium.github.com/Mozzi/

 Mozzi help/discussion/announcements:
 https://groups.google.com/forum/#!forum/mozzi-users
 
 Tim Barrass 2013.
 This example code is in the public domain.

*/

#include <MozziGuts.h>
#include <SampleHuffman.h>
#include "umpah_huff.h"


SampleHuffman umpah(UMPAH_SOUNDDATA,UMPAH_HUFFMAN,UMPAH_SOUNDDATA_BITS);

void setup() {
  umpah.setLoopingOn();
  startMozzi();
}


void updateControl(){
}


int updateAudio(){ 
  return umpah.next();
}


void loop() {
  audioHook();
}
