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

#if (USE_AUDIO_INPUT == true)
#  warning USE_AUDIO_INPUT is not (yet) implemented on this platform
#endif

#include <driver/i2s.h>   // for I2S-based output modes
#include <driver/timer.h> // for EXTERNAL_AUDIO_OUTPUT

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
