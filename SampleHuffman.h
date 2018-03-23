/*
 * SampleHuffman.h
 *
 * Copyright 2013 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */
#ifndef SAMPLEHUFFMAN_H
#define SAMPLEHUFFMAN_H

#include "mozzi_pgmspace.h"

/** A sample player for samples encoded with Huffman compression.

This class and the audio2huff.py script are adapted from "audioout",
an Arduino sketch by Thomas Grill, 2011 http//grrrr.org

Huffman decoding is used on sample differentials,
saving 50-70% of space for 8 bit data, depending on the sample rate.

This implementation just plays back one sample each time next() is called, with no
speed or other adjustments.
It's slow, so it's likely you will only be able to play one sound at a time.

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

*/

class SampleHuffman
{

public:

	/** Constructor
	@param SOUNDDATA the name of the SOUNDDATA table in the huffman sample .h file
	@param HUFFMAN_DATA the name of the HUFFMAN table in the huffman sample .h file
	@param	SOUNDDATA_BITS from the huffman sample .h file
	*/
	SampleHuffman(uint8_t const * SOUNDDATA, int16_t const * HUFFMAN_DATA, uint32_t const SOUNDDATA_BITS):sounddata(SOUNDDATA),huffman(HUFFMAN_DATA),sounddata_bits(SOUNDDATA_BITS)
	{
		setLoopingOff();
	}


	/** Update and return the next audio sample.  So far it just plays back one sample at a time without any variable tuning or speed.
	@return the next audio sample
	@note timing: about 5 to 40 us, varies continuously depending on data
	*/
	inline
	int16_t next()
	{
		if(datapos >= sounddata_bits){
			if(looping){
				// at end of sample, restart from zero, looping the sound
				 datapos = 0;
			}else{
				return 0;
			}
		}
		
		int16_t dif = decode();
		current += dif; // add differential
		return current;
	}


	/** Turns looping on, with the whole sample length as the loop range.
	*/
	inline
	void setLoopingOn()
	{
		looping=true;
	}


	/** Turns looping off.
	*/
	inline
	void setLoopingOff()
	{
		looping=false;
	}

	/** Sets the playhead to the beginning of the sample.
	*/
	inline
	void start()
	{
		current = 0;
		datapos = 0;
		bt = 0;
	}
	
private:
	uint8_t const * sounddata;
	int16_t const * huffman;
	uint32_t const sounddata_bits;
	uint32_t datapos; // current sample position
	int16_t current; // current amplitude value
	bool looping;
	uint8_t bt;
	
	// Get one bit from sound data
	inline 
	bool getbit()
	{
		const uint8_t b = datapos&7;
		//static uint8_t bt;
		if(!b) bt = FLASH_OR_RAM_READ<const uint8_t>(sounddata+((uint32_t)datapos>>3));
		// extract the indexed bit
		return ((uint8_t)bt>>(7-b))&1;
	}


	// Decode bit stream using Huffman codes
	inline
	int16_t decode()
	{
		int16_t const * huffcode = huffman;
		do {
			if(getbit()) {
				const int16_t offs = FLASH_OR_RAM_READ<const int16_t>(huffcode);
				huffcode += offs?offs+1:2;
			}
			datapos++;
		}
		while(FLASH_OR_RAM_READ<const int16_t>(huffcode++));
		return FLASH_OR_RAM_READ<const int16_t>(huffcode);
	}


};

/**
@example 08.Samples/SampleHuffman_Umpah/SampleHuffman_Umpah.ino
This example demonstrates the Sample class.
*/

#endif        //  #ifndef SAMPLEHUFFMAN_H
