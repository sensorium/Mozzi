#ifndef MOZZI_MIDI_H_
#define MOZZI_MIDI_H_

#include "mozzi_fixmath.h"

float mtof(float x);
int mtof(unsigned char midi_note);
int mtof(int midi_note);
Q16n16 Q16n16_mtof(Q16n16 midival);

#endif /* MOZZI_MIDI_H_ */
