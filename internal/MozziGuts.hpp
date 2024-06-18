/*
 * MozziGuts.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#include <Arduino.h>

#include "CircularBuffer.h"
#include "mozzi_analog.h"
#include "internal/mozzi_rand_p.h"
#include "AudioOutput.h"

/** @brief Internal. Do not use function in this namespace in your sketch!

This namespace contains various functions that are used by Mozzi, internally, but are not meant to be used in a sketch.

The details of these may change without warning. I repeat: Do not use these in your sketch!
*/
namespace MozziPrivate {

// Forward declarations of functions to be provided by platform specific implementations
#if (!BYPASS_MOZZI_OUTPUT_BUFFER)
static void CACHED_FUNCTION_ATTR defaultAudioOutput();
#endif
#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
static void advanceADCStep();                       // to be provided by platform implementation
static void startSecondADCReadOnCurrentChannel();   // to be provided by platform implementation
static uint8_t adc_count = 0;                       // needed below
#endif

// TODO: make this helper public?
template<byte BITS_IN, byte BITS_OUT, typename T> constexpr T smartShift(T value) {
    return (BITS_IN > BITS_OUT) ? value >> (BITS_IN - BITS_OUT) : (BITS_IN < BITS_OUT) ? value << (BITS_OUT - BITS_IN) : value;
}
}

// Include the appropriate implementation
#if IS_AVR()
#  include "MozziGuts_impl_AVR.hpp"
#elif IS_STM32MAPLE()
#  include "MozziGuts_impl_STM32.hpp"
#elif IS_STM32DUINO()
#  include "MozziGuts_impl_STM32duino.hpp"
#elif IS_ESP32()
#  include "MozziGuts_impl_ESP32.hpp"
#elif IS_ESP8266()
#  include "MozziGuts_impl_ESP8266.hpp"
#elif (IS_TEENSY3() || IS_TEENSY4())
#  include "MozziGuts_impl_TEENSY.hpp"
#elif (IS_SAMD21())
#  include "MozziGuts_impl_SAMD.hpp"
#elif (IS_RP2040())
#  include "MozziGuts_impl_RP2040.hpp"
#elif (IS_MBED())
#  include "MozziGuts_impl_MBED.hpp"
#elif (IS_RENESAS())
#  include "MozziGuts_impl_RENESAS.hpp"
#else
#  error "Platform not (yet) supported. Check MozziGuts_impl_template.hpp and existing implementations for a blueprint for adding your favorite MCU."
#endif

/* Retro-compatibility with "legacy" boards which use the async
   ADC for getting AUDIO_INPUT
*/
#if !defined(MOZZI__LEGACY_AUDIO_INPUT_IMPL)
#  if !MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE)
#    define MOZZI__LEGACY_AUDIO_INPUT_IMPL 1
#  else
#    define MOZZI__LEGACY_AUDIO_INPUT_IMPL 0
#  endif
#endif

