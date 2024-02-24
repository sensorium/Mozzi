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

#if !(IS_MBED())
#  error "Wrong implementation included for this platform"
#endif

#define CHUNKSIZE 64

////// BEGIN analog input code ////////

#if (USE_AUDIO_INPUT)
#define AUDIO_INPUT_MODE AUDIO_INPUT_CUSTOM

#include <Arduino_AdvancedAnalog.h>

AdvancedADC adc(AUDIO_INPUT_PIN);
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
  if (!adc.begin(AN_RESOLUTION_12, AUDIO_RATE, CHUNKSIZE, 256/CHUNKSIZE)) {
    Serial.println("Failed to start analog acquisition!");
    while (1);
  }
}
#endif


/** NOTE: This section deals with implementing (fast) asynchronous analog reads, which form the backbone of mozziAnalogRead(), but also of USE_AUDIO_INPUT (if enabled).
 *  This template provides empty/dummy implementations to allow you to skip over this section, initially. Once you have an implementation, be sure to enable the
 *  #define, below: */
//#define MOZZI_FAST_ANALOG_IMPLEMENTED

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
#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}

/** NOTE: Code needed to trigger a conversion on a new channel */
void adcStartConversion(uint8_t channel) {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to trigger a subsequent conversion on the latest channel. If your platform has no special code for it, you should store the channel from
 *  adcStartConversion(), and simply call adcStartConversion(previous_channel), here. */
void startSecondADCReadOnCurrentChannel() {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to set up faster than usual analog reads, e.g. specifying the number of CPU cycles that the ADC waits for the result to stabilize.
 *  This particular function is not super important, so may be ok to leave empty, at least, if the ADC is fast enough by default. */
void setupFastAnalogRead(int8_t speed) {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to initialize the ADC for asynchronous reads. Typically involves setting up an interrupt handler for when conversion is done, and
 *  possibly calibration. */
void setupMozziADC(int8_t speed) {
#warning Fast analog read not implemented on this platform
  #if (USE_AUDIO_INPUT)
      startAudioInput();
  #endif
}

/* NOTE: Most platforms call a specific function/ISR when conversion is complete. Provide this function, here.
 * From inside its body, simply call advanceADCStep(). E.g.:
void stm32_adc_eoc_handler() {
  advanceADCStep();
}
*/



////// END analog input code ////////

////// BEGIN audio output code //////
#if (EXTERNAL_AUDIO_OUTPUT == true)

#define US_PER_AUDIO_TICK (1000000L / AUDIO_RATE)
#include <mbed.h>
mbed::Ticker audio_output_timer;

volatile bool audio_output_requested = false;
inline void defaultAudioOutputCallback() {
  audio_output_requested = true;
}

#define AUDIO_HOOK_HOOK { if (audio_output_requested) { audio_output_requested = false; defaultAudioOutput(); } }

static void startAudio() {
  audio_output_timer.attach_us(&defaultAudioOutputCallback, US_PER_AUDIO_TICK);
}

void stopMozzi() {
  audio_output_timer.detach();
}

#elif (MBED_AUDIO_OUT_MODE == INTERNAL_DAC)

#include <Arduino_AdvancedAnalog.h>

AdvancedDAC dac1(AUDIO_CHANNEL_1_PIN);
Sample buf1[CHUNKSIZE];
#if (AUDIO_CHANNELS > 1)
AdvancedDAC dac2(AUDIO_CHANNEL_2_PIN);
Sample buf2[CHUNKSIZE];
#endif
int bufpos = 0;

inline void commitBuffer(Sample buffer[], AdvancedDAC &dac) {
  SampleBuffer dmabuf = dac.dequeue();
  // NOTE: Yes, this is silly code, and originated as an accident. Somehow it appears to help _a little_ against current problem wrt DAC stability
  for (unsigned int i=0;i<CHUNKSIZE;i++) memcpy(dmabuf.data(), buffer, CHUNKSIZE*sizeof(Sample));
  dac.write(dmabuf);
}

/** NOTE: This is the function that actually write a sample to the output. In case of EXTERNAL_AUDIO_OUTPUT == true, it is provided by the library user, instead. */
inline void audioOutput(const AudioOutput f) {
  if (bufpos >= CHUNKSIZE) {
    commitBuffer(buf1, dac1);
#if (AUDIO_CHANNELS > 1)
    commitBuffer(buf2, dac2);
#endif
    bufpos = 0;
  }
  buf1[bufpos] = f.l()+AUDIO_BIAS;
#if (AUDIO_CHANNELS > 1)
  buf2[bufpos] = f.r()+AUDIO_BIAS;
#endif
  ++bufpos;
}

bool canBufferAudioOutput() {
  return (bufpos < CHUNKSIZE || (dac1.available()
#if (AUDIO_CHANNELS > 1)
    && dac2.available()
#endif
  ));
}

static void startAudio() {
  //NOTE: DAC setup currently affected by https://github.com/arduino-libraries/Arduino_AdvancedAnalog/issues/35 . Don't expect this to work, until using a fixed version fo Arduino_AdvancedAnalog!
  if (!dac1.begin(AN_RESOLUTION_12, AUDIO_RATE, CHUNKSIZE, 256/CHUNKSIZE)) {
    Serial.println("Failed to start DAC1 !");
    while (1);
  }
#if (AUDIO_CHANNELS > 1)
  if (!dac2.begin(AN_RESOLUTION_12, AUDIO_RATE, CHUNKSIZE, 256/CHUNKSIZE)) {
    Serial.println("Failed to start DAC2 !");
    while (1);
  }
#endif
}

void stopMozzi() {
  dac1.stop();
#if (AUDIO_CHANNELS > 1)
  dac2.stop();
#endif
}

#elif (MBED_AUDIO_OUT_MODE == PDM_VIA_SERIAL)

#include <mbed.h>

mbed::BufferedSerial serial_out1(digitalPinToPinName(PDM_SERIAL_UART_TX_CHANNEL_1), digitalPinToPinName(PDM_SERIAL_UART_RX_CHANNEL_1));
uint8_t buf[PDM_RESOLUTION*4];

bool canBufferAudioOutput() {
  return serial_out1.writable();
}

inline void audioOutput(const AudioOutput f) {
  for (uint8_t i = 0; i < PDM_RESOLUTION*4; ++i) {
    buf[i] = pdmCode8(f.l()+AUDIO_BIAS);
  }
  serial_out1.write(&buf, PDM_RESOLUTION*4);
}

static void startAudio() {
  serial_out1.set_baud(AUDIO_RATE*PDM_RESOLUTION*40); // NOTE: 40 = 4 * (8 bits + stop-bits)
  serial_out1.set_format(8, mbed::BufferedSerial::None, 1);
}

void stopMozzi() {
#warning implement me
}


#endif
////// END audio output code //////
