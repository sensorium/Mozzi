#include "mozzi_midi.h"

/** @ingroup midi
Converts midi note number to frequency. Caution: this can take up to 400
microseconds to run. It can seriously mess up the audio output if you use it in
updateControl() or updateAudio(). This is a good choice in setup(), or where you
need precise midi-pitch conversion and aren't doing much other audio
calculation.
@note Beware this returns an invalid result for midi note 0.
@note Timing: ~350 us
@param midival a midi note number, 1.0 or greater.  Like the mtof object in Pd, midi values can have fractions.
@return the frequency represented by the input midi note number..
 */



float mtof(float midival)
{
	 // http://en.wikipedia.org/wiki/Note
	 // f = pow(2,(p-69/12) * 440Hz
	 // return pow(2.0,(midival-69.0/12.0) * 440.0;
	 
	// code from AF_precision_synthesis sketch, copyright 2009, Adrian Freed.
	float f = 0.0;
	if(midival) f = 8.1757989156 * pow(2.0, midival/12.0);
	return f;
}


/*
static float ucmtof(unsigned char midival)
{
	return 8.1757989156 * pow(2.0, (float)midival/12.0);
}
*/



// static const float __attribute__(()) fmidiToFreq[128] =
// {
// ucmtof(0), ucmtof(1), ucmtof(2),ucmtof(3), ucmtof(4), ucmtof(5), ucmtof(6), ucmtof(7),
// ucmtof(8),ucmtof(9), ucmtof(10), ucmtof(11), ucmtof(12), ucmtof(13), ucmtof(14), ucmtof(15),
// ucmtof(16), ucmtof(17), ucmtof(18), ucmtof(19), ucmtof(20), ucmtof(21), ucmtof(22), ucmtof(23),
// ucmtof(24), ucmtof(25), ucmtof(26), ucmtof(27), ucmtof(28), ucmtof(29), ucmtof(30), ucmtof(31),
// ucmtof(32), ucmtof(33), ucmtof(34), ucmtof(35), ucmtof(36), ucmtof(37), ucmtof(38), ucmtof(39),
// ucmtof(40), ucmtof(41), ucmtof(42), ucmtof(43), ucmtof(44), ucmtof(45), ucmtof(46), ucmtof(47),
// ucmtof(48), ucmtof(49), ucmtof(50), ucmtof(51), ucmtof(52), ucmtof(53), ucmtof(54), ucmtof(55),
// ucmtof(56), ucmtof(57), ucmtof(58), ucmtof(59), ucmtof(60), ucmtof(61), ucmtof(62), ucmtof(63),
// ucmtof(64), ucmtof(65), ucmtof(66), ucmtof(67), ucmtof(68), ucmtof(69), ucmtof(70), ucmtof(71),
// ucmtof(72), ucmtof(73), ucmtof(74), ucmtof(75), ucmtof(76), ucmtof(77), ucmtof(78), ucmtof(79),
// ucmtof(80), ucmtof(81), ucmtof(82), ucmtof(83), ucmtof(84), ucmtof(85), ucmtof(86), ucmtof(87),
// ucmtof(88), ucmtof(89), ucmtof(90), ucmtof(91), ucmtof(92), ucmtof(93), ucmtof(94), ucmtof(95),
// ucmtof(96),ucmtof(97), ucmtof(98), ucmtof(99), ucmtof(100), ucmtof(101), ucmtof(102), ucmtof(103),
// ucmtof(104),ucmtof(105), ucmtof(106), ucmtof(107), ucmtof(108), ucmtof(109), ucmtof(110), ucmtof(111),
// ucmtof(112),ucmtof(113), ucmtof(114), ucmtof(115), ucmtof(116), ucmtof(117), ucmtof(118), ucmtof(119),
// ucmtof(120), ucmtof(121), ucmtof(122), ucmtof(123), ucmtof(124), ucmtof(125), ucmtof(126), ucmtof(127)
// };
//
// /** @ingroup midi
// Converts midi note number to frequency.  Fast, but only accepts whole note values, no fractions.
// @param midival a midi note number.  Unlike the mtof object in Pd, midi values can have fractions.  Use mtof() or
// Q16n16_mtof() if you want to convert fractional midi values.
// @return the frequency represented by the input midi note number.
// @todo hard-code fmidiToFreq table instead of generating at startup, and add an interpolating m2f which returns a float.
// */
// float m2f(unsigned char midival)
// {
// return (float) pgm_read_float(fmidiToFreq+midival);
// }

// static Q16n16 Q16n16_m2f(float midival)
// {
// 	return float_to_Q16n16(mtof(midival));
// }

//static const Q16n16 __attribute__((section(".progmem.data"))) midiToFreq[128] =
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


static const uint32_t __attribute__((section(".progmem.data"))) midiToFreq[128] =
  {
    0, 567670, 601425, 637188, 675077, 715219, 757748, 802806, 850544, 901120,
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

  
  
/** @ingroup midi
Converts midi note number to frequency with speed and accuracy.  Q16n16_mtofLookup() is a fast
alternative to (float) mtof(), and more accurate than (unsigned char) mtof(),
using Q16n16 fixed-point format instead of floats or byte values. Q16n16_mtof()
uses cheap linear interpolation between whole midi-note frequency equivalents
stored in a lookup table, so is less accurate than the float version, mtof(),
for non-whole midi values.
@note Timing: ~8 us.
@param midival_fractional a midi note number in Q16n16 format, for fractional values.
@return the frequency represented by the input midi note number, in Q16n16
fixed point fractional integer format, where the lower word is a fractional value.
*/
Q16n16  Q16n16_mtof(Q16n16 midival_fractional)
{
	Q16n16 diff_fraction;
	unsigned char index = midival_fractional >> 16;
	unsigned int fraction = (unsigned int) midival_fractional; // keeps low word
	Q16n16 freq1 = (Q16n16) pgm_read_dword(midiToFreq + index);
	Q16n16 freq2 = (Q16n16) pgm_read_dword((midiToFreq + 1) + index);
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

/** @ingroup midi
A good choice if you're using whole note values, want speed and simplicity, and accuracy isn't important.
@param midi_note a midi note number.
@return an integer approximation of the midi note's frequency.
*/
int mtof(unsigned char midi_note){
	return (int) (pgm_read_dword(midiToFreq + midi_note) >> 16);
}


/** @ingroup midi
A good choice if you're using whole note values, want speed and simplicity, and accuracy isn't important.
@param midi_note a midi note number.
@return an integer approximation of the midi note's frequency.
*/
int mtof(int midi_note){
	return (int) (pgm_read_dword(midiToFreq + midi_note) >> 16);
}
