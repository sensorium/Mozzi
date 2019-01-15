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

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "CircularBuffer.h"
#include "MozziGuts.h"
#include "mozzi_analog.h"
#include "mozzi_config.h" // at the top of all MozziGuts and analog files
//#include "mozzi_utils.h"

#if IS_AVR()
#include "FrequencyTimer2.h"
#include "TimerOne.h"
#elif IS_TEENSY3()
// required from http://github.com/pedvide/ADC for Teensy 3.*
#include "IntervalTimer.h"
#include <ADC.h>
#elif IS_STM32()
#include "HardwareTimer.h"
#include <STM32ADC.h>
#elif IS_ESP8266()
#include <Ticker.h>
#include <uart.h>
#endif

#ifdef EXTERNAL_DAC
DAC_MCP49xx dac(DAC_MCP49xx::MCP4922, 10);
#endif

#if (IS_TEENSY3() && F_CPU != 48000000) || (IS_AVR() && F_CPU != 16000000)
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino and 48MHz on Teensy 3!  Results may vary with other speeds."
#endif

#if IS_TEENSY3()
ADC *adc; // adc object
uint8_t teensy_pin;
#elif IS_STM32()
STM32ADC adc(ADC1);
uint8_t stm32_current_adc_pin;
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

#if IS_ESP8266() && (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL)
#include <i2s.h>
uint16_t output_buffer_size = 0;
uint64_t samples_written_to_buffer = 0;
#else
#if IS_ESP8266() && (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
bool output_stopped = true;
#endif
//-----------------------------------------------------------------------------------------------------------------
// ring buffer for audio output
CircularBuffer<unsigned int> output_buffer;  // fixed size 256
#if (STEREO_HACK == true)
CircularBuffer<unsigned int> output_buffer2; // fixed size 256
#endif
//-----------------------------------------------------------------------------------------------------------------
#endif

#if IS_AVR() // not storing backups, just turning timer on and off for pause for
             // teensy 3.*, other ARMs
#ifdef EXTERNAL_DAC // in case an external MCP4922 dac is used.
static void dacMCPAudioOutput() {
  dac.output((unsigned int)output_buffer.read());
}
#if (STEREO_HACK == true)
static void dacMCPAudioOutput2() {
  dac.outputB((unsigned int)output_buffer2.read());
}
#endif
#endif

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

#endif // end of timer backups for non-Teensy 3 boards
//-----------------------------------------------------------------------------------------------------------------

#if (USE_AUDIO_INPUT == true)

// ring buffer for audio input
CircularBuffer<unsigned int> input_buffer; // fixed size 256

static boolean audio_input_is_available;
static int audio_input; // holds the latest audio from input_buffer
uint8_t adc_count = 0;

int getAudioInput() { return audio_input; }

static void startFirstAudioADC() {
#if IS_TEENSY3()
  adc->startSingleRead(
      AUDIO_INPUT_PIN); // ADC lib converts pin/channel in startSingleRead
#elif IS_STM32()
  uint8_t dummy = AUDIO_INPUT_PIN;
  adc.setPins(&dummy, 1);
  adc.startConversion();
#else
  adcStartConversion(adcPinToChannelNum(AUDIO_INPUT_PIN));
#endif
}

/*
static void receiveFirstAudioADC()
{
        // nothing
}
*/

static void startSecondAudioADC() {
#if IS_TEENSY3()
  adc->startSingleRead(AUDIO_INPUT_PIN);
#elif IS_STM32()
  uint8_t dummy = AUDIO_INPUT_PIN;
  adc.setPins(&dummy, 1);
  adc.startConversion();
#else
  ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
#endif
}

static void receiveSecondAudioADC() {
  if (!input_buffer.isFull())
#if IS_TEENSY3()
    input_buffer.write(adc->readSingle());
#elif IS_STM32()
    input_buffer.write(adc.getData());
#else
    input_buffer.write(ADC);
#endif
}

