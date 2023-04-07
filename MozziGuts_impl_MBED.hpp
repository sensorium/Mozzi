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

/** README! 
 * This file is meant to be used as a template when adding support for a new platform. Please read these instructions, first.
 *
 *  Files involved:
 *  1. Modify hardware_defines.h, adding a macro to detect your target platform
 *  2. Modify MozziGuts.cpp to include MozziGuts_impl_YOURPLATFORM.hpp
 *  3. Modify MozziGuts.h to include AudioConfigYOURPLATFORM.h
 *  4. Copy this file to MozziGuts_impl_YOURPLATFORM.hpp and adjust as necessary
 *  (If your platform is very similar to an existing port, it may instead be better to modify the existing MozziGuts_impl_XYZ.hpp/AudioConfigYOURPLATFORM.h,
 *  instead of steps 2-3.).
 *  Some platforms may need small modifications to other files as well, e.g. mozzi_pgmspace.h
 *
 *  How to implement MozziGuts_impl_YOURPLATFORM.hpp:
 *  - Follow the NOTEs provided in this file
 *  - Read the doc at the top of AudioOutput.h for a better understanding of the basic audio output framework
 *  - Take a peek at existing implementations for other hardware (e.g. TEENSY3/4 is rather complete while also simple at the time of this writing)
 *  - Wait for more documentation to arrive
 *  - Ask when in doubt
 *  - Don't forget to provide a PR when done (it does not have to be perfect; e.g. many ports skip analog input, initially)
 */

// The main point of this check is to document, what platform & variants this implementation file is for.
#if !(IS_MBED())
#  error "Wrong implementation included for this platform"
#endif
// Add platform specific includes and declarations, here


////// BEGIN analog input code ////////

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
}

/* NOTE: Most platforms call a specific function/ISR when conversion is complete. Provide this function, here.
 * From inside its body, simply call advanceADCStep(). E.g.:
void stm32_adc_eoc_handler() {
  advanceADCStep();
}
*/
////// END analog input code ////////

////// BEGIN audio output code //////
#if (EXTERNAL_AUDIO_OUTPUT != true && MBED_AUDIO_OUT_MODE == INTERNAL_DAC) // otherwise, the last stage - audioOutput() - will be provided by the user
#include <Arduino_AdvancedAnalog.h>

#define CHUNKSIZE 64
AdvancedDAC dac1(AUDIO_CHANNEL_1_PIN);
Sample buf1[CHUNKSIZE];
#if (AUDIO_CHANNELS > 1)
AdvancedDAC dac2(AUDIO_CHANNEL_2_PIN);
Sample buf2[CHUNKSIZE];
#endif
int bufpos = 0;

inline void commitBuffer(Sample buffer[], AdvancedDAC &dac) {
  SampleBuffer dmabuf = dac.dequeue();
  for (int i = 0; i < dmabuf.size(); ++i) memcpy(dmabuf.data(), buffer, CHUNKSIZE*sizeof(Sample));
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
  // NOTE: In case of stereo, we *assume* the DACs to be running exactly in sync. However, this is safe enough to do, as AdvancedDAC::dequeue() will wait if necessary.
  return (bufpos < CHUNKSIZE || dac1.available());
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

#else
#define US_PER_AUDIO_TICK (1000000L / AUDIO_RATE)
#include <mbed.h>
#include <pinDefinitions.h>
mbed::Ticker audio_output_timer;

#if (EXTERNAL_AUDIO_OUTPUT == true)
volatile bool audio_output_requested = false;
inline void defaultAudioOutputCallback() {
  audio_output_requested = true;
}
#define AUDIO_HOOK_HOOK { if (audio_output_requested) { audio_output_requested = false; defaultAudioOutput(); } }
#else
#define defaultAudioOutputCallback defaultAudioOutput
#endif

#if (MBED_AUDIO_OUT_MODE == TIMEDPWM && EXTERNAL_AUDIO_OUTPUT != true)
#define US_PER_PWM_CYCLE (US_PER_AUDIO_TICK)
mbed::PwmOut pwmpin1(digitalPinToPinName(AUDIO_CHANNEL_1_PIN));
#if (AUDIO_CHANNELS > 1)
mbed::PwmOut pwmpin2(digitalPinToPinName(AUDIO_CHANNEL_2_PIN));
#endif

inline void audioOutput(const AudioOutput f) {
// Unfortunately, this is no good. mBed PWM simply isn't designed for that (too slow, and resets pin on each update).
// We'll need to move closer to the hardware for this...
  pwmpin1.write(.5 + (float) f.l() / ((float) (1L << AUDIO_BITS)));
#if (AUDIO_CHANNELS > 1)
  pwmpin2.write(.5 + (float) f.r() / ((float) (1L << AUDIO_BITS)));
#endif
}
#endif  // #if (MBED_AUDIO_OUT_MODE == TIMEDPWM && EXTERNAL_AUDIO_OUTPUT != true)

static void startAudio() {
#if (MBED_AUDIO_OUT_MODE == TIMEDPWM && EXTERNAL_AUDIO_OUTPUT != true)
  pwmpin1.period_us(US_PER_UPDATE);
  pwmpin1.write(.5);
  #if (AUDIO_CHANNELS > 1)
  pwmpin2.period_us(US_PER_UPDATE);
  pwmpin2.write(.5);
  #endif
#endif
  audio_output_timer.attach_us(&defaultAudioOutputCallback, US_PER_AUDIO_TICK);
}

void stopMozzi() {
  audio_output_timer.detach();
}

#endif
////// END audio output code //////
