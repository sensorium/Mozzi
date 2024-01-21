/* Configuration example

Configure Mozzi for "external" audio output. You will need to provide an audioOutput() function in your sketch.

See TODO: link to relevant tutorial
*/

#include "MozziConfigValues.h"  // for named option values

#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_TIMED
// or use this:
//#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_CUSTOM

//#define MOZZI_AUDIO_RATE 32768  // the default, in this mode
//#define MOZZI_AUDIO_BITS 16 // the default in this mode, but e.g. when connecting to a 24-bit DAC, you'd set 24, here.
