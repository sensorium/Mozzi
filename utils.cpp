#include "utils.h"

/** @ingroup util
Converts midi note number to frequency. Caution: this takes about 400
microseconds to run. It can seriously mess up the audio output if you use it in
updateControl() or updateAudio(). This is a good choice in setup(), or where you
need precise midi-pitch conversion and aren't doing much other audio
calculation.
@note Timing: ~350 us
@param midival a midi note number.  Like the mtof object in Pd, midi values can have fractions.
@return the frequency represented by the input midi note number..
 */
// code from AF_precision_synthesis sketch, copyright 2009, Adrian Freed.
float mtof(float midival)
{
	return 8.1757989156 * pow(2.0, midival/12.0);
}


static Q16n16 Q16n16_m2f(float midival)
{
	return Q16n16_float2fix(mtof(midival));
}

static Q16n16 midiToFreq[128] =
        {
                Q16n16_m2f(0), Q16n16_m2f(1), Q16n16_m2f(2),Q16n16_m2f(3), Q16n16_m2f(4), Q16n16_m2f(5), Q16n16_m2f(6), Q16n16_m2f(7),
                Q16n16_m2f(8),Q16n16_m2f(9), Q16n16_m2f(10), Q16n16_m2f(11), Q16n16_m2f(12), Q16n16_m2f(13), Q16n16_m2f(14), Q16n16_m2f(15),
                Q16n16_m2f(16), Q16n16_m2f(17), Q16n16_m2f(18), Q16n16_m2f(19), Q16n16_m2f(20), Q16n16_m2f(21), Q16n16_m2f(22), Q16n16_m2f(23),
                Q16n16_m2f(24), Q16n16_m2f(25), Q16n16_m2f(26), Q16n16_m2f(27), Q16n16_m2f(28), Q16n16_m2f(29), Q16n16_m2f(30), Q16n16_m2f(31),
                Q16n16_m2f(32), Q16n16_m2f(33), Q16n16_m2f(34), Q16n16_m2f(35), Q16n16_m2f(36), Q16n16_m2f(37), Q16n16_m2f(38), Q16n16_m2f(39),
                Q16n16_m2f(40), Q16n16_m2f(41), Q16n16_m2f(42), Q16n16_m2f(43), Q16n16_m2f(44), Q16n16_m2f(45), Q16n16_m2f(46), Q16n16_m2f(47),
                Q16n16_m2f(48), Q16n16_m2f(49), Q16n16_m2f(50), Q16n16_m2f(51), Q16n16_m2f(52), Q16n16_m2f(53), Q16n16_m2f(54), Q16n16_m2f(55),
                Q16n16_m2f(56), Q16n16_m2f(57), Q16n16_m2f(58), Q16n16_m2f(59), Q16n16_m2f(60), Q16n16_m2f(61), Q16n16_m2f(62), Q16n16_m2f(63),
                Q16n16_m2f(64), Q16n16_m2f(65), Q16n16_m2f(66), Q16n16_m2f(67), Q16n16_m2f(68), Q16n16_m2f(69), Q16n16_m2f(70), Q16n16_m2f(71),
                Q16n16_m2f(72), Q16n16_m2f(73), Q16n16_m2f(74), Q16n16_m2f(75), Q16n16_m2f(76), Q16n16_m2f(77), Q16n16_m2f(78), Q16n16_m2f(79),
                Q16n16_m2f(80), Q16n16_m2f(81), Q16n16_m2f(82), Q16n16_m2f(83), Q16n16_m2f(84), Q16n16_m2f(85), Q16n16_m2f(86), Q16n16_m2f(87),
                Q16n16_m2f(88), Q16n16_m2f(89), Q16n16_m2f(90), Q16n16_m2f(91), Q16n16_m2f(92), Q16n16_m2f(93), Q16n16_m2f(94), Q16n16_m2f(95),
                Q16n16_m2f(96),Q16n16_m2f(97), Q16n16_m2f(98), Q16n16_m2f(99), Q16n16_m2f(100), Q16n16_m2f(101), Q16n16_m2f(102), Q16n16_m2f(103),
                Q16n16_m2f(104),Q16n16_m2f(105), Q16n16_m2f(106), Q16n16_m2f(107), Q16n16_m2f(108), Q16n16_m2f(109), Q16n16_m2f(110), Q16n16_m2f(111),
                Q16n16_m2f(112),Q16n16_m2f(113), Q16n16_m2f(114), Q16n16_m2f(115), Q16n16_m2f(116), Q16n16_m2f(117), Q16n16_m2f(118), Q16n16_m2f(119),
                Q16n16_m2f(120), Q16n16_m2f(121), Q16n16_m2f(122), Q16n16_m2f(123), Q16n16_m2f(124), Q16n16_m2f(125), Q16n16_m2f(126), Q16n16_m2f(127)
        };


