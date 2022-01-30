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

#if !(IS_ESP8266())
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
#define LOOP_YIELD yield();

#include <uart.h>
#include <i2s.h>
uint16_t output_buffer_size = 0;

#if (EXTERNAL_AUDIO_OUTPUT != true) // otherwise, the last stage - audioOutput() - will be provided by the user
#  include "AudioConfigESP.h"

#  if (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S)
#    include <i2s.h>
inline bool canBufferAudioOutput() {
  return (i2s_available() >= PDM_RESOLUTION);
}
inline void audioOutput(const AudioOutput f) {
  for (uint8_t words = 0; words < PDM_RESOLUTION; ++words) {
    i2s_write_sample(pdmCode32(f.l()+AUDIO_BIAS));
  }
}
#  elif (ESP_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
#    include <i2s.h>
inline bool canBufferAudioOutput() {
  return (i2s_available() >= PDM_RESOLUTION);
}
inline void audioOutput(const AudioOutput f) {
  i2s_write_lr(f.l(), f.r());  // Note: i2s_write expects zero-centered output
}
#  else  // (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
// NOTE: This intermediate step is needed because the output timer is running at a rate higher than AUDIO_RATE, and we need to rely on the (tiny)
//       serial buffer itself to achieve appropriate rate control
void CACHED_FUNCTION_ATTR esp8266_serial_audio_output() {
  // Note: That unreadble mess is an optimized version of Serial1.availableForWrite()
  while ((UART_TX_FIFO_SIZE - ((U1S >> USTXC) & 0xff)) > (PDM_RESOLUTION * 4)) {
    defaultAudioOutput();
  }
}

inline void audioOutput(const AudioOutput f) {
  // optimized version of: Serial1.write(...);
  for (uint8_t i = 0; i < PDM_RESOLUTION*4; ++i) {
    U1F = pdmCode8(f+AUDIO_BIAS);
  }
}
#  endif
#endif

static void startAudio() {
#if (EXTERNAL_AUDIO_OUTPUT == true) && (BYPASS_MOZZI_OUTPUT_BUFFER != true) // for external audio output, set up a timer running a audio rate
  timer1_isr_init();
  timer1_attachInterrupt(defaultAudioOutput);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
  timer1_write(F_CPU / AUDIO_RATE);
#elif (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
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
#  if (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S)
  pinMode(2, INPUT); // Set the two unneeded I2S pins to input mode, to reduce
                     // side effects
  pinMode(15, INPUT);
#  endif
  i2s_set_rate(AUDIO_RATE * PDM_RESOLUTION);
  if (output_buffer_size == 0)
    output_buffer_size =
        i2s_available(); // Do not reset count when stopping / restarting
#endif
}


void stopMozzi() {
#if (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL)
  i2s_end();
#else
  timer1_disable();
#endif
  interrupts();
}

#if ((ESP_AUDIO_OUT_MODE == PDM_VIA_I2S) && (PDM_RESOLUTION != 1))
#  define AUDIOTICK_ADJUSTMENT ((output_buffer_size - i2s_available()) / PDM_RESOLUTION)
#else
#  define AUDIOTICK_ADJUSTMENT (output_buffer_size - i2s_available())
#endif

//// END AUDIO OUTPUT code ///////
