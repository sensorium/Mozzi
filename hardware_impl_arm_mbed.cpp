/*
 * MozziMBED.cpp
 * 
 * Mozzi Support for Microporcessors which use the ARM Mbed OS (e.g. Raspberry Pico)
 * 
 * Copyright 2012 Tim Barrass.
 * Copyright 2021 Phil Schatzmann.
 * 
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *()
 */

#include "hardware_defines.h"

#if IS_MBED() 

#include "Mozzi.h"
#include "pinDefinitions.h"
#include "CircularBuffer.h"
#include "mbed.h"


//-----------------------------------------------------------------------------------------------------------------
/// MBED Implementation

mbed::Ticker ticker; // calls a callback repeatedly with a timeout
mbed::PwmOut *pwm_pins[CHANNELS];
uint16_t update_control_timeout;
uint16_t update_control_counter;
const uint MAX = 2*AUDIO_BIAS;
char debug_buffer[80] = {'N','A',0};

//-----------------------------------------------------------------------------------------------------------------
/// Output 

#if BYPASS_MOZZI_OUTPUT_BUFFER == true
void bufferAudioOutput(const MultiChannelOutput f) {
  audioOutput(f);
  ++samples_written_to_buffer;
}
#else
// ring buffer for audio output
CircularBuffer<MultiChannelOutput> output_buffer;  // fixed size 256

bool canBufferAudioOutput(){
  return !output_buffer.isFull();
} 

void bufferAudioOutput(const MultiChannelOutput f){
   output_buffer.write(f);
}
#endif

// forward declaration
void writePWM(mbed::PwmOut &pin, int16_t value);

void audioOutput(MultiChannelOutput audio_output) {
#if (AUDIO_MODE == HIFI)
  writePWM(*pwm_pins[0], (audio_output[0].l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  writePWM(*pwm_pins[1], (audio_output[1].l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#else
  for (int j=0;j<CHANNELS;j++){
    if (pwm_pins[j]!=nullptr){
      writePWM(*pwm_pins[j], audio_output[j].l()+AUDIO_BIAS);
    }
  }
#endif
}

void defaultAudioOutputCallback() {
  audioOutput(output_buffer.read());
}

void setupTimer() {
    // 1000000l / 32768 -> 30 microsends
    ticker.attach_us(defaultAudioOutputCallback, 1000000l / AUDIO_RATE );
    //ticker.attach(defaultAudioOutputCallback,1.0);
}

void setupPWMPin(mbed::PwmOut &pin){
  unsigned long period = 1000000l / PWM_RATE;  // -> 30.517578125 microseconds
  pin.period_us(period);  
  pin.write(0.0f);  // 0% duty cycle ->  
  pin.resume(); // in case it was suspended before
}

void setupPWM(const uint8_t *pins) {
  for (int j=0;j<CHANNELS;j++){
    int gpio = pins[j];
    if  (pwm_pins[j] != nullptr && gpio!=-1)  {
      mbed::PwmOut *pin = new mbed::PwmOut(digitalPinToPinName(gpio));
      pwm_pins[j] = pin;
      if (pin!=nullptr){
        setupPWMPin(*pin);
      }
    } 
  }
}

void writePWM(mbed::PwmOut &pin, int16_t value){
  float float_value = static_cast<float>(value) / MAX;
  // pwm the value is between 0.0 and 1.0 
  pin.write(float_value);  
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

//-----------------------------------------------------------------------------------------------------------------
// Start - Stop

void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void startAudio(const uint8_t *pins) {
  //analogWriteResolution(12);
  LOG_OUTPUT.println("startAudioStandard");

  // this supports all AUDIO_MODE settings
  setupPWM(pins);
  // setup timer for defaultAudioOutput
  setupTimer();
}

void MozziClass::start(int control_rate_hz) {
  // setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST
  //                  // in setup() if desired (not for Teensy 3.* )
  // // delay(200); // so AutoRange doesn't read 0 to start with
  startControl(control_rate_hz);
  startAudio(pins());
}

void MozziClass::stop() {
  ticker.detach();

  // stop all pins
  for (int j=0;j<CHANNELS;j++){
    if (pwm_pins[j] != nullptr)  {
      pwm_pins[j]->suspend();
    } 
  }
}

unsigned long MozziClass::audioTicks() {
    return output_buffer.count();
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
/// Input -> This might not be optimal but we just use the MBED AnalogIn )
#if (USE_AUDIO_INPUT == true)
AnalogIn analog_in(AUDIO_CHANNEL_1_PIN);

int MozziClass::getAudioInput() { 
  // range 0x0, 0xFFFF -> -AUDIO_BIAS, +AUDIO_BIAS
  int value = analog_in.read_u16(); 
  return map(value,0x0,0xFFFF,-AUDIO_BIAS,AUDIO_BIAS);
}
#endif


#endif  // IS_RP2040
