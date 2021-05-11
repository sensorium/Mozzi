/*
 * MozziTeensy3.cpp
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#include "hardware_defines.h"
#if IS_TEENSY3() && USE_LEGACY_GUTS == false

#include "CircularBuffer.h"
#include "Mozzi.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
#include "AudioOutput.h"
#include "IntervalTimer.h" // required from http://github.com/pedvide/ADC for Teensy 3.*
#include <ADC.h>


#if ( F_CPU != 48000000) 
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino and 48MHz on Teensy 3!  Results may vary with other speeds."
#endif


ADC *adc; // adc object
uint8_t teensy_pin;

#if BYPASS_MOZZI_OUTPUT_BUFFER == true
  uint64_t samples_written_to_buffer = 0;
#else
  // ring buffer for audio output
  #if (STEREO_HACK == true)
  CircularBuffer<StereoOutput> output_buffer;  // fixed size 256
  #else
  CircularBuffer<AudioOutput_t> output_buffer;  // fixed size 256
  #endif
#endif

//-----------------------------------------------------------------------------------------------------------------

////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)

// ring buffer for audio input
CircularBuffer<unsigned int> input_buffer; // fixed size 256
static boolean audio_input_is_available;
static int audio_input; // holds the latest audio from input_buffer
uint8_t adc_count = 0;

int MozziClass::getAudioInput() { 
  return audio_input;
 }

static void startFirstAudioADC() {
  adc->startSingleRead(AUDIO_INPUT_PIN); // ADC lib converts pin/channel in startSingleRead
}

static void startSecondAudioADC() {
  adc->startSingleRead(AUDIO_INPUT_PIN);
}

static void receiveSecondAudioADC() {
  if (!input_buffer.isFull())
    input_buffer.write(adc->readSingle());
}

void adc0_isr(void)
{
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

#endif
////// END AUDIO INPUT code ////////


#if BYPASS_MOZZI_OUTPUT_BUFFER == true
inline void bufferAudioOutput(const AudioOutput_t f) {
  audioOutput(f);
  ++samples_written_to_buffer;
}
#else
#define canBufferAudioOutput() (!output_buffer.isFull())
#define bufferAudioOutput(f) output_buffer.write(f)
#endif

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

void MozziClass::audioHook() // 2us on AVR excluding updateAudio()
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

  }
  // setPin13Low();
}


//-----------------------------------------------------------------------------------------------------------------
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
static void CACHED_FUNCTION_ATTR defaultAudioOutput() {
  #if (USE_AUDIO_INPUT == true)
    adc_count = 0;
    startSecondAudioADC();
  #endif
  audioOutput(output_buffer.read());
}
#endif

#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) 
IntervalTimer timer1;

static void startAudioStandard() {
  adc->setAveraging(0);
  adc->setConversionSpeed(
      ADC_CONVERSION_SPEED::MED_SPEED); // could be HIGH_SPEED, noisier

  analogWriteResolution(12);
  timer1.begin(defaultAudioOutput, 1000000UL / AUDIO_RATE);
}
#endif

//-----------------------------------------------------------------------------------------------------------------

static void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void MozziClass::start(int control_rate_hz) {
  setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST
                   // in setup() if desired (not for Teensy 3.* )
  // delay(200); // so AutoRange doesn't read 0 to start with
  startControl(control_rate_hz);
  #if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS)
    startAudioStandard();
  #elif (AUDIO_MODE == HIFI)
    startAudioHiFi();
  #endif
}

void MozziClass::stop() {
  timer1.end();
  interrupts();
}

unsigned long MozziClass::audioTicks() {
  #if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
    return output_buffer.count();
  #else
    return (samples_written_to_buffer - (output_buffer_size - i2s_available()));
  #endif
}

unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}

#endif  // IS_TEENSY
