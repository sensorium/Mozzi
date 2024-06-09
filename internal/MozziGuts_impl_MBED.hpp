/*
 * MozziGuts_impl_MBED.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 T. Combriat and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#if !(IS_MBED())
#  error "Wrong implementation included for this platform"
#endif

#define CHUNKSIZE 64

namespace MozziPrivate {

////// BEGIN analog input code ////////

#if MOZZI_IS(MOZZI_AUDIO_INPUT, MOZZI_AUDIO_INPUT_STANDARD)
#define MOZZI__LEGACY_AUDIO_INPUT_IMPL 0

} // namespace MozziPrivate
#include <Arduino_AdvancedAnalog.h>
namespace MozziPrivate {

AdvancedADC adc(MOZZI_AUDIO_INPUT_PIN);
Sample inbuf[CHUNKSIZE];
int inbufpos=0; 

bool audioInputAvailable() {
  if (inbufpos >= CHUNKSIZE) {
    if (!adc.available()) return false;
    SampleBuffer buf = adc.read();
    memcpy(inbuf,buf.data(), CHUNKSIZE*sizeof(Sample));
    inbufpos = 0;
    buf.release();
    return true;
  }
  else return true;
}
AudioOutputStorage_t readAudioInput(){
  return inbuf[inbufpos++];
}


static void startAudioInput() {
  if (!adc.begin(AN_RESOLUTION_12, MOZZI_AUDIO_RATE, CHUNKSIZE, 256/CHUNKSIZE)) {
    Serial.println("Failed to start analog acquisition!");
    while (1);
  }
}
#else
static void startAudioInput() {}; // dummy to ease coding
#endif

#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
#error not yet implemented
/** NOTE: This section deals with implementing (fast) asynchronous analog reads, which form the backbone of mozziAnalogRead(), but also of USE_AUDIO_INPUT (if enabled).
 *  This template provides empty/dummy implementations to allow you to skip over this section, initially. Once you have an implementation, be sure to enable the
 *  #define, above */

// Insert here code to read the result of the latest asynchronous conversion, when it is finished.
// You can also provide this as a function returning unsigned int, should it be more complex on your platform
#define getADCReading() 0

/** NOTE: On "pins" vs. "channels" vs. "indices"
 *  "Pin" is the pin number as would usually be specified by the user in mozziAnalogRead().
 *  "Channel" is an internal ADC channel number corresponding to that pin. On many platforms this is simply the same as the pin number, on others it differs.
 *      In other words, this is an internal representation of "pin".
 *  "Index" is the index of the reading for a certain pin/channel in the array of analog_readings, ranging from 0 to NUM_ANALOG_PINS. This, again may be the
 *      same as "channel" (e.g. on AVR), however, on platforms where ADC-capable "channels" are not numbered sequentially starting from 0, the channel needs
 *      to be converted to a suitable index.
 *
 *  In summary, the semantics are roughly
 *      mozziAnalogRead(pin) -> _ADCimplementation_(channel) -> analog_readings[index]
 *  Implement adcPinToChannelNum() and channelNumToIndex() to perform the appropriate mapping.
 */
// NOTE: Theoretically, adcPinToChannelNum is public API for historical reasons, thus cannot be replaced by a define
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}

/** NOTE: Code needed to trigger a conversion on a new channel */
void adcStartConversion(uint8_t channel) {
}

/** NOTE: Code needed to trigger a subsequent conversion on the latest channel. If your platform has no special code for it, you should store the channel from
 *  adcStartConversion(), and simply call adcStartConversion(previous_channel), here. */
void startSecondADCReadOnCurrentChannel() {
}

/** NOTE: Code needed to set up faster than usual analog reads, e.g. specifying the number of CPU cycles that the ADC waits for the result to stabilize.
 *  This particular function is not super important, so may be ok to leave empty, at least, if the ADC is fast enough by default. */
void setupFastAnalogRead(int8_t speed) {
}

/** NOTE: Code needed to initialize the ADC for asynchronous reads. Typically involves setting up an interrupt handler for when conversion is done, and
 *  possibly calibration. */
void setupMozziADC(int8_t speed) {
}

#endif

////// END analog input code ////////

////// BEGIN audio output code //////
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)

