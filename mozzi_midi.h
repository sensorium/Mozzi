#ifndef MOZZI_MIDI_H_
#define MOZZI_MIDI_H_

#include "mozzi_fixmath.h"
#include "FixMath.h"

float mtof(float x);
int mtof(uint8_t midi_note);
int mtof(int midi_note);
Q16n16 Q16n16_mtof(Q16n16 midival);

inline UFixMath<16,16> mtof(UFixMath<16,16> midival)
{
  return Q16n16_mtof(midival.asRaw());
}

template<byte NI, byte NF>
  inline UFixMath<16,16> mtof(UFixMath<NI,NF> midival)
{
  return Q16n16_mtof(UFixMath<16,16>(midival).asRaw());
}

template<byte NI, byte NF>
  inline UFixMath<16,16> mtof(SFixMath<NI,NF> midival)
{
  return Q16n16_mtof(UFixMath<16,16>(midival).asRaw());
}

#endif /* MOZZI_MIDI_H_ */
