/*
 * WaveShaper.h
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

#ifndef WAVESHAPER_H_
#define WAVESHAPER_H_

#include "Arduino.h"

/** WaveShaper maps values from its input to values in a table, which are returned as output.
@tparam T the type of numbers being input to be shaped, chosen to match the table.
*/

template <class T>
class WaveShaper
	{}
;


/** char specialisation of WaveShaper template*/
template <>
class WaveShaper < char>
{

public:
	/** Constructor. Use the template parameter to set type of numbers being mapped. For
	example, WaveShaper <int> myWaveShaper; makes a WaveShaper which uses ints.
	@tparam T the type of numbers being input to be shaped, chosen to match the table.
	@param TABLE_NAME the name of the table being used, which can be found in the
	".h" file containing the table. */
	WaveShaper(const char  * TABLE_NAME):table(TABLE_NAME)
	{
		;
	}


	/** Maps input to output, transforming it according to the table being used.
	@param in the input signal. For flexibility, it's up to you to give the correct offset
	to your input signal. So if you're mapping a signed 8-bit signal (such as the output of
	an Oscil) into a 256 cell table centred around cell 128, add 128 to offset the
	input value.
	@return the shaped signal.
	 */
	inline
	char next(unsigned char in)
	{
		return (char) pgm_read_byte_near(table + in);
	}

private:
	const char  * table;
};



/** int specialisation of WaveShaper template*/
template <>
class WaveShaper <int>
{

public:
	/** Constructor. Use the template parameter to set type of numbers being mapped. For
	example, WaveShaper <int> myWaveShaper; makes a WaveShaper which uses ints.
	@tparam T the type of numbers being input to be shaped, chosen to match the table.
	@param TABLE_NAME the name of the table being used, which can be found in the
	".h" file containing the table. */
	WaveShaper(const int * TABLE_NAME):table(TABLE_NAME)
	{
		;
	}


	/** Maps input to output, transforming it according to the table being used.
	@param in the input signal. For flexibility, it's up to you to give the
	correct offset to your input signal. So if you're mapping a signed 9-bit signal
	(such as the sum of 2 8-bit Oscils) into a 512 cell table centred around
	cell 256, add 256 to offset the input value. With a sigmoid table, this
	may be useful for compressing a bigger signal into the -244 to 243
	output range of Mozzi, rather than dividing the signal and returning a
	char from updateAudio().
	@return the shaped signal.
	 */
	inline
	int next(int in)
	{
		return (unsigned int) pgm_read_word_near(table + in);
	}

private:
	const int * table;
};

/** @example 06.Synthesis/WaveShaper/WaveShaper.ino
This is an example of how to use the WaveShaper class.
*/
#endif /* WAVESHAPER_H_ */


