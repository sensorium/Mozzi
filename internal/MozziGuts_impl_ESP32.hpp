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

} // namespace MozziPrivate
  //// BEGIN AUDIO OUTPUT code ///////
  //#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
#if /*MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC) || */MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)

//#  include <driver/i2s.h>   // for I2S-based output modes, including - technically - internal DAC
#include<driver/i2s_std.h>

#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#include<driver/dac_continuous.h>
namespace MozziPrivate {
  /* static*/ dac_continuous_handle_t dac_handle;
}
#endif

//#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
//#include<driver/i2s_pdm.h>
namespace MozziPrivate {
  #if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
  const i2s_port_t i2s_num = MOZZI_I2S_PORT;
  static i2s_chan_handle_t tx_handle;
  // On ESP32 we cannot test wether the DMA buffer has room. Instead, we have to use a one-sample mini buffer. In each iteration we
  // _try_ to write that sample to the DMA buffer, and if successful, we can buffer the next sample. Somewhat cumbersome, but works.
  // TODO: Should ESP32 gain an implemenation of i2s_available(), we should switch to using that, instead.
#endif
  static bool _esp32_can_buffer_next = true;
  
    #  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
    static uint8_t _esp32_prev_sample[2];
  // #    define ESP_SAMPLE_SIZE (2*sizeof(uint8_t))
  #    define ESP_SAMPLE_SIZE (sizeof(uint8_t))
    #  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
    static int16_t _esp32_prev_sample[2];
    #    define ESP_SAMPLE_SIZE I2S_DATA_BIT_WIDTH_16BIT
    #  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
    static uint32_t _esp32_prev_sample[PDM_RESOLUTION];
    #    define ESP_SAMPLE_SIZE (PDM_RESOLUTION*sizeof(uint32_t))
    #  endif
  // static int16_t _esp32_prev_sample[2];
  


  inline bool esp32_tryWriteSample() {
    size_t bytes_written;
    //i2s_write(i2s_num, &_esp32_prev_sample, ESP_SAMPLE_SIZE, &bytes_written, 0);
    #if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
    i2s_channel_write(tx_handle,&_esp32_prev_sample, ESP_SAMPLE_SIZE, &bytes_written, 0);
    
     #elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
    dac_continuous_write(dac_handle, _esp32_prev_sample, ESP_SAMPLE_SIZE, &bytes_written, 0);
    /*uint8_t tt = 120;
    esp_err_t err;
    err = dac_continuous_write(dac_handle, &tt , 2, &bytes_written, 0);
    Serial.println(err==ESP_ERR_TIMEOUT);*/

    #endif
    return (bytes_written != 0);
  }

  inline bool canBufferAudioOutput() {
    if (_esp32_can_buffer_next) return true;
    _esp32_can_buffer_next = esp32_tryWriteSample();
    //if(_esp32_can_buffer_next) digitalWrite(2,!digitalRead(2));
    return _esp32_can_buffer_next;
  }

# if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
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
#  else  // EXTERNAL I2S
    // PT8211 takes signed samples
    _esp32_prev_sample[0] = f.l();
    _esp32_prev_sample[1] = f.r();
#  endif
    _esp32_can_buffer_next = esp32_tryWriteSample();
  }
  #endif
  } // namespace MozziPrivate


namespace MozziPrivate {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
#include <driver/gptimer.h>

  bool CACHED_FUNCTION_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
  {
    defaultAudioOutput();
    return true;
  }
#endif

  static void startAudio() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)  // for external audio output, set up a timer running a audio rate
  
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
      .clk_src = GPTIMER_CLK_SRC_DEFAULT,
      .direction = GPTIMER_COUNT_UP,
      .resolution_hz = 40 * 1000 * 1000, // 40MHz
    
    };
    gptimer_new_timer(&timer_config, &gptimer);
 
    gptimer_alarm_config_t alarm_config; // note: inline config for the flag does not work unless we have access to c++20, hence the manual attributes setting.
    alarm_config.reload_count = 0;
    alarm_config.alarm_count = (40000000UL / MOZZI_AUDIO_RATE);
    alarm_config.flags.auto_reload_on_alarm = true;

    gptimer_set_alarm_action(gptimer, &alarm_config);

    gptimer_event_callbacks_t cbs = {
      .on_alarm = timer_on_alarm_cb, // register user callback
    };

    gptimer_register_event_callbacks(gptimer,&cbs,NULL);
    gptimer_enable(gptimer);
    gptimer_start(gptimer);

#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)

    static i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(MOZZI_I2S_PORT, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, &tx_handle, NULL);
    //static const i2s_config_t i2s_config = {
    i2s_std_config_t std_cfg = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(MOZZI_AUDIO_RATE),    
      .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(ESP_SAMPLE_SIZE, I2S_SLOT_MODE_STEREO), // TODO: add between the different modes?
      .gpio_cfg = {
        .mclk = I2S_GPIO_UNUSED,
        .bclk = gpio_num_t(MOZZI_I2S_PIN_BCK),
        .ws = gpio_num_t(MOZZI_I2S_PIN_WS),
        .dout = gpio_num_t(MOZZI_I2S_PIN_DATA),
        .din = I2S_GPIO_UNUSED,
        .invert_flags = {
	  .mclk_inv = false,
	  .bclk_inv = false,
	  .ws_inv = false,
        },
      },
	
      /*	
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
      .slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(ESP_SAMPLE_SIZE, I2S_SLOT_MODE_STEREO),
      #  endif*/
  
    };
  

    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_enable(tx_handle);
    
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
    // static dac_continuous_handle_t dac_handle = NULL;
     dac_continuous_config_t dac_config= {
       .chan_mask = DAC_CHANNEL_MASK_ALL,
       /*.desc_num = 8,
	 .buf_size = 64,*/
       .desc_num = 2,
	 .buf_size = 4,
       .freq_hz = MOZZI_AUDIO_RATE,
       .offset = 120,
       .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
       .chan_mode = DAC_CHANNEL_MODE_SIMUL, };
     
     esp_err_t err;
     dac_continuous_new_channels(&dac_config, &dac_handle);
     err =  dac_continuous_enable(dac_handle);
    
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
