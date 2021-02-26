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
#include "AudioOutput.h"


#if IS_AVR()
#include "FrequencyTimer2.h"
#include "TimerOne.h"
#elif IS_TEENSY3()
// required from http://github.com/pedvide/ADC for Teensy 3.*
#include "IntervalTimer.h"
#include <ADC.h>
#elif IS_STM32()
#include "HardwareTimer.h"
//#include <STM32ADC.h>  // Disabled, here. See AudioConfigSTM32.h
#elif IS_ESP8266()
#include <uart.h>
#include <i2s.h>
uint16_t output_buffer_size = 0;
#elif IS_ESP32()
#include <driver/i2s.h>   // for I2S-based output modes
#include <driver/timer.h> // for EXTERNAL_AUDIO_OUTPUT
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

#if BYPASS_MOZZI_OUTPUT_BUFFER == true
uint64_t samples_written_to_buffer = 0;
#else
//-----------------------------------------------------------------------------------------------------------------
// ring buffer for audio output
#if (STEREO_HACK == true)
CircularBuffer<StereoOutput> output_buffer;  // fixed size 256
#else
CircularBuffer<AudioOutput_t> output_buffer;  // fixed size 256
#endif
//-----------------------------------------------------------------------------------------------------------------
#endif

#if IS_AVR() // not storing backups, just turning timer on and off for pause for
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

#endif // end of timer backups for non-Teensy 3 boards
//-----------------------------------------------------------------------------------------------------------------

////// BEGIN AUDIO INPUT code ////////
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

#if IS_TEENSY3() || IS_STM32() || IS_AVR()
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
////// END AUDIO INPUT code ////////


#if IS_SAMD21()
// These are ARM SAMD21 Timer 5 routines to establish a sample rate interrupt
static bool tcIsSyncing() {
  return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY;
}

static void tcReset() {
  // Reset TCx
  TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
  while (tcIsSyncing())
    ;
  while (TC5->COUNT16.CTRLA.bit.SWRST)
    ;
}
/* Not currently used, and does not compile with EXTERNAL_AUDIO_OUTPUT
static void tcEnd() {
  // Disable TC5
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  while (tcIsSyncing())
    ;
  tcReset();
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
} */

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

#if (IS_ESP8266() && (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL) && (EXTERNAL_AUDIO_OUTPUT != true))
void CACHED_FUNCTION_ATTR esp8266_serial_audio_output() {
  // Note: That unreadble mess is an optimized version of Serial1.availableForWrite()
  while ((UART_TX_FIFO_SIZE - ((U1S >> USTXC) & 0xff)) > (PDM_RESOLUTION * 4)) {
    audioOutput(output_buffer.read());
  }
}
#endif

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

void audioHook() // 2us on AVR excluding updateAudio()
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
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
static void CACHED_FUNCTION_ATTR defaultAudioOutput() {
#if (USE_AUDIO_INPUT == true)
  adc_count = 0;
  startSecondAudioADC();
#endif
  audioOutput(output_buffer.read());
}
#endif

#if (AUDIO_MODE == STANDARD) || (AUDIO_MODE == STANDARD_PLUS) || IS_STM32()
#if IS_TEENSY3()
IntervalTimer timer1;
#elif IS_STM32() && (EXTERNAL_AUDIO_OUTPUT == true)
HardwareTimer audio_update_timer(2);
#elif IS_STM32()
HardwareTimer audio_update_timer(AUDIO_UPDATE_TIMER);
HardwareTimer audio_pwm_timer(AUDIO_PWM_TIMER);
#endif

#if (IS_ESP32() && (BYPASS_MOZZI_OUTPUT_BUFFER != true))
void CACHED_FUNCTION_ATTR timer0_audio_output_isr(void *) {
  TIMERG0.int_clr_timers.t0 = 1;
  TIMERG0.hw_timer[0].config.alarm_en = 1;
  defaultAudioOutput();
}
#endif

#if IS_SAMD21()
#ifdef __cplusplus
extern "C" {
#endif
void samd21AudioOutput() {
  defaultAudioOutput();
  TC5->COUNT16.INTFLAG.bit.MC0 = 1;
}
#ifdef __cplusplus
}
#endif
#endif

