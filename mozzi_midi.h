#ifndef MOZZI_MIDI_H_
#define MOZZI_MIDI_H_

#include "mozzi_fixmath.h"

/** @defgroup midi Midi note number to frequency conversions
Useful if you like playing notes in tune.
*/

float mtof(float x);
unsigned int mtof(unsigned char midi_note);
unsigned int mtof(int midi_note);
Q16n16 Q16n16_mtof(Q16n16 midival);

#endif /* MOZZI_MIDI_H_ */
