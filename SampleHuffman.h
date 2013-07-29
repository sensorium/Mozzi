/*
 * 
 * Adapted from audioout by
 * Thomas Grill, 2011
 * http//grrrr.org
 *
 * Huffman decoding is used on sample differentials,
 * saving 50-70% of space for 8 bit data, depending on the sample rate.
 *
 * Audio data, Huffman decoder table, sample rate and bit depth are defined
 * in the sounddata.h header file.
 * This file can be generated for a sound file with the 
 * accompanying Python script audio2huff.py
 *
 * Invoke with:
 * python audio2huff.py --sndfile=arduinosnd.wav --hdrfile=sounddata.h --bits=8
 *
 * You can resample and dither your audio file with SOX, 
 * e.g. to 8 bits depth @ 10kHz sample rate:
 * sox fullglory.wav -b 8 -r 10000 arduinosnd.wav
 *
 * The header file contains two lengthy arrays:
 * One is "sounddata" which must fit into Flash RAM (available in total: 16k for ATMega168, 32k for ATMega328)
 * The other is "huffman" which must fit into SRAM (available in total: 1k for ATMega168, 2k for ATMega328)
 *
 * References:
 * Arduino: http://www.arduino.cc/
 * Arduino PWM insights: http://www.arcfn.com/2009/07/secrets-of-arduino-pwm.html
 * ATMega data sheet: http://www.sparkfun.com/datasheets/Components/SMD/ATMega328.pdf
 * SOX: http://sox.sourceforge.net/
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "sounddata.h"

volatile unsigned long datapos = 0; // current sample position
volatile int current = 0;  // current amplitude value

// Get one bit from sound data
inline int getbit(unsigned long pos,boolean autoloadonbit0=false)
{
  const int b = pos&7;
  static unsigned char bt;
  if(!autoloadonbit0 || !b)
    // read indexed byte from Flash memory
    bt = pgm_read_byte(&sounddata[pos>>3]);
  // extract the indexed bit
  return (bt>>(7-b))&1;
}

// Decode bit stream using Huffman codes
static int decode(unsigned long &pos,int const *huffcode)
{
  unsigned long p = pos;
  do {
    const int b = getbit(p++,true);
    if(b) {
      const int offs = *huffcode;
      huffcode += offs?offs+1:2;
    }
  } while(*(huffcode++));
  pos = p;
  return *huffcode;
}

// This is called at sample rate to load the next sample.
getNextSample()
{
  unsigned long pos = datapos;
  
  // at end of sample, restart from zero, looping the sound
  if(pos >= sounddata_bits) pos = 0;
    
  int dif = decode(pos,huffman);
  current += dif; // add differential
    
  // set 16-bit PWM register with sample value
  OCR1A = constrain(current+(1<<(SAMPLE_BITS-1)),0,(1<<SAMPLE_BITS)-1); 
  
  datapos = pos;
}





