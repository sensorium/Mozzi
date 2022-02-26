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

#if !(IS_ESP32())
#  error "Wrong implementation included for this platform"
#endif

////// BEGIN analog input code ////////
//#define MOZZI_FAST_ANALOG_IMPLEMENTED // not yet
#define getADCReading() 0
#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}
void adcStartConversion(uint8_t channel) {
#warning Fast analog read not implemented on this platform
}
void startSecondADCReadOnCurrentChannel() {
#warning Fast analog read not implemented on this platform
}
void setupFastAnalogRead(int8_t speed) {
#warning Fast analog read not implemented on this platform
}
void setupMozziADC(int8_t speed) {
#warning Fast analog read not implemented on this platform
}
////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////
#include <driver/i2s.h>   // for I2S-based output modes
#include <driver/timer.h> // for EXTERNAL_AUDIO_OUTPUT

#if (EXTERNAL_AUDIO_OUTPUT != true)
#  include "AudioConfigESP32.h"
// On ESP32 we cannot test wether the DMA buffer has room. Instead, we have to use a one-sample mini buffer. In each iteration we
// _try_ to write that sample to the DMA buffer, and if successful, we can buffer the next sample. Somewhat cumbersome, but works.
// TODO: Should ESP32 gain an implemenation of i2s_available(), we should switch to using that, instead.
static bool _esp32_can_buffer_next = true;
#  if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
static uint16_t _esp32_prev_sample[2];
#    define ESP_SAMPLE_SIZE (2*sizeof(uint16_t))
#  elif (ESP32_AUDIO_OUT_MODE == PT8211_DAC)
static int16_t _esp32_prev_sample[2];
#    define ESP_SAMPLE_SIZE (2*sizeof(int16_t))
#  elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
static uint32_t _esp32_prev_sample[PDM_RESOLUTION];
#    define ESP_SAMPLE_SIZE (PDM_RESOLUTION*sizeof(uint32_t))
#  endif

inline bool esp32_tryWriteSample() {
  size_t bytes_written;
  i2s_write(i2s_num, &_esp32_prev_sample, ESP_SAMPLE_SIZE, &bytes_written, 0);
  return (bytes_written != 0);
}

inline bool canBufferAudioOutput() {
  if (_esp32_can_buffer_next) return true;
  _esp32_can_buffer_next = esp32_tryWriteSample();
  return _esp32_can_buffer_next;
}

inline void audioOutput(const AudioOutput f) {
#  if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
  _esp32_prev_sample[0] = (f.l() + AUDIO_BIAS) << 8;
#    if (AUDIO_CHANNELS > 1)
  _esp32_prev_sample[1] = (f.r() + AUDIO_BIAS) << 8;
#    else
  // For simplicity of code, even in mono, we're writing stereo samples
  _esp32_prev_sample[1] = _esp32_prev_sample[0];
#    endif
#  elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
  for (uint8_t i=0; i<PDM_RESOLUTION; ++i) {
    _esp32_prev_sample[i] = pdmCode32(f.l() + AUDIO_BIAS);
  }
#  else
  // PT8211 takes signed samples
  _esp32_prev_sample[0] = f.l();
  _esp32_prev_sample[1] = f.r();
#  endif
  _esp32_can_buffer_next = esp32_tryWriteSample();
}
#endif

#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
void CACHED_FUNCTION_ATTR timer0_audio_output_isr(void *) {
  TIMERG0.int_clr_timers.t0 = 1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
  defaultAudioOutput();
}
#endif

static void startAudio() {
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)  // for external audio output, set up a timer running a audio rate
  static intr_handle_t s_timer_handle;
  timer_config_t config = {
    .alarm_en = true,
    .counter_en = false,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = true,
    .divider = 1 // For max available precision: The APB_CLK clock signal is running at 80 MHz, i.e. 1/80 uS per tick
  };
  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 80000000UL / AUDIO_RATE);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer0_audio_output_isr, nullptr, 0, &s_timer_handle);
  timer_start(TIMER_GROUP_0, TIMER_0);

#else
  static const i2s_config_t i2s_config = {
#  if (ESP32_AUDIO_OUT_MODE == PT8211_DAC) || (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#  elif (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
#  endif
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
#  if (ESP32_AUDIO_OUT_MODE == PT8211_DAC) || (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
  static const i2s_pin_config_t pin_config = {
    .bck_io_num = ESP32_I2S_BCK_PIN,
    .ws_io_num = ESP32_I2S_WS_PIN,
    .data_out_num = ESP32_I2S_DATA_PIN,
    .data_in_num = -1
  };
  i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
#  elif (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
  i2s_set_pin((i2s_port_t)i2s_num, NULL);
  i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
#  endif
  i2s_zero_dma_buffer((i2s_port_t)i2s_num);

#endif
}

void stopMozzi() {
  // TODO: implement me
}
//// END AUDIO OUTPUT code ///////
