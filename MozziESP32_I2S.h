/*
 * MozziESP32_I2S.h
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
#include "MozziCommon.h"
#include <driver/i2s.h>   // for I2S-based output modes
#include <driver/timer.h> // for EXTERNAL_AUDIO_OUTPUT


class MozziESP32_I2S  {

  public:

    void start(int control_rate_hz) {
      setupMozziADC(); 
      Common.startControl(control_rate_hz);
      startAudioStandard();
    }

    void stop() {
       i2s_driver_uninstall((i2s_port_t)i2s_num);   
    }

    int getAudioInput() { 
      return Common.audio_input;
    }

    unsigned long audioTicks() {
      return samples_written_to_buffer;
    }

    unsigned long mozziMicros() { 
      return samples_written_to_buffer * MICROS_PER_AUDIO_TICK;
    }

    void audioHook()  {
      Common.audioHook();
    }

  protected:
    uint64_t samples_written_to_buffer = 0;


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

    ////// BEGIN AUDIO INPUT code ////////
    #if (USE_AUDIO_INPUT == true)

    // ring buffer for audio input
    CircularBuffer<unsigned int> input_buffer; // fixed size 256
    static boolean audio_input_is_available;
    static int audio_input; // holds the latest audio from input_buffer
    uint8_t adc_count = 0;


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

    #if (EXTERNAL_AUDIO_OUTPUT != true)
    // On ESP32 we cannot test wether the DMA buffer has room. Instead, we have to use a one-sample mini buffer. In each iteration we
    // _try_ to write that sample to the DMA buffer, and if successful, we can buffer the next sample. Somewhat cumbersome, but works.
    // TODO: Should ESP32 gain an implemenation of i2s_available(), we should switch to using that, instead.
    bool _esp32_can_buffer_next = true;
    #if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
    uint16_t _esp32_prev_sample[2];
    #define ESP_SAMPLE_SIZE (2*sizeof(uint16_t))
    #elif (ESP32_AUDIO_OUT_MODE == PT8211_DAC)
    int16_t _esp32_prev_sample[2];
    #define ESP_SAMPLE_SIZE (2*sizeof(int16_t))
    #elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
    uint32_t _esp32_prev_sample[PDM_RESOLUTION];
    #define ESP_SAMPLE_SIZE (PDM_RESOLUTION*sizeof(uint32_t))
    #endif

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
    #if (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
      _esp32_prev_sample[0] = (f.l() + AUDIO_BIAS) << 8;
    #if (STEREO_HACK == true)
      _esp32_prev_sample[1] = (f.r() + AUDIO_BIAS) << 8;
    #else
      // For simplicity of code, even in mono, we're writing stereo samples
      _esp32_prev_sample[1] = _esp32_prev_sample[0];
    #endif
    #elif (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
      for (uint8_t i=0; i<PDM_RESOLUTION; ++i) {
        _esp32_prev_sample[i] = pdmCode32(f.l() + AUDIO_BIAS);
      }
    #else
      // PT8211 takes signed samples
      _esp32_prev_sample[0] = f.l();
      _esp32_prev_sample[1] = f.r();
    #endif
      _esp32_can_buffer_next = esp32_tryWriteSample();
    }

    #endif

    inline void bufferAudioOutput(const AudioOutput_t f) {
      audioOutput(f);
      ++samples_written_to_buffer;
    }


};


#endif
