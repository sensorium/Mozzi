/*
 * MozziStm32.cpp
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
#if IS_STM32() && USE_LEGACY_GUTS == false

#include "MozziSTM32.h"
#include "HardwareTimer.h"
#include "AudioConfigSTM32.h"
//#include <STM32ADC.h>  // Disabled, here. See AudioConfigSTM32.h


STM32ADC adc(ADC1);
uint8_t stm32_current_adc_pin;

#if (EXTERNAL_AUDIO_OUTPUT != true)
inline void audioOutput(const AudioOutput f)
{
#if (AUDIO_MODE == HIFI)
  pwmWrite(AUDIO_CHANNEL_1_PIN, (f.l()+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwmWrite(AUDIO_CHANNEL_1_PIN_HIGH, (f.l()+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
#else
  pwmWrite(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
#if (STEREO_HACK == true)
  pwmWrite(AUDIO_CHANNEL_2_PIN, f.r()+AUDIO_BIAS);
#endif
#endif
}
#endif


#if BYPASS_MOZZI_OUTPUT_BUFFER == true
uint64_t samples_written_to_buffer = 0;
#else
//-----------------------------------------------------------------------------------------------------------------
// ring buffer for audio output
CircularBuffer<AudioOutput_t> output_buffer;  // fixed size 256
//-----------------------------------------------------------------------------------------------------------------
#endif

//-----------------------------------------------------------------------------------------------------------------

////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)

// ring buffer for audio input
CircularBuffer<unsigned int> input_buffer; // fixed size 256

static boolean audio_input_is_available;
static int audio_input; // holds the latest audio from input_buffer
uint8_t adc_count = 0;

int MozziSTM32::getAudioInput() { return audio_input; }

static void startFirstAudioADC() {
  uint8_t dummy = AUDIO_INPUT_PIN;
  adc.setPins(&dummy, 1);
  adc.startConversion();
}

/*
static void receiveFirstAudioADC()
{
        // nothing
}
*/

static void startSecondAudioADC() {
  uint8_t dummy = AUDIO_INPUT_PIN;
  adc.setPins(&dummy, 1);
  adc.startConversion();
}

static void receiveSecondAudioADC() {
  if (!input_buffer.isFull())
    input_buffer.write(adc.getData());
}

void stm32_adc_eoc_handler()
{
  switch (adc_count) {
  case 0:
    // 6us
    receiveSecondAudioADC();
    adcReadSelectedChannels();
    break;

  case 1:
    // <2us, <1us w/o receive
    // receiveFirstControlADC();
    startSecondControlADC();
    break;

  case 2:
    // 3us
    receiveSecondControlADC();
    startFirstAudioADC();
    break;

    //      	case 3:
    // invisible
    //      	receiveFirstAudioADC();
    //      	break;
  }
  adc_count++;
}
#endif
////// END AUDIO INPUT code ////////




#if BYPASS_MOZZI_OUTPUT_BUFFER == true
inline void bufferAudioOutput(const AudioOutput_t f) {
  audioOutput(f);
  ++samples_written_to_buffer;
}
#else
#define canBufferAudioOutput() (!output_buffer.isFull())
#define bufferAudioOutput(f) output_buffer.write(f)
#endif

static uint16_t update_control_timeout;
static uint16_t update_control_counter;

inline void advanceControlLoop() {
  if (!update_control_counter) {
    update_control_counter = update_control_timeout;
    updateControl();
    adcStartReadCycle();
  } else {
    --update_control_counter;
  }
}

void MozziSTM32::audioHook() // 2us on AVR excluding updateAudio()
{
// setPin13High();
#if (USE_AUDIO_INPUT == true)
  if (!input_buffer.isEmpty())
    audio_input = input_buffer.read();
#endif

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
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
static void CACHED_FUNCTION_ATTR defaultAudioOutput() {
#if (USE_AUDIO_INPUT == true)
  adc_count = 0;
  startSecondAudioADC();
#endif
  audioOutput(output_buffer.read());
}
#endif

#if   IS_STM32 && (EXTERNAL_AUDIO_OUTPUT == true)
HardwareTimer audio_update_timer(2);
#else
HardwareTimer audio_update_timer(AUDIO_UPDATE_TIMER);
HardwareTimer audio_pwm_timer(AUDIO_PWM_TIMER);
#endif



static void startAudioStandard() {

  audio_update_timer.pause();
  //audio_update_timer.setPeriod(1000000UL / AUDIO_RATE);
  // Manually calculate prescaler and overflow instead of using setPeriod, to avoid rounding errors
  uint32_t period_cyc = F_CPU / AUDIO_RATE;
  uint16_t prescaler = (uint16_t)(period_cyc / 65535 + 1);
  uint16_t overflow = (uint16_t)((period_cyc + (prescaler / 2)) / prescaler);
  audio_update_timer.setPrescaleFactor(prescaler);
  audio_update_timer.setOverflow(overflow);
  audio_update_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  audio_update_timer.setCompare(TIMER_CH1,
                                1); // Interrupt 1 count after each update
  audio_update_timer.attachCompare1Interrupt(defaultAudioOutput);
  audio_update_timer.refresh();
  audio_update_timer.resume();

#if (EXTERNAL_AUDIO_OUTPUT != true)
  pinMode(AUDIO_CHANNEL_1_PIN, PWM);
#if (AUDIO_MODE == HIFI)
  pinMode(AUDIO_CHANNEL_1_PIN_HIGH, PWM);
#elif (STEREO_HACK == true)
  pinMode(AUDIO_CHANNEL_2_PIN, PWM);
#endif

#define MAX_CARRIER_FREQ (F_CPU / (1 << AUDIO_BITS_PER_CHANNEL))
#if MAX_CARRIER_FREQ < AUDIO_RATE
#error Configured audio resolution is definitely too high at the configured audio rate (and the given CPU speed)
#elif MAX_CARRIER_FREQ < (AUDIO_RATE * 3)
#warning Configured audio resolution may be higher than optimal at the configured audio rate (and the given CPU speed)
#endif

#if MAX_CARRIER_FREQ < (AUDIO_RATE * 5)
  // Generate as fast a carrier as possible
  audio_pwm_timer.setPrescaleFactor(1);
#else
  // No point in generating arbitrarily high carrier frequencies. In fact, if
  // there _is_ any headroom, give the PWM pin more time to swing from HIGH to
  // LOW and BACK, cleanly
  audio_pwm_timer.setPrescaleFactor((int)MAX_CARRIER_FREQ / (AUDIO_RATE * 5));
#endif
  audio_pwm_timer.setOverflow(
      1 << AUDIO_BITS_PER_CHANNEL); // Allocate enough room to write all
                                    // intended bits
#endif

}

// end STANDARD

//-----------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------

static void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void MozziSTM32::start(int control_rate_hz) {
  setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST
                   // in setup() if desired (not for Teensy 3.* )
  // delay(200); // so AutoRange doesn't read 0 to start with
  startControl(control_rate_hz);
#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) ||               \
    IS_STM32 // Sorry, this is really hacky. But on STM32 regular and HIFI
               // audio modes are so similar to set up, that we do it all in one
               // function.
  startAudioStandard();
#elif (AUDIO_MODE == HIFI)
  startAudioHiFi();
#endif
}

void MozziSTM32::stop() {
  audio_update_timer.pause();
  // ps - nointerrupts was never called so the following is not necessary: 
  // interrupts();
}

unsigned long MozziSTM32::audioTicks() {
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
  return output_buffer.count();
#else
  return (samples_written_to_buffer - (output_buffer_size - i2s_available()));
#endif
}

unsigned long MozziSTM32::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}

#endif
