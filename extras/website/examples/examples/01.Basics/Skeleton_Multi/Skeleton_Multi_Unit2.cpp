#include <MozziHeadersOnly.h>        // <Mozzi.h> should be included only once in the whole program. Sketches needing
                                     // core Mozzi functions in more than one .cpp file, shall include MozziHeadersOnly.h
                                     // in all but one.

AudioOutput_t updateAudio() {
  return MonoOutput::from8Bit(0);  // just a dummy
}
