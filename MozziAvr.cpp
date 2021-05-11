/*
 * MozziAvr.cpp
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
#if IS_AVR() && USE_LEGACY_GUTS == false

#include "CircularBuffer.h"
#include "Mozzi.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
#include "AudioOutput.h"
#include "FrequencyTimer2.h"
#include "TimerOne.h"


#if (F_CPU != 16000000)
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino and 48MHz on Teensy 3!  Results may vary with other speeds."
#endif


/*
ATmega328 technical manual, Section 12.7.4:
The dual-slope operation [of phase correct pwm] has lower maximum operation
frequency than single slope operation. However, due to the symmetric feature
of the dual-slope PWM modes, these modes are preferred for motor control
applications.
Due to the single-slope operation, the operating frequency of the
fast PWM mode can be twice as high as the phase correct PWM mode that use
dual-slope operation. This high frequency makes the fast PWM mode well suited
for power regulation, rectification, and DAC applications. High frequency allows
physically small sized external components (coils, capacitors)..
DAC, that's us!  Fast PWM.
PWM frequency tests
62500Hz, single 8 or dual 16 bits, bad aliasing
125000Hz dual 14 bits, sweet
250000Hz dual 12 bits, gritty, if you're gonna have 2 pins, have 14 bits
500000Hz dual 10 bits, grittier
16384Hz single nearly 9 bits (original mode) not bad for a single pin, but
carrier freq noise can be an issue
*/

#if BYPASS_MOZZI_OUTPUT_BUFFER == true
uint64_t samples_written_to_buffer = 0;
#else
//-----------------------------------------------------------------------------------------------------------------
// ring buffer for audio output
CircularBuffer<OUTPUT_TYPE> output_buffer;  // fixed size 256
//-----------------------------------------------------------------------------------------------------------------
#endif

             // teensy 3.*, other ARMs

// to store backups of timer registers so Mozzi can be stopped and pre_mozzi
// timer values can be restored
static uint8_t pre_mozzi_TCCR1A, pre_mozzi_TCCR1B, pre_mozzi_OCR1A,
    pre_mozzi_TIMSK1;

#if (AUDIO_MODE == HIFI)
#if defined(TCCR2A)
static uint8_t pre_mozzi_TCCR2A, pre_mozzi_TCCR2B, pre_mozzi_OCR2A,
    pre_mozzi_TIMSK2;
#elif defined(TCCR2)
static uint8_t pre_mozzi_TCCR2, pre_mozzi_OCR2, pre_mozzi_TIMSK;
#elif defined(TCCR4A)
static uint8_t pre_mozzi_TCCR4A, pre_mozzi_TCCR4B, pre_mozzi_TCCR4C,
    pre_mozzi_TCCR4D, pre_mozzi_TCCR4E, pre_mozzi_OCR4C, pre_mozzi_TIMSK4;
#endif
#endif

static void backupPreMozziTimer1() {
  // backup pre-mozzi register values for pausing later
  pre_mozzi_TCCR1A = TCCR1A;
  pre_mozzi_TCCR1B = TCCR1B;
  pre_mozzi_OCR1A = OCR1A;
  pre_mozzi_TIMSK1 = TIMSK1;
}

//-----------------------------------------------------------------------------------------------------------------

#if (AUDIO_MODE == HIFI)
#if defined(TCCR2A)
static uint8_t mozzi_TCCR2A, mozzi_TCCR2B, mozzi_OCR2A, mozzi_TIMSK2;
#elif defined(TCCR2)
static uint8_t mozzi_TCCR2, mozzi_OCR2, mozzi_TIMSK;
#elif defined(TCCR4A)
static uint8_t mozzi_TCCR4A, mozzi_TCCR4B, mozzi_TCCR4C, mozzi_TCCR4D,
    mozzi_TCCR4E, mozzi_OCR4C, mozzi_TIMSK4;
#endif
#endif

//-----------------------------------------------------------------------------------------------------------------

////// BEGIN AUDIO INPUT code ////////
#if (USE_AUDIO_INPUT == true)

