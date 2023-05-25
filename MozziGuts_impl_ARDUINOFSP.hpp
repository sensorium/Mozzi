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

#if !(IS_FSP())
#  error "Wrong implementation included for this platform"
#endif

#include <FspTimer.h>


////// BEGIN analog input code ////////

//#define MOZZI_FAST_ANALOG_IMPLEMENTED

#define getADCReading() 0


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



//// BEGIN AUDIO OUTPUT code ///////
FspTimer timer1;

#if (EXTERNAL_AUDIO_OUTPUT != true) // otherwise, the last stage - audioOutput() - will be provided by the user
inline void audioOutput(const AudioOutput f) {
  analogWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
#if (AUDIO_CHANNELS > 1)
  analogWrite(AUDIO_CHANNEL_2_PIN, f.r()+AUDIO_BIAS);
#endif
}
#endif

void timer_callbar_dummy(timer_callback_args_t __attribute__((unused)) *args){defaultAudioOutput();};

static void startAudio() {

  pinMode(AUDIO_CHANNEL_1_PIN,OUTPUT);
  timer1.set_period(1.0 / AUDIO_RATE);
  timer1.set_irq_callback(timer_callbar_dummy);


  uint8_t type=0;
  int8_t tindex = FspTimer::get_available_timer(type);

    if (tindex < 0) {
    tindex = FspTimer::get_available_timer(type, true);
  }

  if (tindex >= 0) {
    // defaultAudioOutput cannot be used straight, so we are using a dummy, with no argument, linking to defaultAudioOutput
    timer1.begin(TIMER_MODE_PERIODIC, type, tindex,AUDIO_RATE, 50.0f, timer_callbar_dummy);
    timer1.setup_overflow_irq();
    timer1.open();
    timer1.start();
  }

  
  //Serial.println("sa");
  //pinMode(AUDIO_CHANNEL_1_PIN,OUTPUT);

  // Add here code to get audio output going. This usually involves:
  // 1) setting up some DAC mechanism (e.g. setting up a PWM pin with appropriate resolution
  // 2a) setting up a timer to call defaultAudioOutput() at AUDIO_RATE
  // OR 2b) setting up a buffered output queue such as I2S (see ESP32 / ESP8266 for examples for this setup)
#if (EXTERNAL_AUDIO_OUTPUT != true)
  // remember that the user may configure EXTERNAL_AUDIO_OUTPUT, in which case, you'll want to provide step 2a), and only that.
  #endif
}

void stopMozzi() {
  timer1.stop();
}
//// END AUDIO OUTPUT code ///////
