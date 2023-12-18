#ifndef MOZZI_MIDI_H_
#define MOZZI_MIDI_H_

#include "mozzi_fixmath.h"
#include "FixMath.h"

float mtof(float x);
int mtof(uint8_t midi_note);
int mtof(int midi_note);
Q16n16 Q16n16_mtof(Q16n16 midival);
UFixMath<16,16> mtof(UFixMath<16,16> midival);

template<byte NI, byte NF>
  UFixMath<16,16> mtof(UFixMath<NI,NF> midival);

#endif /* MOZZI_MIDI_H_ */
