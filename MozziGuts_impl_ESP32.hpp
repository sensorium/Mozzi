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
#include "AudioConfigESP32.h"
#include <driver/i2s.h>   // for I2S-based output modes
#include <driver/timer.h> // for EXTERNAL_AUDIO_OUTPUT

static const char module[]="Mozzi-ESP32";


// ////// BEGIN analog input code ////////
#define getADCReading() 0;

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


#if (EXTERNAL_AUDIO_OUTPUT) // for external audio output, set up a timer running a audio rate

void CACHED_FUNCTION_ATTR timer0_audio_output_isr(void *) {
  TIMERG0.int_clr_timers.t0 = 1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
  defaultAudioOutput();
}


static void startAudio() {
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
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 80000000UL / AUDIO_RATE / div);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer0_audio_output_isr, nullptr, 0, &s_timer_handle);
  timer_start(TIMER_GROUP_0, TIMER_0);
}

void stopMozzi() {
  timer_pause(TIMER_GROUP_0, TIMER_0);
  timer_disable_intr(TIMER_GROUP_0, TIMER_0);
}

#else // I2S Output

// On ESP32 we cannot test wether the DMA buffer has room. Instead, we have to use a one-sample mini buffer. In each iteration we
// _try_ to write that sample to the DMA buffer, and if successful, we can buffer the next sample. Somewhat cumbersome, but works.
// TODO: Should ESP32 gain an implemenation of i2s_available(), we should switch to using that, instead.
static bool _esp32_can_buffer_next = true;
#  if defined(IS_INTERNAL_DAC)
static uint16_t _esp32_prev_sample[2];
#  elif defined(IS_I2S_DAC)
static int16_t _esp32_prev_sample[2];
#  elif defined(IS_PDM)
static uint32_t _esp32_prev_sample[PDM_RESOLUTION];
#  endif


/// Make sure that we provide a supported port
int getI2SPort(){
  switch (ESP32_AUDIO_OUT_MODE){
    case INTERNAL_DAC: 
      return 0;
    case PDM_VIA_I2S:  
      return 0;
    case PT8211_DAC:     
      return i2s_num;
    case I2S_DAC_AND_I2S_ADC:  
      return i2s_num;
  }
  ESP_LOGE(module, "%s - %s", __func__, "ESP32_AUDIO_OUT_MODE invalid");
  return -1;
}


/// Determine the I2S Output Mode (and input mode if on same port)
int getI2SMode(){
  switch (ESP32_AUDIO_OUT_MODE){
    case INTERNAL_DAC: 
      ESP_LOGD(module, "%s: %s", __func__, "I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN");
      return I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN;
    case PDM_VIA_I2S:  
      ESP_LOGD(module, "%s: %s", __func__, "I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_PDM");
      return I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_PDM;
    case PT8211_DAC:     
      ESP_LOGD(module, "%s: %s", __func__, "I2S_MODE_MASTER | I2S_MODE_TX");
      return I2S_MODE_MASTER | I2S_MODE_TX;
    case I2S_DAC_AND_I2S_ADC:  
      ESP_LOGD(module, "%s: %s", __func__, "I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX");
      return I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX;
  }
   ESP_LOGE(module, "%s: %s", __func__, "ESP32_AUDIO_OUT_MODE invalid");
 return -1;
}

/// @brief  Reads a sample from the I2S buffer. I2S is stereo, so we combine the result to one single sample
int getAudioInput() {
  static const int i2s_port = getI2SPort();
  if (i2s_port==-1) return 0;

  int16_t tmp[2];
  size_t result;
  esp_err_t rc = i2s_read((i2s_port_t)i2s_port, tmp, sizeof(tmp), &result, portMAX_DELAY);
  return ADC_VALUE((tmp[0]+tmp[1]) / 2);
}


inline bool esp32_tryWriteSample() {
  static i2s_port_t port = (i2s_port_t) getI2SPort();
  size_t bytes_written;
  int write_len = sizeof(_esp32_prev_sample);
  i2s_write(port, &_esp32_prev_sample, write_len, &bytes_written, 0);
  //ESP_LOGD(module, "%s port:%d, len: %d, written: %d, value: %d", __func__, port, write_len, bytes_written, _esp32_prev_sample[0]);
  return (bytes_written != 0);
}

inline bool canBufferAudioOutput() {
  if (_esp32_can_buffer_next) return true;
  _esp32_can_buffer_next = esp32_tryWriteSample();
  return _esp32_can_buffer_next;
}

inline void audioOutput(const AudioOutput f) {
#  if defined(IS_INTERNAL_DAC)
  _esp32_prev_sample[0] = (f.l() + AUDIO_BIAS) << 8;
#    if (AUDIO_CHANNELS > 1)
  _esp32_prev_sample[1] = (f.r() + AUDIO_BIAS) << 8;
#    else
  // For simplicity of code, even in mono, we're writing stereo samples
  _esp32_prev_sample[1] = _esp32_prev_sample[0];
#    endif
#  elif defined(IS_PDM)
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

static void startI2SAudio(i2s_port_t port, int mode){
  ESP_LOGI(module, "%s: port=%d, mode=0x%x, rate=%d", __func__, port, mode, AUDIO_RATE * PDM_RESOLUTION);

  static i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)mode,
    .sample_rate = AUDIO_RATE * PDM_RESOLUTION,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  // only the top 8 bits will actually be used by the internal DAC, but using 8 bits straight away seems buggy
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,  // always use stereo output. mono seems to be buggy, and the overhead is insignifcant on the ESP32
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),  // this appears to be the correct setting for internal DAC and PT8211, but not for other dacs
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,    // 8*128 bytes of buffer corresponds to 256 samples (2 channels, see above, 2 bytes per sample per channel)
    .dma_buf_len = 128,
    .use_apll = false,
  };

  // install i2s driver
  if (i2s_driver_install(port, &i2s_config, 0, NULL)!=ESP_OK){
    ESP_LOGE(module, "%s - %s : %d", __func__, "i2s_driver_install", port);
  }

  // Internal DAC
  if (ESP32_AUDIO_OUT_MODE==INTERNAL_DAC){
    if (i2s_set_pin(port, NULL)!=ESP_OK) {
        ESP_LOGE(module, "%s - %s", __func__, "i2s_set_pin");
    }
    if (i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN)!=ESP_OK) {
        ESP_LOGE(module, "%s - %s", __func__, "i2s_set_dac_mode");
    }
  // Regular I2S
  } else {
    static const i2s_pin_config_t pin_config = {
      .bck_io_num = ESP32_I2S_BCK_PIN,
      .ws_io_num = ESP32_I2S_WS_PIN,
      .data_out_num = ESP32_I2S_DATA_PIN,
      .data_in_num = ESP32_I2S_DATA_PIN_IN,
    };
    if (i2s_set_pin(port, &pin_config)!=ESP_OK) {
      ESP_LOGE(module, "%s - %s", __func__, "i2s_set_pin");
    }
  }
  i2s_zero_dma_buffer(port);
}

/// Use I2S for the Output and Input
static void startAudio() {
  // start output
  i2s_port_t port = (i2s_port_t)getI2SPort();
  startI2SAudio(port, getI2SMode());
}

void stopMozzi() {
  i2s_port_t port = (i2s_port_t)getI2SPort();
  i2s_stop(port);
  i2s_driver_uninstall(port);
}

#endif

//// END AUDIO OUTPUT code ///////
