/*
 * MozziGuts_impl_template.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

/** @file MozziGuts_impl_template.hpp Template for implementation of new ports
 *
 * README!
 * This file is meant to be used as a template when adding support for a new platform. Please read these instructions, first.
 *
 *  Files involved:
 *  1. Modify hardware_defines.h, adding a macro to detect your target platform
 *  2. Modify MozziGuts.cpp to include MozziGuts_impl_YOURPLATFORM.hpp
 *  3. Modify internal/config_checks_generic.h to include internal/config_checks_YOURPLATFORM.h
 *  4. Copy this file to MozziGuts_impl_YOURPLATFORM.hpp and adjust as necessary, same for internal/config_checks_template.h
 *  (If your platform is very similar to an existing port, it may instead be better to modify the existing MozziGuts_impl_XYZ.hpp/config_checks_XYZ.h,
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
#if !(IS_MYPLATFORM())
#  error "Wrong implementation included for this platform"
#endif
// Add platform specific includes and declarations, here

//#include <my_hardware_header.h>

// In order to allow simple yet efficient user configuration, the entire contents of this file are compiled in the same translation unit
// as (the main file of) the user sketch. To avoid name clashes, we encapsulate everyghing in a namespace.
// For the most part, this namescape can just extend from the start of the file to the end (as shown, here), and you will not have to
// worry about it. However, there may be a few situations, where you have to "leave" the MozziPrivate namespace. This includes:
// - When you include a further (hardware-dependent library). Consider gathering all includes at the top of this file, instead.
// - When you provide definitions for special names, importantly for ISR-functions. If these would be placed in the namespace, the linker would not
//   recognize them as the definition of the intended ISR-vector. See MozziGuts_impl_AVR.hpp for examples.

namespace MozziPrivate {

////// BEGIN analog input code ////////

#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
/** NOTE: This section deals with implementing (fast) asynchronous analog reads, which form the backbone of mozziAnalogRead(), but also of MOZZI_AUDIO_INPUT (if enabled).
 *
 * It is possible, and even recommended, to skip over this section, initially, when starting a new port. Once you have an implementation, be sure to include something like this
 * in your platform configuration checks:
 *
 * // analog reads shall be enabled by default on platforms that support it 
 * #if not defined(MOZZI_ANALOG_READ)
 * #define MOZZI_ANALOG_READ MOZZI_ANALOG_READ_STANDARD
 * #endif
 *
 * Also, of course, remove the #error line, below
 */
#error not yet implemented

// Insert here code to read the result of the latest asynchronous conversion, when it is finished.
// You can also provide this as a function returning unsigned int, should it be more complex on your platform
#define getADCReading() GET_MY_PLATFORM_ADC_REGISTER

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
}

/** NOTE: Code needed to trigger a subsequent conversion on the latest channel. If your platform has no special code for it, you should store the channel from
 *  adcStartConversion(), and simply call adcStartConversion(previous_channel), here. */
void startSecondADCReadOnCurrentChannel() {
}

/** NOTE: Code needed to initialize the ADC for asynchronous reads. Typically involves setting up an interrupt handler for when conversion is done, and
 *  possibly calibration. */
void setupMozziADC(int8_t speed) {
  setupFastAnalogRead(speed);
  // insert further custom code
}

/* NOTE: Most platforms call a specific function/ISR when conversion is complete. Provide this function, here.
 * From inside its body, simply call advanceADCStep(). E.g.:
void stm32_adc_eoc_handler() {
  advanceADCStep();
}
*/

/** NOTE: Code needed to set up faster than usual analog reads, e.g. specifying the number of CPU cycles that the ADC waits for the result to stabilize.
 *  This particular function is not super important, so may be ok to leave empty, at least, if the ADC is fast enough by default. */
void setupFastAnalogRead(int8_t speed) {
}

#endif

////// END analog input code ////////

////// BEGIN audio output code //////
/* NOTE: Some platforms rely on control returning from loop() every so often. However, updateAudio() may take too long (it tries to completely fill the output buffer,
 * which of course is being drained at the same time, theoretically it may not return at all). If you set this define, it will be called once per audio frame to keep things
 * running smoothly. */
//#define LOOP_YIELD yield();

/* NOTE: On some platforms, what can be called in the ISR used to output the sound is limited.
 * This define can be used, for instance, to output the sound in audioHook() instead to overcome
 * this limitation (see MozziGuts_impl_MBED.hpp). It can also be used if something needs to be called in audioHook() regarding
 * analog reads for instance. */
//#define AUDIO_HOOK_HOOK

/* NOTE: Code sections that are needed for a certain audio mode, only, should be guarded as follows (in this example, code will compile for the
 * two modes MOZZI_OUTPUT_PWM, and MOZZI_OUTPUT_INTERNAL_DAC (should your port happen to support these two).
 *
 * Keep in mind that you probably want to support MOZZI_OUTPUT_EXTERNAL_TIMED, and MOZZI_OUTPUT_EXTERNAL_CUSTOM, too, which is usually very
 * easy: For both, do *not* provide an audioOutput() function, as this will be provided by the user. For MOZZI_OUTPUT_EXTERNAL_TIMED make sure some
 * timer is set up to call defaultAudioOutput() at MOZZI_AUDIO_RATE. For MOZZI_OUTPUT_EXTERNAL_CUSTOM, nothing else will be needed. */

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_INTERNAL_DAC)  // just an example!
/** NOTE: This is the function that actually write a sample to the output. In case of the two EXTERNAL modes, it is provided by the library user, instead. */
inline void audioOutput(const AudioOutput f) {
  // e.g. analogWrite(MOZZI_AUDIO_CHANNEL_1_PIN, f.l()+MOZZI_AUDIO_BIAS);
#  if (MOZZI_AUDIO_CHANNELS > 1)
  // e.g. analogWrite(MOZZI_AUDIO_CHANNEL_2_PIN, f.r()+MOZZI_AUDIO_BIAS);
#  endif
}
#endif

static void startAudio() {
  // Add here code to get audio output going. This usually involves:
  // 1) setting up some DAC mechanism (e.g. setting up a PWM pin with appropriate resolution
  // 2a) setting up a timer to call defaultAudioOutput() at MOZZI_AUDIO_RATE
  // OR 2b) setting up a buffered output queue such as I2S (see ESP32 / ESP8266 for examples for this setup)
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
  // [...]
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
  // [...]
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
  // remember that the user may configure MOZZI_OUTPUT_EXTERNAL_TIMED, in which case, you'll want to provide step 2a), and only that.
#endif
}

void stopMozzi() {
  // Add here code to pause whatever mechanism moves audio samples to the output
}
////// END audio output code //////

//// BEGIN Random seeding ////////
void MozziRandPrivate::autoSeed() {
  // Add here code to initialize the values of MozziRandPrivate::x, MozziRandPrivate::y, and MozziRandPrivate::z to some random values
  // This doesn't need to be crypographically safe. If nothing better is available, e.g. try reading an internal temperature sensor
  // in order to get some noise. It also doesn't have to be fast.
  // It *should* however ensure that rand() sequences will differ across reboots, after randSeed() has been called.
  // x, y, and z are already initialized to non-zero, when this function is called.
  // It's ok to leave this unimplemented, initially.
#warning Automatic random seeding is not implemented on this platform
}
//// END Random seeding ////////

} // namespace MozziPrivate