#if !IS_SAMD21()

#if IS_TEENSY3()
void adc0_isr(void)
#elif IS_STM32()
void stm32_adc_eoc_handler()
#else
ISR(ADC_vect, ISR_BLOCK)
#endif
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
#endif // end main audio input section
#endif

#if IS_SAMD21()
// These are ARM SAMD21 Timer 5 routines to establish a sample rate interrupt
static bool tcIsSyncing() {
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

static void tcStartCounter() {
  // Enable TC

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  while (tcIsSyncing())
    ;
}

static void tcReset() {
  // Reset TCx
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing())
    ;
  while (TC5->COUNT16.CTRLA.bit.SWRST)
    ;
}

static void tcDisable() {
  // Disable TC5
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing())
    ;
}
static void tcEnd() {
  tcDisable();
  tcReset();
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
}

static void tcConfigure(uint32_t sampleRate) {
  // Enable GCLK for TCC2 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                                 GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY)
    ;

  tcReset();

  // Set Timer counter Mode to 16 bits
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;

  // Set TC5 mode as match frequency
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE;

  TC5->COUNT16.CC[0].reg = (uint16_t)(SystemCoreClock / sampleRate - 1);
  while (tcIsSyncing())
    ;

  // Configure interrupt request
  NVIC_DisableIRQ(TC5_IRQn);
  NVIC_ClearPendingIRQ(TC5_IRQn);
  NVIC_SetPriority(TC5_IRQn, 0);
  NVIC_EnableIRQ(TC5_IRQn);

  // Enable the TC5 interrupt request
  TC5->COUNT16.INTENSET.bit.MC0 = 1;
  while (tcIsSyncing())
    ;
}
#endif

#if IS_ESP8266()
// lookup table for fast pdm coding on 8 output bits at a time
static byte fast_pdm_table[]{0,          0b00010000, 0b01000100,
                             0b10010010, 0b10101010, 0b10110101,
                             0b11011101, 0b11110111, 0b11111111};

inline void writePDMCoded(uint16_t sample) {
  static uint32_t lastwritten = 0;
  static uint32_t nexttarget = 0;

  // We can write 32 bits at a time to the output buffer, typically, we'll do
  // this either once of twice per sample
  for (uint8_t words = 0; words < PDM_RESOLUTION; ++words) {
    uint32_t outbits = 0;
    for (uint8_t i = 0; i < 4; ++i) {
      nexttarget += sample - lastwritten;
      lastwritten =
          nexttarget &
          0b11110000000000000; // code the highest 3-and-a-little bits next.
                               // Note that sample only has 16 bits, while the
                               // highest bit we consider for writing is bit 17.
                               // Thus, if the highest bit is set, the next
                               // three bits cannot be.
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
      U1F = fast_pdm_table[lastwritten >>
                           13]; // optimized version of: Serial1.write(...);
#else
      outbits = outbits << 8;
      outbits |= fast_pdm_table[lastwritten >> 13];
#endif
    }
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S)
    i2s_write_sample(outbits);
#endif
  }
}

#if (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
void ICACHE_RAM_ATTR write_audio_to_serial_tx() {
#define OPTIMIZED_SERIAL1_AVAIALABLEFORWRITE                                   \
  (UART_TX_FIFO_SIZE - ((U1S >> USTXC) & 0xff))
  if (output_stopped)
    return;
  while (OPTIMIZED_SERIAL1_AVAIALABLEFORWRITE > (PDM_RESOLUTION * 4)) {
    writePDMCoded(output_buffer.read());
  }
}
#else
inline void espWriteAudioToBuffer() {
#if (ESP_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
#if (STEREO_HACK == true)
  updateAudio();
  i2s_write_lr(audio_out_1, audio_out_2);
#else
  i2s_write_lr(updateAudio(), 0);
#endif
#else
  uint16_t sample = updateAudio() + AUDIO_BIAS;
  writePDMCoded(sample);
#endif
  ++samples_written_to_buffer;
}
#endif
#endif