// ring buffer for audio input
CircularBuffer<unsigned int> input_buffer; // fixed size 256

static boolean audio_input_is_available;
static int audio_input; // holds the latest audio from input_buffer
uint8_t adc_count = 0;

int MozziClass::getAudioInput() { return audio_input; }

static void startFirstAudioADC() {
  adcStartConversion(adcPinToChannelNum(AUDIO_INPUT_PIN));
}

/*
static void receiveFirstAudioADC()
{
        // nothing
}
*/

static void startSecondAudioADC() {
  ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
}

static void receiveSecondAudioADC() {
  if (!input_buffer.isFull())
    input_buffer.write(ADC);
}

ISR(ADC_vect, ISR_BLOCK)
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

void MozziClass::audioHook() // 2us on AVR excluding updateAudio()
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

#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) 

#if (EXTERNAL_AUDIO_OUTPUT == true)
static void startAudioStandard() {
  backupPreMozziTimer1();
  Timer1.initializeCPUCycles(
      F_CPU / AUDIO_RATE,
      PHASE_FREQ_CORRECT); // set period, phase and frequency correct
  TIMSK1 = _BV(TOIE1); // Overflow Interrupt Enable (when not using
                       // Timer1.attachInterrupt())
}

ISR(TIMER1_OVF_vect, ISR_BLOCK) {
  defaultAudioOutput();
}

#else

// avr architecture
static void startAudioStandard() {
  backupPreMozziTimer1();

  pinMode(AUDIO_CHANNEL_1_PIN, OUTPUT); // set pin to output for audio
  //	pinMode(AUDIO_CHANNEL_2_PIN, OUTPUT);	// set pin to output for audio
#if (AUDIO_MODE == STANDARD)
  Timer1.initializeCPUCycles(
      F_CPU / AUDIO_RATE,
      PHASE_FREQ_CORRECT); // set period, phase and frequency correct
#else // (AUDIO_MODE == STANDARD_PLUS)
  Timer1.initializeCPUCycles(F_CPU / PWM_RATE,
                             FAST); // fast mode enables higher PWM rate
#endif
  Timer1.pwm(AUDIO_CHANNEL_1_PIN,
             AUDIO_BIAS); // pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
#if (STEREO_HACK == true)
  Timer1.pwm(AUDIO_CHANNEL_2_PIN, AUDIO_BIAS); // sets pin to output
#endif
  TIMSK1 = _BV(TOIE1); // Overflow Interrupt Enable (when not using
                       // Timer1.attachInterrupt())
}

/* Interrupt service routine moves sound data from the output buffer to the
Arduino output register, running at AUDIO_RATE. */

ISR(TIMER1_OVF_vect, ISR_BLOCK) {

#if (AUDIO_MODE == STANDARD_PLUS) &&                                           \
    (AUDIO_RATE == 16384) // only update every second ISR, if lower audio rate
  static boolean alternate;
  alternate = !alternate;
  if (alternate) return;
#endif

  defaultAudioOutput();
}
// end avr
#endif
// end STANDARD

//-----------------------------------------------------------------------------------------------------------------
#elif IS_AVR && (AUDIO_MODE == HIFI)

// audio output interrupt on timer 2 (or 4 on ATMEGA32U4 cpu), sets the pwm
// levels of timer 2
static void setupTimer2() {
  backupPreMozziTimer2(); // to reset while pausing
  unsigned long period = F_CPU / AUDIO_RATE;
  FrequencyTimer2::setPeriodCPUCycles(period);
  FrequencyTimer2::setOnOverflow(dummy);
  FrequencyTimer2::enable();
}

