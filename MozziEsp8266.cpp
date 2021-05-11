/*
 * MozziESO8266.cpp
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
#if IS_ESP8266() && USE_LEGACY_GUTS == false

#include "CircularBuffer.h"
#include "Mozzi.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
#include "AudioOutput.h"
#include <uart.h>
#include <i2s.h>

uint16_t output_buffer_size = 0;


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



#if ((ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL) && (EXTERNAL_AUDIO_OUTPUT != true))
void CACHED_FUNCTION_ATTR esp8266_serial_audio_output() {
  // Note: That unreadble mess is an optimized version of Serial1.availableForWrite()
  while ((UART_TX_FIFO_SIZE - ((U1S >> USTXC) & 0xff)) > (PDM_RESOLUTION * 4)) {
    audioOutput(output_buffer.read());
  }
}
#endif

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

    yield();
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

#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) || IS_STM32



static void startAudioStandard() {

#if   IS_ESP8266 && (EXTERNAL_AUDIO_OUTPUT == true)  && (BYPASS_MOZZI_OUTPUT_BUFFER != true) // for external audio output, set up a timer running a audio rate
  timer1_isr_init();
  timer1_attachInterrupt(defaultAudioOutput);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
  timer1_write(F_CPU / AUDIO_RATE);
#else
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
  Serial1.begin(
      AUDIO_RATE * (PDM_RESOLUTION * 40), SERIAL_8N1,
      SERIAL_TX_ONLY); // Note: PDM_RESOLUTION corresponds to packets of 32
                       // encoded bits  However, the UART (unfortunately) adds a
                       // start and stop bit each around each byte, thus sending
                       // a total to 40 bits per audio sample per
                       // PDM_RESOLUTION.
  // set up a timer to copy from Mozzi output_buffer into Serial TX buffer
  timer1_isr_init();
  timer1_attachInterrupt(esp8266_serial_audio_output);
  // UART FIFO buffer size is 128 bytes. To be on the safe side, we keep the
  // interval to the time needed to write half of that. PDM_RESOLUTION * 4 bytes
  // per sample written.
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(F_CPU / (AUDIO_RATE * PDM_RESOLUTION));
#else
  i2s_begin();
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S)
  pinMode(2, INPUT); // Set the two unneeded I2S pins to input mode, to reduce
                     // side effects
  pinMode(15, INPUT);
#endif
  i2s_set_rate(AUDIO_RATE * PDM_RESOLUTION);
  if (output_buffer_size == 0)
    output_buffer_size =
        i2s_available(); // Do not reset count when stopping / restarting
#endif
#endif
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
#if (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL)
  i2s_end();
#else
  timer1_disable();
#endif
  // ps - nointerrupts was never called so the following is not necessary: 
  // interrupts();
}

unsigned long MozziClass::audioTicks() {
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
  return output_buffer.count();
#elif ( (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S) && (PDM_RESOLUTION != 1))
  return (samples_written_to_buffer -
          ((output_buffer_size - i2s_available()) / PDM_RESOLUTION));
#else
  return (samples_written_to_buffer - (output_buffer_size - i2s_available()));
#endif
}

unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}

#endif
