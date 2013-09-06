/*
 * ControlDelay.h
 *
 * Copyright 2012 Tim Barrass.
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

#ifndef CONTROLDELAY_H_
#define CONTROLDELAY_H_

#include "AudioDelay.h"

/** 
@brief Control-rate delay line for delaying control signals.
For example, this could be used to produce echo-like effects using multiple
instances of the same voice, when AudioDelay would be too short for an actual
audio echo.  This is in fact just a wrapper of the AudioDelay code.
@tparam  NUM_BUFFER_SAMPLES is the length of the delay buffer in samples.  This should
be a power of two.
@tparam the type of numbers to use for the signal in the delay.  The default is char, but int could be useful
when adding manual feedback.  When using int, the input should be limited to 15 bits width, ie. -16384 to 16383.
*/

template <unsigned int NUM_BUFFER_SAMPLES, class T = int>
class ControlDelay: public AudioDelay<NUM_BUFFER_SAMPLES, T>
{

};

/**
@example 02.Control/Control_Echo_Theremin/Control_Echo_Theremin.ino
This is an example of how to use the ControlDelay class.
*/

#endif        //  #ifndef CONTROLDELAY_H_

