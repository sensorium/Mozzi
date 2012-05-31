/*
 * Ead.cpp
 *
 * Adapted from: ead.c  -  exponential attack decay envelope
 * Copyright (c) 2000-2003 by Tom Schouten
 *
 * Copyright 2012 sweatsonics@sweatsonics.com, 2000-2003 Tom Schouten
 *
 * This file is part of Cuttlefish.
 *
 * Cuttlefish is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cuttlefish is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cuttlefish.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Ead.h"

/*
decay:
 y = a*pow((1-r),x)
 
 growth:
 y = a*pow((1+r),x)
 */

Ead::Ead(const unsigned int r) :
		UPDATE_RATE(r)
{}

const unsigned char Ead::ENVELOPE_RANGE  = 1; // smallest fraction of 8n8
const unsigned int Ead::ENVELOPE_MAX    = (float2fix(1) - ENVELOPE_RANGE);




/* unsigned char Ead::getcState()
{
	return cstate;
} */

/*
// from http://people.ece.cornell.edu/land/courses/ece4760/Math/GCC644/Karplus_Strong/Karplus_Strong_2_macro.c
// attack and decay constants
volatile unsigned char rise_1 ;
volatile unsigned char fall_1 ;
 
   // rise and decay SHIFT factors  -- bigger is slower -- max value is 8
   fall_1 = 5 ;
   rise_1 = 1 ;
 
 // amplitude waveforms = attack * decay waveforms
volatile int amp_1;
// attack and decay waveforms
volatile int amp_rise_1 ;
volatile int amp_fall_1 ;
 
start:
		amp_fall_1 = max_amp;
		amp_rise_1 = max_amp ;
 
// compute drive exponential attack and decay of amplitude
// the (time & 0x0ff) slows down the decay computation by 256 times
	if ((time & 0x0ff) == 0) begin
		amp_fall_1 = amp_fall_1 - (amp_fall_1>>fall_1) ;
		amp_rise_1 = amp_rise_1 - (amp_rise_1>>rise_1);
	end
 
	// form (1-exp(-t/tau)) for the attack phase
	// product of rise and fall exponentials is the amplitude envelope
	amp_1 = ((max_amp - amp_rise_1)>>8) * (amp_fall_1>>8) ;
	*/