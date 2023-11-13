/** For Mozzi-internal use: Apply hardware specific config defaults and config checks: AVR */

// Step 1: Apply missing defaults
// NOTE: This step is actually required for all ports

#if not defined(MOZZI_AUDIO_MODE)
#define MOZZI_OUTPUT_PWM
#endif

#if not defined(MOZZI_AUDIO_RATE)
#define 16384
#endif

#if not defined(MOZZI_ANALOG_READ)
#define MOZZI_ANALOG_READ MOZZI_ANALOG_READS_STANDARD
#endif

/// TODO much more

// Step 2: Check
// NOTE: This step is not technically required, but a good idea in any port

// TODO much more

