#ifndef MOZZI_MIDI_H_
#define MOZZI_MIDI_H_

#include "mozzi_fixmath.h"
#include "mozzi_pgmspace.h"



class MidiToFreqPrivate {
private:
  friend int mtof(uint8_t);
  friend int mtof(int);
  friend Q16n16  Q16n16_mtof(Q16n16);
  static CONSTTABLE_STORAGE(uint32_t) midiToFreq[128];
};






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

#endif /* MOZZI_MIDI_H_ */