#define US_PER_AUDIO_TICK (1000000L / MOZZI_AUDIO_RATE)
} // namespace MozziPrivate
#include <mbed.h>
namespace MozziPrivate {
mbed::Ticker audio_output_timer;

volatile bool audio_output_requested = false;
inline void defaultAudioOutputCallback() {
  audio_output_requested = true;
}

#define AUDIO_HOOK_HOOK { if (audio_output_requested) { audio_output_requested = false; defaultAudioOutput(); } }

static void startAudio() {
  audio_output_timer.attach_us(&defaultAudioOutputCallback, US_PER_AUDIO_TICK);
  startAudioInput();
}

void stopMozzi() {
  audio_output_timer.detach();
}

#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)

} // namespace MozziPrivate
#include <Arduino_AdvancedAnalog.h>
namespace MozziPrivate {

AdvancedDAC dac1(MOZZI_AUDIO_PIN_1);
Sample buf1[CHUNKSIZE];
#if (MOZZI_AUDIO_CHANNELS > 1)
AdvancedDAC dac2(MOZZI_AUDIO_PIN_2);
Sample buf2[CHUNKSIZE];
#endif
int bufpos = 0;

inline void commitBuffer(Sample buffer[], AdvancedDAC &dac) {
  SampleBuffer dmabuf = dac.dequeue();
  // NOTE: Yes, this is silly code, and originated as an accident. Somehow it appears to help _a little_ against current problem wrt DAC stability
  for (unsigned int i=0;i<CHUNKSIZE;i++) memcpy(dmabuf.data(), buffer, CHUNKSIZE*sizeof(Sample));
  dac.write(dmabuf);
}

inline void audioOutput(const AudioOutput f) {
  if (bufpos >= CHUNKSIZE) {
    commitBuffer(buf1, dac1);
#if (MOZZI_AUDIO_CHANNELS > 1)
    commitBuffer(buf2, dac2);
#endif
    bufpos = 0;
  }
  buf1[bufpos] = f.l()+MOZZI_AUDIO_BIAS;
#if (MOZZI_AUDIO_CHANNELS > 1)
  buf2[bufpos] = f.r()+MOZZI_AUDIO_BIAS;
#endif
  ++bufpos;
}

bool canBufferAudioOutput() {
  return (bufpos < CHUNKSIZE || (dac1.available()
#if (MOZZI_AUDIO_CHANNELS > 1)
    && dac2.available()
#endif
  ));
}

static void startAudio() {
  //NOTE: DAC setup currently affected by https://github.com/arduino-libraries/Arduino_AdvancedAnalog/issues/35 . Don't expect this to work, until using a fixed version fo Arduino_AdvancedAnalog!
  if (!dac1.begin(AN_RESOLUTION_12, MOZZI_AUDIO_RATE, CHUNKSIZE, 256/CHUNKSIZE)) {
    Serial.println("Failed to start DAC1 !");
    while (1);
  }
#if (MOZZI_AUDIO_CHANNELS > 1)
  if (!dac2.begin(AN_RESOLUTION_12, MOZZI_AUDIO_RATE, CHUNKSIZE, 256/CHUNKSIZE)) {
    Serial.println("Failed to start DAC2 !");
    while (1);
  }
#endif
  startAudioInput();
}

void stopMozzi() {
  dac1.stop();
#if (MOZZI_AUDIO_CHANNELS > 1)
  dac2.stop();
#endif
}

#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PDM_VIA_SERIAL)

} // namespace MozziPrivate
#include <mbed.h>
namespace MozziPrivate {

mbed::BufferedSerial serial_out1(digitalPinToPinName(MOZZI_SERIAL_PIN_TX), digitalPinToPinName(MOZZI_SERIAL_PIN_RX));
uint8_t buf[MOZZI_PDM_RESOLUTION*4];

bool canBufferAudioOutput() {
  return serial_out1.writable();
}

inline void audioOutput(const AudioOutput f) {
  for (uint8_t i = 0; i < MOZZI_PDM_RESOLUTION*4; ++i) {
    buf[i] = pdmCode8(f.l()+MOZZI_AUDIO_BIAS);
  }
  serial_out1.write(&buf, MOZZI_PDM_RESOLUTION*4);
}

static void startAudio() {
  serial_out1.set_baud(MOZZI_AUDIO_RATE*MOZZI_PDM_RESOLUTION*40); // NOTE: 40 == 4 * (8 bits + stop-bits)
  serial_out1.set_format(8, mbed::BufferedSerial::None, 1);
}

void stopMozzi() {
#warning implement me
}


#endif
////// END audio output code //////

//// BEGIN Random seeding ////////
void MozziRandPrivate::autoSeed() {
#warning Automatic random seeding is not implemented on this platform
}
//// END Random seeding ////////

} // namespace MozziPrivate

#undef CHUNKSIZE
#undef US_PER_AUDIO_TICK
