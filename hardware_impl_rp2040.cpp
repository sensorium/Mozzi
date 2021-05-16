/*
 * MozziRP2040.cpp
 * 
 * Mozzi Support for the Rasperry Pico for the Arduino Kernels which provide the full Rasperry Pico API
 * ATTENTION: The standard Arduino implementation uses the ARM Mbed! So this would be the wrong place
  
 * Copyright 2012 Tim Barrass.
 * Copyright 2021 Phil Schatzmann.
 * 
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#include "hardware_defines.h"

#if IS_RP2040() 

#include "Mozzi.h"
#include "CircularBuffer.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/time.h"

//-----------------------------------------------------------------------------------------------------------------
/// local variables
//-----------------------------------------------------------------------------------------------------------------

uint16_t update_control_timeout;
uint16_t update_control_counter;

uint pwm_slice_num;
alarm_id_t alarm_id=-1;
alarm_pool_t *ap;
repeating_timer_t timer;

// ring buffer for audio output
CircularBuffer<AudioOutput> output_buffer;  // fixed size 256
#define canBufferAudioOutput() (!output_buffer.isFull())
#define bufferAudioOutput(f) output_buffer.write(f)


//-----------------------------------------------------------------------------------------------------------------
// Some forware declarations that we can use method in logical sections
//-----------------------------------------------------------------------------------------------------------------
void setupOutputTimer();
bool defaultAudioOutputCallback(repeating_timer* ptr);
void setupADC(); 


//-----------------------------------------------------------------------------------------------------------------
/// Setup
//-----------------------------------------------------------------------------------------------------------------

// Setup PWM On 2 output Pins
void setupPWM() {
  // start pwm
  gpio_set_function(AUDIO_CHANNEL_1_PIN, GPIO_FUNC_PWM);
  if (CHANNELS>1){
    gpio_set_function(AUDIO_CHANNEL_2_PIN, GPIO_FUNC_PWM);
  }
  pwm_config cfg = pwm_get_default_config();
  pwm_config_set_clkdiv (&cfg, PWM_CLOCK_DIV);

  // Find out which PWM slice is connected to GPIO 0 (it's slice 0)
  pwm_slice_num = pwm_gpio_to_slice_num(AUDIO_CHANNEL_1_PIN);

  // Set period of AUDIO_BITS cycles (0 to AUDIO_BITS inclusive)
  pwm_set_wrap(pwm_slice_num, PWM_CYCLES);
  // Set channel A output high for 0 cycle before dropping
  pwm_set_chan_level(pwm_slice_num, PWM_CHAN_A, 0); // pin 14

  if (CHANNELS>1){
    // Set initial B output high for 0 cycles before dropping
    pwm_set_chan_level(pwm_slice_num, PWM_CHAN_B, 0); // pin 15
  }
  // Set the PWM running
  pwm_set_enabled(pwm_slice_num, true);
}

void setupOutputTimer() {
    uint64_t time = 1000000UL / AUDIO_RATE;
    if (!add_repeating_timer_ms(-time, defaultAudioOutputCallback, nullptr, &timer)){
      Serial.println("Error: alarm_pool_add_repeating_timer_us failed; no alarm slots available");
    }
}

void startAudio() {
  //analogWriteResolution(12);
  Serial.println("startAudioStandard");

  // this supports all AUDIO_MODE settings
  setupPWM();
  // setup timer for defaultAudioOutput
  setupOutputTimer();
}

void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

//-----------------------------------------------------------------------------------------------------------------
/// 

inline void advanceControlLoop() {
  if (!update_control_counter) {
    update_control_counter = update_control_timeout;
    updateControl();
    //adcStartReadCycle();
  } else {
    --update_control_counter;
  }
}


void MozziClass::audioHook() 
{
  if (canBufferAudioOutput()) {
    advanceControlLoop();
    bufferAudioOutput(updateAudio());
  }
}


//-----------------------------------------------------------------------------------------------------------------
// Class Methods: Start - Stop
//-----------------------------------------------------------------------------------------------------------------

void MozziClass::start(int control_rate_hz) {
  setupADC(); 
  startControl(control_rate_hz);
  startAudio();
  Serial.println("Mozzi started");
}

void MozziClass::stop() {
    if (alarm_id!=-1){
      cancel_repeating_timer(&timer);
    }
    pwm_set_enabled(pwm_slice_num, false);
}

unsigned long MozziClass::audioTicks() {
    return output_buffer.count();
}


unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}



//-----------------------------------------------------------------------------------------------------------------
// Input
//-----------------------------------------------------------------------------------------------------------------

#if (USE_AUDIO_INPUT == true)

void  setupADC() {
  adc_init();

  // Make sure GPIO is high-impedance, no pullups etc
  adc_gpio_init(AUDIO_INPUT_PIN);
  // Select ADC input 0 (GPIO26)
  adc_select_input(AUDIO_CHANNEL_IN);
}

int MozziClass::getAudioInput() { 
  // range 0x0, 0xFFFF
  uint16_t result = adc_read();
  return result;
}
#else

void setupADC(){ }

#endif


//-----------------------------------------------------------------------------------------------------------------
// Output
//-----------------------------------------------------------------------------------------------------------------
// Timer Callback
bool defaultAudioOutputCallback(repeating_timer* ptr) {
  audioOutput(output_buffer.read());
  return true;
}

inline void writePWM(uint channel, int16_t value){
  pwm_set_chan_level(pwm_slice_num, channel, value);
}


/// Output will always be on both pins!
inline void audioOutput(const AudioOutput f) {
  writePWM(PWM_CHAN_A, f+AUDIO_BIAS);
 #if (AUDIO_MODE == HIFI)
   writePWM(PWM_CHAN_A, (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
   writePWM(PWM_CHAN_B, (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
 #else
  writePWM(PWM_CHAN_A, f.l()+AUDIO_BIAS);
  #if (STEREO_HACK == true)
    writePWM(PWM_CHAN_B, f.r()+AUDIO_BIAS);
  #else
    writePWM(PWM_CHAN_B, f.l()+AUDIO_BIAS);
  #endif
#endif
}

#endif  // IS_RP2040
