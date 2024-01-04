// This file, too, will have to include the Mozzi headers, but in order to avoid "multiple definition" errors,
// all secondary .cpp files in the sketch must set MOZZI_HEADER_ONLY before including any Mozzi headers.
// TODO: Maybe that is not even the final word, and this will be required in the end, but it seems a useful intermediate step
#define MOZZI_HEADER_ONLY
#include <MozziGuts.h>

AudioOutput_t updateAudio() {
  return MonoOutput::from8Bit(0);  // just a dummy
}
