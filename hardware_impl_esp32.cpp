/*
 * MozziESP32.cpp
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
#if IS_ESP32() && USE_LEGACY_GUTS == false
#include "CircularBuffer.h"
#include "Mozzi.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
#include "AudioOutput.h"
#include <driver/i2s.h>   // for I2S-based output modes
#include <driver/timer.h> // for EXTERNAL_AUDIO_OUTPUT


uint64_t samples_written_to_buffer = 0;

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
  adcStartConversion(adcPinToChannelNum(AUDIO_INPUT_PIN));
}

// ps - I think this does not make any sense
// static void startSecondAudioADC() {
//   ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
// }

// static void receiveSecondAudioADC() {
//   if (!input_buffer.isFull())
//     input_buffer.write(ADC);
// }

#endif


inline void bufferAudioOutput(const AudioOutput_t f) {
  audioOutput(f);
  ++samples_written_to_buffer;
}

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
    if (!input_buffer.isEmpty()) {
      audio_input = input_buffer.read();
    }
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
}

#if (ESP32_AUDIO_OUT_MODE == PT8211_DAC) || (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)

static void startAudioStandard() {
    static const i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = AUDIO_RATE * PDM_RESOLUTION,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  // only the top 8 bits will actually be used by the internal DAC, but using 8 bits straight away seems buggy
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,  // always use stereo output. mono seems to be buggy, and the overhead is insignifcant on the ESP32
      .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB),  // this appears to be the correct setting for internal DAC and PT8211, but not for other dacs
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,    // 8*128 bytes of buffer corresponds to 256 samples (2 channels, see above, 2 bytes per sample per channel)
      .dma_buf_len = 128,
      .use_apll = false
    };

    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    static const i2s_pin_config_t pin_config = {
      .bck_io_num = ESP32_I2S_BCK_PIN,
      .ws_io_num = ESP32_I2S_WS_PIN,
      .data_out_num = ESP32_I2S_DATA_PIN,
      .data_in_num = -1
    };
    i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
    i2s_zero_dma_buffer((i2s_port_t)i2s_num);
}

#elif (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)

static void startAudioStandard() {
    static const i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
      .sample_rate = AUDIO_RATE * PDM_RESOLUTION,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  // only the top 8 bits will actually be used by the internal DAC, but using 8 bits straight away seems buggy
      .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,  // always use stereo output. mono seems to be buggy, and the overhead is insignifcant on the ESP32
      .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB),  // this appears to be the correct setting for internal DAC and PT8211, but not for other dacs
      .intr_alloc_flags = 0, // default interrupt priority
      .dma_buf_count = 8,    // 8*128 bytes of buffer corresponds to 256 samples (2 channels, see above, 2 bytes per sample per channel)
      .dma_buf_len = 128,
      .use_apll = false
    };

    i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin((i2s_port_t)i2s_num, NULL);
    i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
    i2s_zero_dma_buffer((i2s_port_t)i2s_num);
}
#endif


static void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void MozziClass::start(int control_rate_hz) {
  setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST
                   // in setup() if desired (not for Teensy 3.* )
                   // delay(200); // so AutoRange doesn't read 0 to start with
  startControl(control_rate_hz);
  startAudioStandard();
}

void MozziClass::stop() {
  // ps - added missing i2s_driver_uninstall
  i2s_driver_uninstall((i2s_port_t)i2s_num);   
  // ps - nointerrupts was never called so the following is not necessary: 
  // interrupts();
}

unsigned long MozziClass::audioTicks() {
  return samples_written_to_buffer;
}

unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK; 
}


#endif
