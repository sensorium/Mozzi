/*
 * WavePacket.h
 *
 * Copyright 2013 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mozzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mozzi.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef WAVEPACKET_H
#define WAVEPACKET_H

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <mozzi_fixmath.h>
#include <Phasor.h>
#include <Line.h>
#include <meta.h>


enum algorithms {SINGLE,DOUBLE};

/**
Wavepacket synthesis, with two overlapping streams of wave packets. Draws on
Miller Puckette's Pure Data example, F14.wave.packet.pd. Each packet is an
enveloped grain of a sin (or cos) wave. The frequency of the wave, the width of
the envelopes and the rate of release of envelopes are the parameters which can
be changed.
@tparam ALGORITHM options are SINGLE or DOUBLE, for a single non-overlapping stream of packets or a double, overlapping stream.
*/
template <char ALGORITHM>
class WavePacket
{
public:

	/** Constructor.
	*/
	WavePacket():AUDIO_STEPS_PER_CONTROL(AUDIO_RATE / CONTROL_RATE)
	{
		aCos.setTable(COS8192_DATA);
	}
	
	
	/** Set all the parameters for the synthesis.
	The function is designed so that usable ranges for parameters can come from analog inputs, ie. 0-1023.
	@param fundamental the rate at which packets are produced.
	@param bandwidth the width of each packet.  A lower value allows more 
	of the centre frequency to be audible, a rounder sound.  
	A higher value produces narrower packets, a more buzzing sound.
	@param centrefreq the oscillation frequency within each packet.
	*/
	inline
	void set(int fundamental, int bandwidth, int centrefreq)
	{
		setFundamental(fundamental);
		setBandwidth(bandwidth);
		setCentreFreq(centrefreq);
	}


	/** Set the fundamental frequency.
	The function is designed so that usable ranges for parameters can come from analog inputs, ie. 0-1023.
	@param fundamental the rate at which packets are produced.
	*/
	inline
	void setFundamental(int fundamental)
	{
		aPhasor.setFreq(fundamental);
		invFreq = Q8n24_FIX1 / fundamental;
	}



	/** Set the bandwidth.
	The function is designed so that usable ranges for parameters can come from analog inputs, ie. 0-1023.
	@param bandwidth the width of each packet.  A lower value allows more of the 
	centre frequency to be audible, a rounder sound.  
	A higher value produces narrower packets, a more buzzing sound.
	*/
	inline
	void setBandwidth(int bandwidth)
	{
		Q15n16 bw = invFreq*bandwidth;
		bw >>= 9;
		bw = max(bw, Q15n16_FIX1>>3);
		aBandwidth.set(bw, AUDIO_STEPS_PER_CONTROL);
	}



	/** Set the centre frequency.
	The function is designed so that usable ranges for parameters can come from analog inputs, ie. 0-1023.
	@param centrefreq the oscillation frequency within each packet.
	*/
	inline
	void setCentreFreq(int centrefreq)
	{
		Q15n16 cf = invFreq * centrefreq;
		cf >>= 3;
		aCentrefreq.set(cf, AUDIO_STEPS_PER_CONTROL);
	}


/** Calculate the next synthesised sample.
@return a full-scale 16 bit value, which needs to be scaled to suit your sketch.  If you're using it straight as the sketch output,
then that will be yourThing.next()>>2 for HIFI 14 bit output, or >>8 for STANDARD 8+ bit output.
*/
	inline
	int next()
	{
		gcentrefreq = aCentrefreq.next();
		gbandwidth = aBandwidth.next();
		int phase1 = aPhasor.next()>>16; // -0.5 to 0.5, 0n15
		if (ALGORITHM == DOUBLE) {
			return (signalPath(params1, phase1)+signalPath(params2, phase1+32768))>>1;
		} else {
			return signalPath(params1, phase1);
		}
	}

	
	
private:
	//Q15n16 fundamental; // range 4 to 6271 in pd patch, 13 integer bits
	Q8n24 invFreq;
	Q15n16 gcentrefreq;   // range 0 to 3068, 12 integer bits
	Q15n16 gbandwidth;   // range 1 to 3068, 12 integer bits

	// Lines to interpolate controls at audio rate
	Line <Q15n16> aCentrefreq;
	Line <Q16n16> aBandwidth;
	Line <Q16n16> aFreq;

	// different sets of params for each audio phase stream
	struct parameters
	{
		int previous_phase;
		Q15n16 centrefreq;
		Q23n8 bandwidth;
	}
	params1,params2;

	// the number of audio steps the line has to take to reach the next control value
	const unsigned int AUDIO_STEPS_PER_CONTROL;

	Oscil <COS8192_NUM_CELLS, AUDIO_RATE> aCos;
	Phasor <AUDIO_RATE> aPhasor;


	inline
	int signalPath(struct parameters &param, int phase) // 25us?  * 2
	{
		//setPin13High();
		int index;

		if(phase<param.previous_phase)
		{
			param.centrefreq = gcentrefreq>>8;
			param.bandwidth = Q15n16_to_Q23n8(gbandwidth);
		}
		param.previous_phase = phase;

		// oscillation
		index = (param.centrefreq * phase)>>16;
		// hack to make peak in middle of packet, otherwise it centres around a zero-crossing.. 
		index += COS8192_NUM_CELLS>>1; 
		index &= COS8192_NUM_CELLS-1;
		char sig1 = aCos.atIndex(index);

		// packet envelope
		Q23n8 bwphase = (param.bandwidth * phase)>>8;
		bwphase += COS8192_NUM_CELLS>>1;
		index = constrain(bwphase, 0, (COS8192_NUM_CELLS-1));
		unsigned char packet_width = 128 + aCos.atIndex(index);
		// if (AUDIO_MODE == HIFI){
			// return ((int) sig1 * packet_width)>>3; // scaled to fit HIFI updateAudio output
		// }else{
			// return ((int) sig1 * packet_width)>>8; // scaled to fit STANDARD updateAudio output
		// }
		
		return ((int) sig1 * packet_width);
	}

};

/** @example 06.Synthesis/WavePacket/WavePacket.ino
This is an example of how to use the WavePacket class.
*/

#endif        //  #ifndef WAVEPACKET_H