static uint16_t update_control_timeout;
static uint16_t update_control_counter;
static void updateControlWithAutoADC();

void audioHook() // 2us excluding updateAudio()
{
// setPin13High();
#if (USE_AUDIO_INPUT == true)
  if (!input_buffer.isEmpty())
    audio_input = input_buffer.read();
#endif

#if IS_ESP8266() && (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL)
#if (PDM_RESOLUTION != 1)
  if (i2s_available() >= PDM_RESOLUTION) {
#else
  if (!i2s_is_full()) {
#endif
#else
  if (!output_buffer.isFull()) {
#endif
    if (!update_control_counter) {
      update_control_counter = update_control_timeout;
      updateControlWithAutoADC();
    } else {
      --update_control_counter;
    }
#if IS_ESP8266() && (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL)
    // NOTE: On ESP / output via I2S, we simply use the I2S buffer as the output
    // buffer, which saves RAM, but also simplifies things a lot
    // esp. since i2s output already has output rate control -> no need for a
    // separate output timer
    espWriteAudioToBuffer();
#else
#if (STEREO_HACK == true)
    updateAudio(); // in hacked version, this returns void
    output_buffer.write((unsigned int)(audio_out_1 + AUDIO_BIAS));
    output_buffer2.write((unsigned int)(audio_out_2 + AUDIO_BIAS));
#else
    output_buffer.write((unsigned int)(updateAudio() + AUDIO_BIAS));
#endif
#endif

#if IS_ESP8266()
    yield();
#endif
  }
  // setPin13Low();
}

#if IS_SAMD21()
void TC5_Handler(void) __attribute__((weak, alias("samd21AudioOutput")));
#endif

//-----------------------------------------------------------------------------------------------------------------
#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) || IS_STM32()
#if IS_SAMD21()
#ifdef __cplusplus
extern "C" {
#endif

void samd21AudioOutput(void);
#ifdef __cplusplus
}
#endif

#elif IS_TEENSY3()
IntervalTimer timer1;
#elif IS_STM32()
HardwareTimer audio_update_timer(AUDIO_UPDATE_TIMER);
HardwareTimer audio_pwm_timer(AUDIO_PWM_TIMER);

#endif

#if IS_SAMD21()
#ifdef __cplusplus
extern "C" {
#endif

void samd21AudioOutput() {

#if (USE_AUDIO_INPUT == true)
  adc_count = 0;
  startSecondAudioADC();
#endif
  analogWrite(AUDIO_CHANNEL_1_PIN, (int)output_buffer.read());
  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}
#ifdef __cplusplus
}
#endif
#elif IS_TEENSY3()
static void teensyAudioOutput() {

#if (USE_AUDIO_INPUT == true)
  adc_count = 0;
  startSecondAudioADC();
#endif

  analogWrite(AUDIO_CHANNEL_1_PIN, (int)output_buffer.read());
}
#elif IS_STM32()
static void pwmAudioOutput() {
#if (USE_AUDIO_INPUT == true)
  adc_count = 0;
  startSecondAudioADC();
#endif

#if (AUDIO_MODE == HIFI)
  int out = output_buffer.read();
  pwmWrite(AUDIO_CHANNEL_1_PIN, out & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
  pwmWrite(AUDIO_CHANNEL_1_PIN_HIGH, out >> AUDIO_BITS_PER_CHANNEL);
#else
  pwmWrite(AUDIO_CHANNEL_1_PIN, (int)output_buffer.read());
#endif
}
#endif

#if !IS_AVR()
static void startAudioStandard() {

#if IS_TEENSY3()
  adc->setAveraging(0);
  adc->setConversionSpeed(
      ADC_CONVERSION_SPEED::MED_SPEED); // could be HIGH_SPEED, noisier

  analogWriteResolution(12);
  timer1.begin(teensyAudioOutput, 1000000UL / AUDIO_RATE);
#elif IS_SAMD21()
#ifdef ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS
  {
    static const int CPLAY_SPEAKER_SHUTDOWN = 11;
    pinMode(CPLAY_SPEAKER_SHUTDOWN, OUTPUT);
    digitalWrite(CPLAY_SPEAKER_SHUTDOWN, HIGH);
  }

#endif
  analogWriteResolution(12);
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
  tcConfigure(AUDIO_RATE);
#elif IS_STM32()
  audio_update_timer.pause();
  audio_update_timer.setPeriod(1000000UL / AUDIO_RATE);
  audio_update_timer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  audio_update_timer.setCompare(TIMER_CH1,
                                1); // Interrupt 1 count after each update
  audio_update_timer.attachCompare1Interrupt(pwmAudioOutput);
  audio_update_timer.refresh();
  audio_update_timer.resume();

  pinMode(AUDIO_CHANNEL_1_PIN, PWM);
#if (AUDIO_MODE == HIFI)
  pinMode(AUDIO_CHANNEL_1_PIN_HIGH, PWM);
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

#elif IS_ESP8266()
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
  output_stopped = false;
  Serial1.begin(
      AUDIO_RATE * (PDM_RESOLUTION * 40), SERIAL_8N1,
      SERIAL_TX_ONLY); // Note: PDM_RESOLUTION corresponds to packets of 32
                       // encoded bits  However, the UART (unfortunately) adds a
                       // start and stop bit each around each byte, thus sending
                       // a total to 40 bits per audio sample per
                       // PDM_RESOLUTION.
  // set up a timer to copy from Mozzi output_buffer into Serial TX buffer
  timer1_isr_init();
  timer1_attachInterrupt(write_audio_to_serial_tx);
  // UART FIFO buffer size is 128 bytes. To be on the safe side, we keep the
  // interval to the time needed to write half of that. PDM_RESOLUTION * 4 bytes
  // per sample written.
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(F_CPU / (AUDIO_RATE * PDM_RESOLUTION));
#else
  i2s_begin();
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S)
  pinMode(2, INPUT); // Set the two unneeded I2S pins to input mode, to reduce
                     // side effects
  pinMode(15, INPUT);
#endif
  i2s_set_rate(AUDIO_RATE * PDM_RESOLUTION);
  if (output_buffer_size == 0)
    output_buffer_size =
        i2s_available(); // Do not reset count when stopping / restarting
#endif
#endif
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
  if (alternate) {
#endif

#if (USE_AUDIO_INPUT == true)
    adc_count = 0;
    startSecondAudioADC();
#endif

#ifdef EXTERNAL_DAC
    dacMCPAudioOutput();
#if (STEREO_HACK == true)
    dacMCPAudioOutput2();
#endif
#else
  AUDIO_CHANNEL_1_OUTPUT_REGISTER = output_buffer.read();
#endif
#if (STEREO_HACK == true)
    AUDIO_CHANNEL_2_OUTPUT_REGISTER = output_buffer2.read();
#endif

#if (AUDIO_MODE == STANDARD_PLUS) &&                                           \
    (AUDIO_RATE == 16384) // all this conditional compilation is so clutsy!
  }
#endif
}
// end avr
#endif
// end STANDARD

//-----------------------------------------------------------------------------------------------------------------
#elif IS_AVR() && (AUDIO_MODE == HIFI)

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

// audio output interrupt on timer 2 (or 4 on ATMEGA32U4 cpu), sets the pwm
// levels of timer 2
static void setupTimer2() {
  backupPreMozziTimer2(); // to reset while pausing
  unsigned long period = F_CPU / AUDIO_RATE;
  FrequencyTimer2::setPeriodCPUCycles(period);
  FrequencyTimer2::setOnOverflow(dummy);
  FrequencyTimer2::enable();
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
#if (USE_AUDIO_INPUT == true)
  adc_count = 0;
  startSecondAudioADC();
#endif

  // read about dual pwm at
  // http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
  // sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,
  // http://wiki.openmusiclabs.com/wiki/MiniArDSP
  // if (!output_buffer.isEmpty()){
  unsigned int out = output_buffer.read();
  // 14 bit, 7 bits on each pin
  // AUDIO_CHANNEL_1_highByte_REGISTER = out >> 7; // B00111111 10000000 becomes
  // B1111111
  // try to avoid looping over 7 shifts - need to check timing or disassemble to
  // see what really happens unsigned int out_high = out<<1; // B00111111
  // 10000000 becomes B01111111 00000000
  // AUDIO_CHANNEL_1_highByte_REGISTER = out_high >> 8; // B01111111 00000000
  // produces B01111111 AUDIO_CHANNEL_1_lowByte_REGISTER = out & 127;
  /* Atmega manual, p123
  The high byte (OCR1xH) has to be written first.
  When the high byte I/O location is written by the CPU,
  the TEMP Register will be updated by the value written.
  Then when the low byte (OCR1xL) is written to the lower eight bits,
  the high byte will be copied into the upper 8-bits of
  either the OCR1x buffer or OCR1x Compare Register in
  the same system clock cycle.
  */
  AUDIO_CHANNEL_1_highByte_REGISTER = out >> AUDIO_BITS_PER_REGISTER;
  AUDIO_CHANNEL_1_lowByte_REGISTER = out & ((1 << AUDIO_BITS_PER_REGISTER) - 1);
}

//  end of HIFI

#endif

//-----------------------------------------------------------------------------------------------------------------

static void updateControlWithAutoADC() {
  updateControl();
  /*
  #if (USE_AUDIO_INPUT==true)
          adc_count = 0;
          startSecondAudioADC();
#endif
*/
  adcStartReadCycle();
}

static void startControl(unsigned int control_rate_hz) {
  update_control_counter = 0;
  update_control_timeout = AUDIO_RATE / control_rate_hz;
}

void startMozzi(int control_rate_hz) {
  setupMozziADC(); // you can use setupFastAnalogRead() with FASTER or FASTEST
                   // in setup() if desired (not for Teensy 3.* )
  // delay(200); // so AutoRange doesn't read 0 to start with
  startControl(control_rate_hz);
#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) ||               \
    IS_STM32() // Sorry, this is really hacky. But on STM32 regular and HIFI
               // audio modes are so similar to set up, that we do it all in one
               // function.
  startAudioStandard();
#elif (AUDIO_MODE == HIFI)
  startAudioHiFi();
#endif
}

void stopMozzi() {
#if IS_TEENSY3()
  timer1.end();
#elif IS_STM32()
  audio_update_timer.pause();
#elif IS_ESP8266()
#if (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL)
  i2s_end();
#else
  output_stopped = true; // NOTE: No good way to stop the serial output itself,
                         // but probably not needed, anyway
#endif
#elif IS_SAMD21()
#else

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
#endif
  interrupts();
}

unsigned long audioTicks() {
#if (IS_ESP8266() && (ESP_AUDIO_OUT_MODE != PDM_VIA_SERIAL))
#if ((ESP_AUDIO_OUT_MODE == PDM_VIA_I2S) && (PDM_RESOLUTION != 1))
  return (samples_written_to_buffer -
          ((output_buffer_size - i2s_available()) / PDM_RESOLUTION));
#else
  return (samples_written_to_buffer - (output_buffer_size - i2s_available()));
#endif
#else
  return output_buffer.count();
#endif
}

unsigned long mozziMicros() { return audioTicks() * MICROS_PER_AUDIO_TICK; }

// Unmodified TimerOne.cpp has TIMER3_OVF_vect.
// Watch out if you update the library file.
// The symptom will be no sound.
// ISR(TIMER1_OVF_vect)
// {
// 	Timer1.isrCallback();
// }
