/*
 * ControlDelay.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
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
@tparam the type of numbers to use for the signal in the delay.  The default is int8_t, but int could be useful
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
