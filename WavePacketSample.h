/*
 * WavePacketSample.h
 *
 * Copyright 2013 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */
 
#ifndef WAVEPACKETSAMPLE_H
#define WAVEPACKETSAMPLE_H


#include "WavePacket.h"
/** A WavePacket which allows a custom table to be set as the audio source for the wavepackets (or grains).
@tparam ALGORITHM options are SINGLE or DOUBLE, for a single non-overlapping stream of packets or a double, overlapping stream.

*/
template <int8_t ALGORITHM>
class WavePacketSample: public WavePacket<ALGORITHM>
{
public:
	/** Change the sound table which will be played.  Needs to be 8192 cells long for now.
	@param TABLE_NAME is the name of the array in the table ".h" file you're using.
	*/
	inline
	void setTable(const int8_t * TABLE_NAME)
	{
		aWav.setTable(TABLE_NAME);
	}
	
private:
		Oscil <8192, MOZZI_AUDIO_RATE> aWav;
};

/** @example 06.Synthesis/WavePacket_Sample/WavePacket_Sample.ino
This is an example of how to use the WavePacketSample class.
*/
#endif        //  #ifndef WAVEPACKETSAMPLE_H

