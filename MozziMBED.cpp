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

//-----------------------------------------------------------------------------------------------------------------
/// bufferAudioOutput

#if BYPASS_MOZZI_OUTPUT_BUFFER == true
inline void bufferAudioOutput(const AudioOutput_t f) {
  audioOutput(f);
  ++samples_written_to_buffer;
}
#else
// ring buffer for audio output
CircularBuffer<OUTPUT_TYPE> output_buffer;  // fixed size 256
#define canBufferAudioOutput() (!output_buffer.isFull())
#define bufferAudioOutput(f) output_buffer.write(f)
#endif

//-----------------------------------------------------------------------------------------------------------------
/// MBED Implementation

using namespace std::chrono;
using namespace mbed;

Ticker ticker; // calls a callback repeatedly with a timeout
PwmOut pin1(digitalPinToPinName(AUDIO_CHANNEL_1_PIN));
PwmOut pin2(digitalPinToPinName(AUDIO_CHANNEL_2_PIN));

void defaultAudioOutput() {
  strcpy(debug_buffer,"defaultAudioOutput-begin");
  audioOutput(output_buffer.read());
  strcpy(debug_buffer,"defaultAudioOutput-end");
}

void setupTimer() {
    //ticker.attach(defaultAudioOutput, 1000000us / AUDIO_RATE );
    //ticker.attach(defaultAudioOutput, 1s );
    //ticker.attach(defaultAudioOutput,1.0);
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

const uint MAX = 2*AUDIO_BIAS;

inline void writePWM(PwmOut &pin, int16_t value){
  strcpy(debug_buffer,"writePWM-1");
  float float_value = static_cast<float>(value) / MAX;
  strcpy(debug_buffer,"writePWM-2");
  // pwm the value is between 0.0 and 1.0 
  sprintf(debug_buffer, "test: %d - %f", sizeof(OUTPUT_TYPE), float_value );
  strcpy(debug_buffer,"writePWM-3");
  //pin.write(float_value);  
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
  strcpy(debug_buffer,"audioHook");
  if (canBufferAudioOutput()) {
    strcpy(debug_buffer,"audioHook-1");
    advanceControlLoop();
    strcpy(debug_buffer,"audioHook-2");

    #if (STEREO_HACK == true)
        updateAudio(); // in hacked version, this returns void
        bufferAudioOutput(StereoOutput(audio_out_1, audio_out_2));
    #else
        strcpy(debug_buffer,"audioHook-3");
        bufferAudioOutput(updateAudio());
        strcpy(debug_buffer,"audioHook-4");
    #endif
  }
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

//-----------------------------------------------------------------------------------------------------------------
/// Output
char debug_buffer[80] = {'N','A',0};

/// Output will always be on both pins!
inline void audioOutput(const AudioOutput audio_output) {
  strcpy(debug_buffer,"audioOutput");

#if (AUDIO_MODE == HIFI)
  writePWM(pin1, (audio_output.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  writePWM(pin2, (audio_output.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#else
  writePWM(pin1, audio_output.l()+AUDIO_BIAS);
  #if (STEREO_HACK == true)
    writePWM(pin2, audio_output.r()+AUDIO_BIAS);
  #else
    writePWM(pin2, audio_output.l()+AUDIO_BIAS);
  #endif
#endif
}

#endif  // IS_RP2040