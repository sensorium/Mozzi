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

#if (USE_AUDIO_INPUT == true)
#  warning USE_AUDIO_INPUT is not (yet) implemented on this platform
#endif

#include <uart.h>
#include <i2s.h>
uint16_t output_buffer_size = 0;

#if ((ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL) && (EXTERNAL_AUDIO_OUTPUT != true))
void CACHED_FUNCTION_ATTR esp8266_serial_audio_output() {
  // Note: That unreadble mess is an optimized version of Serial1.availableForWrite()
  while ((UART_TX_FIFO_SIZE - ((U1S >> USTXC) & 0xff)) > (PDM_RESOLUTION * 4)) {
    audioOutput(output_buffer.read());
  }
}
#endif


static void startAudio() {
#if (EXTERNAL_AUDIO_OUTPUT == true)  && (BYPASS_MOZZI_OUTPUT_BUFFER != true) // for external audio output, set up a timer running a audio rate
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
#  define AUDIOTICK_ADJUSTMENT ((output_buffer_size - i2s_available()) / PDM_RESOLUTION);
#else
#  define AUDIOTICK_ADJUSTMENT (output_buffer_size - i2s_available());
#endif