#if !IS_AVR()
static void startAudioStandard() {

#if IS_TEENSY3()
  adc->setAveraging(0);
  adc->setConversionSpeed(
      ADC_CONVERSION_SPEED::MED_SPEED); // could be HIGH_SPEED, noisier

  analogWriteResolution(12);
  timer1.begin(defaultAudioOutput, 1000000UL / AUDIO_RATE);
#elif IS_SAMD21()
#ifdef ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS
  {
    static const int CPLAY_SPEAKER_SHUTDOWN = 11;
    pinMode(CPLAY_SPEAKER_SHUTDOWN, OUTPUT);
    digitalWrite(CPLAY_SPEAKER_SHUTDOWN, HIGH);
  }

#endif
  analogWriteResolution(10);
#if (EXTERNAL_AUDIO_OUTPUT != true)
  analogWrite(AUDIO_CHANNEL_1_PIN, 0);
#endif
  tcConfigure(AUDIO_RATE);

#elif IS_ESP32() && (BYPASS_MOZZI_OUTPUT_BUFFER != true)  // for external audio output, set up a timer running a audio rate
  static intr_handle_t s_timer_handle;
  timer_config_t config = {
    .alarm_en = true,
    .counter_en = false,
    .intr_type = TIMER_INTR_LEVEL,
    .counter_dir = TIMER_COUNT_UP,
    .auto_reload = true,
    .divider = 1 // For max available precision: The APB_CLK clock signal is running at 80 MHz, i.e. 1/80 uS per tick
  };
  timer_init(TIMER_GROUP_0, TIMER_0, &config);
  timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
  timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 80000000UL / AUDIO_RATE);
  timer_enable_intr(TIMER_GROUP_0, TIMER_0);
  timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer0_audio_output_isr, nullptr, 0, &s_timer_handle);
  timer_start(TIMER_GROUP_0, TIMER_0);

#elif IS_ESP32()
  static const i2s_config_t i2s_config = {
#if (ESP32_AUDIO_OUT_MODE == PT8211_DAC) || (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
#elif (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
#endif
    .sample_rate = AUDIO_RATE * PDM_RESOLUTION,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,  // only the top 8 bits will actually be used by the internal DAC, but using 8 bits straight away seems buggy
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,  // always use stereo output. mono seems to be buggy, and the overhead is insignifcant on the ESP32
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB),  // this appears to be the correct setting for internal DAC and PT8211, but not for other dacs
    .intr_alloc_flags = 0, // default interrupt priority
    .dma_buf_count = 8,    // 8*128 bytes of buffer corresponds to 256 samples (2 channels, see above, 2 bytes per sample per channel)
    .dma_buf_len = 128,
    .use_apll = false
  };

  i2s_driver_install(i2s_num, &i2s_config, 0, NULL);
  #if (ESP32_AUDIO_OUT_MODE == PT8211_DAC) || (ESP32_AUDIO_OUT_MODE == PDM_VIA_I2S)
  static const i2s_pin_config_t pin_config = {
    .bck_io_num = ESP32_I2S_BCK_PIN,
    .ws_io_num = ESP32_I2S_WS_PIN,
    .data_out_num = ESP32_I2S_DATA_PIN,
    .data_in_num = -1
  };
  i2s_set_pin((i2s_port_t)i2s_num, &pin_config);
  #elif (ESP32_AUDIO_OUT_MODE == INTERNAL_DAC)
  i2s_set_pin((i2s_port_t)i2s_num, NULL);
  i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
  #endif
  i2s_zero_dma_buffer((i2s_port_t)i2s_num);

#elif IS_STM32()
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

#elif IS_ESP8266() && (EXTERNAL_AUDIO_OUTPUT == true)  && (BYPASS_MOZZI_OUTPUT_BUFFER != true) // for external audio output, set up a timer running a audio rate
  timer1_isr_init();
  timer1_attachInterrupt(defaultAudioOutput);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
  timer1_write(F_CPU / AUDIO_RATE);
#elif IS_ESP8266()
#if (ESP_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
  Serial1.begin(
      AUDIO_RATE * (PDM_RESOLUTION * 40), SERIAL_8N1,
      SERIAL_TX_ONLY); // Note: PDM_RESOLUTION corresponds to packets of 32
                       // encoded bits  However, the UART (unfortunately) adds a
                       // start and stop bit each around each byte, thus sending
                       // a total to 40 bits per audio sample per
                       // PDM_RESOLUTION.
  // set up a timer to copy from Mozzi output_buffer into Serial TX buffer
  timer1_isr_init();
  timer1_attachInterrupt(esp8266_serial_audio_output);
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

#elif IS_AVR() && (EXTERNAL_AUDIO_OUTPUT == true)
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
  defaultAudioOutput();
}

//  end of HIFI

#endif

//-----------------------------------------------------------------------------------------------------------------

static void startControl(unsigned int control_rate_hz) {
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
  timer1_disable();
#endif
#elif IS_SAMD21()
#elif IS_ESP32()
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
#if (BYPASS_MOZZI_OUTPUT_BUFFER != true)
  return output_buffer.count();
#elif IS_ESP32()
  // TODO: This is not entirely accurate, as it does not account for samples still in the buffer
  //       and not yet written to output. There does not seem to be an API to retrieve I2S-buffer
  //       fill state on ESP32.
  return samples_written_to_buffer;
#elif (IS_ESP8266() && (ESP_AUDIO_OUT_MODE == PDM_VIA_I2S) && (PDM_RESOLUTION != 1))
  return (samples_written_to_buffer -
          ((output_buffer_size - i2s_available()) / PDM_RESOLUTION));
#else
  return (samples_written_to_buffer - (output_buffer_size - i2s_available()));
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
