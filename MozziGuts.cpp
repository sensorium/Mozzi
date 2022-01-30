/*
 * MozziGuts.cpp
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */
#include <Arduino.h>

#include "CircularBuffer.h"
#include "MozziGuts.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
//#include "mozzi_utils.h"
#include "AudioOutput.h"

// forward-declarations for use in hardware-specific implementations:
static void advanceADCStep();
static uint8_t adc_count = 0;

// forward-declarations; to be supplied by plaform specific implementations
static void startSecondADCReadOnCurrentChannel();

////// BEGIN Output buffering /////
#if BYPASS_MOZZI_OUTPUT_BUFFER == true
uint64_t samples_written_to_buffer = 0;

inline void bufferAudioOutput(const AudioOutput_t f) {
  audioOutput(f);
  ++samples_written_to_buffer;
}
#else
#  if (STEREO_HACK == true)
// ring buffer for audio output
CircularBuffer<StereoOutput> output_buffer;  // fixed size 256
#  else
CircularBuffer<AudioOutput_t> output_buffer;  // fixed size 256
#  endif
#  define canBufferAudioOutput() (!output_buffer.isFull())
#  define bufferAudioOutput(f) output_buffer.write(f)
static void CACHED_FUNCTION_ATTR defaultAudioOutput() {
#  if (USE_AUDIO_INPUT == true)
  adc_count = 0;
  startSecondADCReadOnCurrentChannel();  // the current channel is the AUDIO_INPUT pin
#  endif
  audioOutput(output_buffer.read());
}
#endif
////// END Output buffering ///////


// Include the appropriate implementation
#if IS_AVR()
#  include "MozziGuts_impl_AVR.hpp"
#elif IS_STM32()
#  include "MozziGuts_impl_STM32.hpp"
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
#else
#  error "Platform not (yet) supported. Check MozziGuts_impl_template.hpp and existing implementations for a blueprint for adding your favorite MCU."
#endif


////// BEGIN Analog inpput code ////////
/* Analog input code was informed initially by a discussion between
jRaskell, bobgardner, theusch, Koshchi, and code by jRaskell.
http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&p=789581
*/

#include "Stack.h"
static volatile int analog_readings[NUM_ANALOG_INPUTS];
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
#if (USE_AUDIO_INPUT == true)
		adc_channels_to_read.push(AUDIO_INPUT_PIN); // for audio
#else
		adcReadSelectedChannels();
		adc_count = 0;
#endif
	}
}

int mozziAnalogRead(uint8_t pin) {
#if defined(MOZZI_FAST_ANALOG_IMPLEMENTED)
	pin = adcPinToChannelNum(pin); // allow for channel or pin numbers; on most platforms other than AVR this has no effect. See note on pins/channels
	adc_channels_to_read.push(pin);
	return analog_readings[channelNumToIndex(pin)];
#else
#  warning Asynchronouos analog reads not implemented for this platform
	return analogRead(pin);
#endif
}

#if (USE_AUDIO_INPUT == true)
// ring buffer for audio input
CircularBuffer<unsigned int> input_buffer; // fixed size 256

static int audio_input; // holds the latest audio from input_buffer

int getAudioInput() { return audio_input; }

/** NOTE: Triggered at AUDIO_RATE via defaultAudioOutput(). In addition to the AUDIO_INPUT_PIN, at most one reading is taken for mozziAnalogRead().  */
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
    adcStartConversion(adcPinToChannelNum(AUDIO_INPUT_PIN));  // -> result is ignored, but first thing in the next cycle, a second reading is taken.
    break;

  }
  adc_count++;
}
#else
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

////// END analog input code ////////


////// BEGIN audio/control hook /////
static uint16_t update_control_timeout;
static uint16_t update_control_counter;

inline void advanceControlLoop() {
  if (!update_control_counter) {
    update_control_counter = update_control_timeout;
    updateControl();
    adcStartReadCycle();
  } else {
    --update_control_counter;
  }
}

void audioHook() // 2us on AVR excluding updateAudio()
{
// setPin13High();
#if (USE_AUDIO_INPUT == true)
  if (!input_buffer.isEmpty())
    audio_input = input_buffer.read();
#endif

  if (canBufferAudioOutput()) {
    advanceControlLoop();
#if (STEREO_HACK == true)
    updateAudio(); // in hacked version, this returns void
    bufferAudioOutput(StereoOutput(audio_out_1, audio_out_2));
#else
    bufferAudioOutput(updateAudio());
#endif

#if defined(LOOP_YIELD)
    LOOP_YIELD
#endif
  }
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
  setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST
                   // in setup() if desired (not for Teensy 3.* )
  setupFastAnalogRead();
  // delay(200); // so AutoRange doesn't read 0 to start with
  update_control_timeout = AUDIO_RATE / control_rate_hz;
  startAudio();
}

////// END initialization ///////