namespace MozziPrivate {
////// BEGIN Output buffering /////
#if BYPASS_MOZZI_OUTPUT_BUFFER == true
uint64_t samples_written_to_buffer = 0;

inline void bufferAudioOutput(const AudioOutput f) {
  audioOutput(f);
  ++samples_written_to_buffer;
}
#else
CircularBuffer<AudioOutput> output_buffer;  // fixed size 256
#  define canBufferAudioOutput() (!output_buffer.isFull())
#  define bufferAudioOutput(f) output_buffer.write(f)
static void CACHED_FUNCTION_ATTR defaultAudioOutput() {

#if MOZZI_IS(MOZZI__LEGACY_AUDIO_INPUT_IMPL, 1) // in that case, we rely on asynchroneous ADC reads implemented for mozziAnalogRead to get the audio in samples
  MOZZI_ASSERT_NOTEQUAL(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE);
  adc_count = 0;
  startSecondADCReadOnCurrentChannel();  // the current channel is the AUDIO_INPUT pin
#  endif
  audioOutput(output_buffer.read());
}
#endif  // #if (AUDIO_INPUT_MODE == AUDIO_INPUT_LEGACY)
////// END Output buffering ///////


////// BEGIN Analog input code ////////
/* Analog input code was informed initially by a discussion between
jRaskell, bobgardner, theusch, Koshchi, and code by jRaskell.
http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=789581
*/

#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)

#include "Stack.h"
static volatile uint16_t analog_readings[NUM_ANALOG_INPUTS];
static Stack <volatile int8_t,NUM_ANALOG_INPUTS> adc_channels_to_read;
volatile static int8_t current_channel = -1; // volatile because accessed in control and adc ISRs

/* gets the next channel to read off the stack, and if there is a channel there, it changes to that channel and starts a conversion.
*/
void adcReadSelectedChannels() {
	// ugly syntax below saves a few bytes/instructions (as current_channel is declared volatile)
	if((current_channel = adc_channels_to_read.pop()) >= 0) adcStartConversion(current_channel);
}

/* Called each time in updateControlWithAutoADC(), after updateControl()
   Forbidding inline, here, saves a wholesome 16 bytes flash on AVR (without USE_AUDIO_INPUT). No idea, why.
*/
__attribute__((noinline)) void adcStartReadCycle() {
	if (current_channel < 0) // last read of adc_channels_to_read stack was empty, ie. all channels from last time have been read
	{
#if MOZZI_IS(MOZZI__LEGACY_AUDIO_INPUT_IMPL, 1) // use of async ADC for audio input
		adc_channels_to_read.push(MOZZI_AUDIO_INPUT_PIN); // for audio
#else
		adcReadSelectedChannels();
		adc_count = 0;
#endif
	}
}

uint16_t mozziAnalogRead(uint8_t pin) {
	pin = adcPinToChannelNum(pin); // allow for channel or pin numbers; on most platforms other than AVR this has no effect. See note on pins/channels
	adc_channels_to_read.push(pin);
	return analog_readings[channelNumToIndex(pin)];
}

#if !MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE)
static uint16_t audio_input; // holds the latest audio from input_buffer
uint16_t getAudioInput() { return audio_input; }
#endif

#if MOZZI_IS(MOZZI__LEGACY_AUDIO_INPUT_IMPL, 1)
// ring buffer for audio input
CircularBuffer<uint16_t> input_buffer; // fixed size 256
#define audioInputAvailable() (!input_buffer.isEmpty())
#define readAudioInput() (input_buffer.read())
/** NOTE: Triggered at MOZZI_AUDIO_RATE via defaultAudioOutput(). In addition to the MOZZI_AUDIO_INPUT_PIN, at most one reading is taken for mozziAnalogRead().  */
inline void advanceADCStep() {
  switch (adc_count) {
  case 0:
    // 6us
    // the input pin was the last thing we read 
    input_buffer.write(getADCReading());
    adcReadSelectedChannels();     // TODO: doesn't this stop the cycle, in case no pins to read?
    break;

  case 1:
    // <2us, <1us w/o receive
    // receiveFirstControlADC();
    startSecondADCReadOnCurrentChannel();
    break;

  case 2:
    // 3us
    analog_readings[channelNumToIndex(current_channel)] = getADCReading();
    adcStartConversion(adcPinToChannelNum(MOZZI_AUDIO_INPUT_PIN));  // -> result is ignored, but first thing in the next cycle, a second reading is taken.
    break;

  }
  adc_count++;
}
#else  // no (legacy) audio input
/** NOTE: Triggered at CONTROL_RATE via advanceControlLoop().

This interrupt handler cycles through all analog inputs on the adc_channels_to_read Stack,
doing 2 conversions on each channel but only keeping the second conversion each time,
because the first conversion after changing channels is often inaccurate (on atmel-based arduinos).*/
inline void advanceADCStep() {
  if (!adc_count) {  // i.e. first step
    //<1us
    startSecondADCReadOnCurrentChannel();   // discard fist - noisy - reading, start another on same pin
    adc_count=1;
  } else {
    // 3us
    analog_readings[channelNumToIndex(current_channel)] = getADCReading();  // register second reading
    adcReadSelectedChannels();  // start first reading on next pin (if any)
    adc_count=0;
  }
}
#endif

#else
MOZZI_ASSERT_EQUAL(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)

uint16_t mozziAnalogRead(uint8_t pin) {
  return analogRead(pin);
}

#endif  // MOZZI_ANALOG_READ

////// END analog input code ////////


////// BEGIN audio/control hook /////
static uint16_t update_control_timeout;
static uint16_t update_control_counter;

inline void advanceControlLoop() {
  if (!update_control_counter) {
    update_control_counter = update_control_timeout;
    updateControl();
#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
    adcStartReadCycle();
#endif
  } else {
    --update_control_counter;
  }
}

void audioHook() // 2us on AVR excluding updateAudio()
{
// setPin13High();
  if (canBufferAudioOutput()) {
    advanceControlLoop();
    bufferAudioOutput(updateAudio());

#if defined(LOOP_YIELD)
    LOOP_YIELD
#endif

#if !MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE)
    if (audioInputAvailable()) audio_input = readAudioInput(); 
#endif
  }
// Like LOOP_YIELD, but running every cycle of audioHook(), not just once per sample
#if defined(AUDIO_HOOK_HOOK)
    AUDIO_HOOK_HOOK
#endif
  // setPin13Low();
}

// NOTE: This function counts the ticks of audio _output_, corresponding to real time elapsed.
// It does _not_ provide the count of the current audio frame to be generated by updateAudio(). These two things will differ, slightly,
// depending on the fill state of the buffer.
// TODO: In many - but not all - use cases, it might be more useful to provide a count of the current audio frame to be generated, however,
// the existing semantics have always been in place, so far.
unsigned long audioTicks() {
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
  return output_buffer.count();
#elif defined(AUDIOTICK_ADJUSTMENT)
  return samples_written_to_buffer - (AUDIOTICK_ADJUSTMENT);
#else
  return samples_written_to_buffer;
#endif
}

unsigned long mozziMicros() { return audioTicks() * MICROS_PER_AUDIO_TICK; }

////// END audio/control hook /////

////// BEGIN initialization ///////
void startMozzi(int control_rate_hz) {
#if !MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_NONE)
  MozziPrivate::setupMozziADC(FAST_ADC); // you can use setupFastAnalogRead() with FASTER_ADC or FASTEST_ADC
                   // in setup() if desired (not for Teensy 3.* )
#endif
  // delay(200); // so AutoRange doesn't read 0 to start with
  update_control_timeout = MOZZI_AUDIO_RATE / control_rate_hz - 1;
  startAudio();
}

uint32_t MozziRandPrivate::x=132456789;
uint32_t MozziRandPrivate::y=362436069;
uint32_t MozziRandPrivate::z=521288629;

////// END initialization ///////
}

