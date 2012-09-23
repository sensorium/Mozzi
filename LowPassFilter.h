/*
 * LowPassFilter.h
 *
 * Copyright 2012 Tim Barrass
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

#ifndef LOWPASS_H_
#define LOWPASS_H_

/*
simple resonant filter posted to musicdsp.org by Paul Kellett http://www.musicdsp.org/archive.php?classid=3#259

// set feedback amount given f and q between 0 and 1
fb = q + q/(1.0 - f);

// for each sample...
buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
buf1 = buf1 + f * (buf0 - buf1);
out = buf1;

fixed point version of the filter
"dave's blog of art and programming" http://www.pawfal.org/dave/blog/2011/09/
*/


// we are using .n fixed point (n bits for the fractional part)
#define FX_SHIFT 8
#define SHIFTED_1 256

/** A resonant low pass filter for audio signals.
*/
class LowPassFilter
{

public:


	/** Constructor.
	*/
	LowPassFilter();


	/** Set the cut off frequency,
	@param cutoff use the range 0-255 to represent 0-8192 Hz (AUDIO_RATE/2).
	Be careful of distortion at the lower end, especially with high resonance.
	*/
	void setCutoffFreq(unsigned char cutoff)
	{
		f = cutoff;
		setFeedback((int)cutoff);
	}


	/** Set the resonance.  If you hear unwanted distortion, back off the resonance.
	@param resonance in the range 0-255.
	*/
	void setResonance(unsigned char resonance)
	{
		q = resonance;
	}

	/** Calculate the next sample, given an input signal.
	@param in the signal input.
	@return the signal output.
	*/
	//16us
	inline
	int next(int in)
	{
		buf0+=fxmul(f,  ((in - buf0) + fxmul(fb, buf0-buf1)));
		buf1+=fxmul(f, buf0-buf1);
		return buf1;
	}


private:
	int f;
	long fb;
	int q;
	int buf0,buf1;

	inline
	void setFeedback(int f)
	{
		fb = q+fxmul(q, (int)SHIFTED_1 - f);
	}

	// convert an int into to its fixed representation
	inline
	long fx(int i)
	{
		return (i<<FX_SHIFT);
	}


	// // multiply two fixed point numbers (returns fixed point)
	// inline
	// long fxmul(long a, long b)
	// {
	// 	return (a*b)>>FX_SHIFT;
	// }

	// multiply two fixed point numbers (returns fixed point)
	inline
	long fxmul(long a, int b)
	{
		return ((a*b)>>FX_SHIFT);
	}


};

#endif /* LOWPASS_H_ */
