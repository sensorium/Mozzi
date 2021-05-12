/*
 * MozziSamd21.cpp
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
#if IS_SAMD21() && USE_LEGACY_GUTS == false

#include "CircularBuffer.h"
#include "Mozzi.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
#include "AudioOutput.h"


#if BYPASS_MOZZI_OUTPUT_BUFFER == true
uint64_t samples_written_to_buffer = 0;
#else
//-----------------------------------------------------------------------------------------------------------------
// ring buffer for audio output
CircularBuffer<OUTPUT_TYPE> output_buffer;  // fixed size 256
//-----------------------------------------------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------------------------------------------

////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)

// ring buffer for audio input
CircularBuffer<unsigned int> input_buffer; // fixed size 256

static boolean audio_input_is_available;
static int audio_input; // holds the latest audio from input_buffer
uint8_t adc_count = 0;

int MozziClass::getAudioInput() { return audio_input; }

static void startFirstAudioADC() {
  adcStartConversion(adcPinToChannelNum(AUDIO_INPUT_PIN));
}

/*
static void receiveFirstAudioADC()
{
        // nothing
}
*/

static void startSecondAudioADC() {
  ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
}

static void receiveSecondAudioADC() {
  if (!input_buffer.isFull())
    input_buffer.write(ADC);
}

#endif
////// END AUDIO INPUT code ////////


// These are ARM SAMD21 Timer 5 routines to establish a sample rate interrupt
static bool tcIsSyncing() {
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

static void tcReset() {
  // Reset TCx
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing())
    ;
  while (TC5->COUNT16.CTRLA.bit.SWRST)
    ;
}
/* Not currently used, and does not compile with EXTERNAL_AUDIO_OUTPUT
static void tcEnd() {
  // Disable TC5
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing())
    ;
  tcReset();
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
} */

static void tcConfigure(uint32_t sampleRate) {
  // Enable GCLK for TCC2 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                                 GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;

  tcReset();

  // Set Timer counter Mode to 16 bits
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

  // Set TC5 mode as match frequency
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

  TC5->COUNT16.CC[0].reg = (uint16_t)(SystemCoreClock / sampleRate - 1);
  while (tcIsSyncing())
    ;

  // Configure interrupt request
  NVIC_DisableIRQ(TC5_IRQn);
  NVIC_ClearPendingIRQ(TC5_IRQn);
  NVIC_SetPriority(TC5_IRQn, 0);
  NVIC_EnableIRQ(TC5_IRQn);

  // Enable the TC5 interrupt request
  TC5->COUNT16.INTENSET.bit.MC0 = 1;
  while (tcIsSyncing())
    ;
}


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

void TC5_Handler(void) __attribute__((weak, alias("samd21AudioOutput")));

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

#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) || IS_STM32


#ifdef __cplusplus
extern "C" {
#endif
void samd21AudioOutput() {
  defaultAudioOutput();
  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}
#ifdef __cplusplus
}
#endif

static void startAudioStandard() {

#ifdef ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS
  {
    static const int CPLAY_SPEAKER_SHUTDOWN = 11;
    pinMode(CPLAY_SPEAKER_SHUTDOWN, OUTPUT);
    digitalWrite(CPLAY_SPEAKER_SHUTDOWN, HIGH);
  }

#endif
  analogWriteResolution(10);
#if (EXTERNAL_AUDIO_OUTPUT != true)
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
#endif
  tcConfigure(AUDIO_RATE);

}

// end STANDARD

//-----------------------------------------------------------------------------------------------------------------
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
#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) ||               \
    IS_STM32 // Sorry, this is really hacky. But on STM32 regular and HIFI
               // audio modes are so similar to set up, that we do it all in one
               // function.
  startAudioStandard();
#elif (AUDIO_MODE == HIFI)
  startAudioHiFi();
#endif
}

void MozziClass::stop() {
  // ps - nointerrupts was never called so the following is not necessary: 
  // interrupts();
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

#endif
