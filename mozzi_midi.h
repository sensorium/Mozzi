/*
 * mozzi_midi.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

#ifndef MOZZI_MIDI_H_
#define MOZZI_MIDI_H_

#include "mozzi_fixmath.h"
#include "FixMath.h"

#include "mozzi_pgmspace.h"

/**  @brief Internal. Do not use in your sketches.

Internal helper class. Not intended for use in your sketches, and details may change without notic. */ 
class MidiToFreqPrivate {
private:
  friend int mtof(uint8_t);
  friend int mtof(int);
  friend Q16n16 Q16n16_mtof(Q16n16);
  template<int8_t NI, uint64_t RANGE>
  friend UFix<16,16> mtof(UFix<NI,0,RANGE>);

  template<int8_t NI, uint64_t RANGE>
  friend UFix<16,16> mtof(SFix<NI,0,RANGE>);
  
  static CONSTTABLE_STORAGE(uint32_t) midiToFreq[128];
};


CONSTTABLE_STORAGE(uint32_t) MidiToFreqPrivate::midiToFreq[128] =
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


/** @defgroup midi Midi note number to frequency conversions

Useful if you like playing notes in tune.
*/

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
inline float mtof(float midival)
{
	 // http://en.wikipedia.org/wiki/Note
	 // f = pow(2,(p-69/12) * 440Hz
	 // return pow(2.0,(midival-69.0/12.0) * 440.0;
	 
	// code from AF_precision_synthesis sketch, copyright 2009, Adrian Freed.
	float f = 0.0;
	if(midival) f = 8.1757989156 * pow(2.0, midival/12.0);
	return f;
};


/** @ingroup midi
A good choice if you're using whole note values, want speed and simplicity, and accuracy isn't important.
@param midi_note a midi note number.
@return an integer approximation of the midi note's frequency.
*/
inline int mtof(uint8_t midi_note){
  return (FLASH_OR_RAM_READ<const uint32_t>(MidiToFreqPrivate::midiToFreq + midi_note) >> 16);
};

/** @ingroup midi
A good choice if you're using whole note values, want speed and simplicity, and accuracy isn't important.
@param midi_note a midi note number.
@return an integer approximation of the midi note's frequency.
*/
inline int mtof(int midi_note){
	return (FLASH_OR_RAM_READ<const uint32_t>(MidiToFreqPrivate::midiToFreq + midi_note) >> 16);
};


/** @ingroup midi
Converts midi note number to frequency with speed and accuracy.  Q16n16_mtofLookup() is a fast
alternative to (float) mtof(), and more accurate than (uint8_t) mtof(),
using Q16n16 fixed-point format instead of floats or uint8_t values. Q16n16_mtof()
uses cheap linear interpolation between whole midi-note frequency equivalents
stored in a lookup table, so is less accurate than the float version, mtof(),
for non-whole midi values.
@note Timing: ~8 us.
@param midival_fractional a midi note number in Q16n16 format, for fractional values.
@return the frequency represented by the input midi note number, in Q16n16
fixed point fractional integer format, where the lower word is a fractional value.
*/
inline Q16n16  Q16n16_mtof(Q16n16 midival_fractional)
{
	Q16n16 diff_fraction;
	uint8_t index = midival_fractional >> 16;
	uint16_t fraction = (uint16_t) midival_fractional; // keeps low word
	Q16n16 freq1 = (Q16n16) FLASH_OR_RAM_READ<const uint32_t>(MidiToFreqPrivate::midiToFreq + index);
	Q16n16 freq2 = (Q16n16) FLASH_OR_RAM_READ<const uint32_t>((MidiToFreqPrivate::midiToFreq + 1) + index);
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
};

/** @ingroup midi
Converts midi note number with speed and accuracy from a UFix<16,16>.
Uses Q16n16_mtof internally.
*/
template <uint64_t RANGE>
inline UFix<16,16> mtof(UFix<16,16,RANGE> midival)
{
  return UFix<16,16>::fromRaw(Q16n16_mtof(midival.asRaw()));
};

/** @ingroup midi
Converts midi note number with speed and accuracy from any UFix.
Uses Q16n16_mtof internally.
*/
template<int8_t NI, int8_t NF, uint64_t RANGE>
inline UFix<16,16> mtof(UFix<NI,NF,RANGE> midival)
{
  return UFix<16,16>::fromRaw(Q16n16_mtof(UFix<16,16>(midival).asRaw()));
};

/** @ingroup midi
Converts midi note number with speed and accuracy from any SFix.
Uses Q16n16_mtof internally.
*/
template<int8_t NI, int8_t NF, uint64_t RANGE>
inline UFix<16,16> mtof(SFix<NI,NF,RANGE> midival)
{
  return UFix<16,16>::fromRaw(Q16n16_mtof(UFix<16,16>(midival).asRaw()));
};

/** @ingroup midi
Converts *whole* midi note number with speed and accuracy (more accurate that mtof(uint8_t))
*/
template<int8_t NI, uint64_t RANGE>
inline UFix<16,16> mtof(UFix<NI,0,RANGE> midival)
{
  return UFix<16,16>::fromRaw((FLASH_OR_RAM_READ<const uint32_t>(MidiToFreqPrivate::midiToFreq + midival.asRaw())));
};

/** @ingroup midi
Converts *whole* midi note number with speed and accuracy (more accurate that mtof(uint8_t))
*/
template<int8_t NI, uint64_t RANGE>
inline UFix<16,16> mtof(SFix<NI,0,RANGE> midival)
{
  return UFix<16,16>::fromRaw((FLASH_OR_RAM_READ<const uint32_t>(MidiToFreqPrivate::midiToFreq + midival.asUFix().asRaw())));
};

#endif /* MOZZI_MIDI_H_ */
