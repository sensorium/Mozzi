/*
 * MozziGuts_impl_ESP32.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2020-2024 Dieter Vandoren, Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#if !(IS_ESP32())
#  error "Wrong implementation included for this platform"
#endif

namespace MozziPrivate {
////// BEGIN analog input code ////////
#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
#error not yet implemented

#define getADCReading() 0
#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}
void adcStartConversion(uint8_t channel) {
}
void startSecondADCReadOnCurrentChannel() {
}
void setupMozziADC(int8_t speed) {
}
void setupFastAnalogRead(int8_t speed) {
}

#endif
////// END analog input code ////////


//// BEGIN AUDIO OUTPUT code ///////
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
} // namespace MozziPrivate
#  include <driver/i2s.h>   // for I2S-based output modes, including - technically - internal DAC
namespace MozziPrivate {
const i2s_port_t i2s_num = MOZZI_I2S_PORT;

// On ESP32 we cannot test wether the DMA buffer has room. Instead, we have to use a one-sample mini buffer. In each iteration we
// _try_ to write that sample to the DMA buffer, and if successful, we can buffer the next sample. Somewhat cumbersome, but works.
// TODO: Should ESP32 gain an implemenation of i2s_available(), we should switch to using that, instead.
static bool _esp32_can_buffer_next = true;
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
static uint16_t _esp32_prev_sample[2];
#    define ESP_SAMPLE_SIZE (2*sizeof(uint16_t))
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
static int16_t _esp32_prev_sample[2];
#    define ESP_SAMPLE_SIZE (2*sizeof(int16_t))
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
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
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
  _esp32_prev_sample[0] = (f.l() + MOZZI_AUDIO_BIAS) << 8;
#    if (MOZZI_AUDIO_CHANNELS > 1)
  _esp32_prev_sample[1] = (f.r() + MOZZI_AUDIO_BIAS) << 8;
#    else
  // For simplicity of code, even in mono, we're writing stereo samples
  _esp32_prev_sample[1] = _esp32_prev_sample[0];
#    endif
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
  for (uint8_t i=0; i<MOZZI_PDM_RESOLUTION; ++i) {
    _esp32_prev_sample[i] = pdmCode32(f.l() + MOZZI_AUDIO_BIAS);
  }
#  else
  // PT8211 takes signed samples
  _esp32_prev_sample[0] = f.l();
  _esp32_prev_sample[1] = f.r();
#  endif
  _esp32_can_buffer_next = esp32_tryWriteSample();
}
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)

} // namespace MozziPrivate
#  include <driver/timer.h>
namespace MozziPrivate {

void CACHED_FUNCTION_ATTR timer0_audio_output_isr(void *) {
  TIMERG0.int_clr_timers.t0 = 1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
  defaultAudioOutput();
}
#endif

static void startAudio() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)  // for external audio output, set up a timer running a audio rate
  static intr_handle_t s_timer_handle;
  const int div = 2;
  timer_config_t config = {
    .alarm_en = (timer_alarm_t)true,
    .counter_en = (timer_start_t)false,
    .intr_type = (timer_intr_mode_t) TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = (timer_autoreload_t) true,
    .divider = div // For max available precision: The APB_CLK clock signal is running at 80 MHz, i.e. 2/80 uS per tick
                 // Min acceptable value is 2
  };
  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 80000000UL / MOZZI_AUDIO_RATE / div);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer0_audio_output_isr, nullptr, 0, &s_timer_handle);
  timer_start(TIMER_GROUP_0, TIMER_0);

#elif !MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
  static const i2s_config_t i2s_config = {
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
#  endif
    .sample_rate = MOZZI_AUDIO_RATE * MOZZI_PDM_RESOLUTION,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  // only the top 8 bits will actually be used by the internal DAC, but using 8 bits straight away seems buggy
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,  // always use stereo output. mono seems to be buggy, and the overhead is insignifcant on the ESP32
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB),  // this appears to be the correct setting for internal DAC and PT8211, but not for other dacs
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,    // 8*128 bytes of buffer corresponds to 256 samples (2 channels, see above, 2 bytes per sample per channel)
    .dma_buf_len = 128,
    .use_apll = false
  };

  i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
  static const i2s_pin_config_t pin_config = {
    .bck_io_num = MOZZI_I2S_PIN_BCK,
    .ws_io_num = MOZZI_I2S_PIN_WS,
    .data_out_num = MOZZI_I2S_PIN_DATA,
    .data_in_num = -1
  };
  i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
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

//// BEGIN Random seeding ////////
void MozziRandPrivate::autoSeed() {
  x = esp_random();
  y = esp_random();
  z = esp_random();
}
//// END Random seeding ////////

#undef ESP_SAMPLE_SIZE    // only used inside this file

} // namespace MozziPrivate