/** @ingroup util
Converts midi note number to frequency. Q16n16_mtofLookup() is a fast
alternative to mtof(), using Q16n16 fixed-point format instead of floats. It's a
good choice if you're using whole note values. Q16n16_mtof() uses cheap linear
interpolation between whole midi-note frequency equivalents stored in a lookup
table, so is less accurate than the float version, mtof(), for non-whole midi
values.
@note Timing: ~14 us.
@param midival a midi note number in Q16n16 format, for fractional values.
@return the frequency represented by the input midi note number, in Q16n16
fixed point fractional integer format, where the lower word is a fractional value.
*/

Q16n16  Q16n16_mtofLookup(Q16n16 midival_fractional)
{
	Q16n16 diff_fraction;
	unsigned char index = midival_fractional >> 16;
	unsigned int fraction = (unsigned int) midival_fractional; // keeps low word
	Q16n16 freq1 = midiToFreq[index];
	Q16n16 freq2 = midiToFreq[index+1];
	Q16n16 difference = freq2 - freq1;
	if (difference>=65536)
	{
		diff_fraction = ((difference>>8) * fraction) >> 8;
	}
	else
	{
		diff_fraction = (difference * fraction) >> 16;
	}
	return (Q16n16) (freq1+ diff_fraction);
}

#define Q8n8_8point1757989156  ((Q8n8)2093)
/** @ingroup util
Converts midi note number to frequency. Q16n16_mtof() is a fast alternative to
mtof(), using Q16n16 fixed-point format instead of floats. Q16n16_mtof()
calculates the frequency using fixed point maths and the results are quite
inaccurate in the higher ranges. However, if you don't need note-perfect pitch,
this is a good choice to get a reasonable approximation of frequency from a
fractional midi value.
@note Timing: ~55 us
@param midival a midi note number in Q16n16 format, for fractional values.
@return the frequency represented by the input midi note number, in Q16n16
fixed point fractional integer format, where the lower word is a fractional value.
@todo Rewrite to change div by constant to multiply by inverse.
*/
Q16n16  Q16n16_mtof(Q16n16 Q16n16_midival)
{
	return ((Q16n16) Q8n8_8point1757989156 * (Q16n16_pow2((Q8n8) ((Q16n16_midival/12)>>8))>>8));
}


/** @ingroup util
Generates a random number in the range for midi notes.
@return a random value between 0 and 127 inclusive
*/
unsigned char randomMidi()
{
	return lowByte(xorshift96())>>1;
}


/** @ingroup util
Random number generator.
A faster replacement for Arduino's random function,
which is too slow to use with Mozzi.
@return a random 32 bit integer.
 */
// Based on Marsaglia, George. (2003). Xorshift RNGs. http://www.jstatsoft.org/v08/i14/xorshift.pdf
unsigned long xorshift96()
{          //period 2^96-1
	static unsigned long x=123456789, y=362436069, z=521288629;
	unsigned long t;

	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}




/** @ingroup util
Make analogRead() faster than the standard Arduino version, changing the
duration from about 105 in unmodified Arduino to 15 microseconds for a
dependable analogRead(). Put this in setup() if you intend to use analogRead()
with Mozzi, to avoid glitches.
See: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1208715493/11
 
*/
void setupFastAnalogRead()
{
	// fastest predivided rate (divide by 16, giving 1Mhz) for which behaviour is defined (~16us per sample)
	sbi(ADCSRA,ADPS2);
	cbi(ADCSRA,ADPS1);
	cbi(ADCSRA,ADPS0);
}


