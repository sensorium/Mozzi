/*
 * MozziGuts_impl_ESP32.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2020-2024 Dieter Vandoren, Thomas Friedrichsmeier and the Mozzi Team
 * Copyright 2025 Thomas Combriat and the Mozzi Team
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

////// FIX BAD IDF MACRO AS PER: https://github.com/espressif/arduino-esp32/issues/10975
////// TODO: REMOVE WHEN FIX PROPAGATED TO ARDUINO-ESP32
#if SOC_I2S_HW_VERSION_2
#undef I2S_STD_CLK_DEFAULT_CONFIG
#define I2S_STD_CLK_DEFAULT_CONFIG(rate) \
  { .sample_rate_hz = rate, .clk_src = I2S_CLK_SRC_DEFAULT, .ext_clk_freq_hz = 0, .mclk_multiple = I2S_MCLK_MULTIPLE_256, }
#endif
/////// END FIX
/*
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
#include<driver/dac_continuous.h>
namespace MozziPrivate {
  static dac_continuous_handle_t dac_handle;
  }*/
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
  #    define ESP_SAMPLE_SIZE (sizeof(uint8_t))
    #  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)  
    #    define ESP_SAMPLE_SIZE I2S_DATA_BIT_WIDTH_16BIT
  static int16_t _esp32_prev_sample[2]; // for simplicity, but also because the output is cleaner (why??) we always send stereo samples
    #  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
    static uint32_t _esp32_prev_sample[MOZZI_PDM_RESOLUTION];
  #    define ESP_SAMPLE_SIZE I2S_DATA_BIT_WIDTH_16BIT
    #  endif
  


  inline bool esp32_tryWriteSample() {
    size_t bytes_written;
    //i2s_write(i2s_num, &_esp32_prev_sample, ESP_SAMPLE_SIZE, &bytes_written, 0);
    #if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
        i2s_channel_write(tx_handle,_esp32_prev_sample, 2*sizeof(_esp32_prev_sample[0]), &bytes_written, 0);
    #elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
	i2s_channel_write(tx_handle,_esp32_prev_sample, MOZZI_PDM_RESOLUTION*sizeof(_esp32_prev_sample[0]), &bytes_written, 0);
	/* #elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
    dac_continuous_write(dac_handle, _esp32_prev_sample, ESP_SAMPLE_SIZE, &bytes_written, 0);
	*/
    #endif
    return (bytes_written != 0);
  }

  inline bool canBufferAudioOutput() {
    if (_esp32_can_buffer_next) return true;
    _esp32_can_buffer_next = esp32_tryWriteSample();
    return _esp32_can_buffer_next;
  }

  
# if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)  || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
  inline void audioOutput(const AudioOutput f) {
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
    dacWrite(25, f.l() + MOZZI_AUDIO_BIAS);
#    if (MOZZI_AUDIO_CHANNELS > 1)
    dacWrite(26, f.r() + MOZZI_AUDIO_BIAS);
#    endif
    
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
    ledcWrite(MOZZI_AUDIO_PIN_1,(f.l()+MOZZI_AUDIO_BIAS));
#    if (MOZZI_AUDIO_CHANNELS > 1)
    ledcWrite(MOZZI_AUDIO_PIN_2,(f.r()+MOZZI_AUDIO_BIAS));
#    endif
    
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
    for (uint8_t i=0; i<MOZZI_PDM_RESOLUTION; ++i) {
      _esp32_prev_sample[i] = pdmCode32(f.l() + MOZZI_AUDIO_BIAS);
    }
#  else  // EXTERNAL I2S
    // PT8211 takes signed samples
    _esp32_prev_sample[0] = f.l();
    #    if (MOZZI_AUDIO_CHANNELS > 1)
    _esp32_prev_sample[1] = f.r();
    #    else
    _esp32_prev_sample[1] = 0;
    #    endif
#   endif
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
     _esp32_can_buffer_next = esp32_tryWriteSample();
#  endif
  }
  #endif
  } // namespace MozziPrivate


