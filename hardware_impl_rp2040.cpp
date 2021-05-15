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
CircularBuffer<MultiChannelOutput> output_buffer;  // fixed size 256
#define canBufferAudioOutput() (!output_buffer.isFull())
#define bufferAudioOutput(f) output_buffer.write(f)


//-----------------------------------------------------------------------------------------------------------------
// Some forware declarations that we can use method in logical sections
//-----------------------------------------------------------------------------------------------------------------
void setupOutputTimer();
void defaultAudioOutput();


//-----------------------------------------------------------------------------------------------------------------
/// Setup
//-----------------------------------------------------------------------------------------------------------------

// Setup PWM On 2 output Pins
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




//-----------------------------------------------------------------------------------------------------------------
/// Logic with Buffer

unsigned long MozziClass::audioTicks() {
    return output_buffer.count();
}



inline void advanceControlLoop() {
  if (!update_control_counter) {
    update_control_counter = update_control_timeout;
    updateControl();
    //adcStartReadCycle();
  } else {
    --update_control_counter;
  }
}

void startAudio() {
  //analogWriteResolution(12);
  LOG_OUTPUT.println("startAudioStandard");

  // this supports all AUDIO_MODE settings
  setupPWM();
  // setup timer for defaultAudioOutput
  setupOutputTimer();
}

//-----------------------------------------------------------------------------------------------------------------
/// Input -> This might not be optimal but we just use the Pico API adc_read 
#if (USE_AUDIO_INPUT == true)

void  setupADC() {
  adc_init();

  // Make sure GPIO is high-impedance, no pullups etc
  adc_gpio_init(AUDIO_CHANNEL_1_PIN);
  // Select ADC input 0 (GPIO26)
  adc_select_input(AUDIO_CHANNEL_IN);
}

int MozziClass::getAudioInput() { 
  // range 0x0, 0xFFFF
  uint16_t result = adc_read();
  return result;
}
#else
#define setupInput()
#endif

//-----------------------------------------------------------------------------------------------------------------
// Class Methods: Start - Stop
//-----------------------------------------------------------------------------------------------------------------

void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void MozziClass::start(int control_rate_hz) {
  setupADC(); 
  startControl(control_rate_hz);
  startAudio();
}

void MozziClass::stop() {
    if (alarm_id!=-1){
      cancel_repeating_timer(&timer);
    }
    pwm_set_enabled(pwm_slice_num, false);
}

unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}

void MozziClass::audioHook() 
{
  if (canBufferAudioOutput()) {
    advanceControlLoop();
    MultiChannelOutput out;
    if (updateAudio!=nullptr){
      out[0]=updateAudio();
      bufferAudioOutput(out);
    } else {
      if (updateAudioN!=nullptr){
        updateAudioN(CHANNELS, out);
        bufferAudioOutput(out);
      }
    }
  }
  yield();
}

//-----------------------------------------------------------------------------------------------------------------
// Input
//-----------------------------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------------------------
// Output
//-----------------------------------------------------------------------------------------------------------------
// Timer Callback
void defaultAudioOutput() {
  audioOutput(output_buffer.read());
}

void setupOutputTimer() {
    uint64_t time = 1000000UL / AUDIO_RATE;
    if (!add_repeating_timer_ms(-time, defaultAudioOutput, nullptr, &timer)){
      LOG_OUTPUT.println("Error: alarm_pool_add_repeating_timer_us failed; no alarm slots available");
    }
}


inline void writePWM(uint channel, int16_t value){
  pwm_set_chan_level(channel==1 ?PWM_CHAN_A:PWM_CHAN_B, channel, value);
}

/// Output will always be on both pins!
inline void audioOutput(const MultiChannelOutput f) {
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