static void startAudioHiFi() {
  backupPreMozziTimer1();
  // pwm on timer 1
  pinMode(AUDIO_CHANNEL_1_highByte_PIN,
          OUTPUT); // set pin to output for audio, use 3.9k resistor
  pinMode(AUDIO_CHANNEL_1_lowByte_PIN,
          OUTPUT); // set pin to output for audio, use 499k resistor
  Timer1.initializeCPUCycles(
      F_CPU / 125000,
      FAST); // set period for 125000 Hz fast pwm carrier frequency = 14 bits
  Timer1.pwm(AUDIO_CHANNEL_1_highByte_PIN,
             0); // pwm pin, 0% duty cycle, ie. 0 signal
  Timer1.pwm(AUDIO_CHANNEL_1_lowByte_PIN,
             0); // pwm pin, 0% duty cycle, ie. 0 signal
  // audio output interrupt on timer 2, sets the pwm levels of timer 1
  setupTimer2();
}

/* set up Timer 2 using modified FrequencyTimer2 library */
void dummy() {}

static void backupPreMozziTimer2() {
  // backup Timer2 register values
#if defined(TCCR2A)
  pre_mozzi_TCCR2A = TCCR2A;
  pre_mozzi_TCCR2B = TCCR2B;
  pre_mozzi_OCR2A = OCR2A;
  pre_mozzi_TIMSK2 = TIMSK2;
#elif defined(TCCR2)
  pre_mozzi_TCCR2 = TCCR2;
  pre_mozzi_OCR2 = OCR2;
  pre_mozzi_TIMSK = TIMSK;
#elif defined(TCCR4A)
  pre_mozzi_TCCR4B = TCCR4A;
  pre_mozzi_TCCR4B = TCCR4B;
  pre_mozzi_TCCR4B = TCCR4C;
  pre_mozzi_TCCR4B = TCCR4D;
  pre_mozzi_TCCR4B = TCCR4E;
  pre_mozzi_OCR4C = OCR4C;
  pre_mozzi_TIMSK4 = TIMSK4;
#endif
}

#if defined(TIMER2_COMPA_vect)
ISR(TIMER2_COMPA_vect)
#elif defined(TIMER2_COMP_vect)
ISR(TIMER2_COMP_vect)
#elif defined(TIMER4_COMPA_vect)
ISR(TIMER4_COMPA_vect)
#else
#error                                                                         \
    "This board does not have a hardware timer which is compatible with FrequencyTimer2"
void dummy_function(void)
#endif
{
  defaultAudioOutput();
}

//  end of HIFI

#endif

//-----------------------------------------------------------------------------------------------------------------

static void startControl(unsigned int control_rate_hz) {
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void MozziClass::start(int control_rate_hz) {
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

void MozziClass::stop() {

  noInterrupts();

  // restore backed up register values
  TCCR1A = pre_mozzi_TCCR1A;
  TCCR1B = pre_mozzi_TCCR1B;
  OCR1A = pre_mozzi_OCR1A;

  TIMSK1 = pre_mozzi_TIMSK1;

#if (AUDIO_MODE == HIFI)
#if defined(TCCR2A)
  TCCR2A = pre_mozzi_TCCR2A;
  TCCR2B = pre_mozzi_TCCR2B;
  OCR2A = pre_mozzi_OCR2A;
  TIMSK2 = pre_mozzi_TIMSK2;
#elif defined(TCCR2)
  TCCR2 = pre_mozzi_TCCR2;
  OCR2 = pre_mozzi_OCR2;
  TIMSK = pre_mozzi_TIMSK;
#elif defined(TCCR4A)
  TCCR4B = pre_mozzi_TCCR4A;
  TCCR4B = pre_mozzi_TCCR4B;
  TCCR4B = pre_mozzi_TCCR4C;
  TCCR4B = pre_mozzi_TCCR4D;
  TCCR4B = pre_mozzi_TCCR4E;
  OCR4C = pre_mozzi_OCR4C;
  TIMSK4 = pre_mozzi_TIMSK4;
#endif
#endif
  interrupts();
}

unsigned long MozziClass::audioTicks() {
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
  return output_buffer.count();
#else
  return (samples_written_to_buffer - (output_buffer_size - i2s_available()));
#endif
}

unsigned long MozziClass::mozziMicros() { 
  return audioTicks() * MICROS_PER_AUDIO_TICK;
}

#endif
