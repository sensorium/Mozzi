#include "utils.h"

/** @ingroup util
Converts midi note number to frequency. Caution: this can take up to 400
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


// static Q16n16 Q16n16_m2f(float midival)
// {
// 	return float_to_Q16n16(mtof(midival));
// }

//static const Q16n16 __attribute__((progmem)) midiToFreq[128] =
// {
//         Q16n16_m2f(0), Q16n16_m2f(1), Q16n16_m2f(2),Q16n16_m2f(3), Q16n16_m2f(4), Q16n16_m2f(5), Q16n16_m2f(6), Q16n16_m2f(7),
//         Q16n16_m2f(8),Q16n16_m2f(9), Q16n16_m2f(10), Q16n16_m2f(11), Q16n16_m2f(12), Q16n16_m2f(13), Q16n16_m2f(14), Q16n16_m2f(15),
//         Q16n16_m2f(16), Q16n16_m2f(17), Q16n16_m2f(18), Q16n16_m2f(19), Q16n16_m2f(20), Q16n16_m2f(21), Q16n16_m2f(22), Q16n16_m2f(23),
//         Q16n16_m2f(24), Q16n16_m2f(25), Q16n16_m2f(26), Q16n16_m2f(27), Q16n16_m2f(28), Q16n16_m2f(29), Q16n16_m2f(30), Q16n16_m2f(31),
//         Q16n16_m2f(32), Q16n16_m2f(33), Q16n16_m2f(34), Q16n16_m2f(35), Q16n16_m2f(36), Q16n16_m2f(37), Q16n16_m2f(38), Q16n16_m2f(39),
//         Q16n16_m2f(40), Q16n16_m2f(41), Q16n16_m2f(42), Q16n16_m2f(43), Q16n16_m2f(44), Q16n16_m2f(45), Q16n16_m2f(46), Q16n16_m2f(47),
//         Q16n16_m2f(48), Q16n16_m2f(49), Q16n16_m2f(50), Q16n16_m2f(51), Q16n16_m2f(52), Q16n16_m2f(53), Q16n16_m2f(54), Q16n16_m2f(55),
//         Q16n16_m2f(56), Q16n16_m2f(57), Q16n16_m2f(58), Q16n16_m2f(59), Q16n16_m2f(60), Q16n16_m2f(61), Q16n16_m2f(62), Q16n16_m2f(63),
//         Q16n16_m2f(64), Q16n16_m2f(65), Q16n16_m2f(66), Q16n16_m2f(67), Q16n16_m2f(68), Q16n16_m2f(69), Q16n16_m2f(70), Q16n16_m2f(71),
//         Q16n16_m2f(72), Q16n16_m2f(73), Q16n16_m2f(74), Q16n16_m2f(75), Q16n16_m2f(76), Q16n16_m2f(77), Q16n16_m2f(78), Q16n16_m2f(79),
//         Q16n16_m2f(80), Q16n16_m2f(81), Q16n16_m2f(82), Q16n16_m2f(83), Q16n16_m2f(84), Q16n16_m2f(85), Q16n16_m2f(86), Q16n16_m2f(87),
//         Q16n16_m2f(88), Q16n16_m2f(89), Q16n16_m2f(90), Q16n16_m2f(91), Q16n16_m2f(92), Q16n16_m2f(93), Q16n16_m2f(94), Q16n16_m2f(95),
//         Q16n16_m2f(96),Q16n16_m2f(97), Q16n16_m2f(98), Q16n16_m2f(99), Q16n16_m2f(100), Q16n16_m2f(101), Q16n16_m2f(102), Q16n16_m2f(103),
//         Q16n16_m2f(104),Q16n16_m2f(105), Q16n16_m2f(106), Q16n16_m2f(107), Q16n16_m2f(108), Q16n16_m2f(109), Q16n16_m2f(110), Q16n16_m2f(111),
//         Q16n16_m2f(112),Q16n16_m2f(113), Q16n16_m2f(114), Q16n16_m2f(115), Q16n16_m2f(116), Q16n16_m2f(117), Q16n16_m2f(118), Q16n16_m2f(119),
//         Q16n16_m2f(120), Q16n16_m2f(121), Q16n16_m2f(122), Q16n16_m2f(123), Q16n16_m2f(124), Q16n16_m2f(125), Q16n16_m2f(126), Q16n16_m2f(127)
// };


static const uint32_t __attribute__((progmem)) midiToFreq[128] =
        {
                535809, 567670, 601425, 637188, 675077, 715219, 757748, 802806, 850544, 901120,
                954703, 1011473, 1071618, 1135340, 1202851, 1274376, 1350154, 1430438, 1515497,
                1605613, 1701088, 1802240, 1909406, 2022946, 2143236, 2270680, 2405702, 2548752,
                2700309, 2860877, 3030994, 3211226, 3402176, 3604479, 3818813, 4045892, 4286472,
                4541359, 4811404, 5097504, 5400618, 5721756, 6061988, 6422452, 6804352, 7208959,
                7637627, 8091785, 8572945, 9082719, 9622808, 10195009, 10801235, 11443507,
                12123974, 12844905, 13608704, 14417917, 15275252, 16183563, 17145888, 18165438,
                19245616, 20390018, 21602470, 22887014, 24247948, 25689810, 27217408, 28835834,
                30550514, 32367136, 34291776, 36330876, 38491212, 40780036, 43204940, 45774028,
                48495912, 51379620, 54434816, 57671668, 61101028, 64734272, 68583552, 72661752,
                76982424, 81560072, 86409880, 91548056, 96991792, 102759240, 108869632,
                115343336, 122202056, 129468544, 137167104, 145323504, 153964848, 163120144,
                172819760, 183096224, 193983648, 205518336, 217739200, 230686576, 244403840,
                258937008, 274334112, 290647008, 307929696, 326240288, 345639520, 366192448,
                387967040, 411036672, 435478400, 461373152, 488807680, 517874016, 548668224,
                581294016, 615859392, 652480576, 691279040, 732384896, 775934592, 822073344
        };

/** @ingroup util
Converts midi note number to frequency. Q16n16_mtofLookup() is a fast
alternative to mtof(), using Q16n16 fixed-point format instead of floats. It's a
good choice if you're using whole note values. Q16n16_mtof() uses cheap linear
interpolation between whole midi-note frequency equivalents stored in a lookup
table, so is less accurate than the float version, mtof(), for non-whole midi
values.
@note Timing: ~8 us.
@param midival a midi note number in Q16n16 format, for fractional values.
@return the frequency represented by the input midi note number, in Q16n16
fixed point fractional integer format, where the lower word is a fractional value.
*/
Q16n16  Q16n16_mtof(Q16n16 midival_fractional)
{
	Q16n16 diff_fraction;
	unsigned char index = midival_fractional >> 16;
	unsigned int fraction = (unsigned int) midival_fractional; // keeps low word
	Q16n16 freq1 = (Q16n16) pgm_read_dword(midiToFreq + index);
	Q16n16 freq2 = (Q16n16) pgm_read_dword(midiToFreq + (index+1));
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



/** @ingroup util
Generates a random number in the range for midi notes.
@return a random value between 0 and 127 inclusive
*/
unsigned char randomMidi()
{
	return lowByte(xorshift96())>>1;
}


// moved these out of xorshift96() so xorshift96() can be reseeded manually
static unsigned long x=132456789, y=362436069, z=521288629;
// static unsigned long x= analogRead(A0)+123456789;
// static unsigned long y= analogRead(A1)+362436069;
// static unsigned long z= analogRead(A2)+521288629;

/** @ingroup util
Initialises Mozzi's (pseudo)random number generator xorshift96(), which is used
in Mozzi's rand() function. This can be useful if you want random sequences to
be different on each run of a sketch, by seeding with fairly random input, such
as analogRead() on an unconnected pin (as explained in the Arduino documentation
for randomSeed(). randSeed is the same as xorshift96Seed(), but easier to
remember.
@param seed an int to use as a seed.
*/
void randSeed(unsigned long seed)
{
	x=seed;
}


/** @ingroup util
Initialises Mozzi's (pseudo)random number generator xorshift96(). This can be
useful if you want random sequences to be different on each run of a sketch, by
seeding with fairly random input, such as analogRead() on an unconnected pin (as
explained in the Arduino documentation for randomSeed().
@param seed an int to use as a seed.
*/
void xorshiftSeed(unsigned long seed)
{
	x=seed;
}


/** @ingroup util
Random number generator.
A faster replacement for Arduino's random function,
which is too slow to use with Mozzi.
@return a random 32 bit integer.
@todo check timing of xorshift96(), rand() and other PRNG candidates.
 */
// Based on Marsaglia, George. (2003). Xorshift RNGs. http://www.jstatsoft.org/v08/i14/xorshift.pdf
unsigned long xorshift96()
{          //period 2^96-1
	// static unsigned long x=123456789, y=362436069, z=521288629;
	//static unsigned long x=123456789, y=362436069, z=analogRead(0);
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
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param minval the minimum signed byte value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum signed byte value of the range to be chosen from.  Maxval-1 will be the largest value  possibly returned by the function.
@return a random char between minval and maxval-1 inclusive.
*/
char rand(char minval, char maxval)
{
	return (char) ((((int) (lowByte(xorshift96()))) * (maxval-minval))/256) + minval;
}


/** @ingroup util
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param minval the minimum unsigned byte value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned char between minval and maxval-1 inclusive.
*/
unsigned char rand(unsigned char minval, unsigned char maxval)
{
	return (unsigned char) ((((unsigned int) (lowByte(xorshift96()))) * (maxval-minval))/256) + minval;
}


/** @ingroup util
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param minval the minimum signed int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between minval and maxval-1 inclusive.
*/
int rand( int minval,  int maxval)
{
	return (int) ((((xorshift96()>>16) * (maxval-minval))>>16) + minval);
}


/** @ingroup util
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param minval the minimum unsigned int value of the range to be chosen from.  Minval will be the minimum value possibly returned by the function.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between minval and maxval-1 inclusive.
*/
unsigned int rand(unsigned int minval, unsigned int maxval)
{
	return (unsigned int) ((((xorshift96()>>16) * (maxval-minval))>>16) + minval);
}


/** @ingroup util
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum signed byte value of the range to be chosen from.  Maxval-1 will be the largest value  possibly returned by the function.
@return a random char between 0 and maxval-1 inclusive.
*/
char rand(char maxval)
{
	return (char) ((((int) (lowByte(xorshift96()))) * maxval)/256);
}


/** @ingroup util
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum unsigned byte value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned char between 0 and maxval-1 inclusive.
*/
unsigned char rand(unsigned char maxval)
{
	return (unsigned char) ((((unsigned int) (lowByte(xorshift96()))) * maxval)/256);
}


/** @ingroup util
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum signed int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random int between 0 and maxval-1 inclusive.
*/
int rand(int maxval)
{
	return (int) (((xorshift96()>>16) * maxval)>>16);
}


/** @ingroup util
Ranged random number generator, faster than Arduino's built-in random function, which is too slow for Mozzi.
@param maxval the maximum unsigned int value of the range to be chosen from.  Maxval-1 will be the largest value possibly returned by the function.
@return a random unsigned int between 0 and maxval-1 inclusive.
*/
unsigned int rand(unsigned int maxval)
{
	return (unsigned int) (((xorshift96()>>16) * maxval)>>16);
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

static unsigned char analog_reference = DEFAULT;

/** @ingroup util
Starts an analog-to-digital conversion of the voltage at a specified pin.  Unlike
Arduino's analogRead() function which waits until a conversion is complete before
returning, startAnalogRead() only sets the conversion to begin, so you can use
the cpu for other things and call for the result later with receiveAnalogRead().
This works well in updateControl(), where you can call startAnalogRead() and
get the result with receiveAnalogRead() the next time the updateControl()
interrupt runs.  Fantastic.
@param pin is the analog pin number to sample from, A0 to A5
(or whatever your board goes up to).
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/

// basically analogRead() chopped in half so the ADC conversion
// can be started in one function and received in another.
void startAnalogRead(unsigned char pin)
{

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if (pin >= 54)
		pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)

	if (pin >= 18)
		pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__)

	if (pin >= 24)
		pin -= 24; // allow for channel or pin numbers
#else

	if (pin >= 14)
		pin -= 14; // allow for channel or pin numbers
#endif

#if defined(__AVR_ATmega32U4__)

	pin = analogPinToChannel(pin);
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)

	ADMUX = (analog_reference << 6) | (pin & 0x07);
#endif

	// without a delay, we seem to read from the wrong channel
	//delay(1);

#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	sbi(ADCSRA, ADSC);
#endif
}

/** @ingroup util
Waits for the result of the most recent startAnalogRead().  If used as the first function
of updateControl(), to receive the result of startAnalogRead() from the end of the last
updateControl(), there will probably be no waiting time, as the ADC conversion will
have happened in between interrupts.  This is a big time-saver, since you don't have to
waste time waiting for analogRead() to return (1us here vs 105 us for standard Arduino).
@return The resut of the most recent startAnalogRead().
@note Timing: about 1us when used in updateControl() with CONTROL_RATE 64.
*/
int receiveAnalogRead()
{
	unsigned char low, high;
#if defined(ADCSRA) && defined(ADCL)
	// ADSC is cleared when the conversion finishes
	while (bit_is_set(ADCSRA, ADSC))
		;

	// we have to read ADCL first; doing so locks both ADCL
	// and ADCH until ADCH is read.  reading ADCL second would
	// cause the results of each conversion to be discarded,
	// as ADCL and ADCH would be locked when it completed.
	low  = ADCL;
	high = ADCH;
#else
	// we dont have an ADC, return 0
	low  = 0;
	high = 0;
#endif

	// combine the two bytes
	return (high << 8) | low;
}

