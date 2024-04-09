/*
 * MozziGuts_impl_ESP8266.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2020-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#if !(IS_ESP8266())
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
#define LOOP_YIELD yield();

} // namespace MozziPrivate
#include <uart.h>
#include <I2S.h>
namespace MozziPrivate {
uint16_t output_buffer_size = 0;

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_I2S_DAC) // i.e. not external

#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
} // namespace MozziPrivate
#    include <i2s.h>
namespace MozziPrivate {
inline bool canBufferAudioOutput() {
  return (i2s_available() >= MOZZI_PDM_RESOLUTION);
}
inline void audioOutput(const AudioOutput f) {
  for (uint8_t words = 0; words < MOZZI_PDM_RESOLUTION; ++words) {
    i2s_write_sample(pdmCode32(f.l()+MOZZI_AUDIO_BIAS));
  }
}
#  elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
} // namespace MozziPrivate
#    include <i2s.h>
namespace MozziPrivate {
inline bool canBufferAudioOutput() {
  return (i2s_available() >= MOZZI_PDM_RESOLUTION);
}
inline void audioOutput(const AudioOutput f) {
  i2s_write_lr(f.l(), f.r());  // Note: i2s_write expects zero-centered output
}
#  else
MOZZI_ASSERT_EQUAL(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_SERIAL)
// NOTE: This intermediate step is needed because the output timer is running at a rate higher than MOZZI_AUDIO_RATE, and we need to rely on the (tiny)
//       serial buffer itself to achieve appropriate rate control
void CACHED_FUNCTION_ATTR esp8266_serial_audio_output() {
  // Note: That unreadble mess is an optimized version of Serial1.availableForWrite()
  while ((UART_TX_FIFO_SIZE - ((U1S >> USTXC) & 0xff)) > (MOZZI_PDM_RESOLUTION * 4)) {
    defaultAudioOutput();
  }
}

inline void audioOutput(const AudioOutput f) {
  // optimized version of: Serial1.write(...);
  for (uint8_t i = 0; i < MOZZI_PDM_RESOLUTION*4; ++i) {
    U1F = pdmCode8(f.l()+MOZZI_AUDIO_BIAS);
  }
}
#  endif
#endif

static void startAudio() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED) // for external audio output, set up a timer running a audio rate
  timer1_isr_init();
  timer1_attachInterrupt(defaultAudioOutput);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
  timer1_write(F_CPU / MOZZI_AUDIO_RATE);
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_SERIAL)
  Serial1.begin(
      MOZZI_AUDIO_RATE * (MOZZI_PDM_RESOLUTION * 40), SERIAL_8N1,
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
  timer1_write(F_CPU / (MOZZI_AUDIO_RATE * MOZZI_PDM_RESOLUTION));
#else
  i2s_begin();
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S)
  pinMode(2, INPUT); // Set the two unneeded I2S pins to input mode, to reduce
                     // side effects
  pinMode(15, INPUT);
#  endif
  i2s_set_rate(MOZZI_AUDIO_RATE * MOZZI_PDM_RESOLUTION);
  if (output_buffer_size == 0)
    output_buffer_size =
        i2s_available(); // Do not reset count when stopping / restarting
#endif
}


void stopMozzi() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S, MOZZI_OUTPUT_I2S_DAC)
  i2s_end();
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_SERIAL, MOZZI_OUTPUT_EXTERNAL_TIMED)
  timer1_disable();
#endif
  interrupts();
}

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_I2S) && (MOZZI_PDM_RESOLUTION != 1)
#  define AUDIOTICK_ADJUSTMENT ((output_buffer_size - i2s_available()) / MOZZI_PDM_RESOLUTION)
#else
#  define AUDIOTICK_ADJUSTMENT (output_buffer_size - i2s_available())
#endif

//// END AUDIO OUTPUT code ///////

//// BEGIN Random seeding ////////
} //namespace MozziPrivate
#include <esp8266_peri.h>
namespace MozziPrivate {
void MozziRandPrivate::autoSeed() {
  x = RANDOM_REG32;
  // TODO: The XORs may not be needed, but for lack of documentation (that I could find), let's assume RANDOM_REG32
  //       itself might not get updated on every read. NOTE: x, y, and z are initialized to non-zero, before this.
  y = y ^ RANDOM_REG32;
  z = z ^ RANDOM_REG32;
}
//// END Random seeding ////////
} // namespace MozziPrivate
