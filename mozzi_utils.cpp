#include "mozzi_utils.h"

/** @ingroup util
Given a power of 2, work out the number to shift right by to do a divide by the number, or shift left to multiply.
@param a power of 2, or any other number for that matter
@return the number of trailing zeros on the right hand end
*/

/* NOTE: previous version is super-nifty, but pulls in software float code on AVR (the platform where it makes a difference), leading to bloat and a compile time warning.
 * Sine the only use-case I could find works on a compile-time constant (template-parameter), I added a constexpr function in mozzi_utils.h, instead. I renamed it, too,
 * so, we'll learn about any use-case outside of this scope. If there are no reports of breakage, the following can probably be removed for good.
long trailingZeros(unsigned long v) {
	// find the number of trailing zeros in v, from http://graphics.stanford.edu/~seander/bithacks.html#ZerosOnRightFloatCast
	// there are faster methods on the bit twiddling site, but this is short
	float f = (float)(v & -v); // cast the least significant bit in v to a float
	return (*(uint32_t *)&f >> 23) - 0x7f;
}

Here's an alternate, trivial version:
uint8_t trailingZeros(uint16_t v) {
	uint8_t ret = 0;
	while ((v % 2) == 0) {
		v = v >> 1;
		++ret;
	}
	return ret;
} */


	/** Convert BPM to milliseconds, which can be used to set the delay between beats for Metronome.
	@param bpm beats per minute
	*/
	unsigned int BPMtoMillis(float bpm){
		float seconds_per_beat = 60.f/bpm;
		return (unsigned int) (seconds_per_beat*1000);
	}