// reduce Macro leakage
#undef LOOP_YIELD
#undef BYPASS_MOZZI_OUTPUT_BUFFER
#undef AUDIO_HOOK_HOOK
#undef AUDIOTICK_ADJUSTMENT
#undef MOZZI__LEGACY_AUDIO_INPUT_IMPL

// "export" publicly accessible functions defined in this file
// NOTE: unfortunately, we cannot just write "using MozziPrivate::mozziMicros()", etc. as that would conflict with, rather than define mozziMicros().
//       Instead, for now, we forward the global-scope functions to their implementations inside MozziPrivate.
//       We might want to rethink how this is done. What matters is that these functions are user accessible, though, while most of what we
//       now keep in MozziPrivate is hidden away.
unsigned long mozziMicros() { return MozziPrivate::mozziMicros(); };
unsigned long audioTicks() { return MozziPrivate::audioTicks(); };
void startMozzi(int control_rate_hz) { MozziPrivate::startMozzi(control_rate_hz); };
void stopMozzi() { MozziPrivate::stopMozzi(); };
template<byte RES> uint16_t mozziAnalogRead(uint8_t pin) { return MozziPrivate::smartShift<MOZZI__INTERNAL_ANALOG_READ_RESOLUTION, RES>(MozziPrivate::mozziAnalogRead(pin));};
#if !MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_NONE)
template<byte RES> uint16_t getAudioInput() { return MozziPrivate::smartShift<MOZZI__INTERNAL_ANALOG_READ_RESOLUTION, RES>(MozziPrivate::getAudioInput()); };
#endif
#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
void setupMozziADC(int8_t speed) { MozziPrivate::setupMozziADC(speed); };
void setupFastAnalogRead(int8_t speed) { MozziPrivate::setupFastAnalogRead(speed); };
uint8_t adcPinToChannelNum(uint8_t pin) { return MozziPrivate::adcPinToChannelNum(pin); };
#endif
void audioHook() { MozziPrivate::audioHook(); };

// This is not strictly needed, but we want it to throw an error, if users have audioOutput() in their sketch without external output configured
#if !MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
MOZZI_DEPRECATED("n/a", "Sketch has audioOutput() function, although external output is not configured.") void audioOutput(const AudioOutput) {};
#endif
#if !MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
// TODO: This won't work without a rename:
//MOZZI_DEPRECATED("n/a", "Sketch has canBufferAudioOutput() function, although custom external output is not configured.") bool canBufferAudioOutput() {};
#endif
