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

#include "CircularBuffer.h"
#include "MozziGuts.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
//#include "mozzi_utils.h"
#include "AudioOutput.h"

// forward-declarations; implemenation is platform specific
static void startAudio();
void stopMozzi();

////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)

// ring buffer for audio input
CircularBuffer<unsigned int> input_buffer; // fixed size 256

static int audio_input; // holds the latest audio from input_buffer
uint8_t adc_count = 0;

int getAudioInput() { return audio_input; }

// forward-declarations; implemenation is platform specific
static void startFirstAudioADC();
/*static void receiveFirstAudioADC();  // nothing */
static void startSecondAudioADC();
static void receiveSecondAudioADC();

inline void advanceADCStep() {
  switch (adc_count) {
  case 0:
    // 6us
    receiveSecondAudioADC();
    adcReadSelectedChannels();
    break;

  case 1:
    // <2us, <1us w/o receive
    // receiveFirstControlADC();
    startSecondControlADC();
    break;

  case 2:
    // 3us
    receiveSecondControlADC();
    startFirstAudioADC();
    break;

    //      	case 3:
    // invisible
    //      	receiveFirstAudioADC();
    //      	break;
  }
  adc_count++;
}
////// END AUDIO INPUT code ////////

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
  startSecondAudioADC();
#  endif
  audioOutput(output_buffer.read());
}
#endif
////// END Output buffering ///////

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

#if IS_ESP8266()
    yield();
#endif
  }
  // setPin13Low();
}

// 
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
  // delay(200); // so AutoRange doesn't read 0 to start with
  update_control_timeout = AUDIO_RATE / control_rate_hz;
  startAudio();
}

////// END initialization ///////

#if IS_AVR()
#  include "MozziGuts_impl_AVR.hpp"
#elif IS_STM32()
#  include "MozziGuts_impl_STM32.hpp"
#endif
