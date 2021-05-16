/*
 * MozziMBED.cpp
 * 
 * Mozzi Support for Microporcessors which use the ARM Mbed OS (e.g. Raspberry Pico)
 *
 * Design Decisions:
 * - We avoid preprocessor statements because ARM processors have enough resources and because if is easier to catch compile errors
 * - We avoid any low level APIs - to be as understandable as possible
 * - These decisions are valid for this implementation only!
 *
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
#include "CircularBuffer.h"
#include "AudioConfigArmMbed.h"
#include "pinDefinitions.h"
#include "mbed.h"

//-----------------------------------------------------------------------------------------------------------------
/// Variables
//-----------------------------------------------------------------------------------------------------------------

mbed::Ticker ticker; // calls a callback repeatedly with a timeout
mbed::Ticker inputTicker; // calls a callback repeatedly with a timeout
mbed::PwmOut *pwm_pins[AUDIO_CHANNELS];
uint16_t update_control_timeout;
uint16_t update_control_counter;
const uint MAX_VALUE = 2*AUDIO_BIAS;
uint8_t max_channel = 0; // last valid channel with a pin defined
CircularBuffer<AudioOutput_t> output_buffer;
char msg[80];

//-----------------------------------------------------------------------------------------------------------------
// forward declaration of some local methods 
//-----------------------------------------------------------------------------------------------------------------

void writePWM(mbed::PwmOut &pin, int16_t value);
void defaultAudioOutputCallback();
void defaultAudioInputCallback();
bool canBufferAudioOutput();
void bufferAudioOutput(const AudioOutput_t f);
void audioOutput(AudioOutput_t audio_output);
void adcStartReadCycle();
void setupInput();
void stopInput();

//-----------------------------------------------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------------------------------------------

void setupTimer() {
    // to test we use an interval of 1 sec
    //ticker.attach(defaultAudioOutputCallback,1.0);

    // We use 1000000l / AUDIO_RATE => 1000000l / 16000 => 62 microsends 
    long wait_time = 1000000l / AUDIO_RATE;
    ticker.attach_us(defaultAudioOutputCallback, wait_time);
}


void setupPWMPin(mbed::PwmOut &pin){
  unsigned long period = 1000000l / PWM_RATE;  // -> 30.517578125 microseconds
  pin.period_us(period);  
  pin.write(0.0f);  // 0% duty cycle ->  
  pin.resume(); // in case it was suspended before
}


void setupPWM(const int16_t pins[]) {
  for (int j=0;j<AUDIO_CHANNELS;j++){
    int16_t gpio = pins[j];
    if  (pwm_pins[j] == nullptr && gpio >- 1)  {
      mbed::PwmOut *pin = new mbed::PwmOut(digitalPinToPinName(gpio));
      pwm_pins[j] = pin;
      if (pin!=nullptr) {
        sprintf(msg,"Channel %d -> pin %d", j, gpio);
        Serial.println(msg);
        setupPWMPin(*pin);
        max_channel = j + 1;
      } else {
        sprintf(msg,"Channel %d -> pin could not be defined", j);
        Serial.println(msg);
      }
    } else {
        sprintf(msg,"Channel %d -> pin %d is already defined", j, gpio);
        Serial.println(msg);
    }
  }
}

//-----------------------------------------------------------------------------------------------------------------
// Start - Stop - Control

void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void startAudio(const int16_t pins[]) {
  if (USE_AUDIO_INPUT) {
    setupInput();
  }

  // this supports all AUDIO_MODE settings
  if (!EXTERNAL_AUDIO_OUTPUT)
    setupPWM(pins);

  // setup timer for defaultAudioOutput
  setupTimer();
}

void MozziClass::start(int control_rate_hz) {
  startControl(control_rate_hz);
  startAudio(pins());

  sprintf(msg,"Mozzi started on %d channels", max_channel);
  Serial.println(msg);
}


void MozziClass::stop() {
  ticker.detach(); // it does not hurt to call this even if it has not been started

  // stop all pins
  for (int j=0;j<max_channel;j++){
    if (pwm_pins[j] != nullptr)  {
      pwm_pins[j]->suspend();
    } 
  }
  stopInput();
}

unsigned long MozziClass::audioTicks() {
  return output_buffer.count();
}

unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}

inline void advanceControlLoop() {
  if (!update_control_counter) {
    update_control_counter = update_control_timeout;
    updateControl();
    adcStartReadCycle();
  } else {
    --update_control_counter;
  }
}

void MozziClass::audioHook()  {
  #if (USE_AUDIO_INPUT == true) 
    if (!input_buffer.isEmpty()) {
      adc_in_value = input_buffer.read();
    }
  #endif

  if (canBufferAudioOutput()) {
    advanceControlLoop();
    bufferAudioOutput(updateAudio());
  }
}

//-----------------------------------------------------------------------------------------------------------------
/// Input -> This might not be optimal but we just use the MBED AnaSerialIn )
//-----------------------------------------------------------------------------------------------------------------

#if USE_AUDIO_INPUT == true
mbed::AnalogIn *analog_in = nullptr;
CircularBuffer<AudioOutput_t> input_buffer;  
AudioOutput_t adc_in_value = 0;

void defaultAudioInputCallback() {
  int value = analog_in->read_u16(); 
  // range 0x0, 0xFFFF -> -AUDIO_BIAS, +AUDIO_BIAS
  if (!input_buffer.isFull()){
    input_buffer.write(map(value,0x0,0xFFFF,-AUDIO_BIAS,AUDIO_BIAS));
  }
}


void setupInput() {
    // We use 1000000l / AUDIO_RATE => 1000000l / 16000 => 62 microsends 
    long wait_time = 1000000l / AUDIO_RATE;
    ticker.attach_us(defaultAudioInputCallback, wait_time);

    if (analog_in==nullptr){
      analog_in = new mbed::AnalogIn(digitalPinToPinName(AUDIO_INPUT_PIN));
    }
}

void stopInput() {
  inputTicker.detach();  // it does not hurt to call this even if it has not been started
  // rmove analog pin
  if (analog_in!=nullptr)
    delete analog_in;
}

// Provides the analog value w/o delay
int16_t MozziClass::getAudioInput() { 
  return adc_in_value;
}

#else
void setupInput() {}
void stopInput() {}

#endif

//-----------------------------------------------------------------------------------------------------------------
/// Output 
//-----------------------------------------------------------------------------------------------------------------

void defaultAudioOutputCallback() {
  if (!output_buffer.isEmpty()){
    audioOutput(output_buffer.read());
  }
}

bool canBufferAudioOutput(){
  return !output_buffer.isFull();
} 

void bufferAudioOutput(const AudioOutput_t f){
   output_buffer.write(f);
}

// mono output
inline void audioOutput(const AudioOutputStorage_t value) {
    float float_value = static_cast<float>(value+AUDIO_BIAS / MAX_VALUE);
    pwm_pins[0]->write(float_value);    // pwm the value is between 0.0 and 1.0 
}

// output an yn channel
inline void audioOutput(const AudioOutput value) {
    // output values to pins
    for (int j=0;j<max_channel;j++){
      AudioOutputStorage_t v = value[j] + AUDIO_BIAS;
      float float_value = static_cast<float>(v / MAX_VALUE);
      pwm_pins[j]->write(float_value);    // pwm the value is between 0.0 and 1.0 
    }
}



#endif  // IS_MBED
