/*
 * MozziTeensy3.cpp
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
#if IS_RP2040() 

#include "CircularBuffer.h"
#include "Mozzi.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
#include "AudioOutput.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// common variables
alarm_id_t alarm_id=-1;
alarm_pool_t *ap;
repeating_timer_t timer;
uint pwm_slice_num;

////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)
#error USE_AUDIO_INPUT not supported yet
#endif
////// END AUDIO INPUT code ////////

//-----------------------------------------------------------------------------------------------------------------
/// Logic with no Buffer
#if BYPASS_MOZZI_OUTPUT_BUFFER == true
#error USE_AUDIO_INPUT not supported yet
#endif

//-----------------------------------------------------------------------------------------------------------------
 /// Logic with Buffer

// ring buffer for audio output
#if (STEREO_HACK == true)
CircularBuffer<StereoOutput> output_buffer;  // fixed size 256
#else
CircularBuffer<AudioOutput_t> output_buffer;  // fixed size 256
#endif

#define canBufferAudioOutput() (!output_buffer.isFull())
#define bufferAudioOutput(f) output_buffer.write(f)

unsigned long MozziClass::audioTicks() {
    return output_buffer.count();
}

//-----------------------------------------------------------------------------------------------------------------

// Timer Callback
bool defaultAudioOutput(repeating_timer *t) {
  audioOutput(output_buffer.read());
  return true;
}

uint16_t update_control_timeout;
uint16_t update_control_counter;

inline void advanceControlLoop() {
  if (!update_control_counter) {
    update_control_counter = update_control_timeout;
    updateControl();
    //adcStartReadCycle();
  } else {
    --update_control_counter;
  }
}

void setupPWM() {
  // start pwm
  gpio_set_function(AUDIO_CHANNEL_1_PIN, GPIO_FUNC_PWM);
  gpio_set_function(AUDIO_CHANNEL_2_PIN, GPIO_FUNC_PWM);

  pwm_config cfg = pwm_get_default_config();
  pwm_config_set_clkdiv (&cfg, PWM_CLOCK_DIV);

  // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
  pwm_slice_num = pwm_gpio_to_slice_num(AUDIO_CHANNEL_1_PIN);

  // Set period of AUDIO_BITS cycles (0 to AUDIO_BITS inclusive)
  pwm_set_wrap(pwm_slice_num, PWM_CYCLES);
  // Set channel A output high for 0 cycle before dropping
  pwm_set_chan_level(pwm_slice_num, PWM_CHAN_A, 0); // pin 14
  // Set initial B output high for 0 cycles before dropping
  pwm_set_chan_level(pwm_slice_num, PWM_CHAN_B, 0); // pin 15
  // Set the PWM running
  pwm_set_enabled(pwm_slice_num, true);
}

void setupTimer() {
  // start output timer
  //ap = alarm_pool_create(ALARM_POOL_HARDWARE_ALARM_NUM, ALARM_POOL_HARDWARE_ALARM_COUNT);
  alarm_pool_init_default();
  ap = alarm_pool_get_default();
  uint64_t time = 1000000UL / AUDIO_RATE;
  if (!alarm_pool_add_repeating_timer_us(ap, -time, defaultAudioOutput, nullptr, &timer)){
    LOG_OUTPUT.println("Could not start timer");
  }

}

void startAudio() {
  //analogWriteResolution(12);
  LOG_OUTPUT.println("startAudioStandard");

  // this supports all AUDIO_MODE settings
  setupPWM();
  // setup timer for defaultAudioOutput
  setupTimer();
}

//-----------------------------------------------------------------------------------------------------------------
// Start - Stop

void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void MozziClass::start(int control_rate_hz) {
  // setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST
  //                  // in setup() if desired (not for Teensy 3.* )
  // // delay(200); // so AutoRange doesn't read 0 to start with
  startControl(control_rate_hz);
  startAudio();
}

void MozziClass::stop() {
  if (alarm_id!=-1){
    cancel_repeating_timer(&timer);
  }
}

unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}

void MozziClass::audioHook() 
{
  if (canBufferAudioOutput()) {
    advanceControlLoop();

    #if (STEREO_HACK == true)
        updateAudio(); // in hacked version, this returns void
        bufferAudioOutput(StereoOutput(audio_out_1, audio_out_2));
    #else
        bufferAudioOutput(updateAudio());
    #endif

  }
  // setPin13Low();
}

//-----------------------------------------------------------------------------------------------------------------
/// Output

inline void pwmWrite(int channel, int16_t value){
  pwm_set_chan_level(pwm_slice_num, channel, value);
  LOG_OUTPUT.println(value);
}

/// Output will always be on both pins!
inline void audioOutput(const AudioOutput f) {
#if (AUDIO_MODE == HIFI)
  pwmWrite(PWM_CHAN_A, (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwmWrite(PWM_CHAN_B, (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#else
  pwmWrite(PWM_CHAN_A, f.l()+AUDIO_BIAS);
  #if (STEREO_HACK == true)
    pwmWrite(PWM_CHAN_B, f.r()+AUDIO_BIAS);
  #else
    pwmWrite(PWM_CHAN_B, f.l()+AUDIO_BIAS);
  #endif
#endif
}

#endif  // IS_RP2040
