/*
 * MozziESP8266.h
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
#include "MozziCommon.h"
#include <uart.h>
#include <i2s.h>

static void CACHED_FUNCTION_ATTR esp8266_serial_audio_output();
static void CACHED_FUNCTION_ATTR defaultAudioOutput();


class MozziESP8266  {
  
  friend void CACHED_FUNCTION_ATTR esp8266_serial_audio_output();
  friend void CACHED_FUNCTION_ATTR defaultAudioOutput();

  public:

    void start(int control_rate_hz) {
      setupMozziADC(); 
      Common.startControl(control_rate_hz);
      startAudioStandard();
    }

    void stop() {
        #if (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL)
          i2s_end();
        #else
          timer1_disable();
        #endif
          // ps - nointerrupts was never called so the following is not necessary: 
          // interrupts();
    }

    int getAudioInput() { 
      return Common.audio_input;
    }

    unsigned long audioTicks() {
      #if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
        return Common.output_buffer.count();
      #elif ( (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S) && (PDM_RESOLUTION != 1))
        return (samples_written_to_buffer -
                ((output_buffer_size - i2s_available()) / PDM_RESOLUTION));
      #else
        return (samples_written_to_buffer - (output_buffer_size - i2s_available()));
      #endif
    }

    unsigned long mozziMicros() { 
      return audioTicks() * MICROS_PER_AUDIO_TICK;
    }

    void audioHook()  {
      Common.audioHook();
      yield();
    }


  protected:

    uint16_t output_buffer_size = 0;
    #if BYPASS_MOZZI_OUTPUT_BUFFER == true
    uint64_t samples_written_to_buffer = 0;
    #endif

    #if (EXTERNAL_AUDIO_OUTPUT != true)
    #if (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S)
    inline bool canBufferAudioOutput() {
      return (i2s_available() >= PDM_RESOLUTION);
    }
    inline void audioOutput(const AudioOutput_t f) {
      for (uint8_t words = 0; words < PDM_RESOLUTION; ++words) {
        i2s_write_sample(pdmCode32(f.l()));
      }
    }
    #elif (ESP_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
    inline bool canBufferAudioOutput() {
      return (i2s_available() >= PDM_RESOLUTION);
    }
    inline void audioOutput(const AudioOutput_t f) {
      i2s_write_lr(f.l(), f.r());  // Note: i2s_write expects zero-centered output
    }
    #else
    inline void audioOutput(const AudioOutput_t f) {
      // optimized version of: Serial1.write(...);
      for (uint8_t i = 0; i < PDM_RESOLUTION*4; ++i) {
        U1F = pdmCode8(f.l());
      }
    }
    #endif
    #endif


    //-----------------------------------------------------------------------------------------------------------------

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

    static void startSecondAudioADC() {
      ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
    }

    static void receiveSecondAudioADC() {
      if (!input_buffer.isFull())
        input_buffer.write(ADC);
    }

    #endif
    ////// END AUDIO INPUT code ////////

    #if BYPASS_MOZZI_OUTPUT_BUFFER == true
    inline void bufferAudioOutput(const AudioOutput_t f) {
      audioOutput(f);
      ++samples_written_to_buffer;
    }
    #else
    #define canBufferAudioOutput() (!Common.output_buffer.isFull())
    #define bufferAudioOutput(f) Common.output_buffer.write(f)
    #endif


    //-----------------------------------------------------------------------------------------------------------------

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
      // set up a timer to copy from Mozzi Common.output_buffer into Serial TX buffer
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
};

// Callbacks

extern PLATFORM_OUTPUT_CLASS Mozzi;

#if ((ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL) && (EXTERNAL_AUDIO_OUTPUT != true))
void CACHED_FUNCTION_ATTR esp8266_serial_audio_output() {
  // Note: That unreadble mess is an optimized version of Serial1.availableForWrite()
  while ((UART_TX_FIFO_SIZE - ((U1S >> USTXC) & 0xff)) > (PDM_RESOLUTION * 4)) {
    Mozzi.audioOutput(Common.output_buffer.read());
  }
}
#endif


#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
static void CACHED_FUNCTION_ATTR defaultAudioOutput() {
#if (USE_AUDIO_INPUT == true)
  Mozzi.adc_count = 0;
  Mozzi.startSecondAudioADC();
#endif
  Mozzi.audioOutput(Common.output_buffer.read());
}
#endif


#endif
