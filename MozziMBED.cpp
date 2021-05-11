/*
 * MozziMBED.cpp
 * 
 * Mozzi Support for Microporcessors which use the arm Mbed OS (e.g. Raspberry Pico)
 * 
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

#if IS_MBED() 

#include "Mozzi.h"
#include "pinDefinitions.h"
#include "CircularBuffer.h"
//#include "AudioOutput.h"
#include "mbed.h"


// ring buffer for audio output
#if (STEREO_HACK == true)
CircularBuffer<StereoOutput> output_buffer;  // fixed size 256
#else
CircularBuffer<AudioOutput_t> output_buffer;  // fixed size 256
#endif

#define canBufferAudioOutput() (!output_buffer.isFull())
#define bufferAudioOutput(f) output_buffer.write(f)


//-----------------------------------------------------------------------------------------------------------------
/// MBED Implementation

using namespace std::chrono;
using namespace mbed;

Ticker ticker; // calls a callback repeatedly with a timeout
PwmOut pin1(digitalPinToPinName(AUDIO_CHANNEL_1_PIN));
PwmOut pin2(digitalPinToPinName(AUDIO_CHANNEL_2_PIN));

void defaultAudioOutput() {
  audioOutput(output_buffer.read());
}

void setupTimer() {
    ticker.attach(defaultAudioOutput, 1000000us / AUDIO_RATE );
}

void setupPWM() {
  unsigned long period = 1000000l / PWM_RATE;  // -> 30.517578125 microseconds
  pin1.period_us(period);  
  pin1.period_us(period);
  pin1.write(0.0f);  // 0% duty cycle ->  
  pin2.write(0.0f);  // 0% duty cycle -> value between 0 and 1
  pin1.resume(); // in case it was suspended before
  pin2.resume(); //in case it was suspended before
}

inline void writePWM(PwmOut &pin, int16_t value){
  float float_value = static_cast<float>(value) / AUDIO_BIAS;
  pin.write(0.0f);  
}


//-----------------------------------------------------------------------------------------------------------------
/// Logic with Buffer

unsigned long MozziClass::audioTicks() {
    return output_buffer.count();
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
  ticker.detach();
  pin1.suspend();
  pin2.suspend();
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
}

//-----------------------------------------------------------------------------------------------------------------
/// Input -> This might not be optimal but we just use the MBED AnalogIn )
#if (USE_AUDIO_INPUT == true)
AnalogIn analog_in(AUDIO_CHANNEL_1_PIN);

int MozziClass::getAudioInput() { 
  // range 0x0, 0xFFFF
  int result analog_in.read_u16(); 
  return result;
}
#endif

//-----------------------------------------------------------------------------------------------------------------
/// Output

/// Output will always be on both pins!
inline void audioOutput(const AudioOutput f) {
#if (AUDIO_MODE == HIFI)
  writePWM(pin1, (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  writePWM(pin2, (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#else
  writePWM(pin1, f.l()+AUDIO_BIAS);
  #if (STEREO_HACK == true)
    writePWM(pin2, f.r()+AUDIO_BIAS);
  #else
    writePWM(pin2, f.l()+AUDIO_BIAS);
  #endif
#endif
}

#endif  // IS_RP2040
