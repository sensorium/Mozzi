#include <MozziGuts.h>        // This file, too, will have to include the Mozzi headers.

AudioOutput_t updateAudio() {
  return MonoOutput::from8Bit(0);  // just a dummy
}
