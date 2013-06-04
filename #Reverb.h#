/*
 * Reverb.h
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

#include <AudioDelay.h>

template <int EARLY_REFLECTION_LENGTH=128, class EARLY_DATATYPE=char, int EARLY_REFLECTION_DELAY1 = 37, 
int EARLY_REFLECTION_DELAY2 = 77, int EARLY_REFLECTION_DELAY3 = 127, 
int LOOP1_LENGTH=128, class LOOP1_DATATYPE=int, int LOOP2_LENGTH=256, class LOOP2_DATATYPE=int>
class
	RecentAverage {

public:
	/** Constructor.
	*/
	Reverb(
		unsigned int loop1_delay=117, unsigned int loop2_delay=254,char feedback_level = 85):
	{
		aLoopDel1.set(loop1_delay);
		aLoopDel2.set(loop2_delay);
		_feedback_level = feedback_level;
	}


	int next(int input){
		static int recycle1, recycle2;

		// early reflections
		int asig = aLoopDel0.next(input>>8, EARLY_REFLECTION_DELAY1);
		asig += aLoopDel0.read(EARLY_REFLECTION_DELAY2);
		asig += aLoopDel0.read(EARLY_REFLECTION_DELAY3);
		asig >>= 3;

		// recirculating delays
		char feedback_sig1 = (char) min(max(((recycle1 * _feedback_level)>>7),-128),127); // feedback clipped
		char feedback_sig2 = (char) min(max(((recycle2 * _feedback_level)>>7),-128),127); // feedback clipped
		int sig3 = aLoopDel1.next(asig+feedback_sig1);
		int sig4 = aLoopDel2.next(asig+feedback_sig2);
		recycle1 = sig3 + sig4;
		recycle2 = sig3 - sig4;

		return recycle1;
	}

	/*
setEarlyReflections(unsigned int EARLY_REFLECTION_DELAY1 = 37, unsigned int EARLY_REFLECTION_DELAY2 = 77, unsigned int EARLY_REFLECTION_DELAY3 = 127){
}
*/

private:
	char _feedback_level;

	AudioDelay <EARLY_REFLECTION_LENGTH, EARLY_DATATYPE> aLoopDel0; // 7.8ms
	AudioDelay <LOOP1_LENGTH, LOOP1_DATATYPE> aLoopDel1; // 7.8ms = 3 metres
	AudioDelay <LOOP2_LENGTH, LOOP2_DATATYPE> aLoopDel2; // 15ms = 5 metres





};