namespace MozziPrivate {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_PWM)
#include <driver/gptimer.h>

  bool CACHED_FUNCTION_ATTR timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
  {
    defaultAudioOutput();
    return true;
  }
#endif

  static void startAudio() {
    /* Normally, the internal DAC can run on DMA, hence self triggering. Did not managed to get that to work (see: https://github.com/espressif/arduino-esp32/issues/10851) so, for now, we are just using the Mozzi buffer and send dacWrite orders.
     */
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_PWM) //|| MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC) ||  MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM) // set up a timer running a audio rate
  
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
    
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)   
    ledcAttach(MOZZI_AUDIO_PIN_1, MOZZI_AUDIO_RATE, MOZZI_AUDIO_BITS);
#     if (MOZZI_AUDIO_CHANNELS > 1)
    ledcAttach(MOZZI_AUDIO_PIN_2, MOZZI_AUDIO_RATE, MOZZI_AUDIO_BITS);
#    endif
#  endif

#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC) || MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)

    static i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(MOZZI_I2S_PORT, I2S_ROLE_MASTER);
    i2s_new_channel(&chan_cfg, &tx_handle, NULL);
    //static const i2s_config_t i2s_config = {
    i2s_std_config_t std_cfg = {
      .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(MOZZI_AUDIO_RATE * MOZZI_PDM_RESOLUTION),
#     if MOZZI_IS(MOZZI_I2S_FORMAT, MOZZI_I2S_FORMAT_PLAIN)
#       if (MOZZI_AUDIO_CHANNELS > 1)
      .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(ESP_SAMPLE_SIZE, I2S_SLOT_MODE_STEREO),
#       else  // for some reason, sound is way better in stereo. Keeping that here in case this gets solved
      .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(ESP_SAMPLE_SIZE, I2S_SLOT_MODE_STEREO),
#       endif
#     elif MOZZI_IS(MOZZI_I2S_FORMAT, MOZZI_I2S_FORMAT_LSBJ)
#       if (MOZZI_AUDIO_CHANNELS > 1)
      .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(ESP_SAMPLE_SIZE, I2S_SLOT_MODE_STEREO), 
#       else   // for some reason, sound is way better in stereo. Keeping that here in case this gets solved 
      .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(ESP_SAMPLE_SIZE, I2S_SLOT_MODE_STEREO), 
#       endif
#     endif
      .gpio_cfg = {
        .mclk = gpio_num_t(MOZZI_I2S_PIN_MCLK),
        .bclk = gpio_num_t(MOZZI_I2S_PIN_BCK),
        .ws = gpio_num_t(MOZZI_I2S_PIN_WS),
        .dout = gpio_num_t(MOZZI_I2S_PIN_DATA),
        .din = I2S_GPIO_UNUSED,
        .invert_flags = {
	  .mclk_inv = MOZZI_I2S_MBCK_INV,
	  .bclk_inv = MOZZI_I2S_BCK_INV,
	  .ws_inv = MOZZI_I2S_WS_INV,
        },
      },
	
      /*	
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
      .slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(ESP_SAMPLE_SIZE, I2S_SLOT_MODE_STEREO),
      #  endif*/
  
    };
  

    i2s_channel_init_std_mode(tx_handle, &std_cfg);
    i2s_channel_enable(tx_handle);
    
    /*   #elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
     dac_continuous_config_t dac_config= {
       .chan_mask = DAC_CHANNEL_MASK_ALL,
       .desc_num = 2,
	 .buf_size = 4,
       .freq_hz = MOZZI_AUDIO_RATE,
       .offset = 120,
       .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
       .chan_mode = DAC_CHANNEL_MODE_SIMUL, };
     
     esp_err_t err;
     dac_continuous_new_channels(&dac_config, &dac_handle);
     err =  dac_continuous_enable(dac_handle);
    */
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
