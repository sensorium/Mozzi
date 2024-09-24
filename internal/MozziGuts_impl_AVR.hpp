/*
 * MozziGuts_impl_AVR.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#include "utility/FrequencyTimer2.h"
#include "utility/TimerOne.h"

#if (F_CPU != 16000000)
#warning                                                                       \
    "Mozzi has been tested with a cpu clock speed of 16MHz on Arduino!  Results may vary with other speeds."
#endif

////// BEGIN analog input code ////////
#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)
extern uint8_t analog_reference;

ISR(ADC_vect, ISR_BLOCK)
{
  MozziPrivate::advanceADCStep();
}

namespace MozziPrivate {
#define getADCReading() ADC  /* officially (ADCL | (ADCH << 8)) but the compiler works it out */
#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
	if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#  if defined(CORE_TEENSY) // special handling for Teensy2, which does not (did not?) have an analogPinToChannel() define (see https://github.com/sensorium/Mozzi/issues/10)
	static const uint8_t PROGMEM adc_mapping[] = {
	// 0, 1, 4, 5, 6, 7, 13, 12, 11, 10, 9, 8
	0, 1, 4, 5, 6, 7, 13, 12, 11, 10, 9, 8, 10, 11, 12, 13, 7, 6, 5, 4, 1, 0, 8
	};
	pin = pgm_read_byte(adc_mapping + (pin));
#  else
	pin = analogPinToChannel(pin);
#  endif
#elif defined(__AVR_ATmega1284__)
	if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#else
	if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif
	return pin;
}

void adcStartConversion(uint8_t channel) {
#if defined(__AVR_ATmega32U4__)
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);
#elif defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((channel >> 3) & 0x01) << MUX5);
#endif

// from wiring_analog.c:
// set the analog reference (high two bits of ADMUX) and select the
// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
// to 0 (the default).
#if defined(ADMUX)
#  if defined(TEENSYDUINO) // analog_reference is not part TEENSY 2.0 codebase
	ADMUX = (1 << REFS0) | (channel & 0x07); // TB2017 this overwrote analog_reference
#  else
	ADMUX = (analog_reference << 6) | (channel & 0x07);
#  endif
#endif
#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	ADCSRA |= (1 << ADSC);
#endif
}

static void startSecondADCReadOnCurrentChannel() {
  ADCSRA |= (1 << ADSC); // start a second conversion on the current channel
}

/*
void adcEnableInterrupt(){
	ADCSRA |= (1 << ADIE);
}
*/

void setupMozziADC(int8_t speed) {
	ADCSRA |= (1 << ADIE); // adc Enable Interrupt
	adcDisconnectAllDigitalIns();
        setupFastAnalogRead(speed);
}

void setupFastAnalogRead(int8_t speed) {
	if (speed == FAST_ADC){ // divide by 16
		ADCSRA |= (1 << ADPS2);
		ADCSRA &= ~(1 << ADPS1);
		ADCSRA &= ~(1 << ADPS0);
	} else if(speed == FASTER_ADC){ // divide by 8
		ADCSRA &= ~(1 << ADPS2);
		ADCSRA |= (1 << ADPS1);
		ADCSRA |= (1 << ADPS0);
	} else if(speed == FASTEST_ADC){ // divide by 4
		ADCSRA &= ~(1 << ADPS2);
		ADCSRA |= (1 << ADPS1);
		ADCSRA &= ~(1 << ADPS0);
	}
}
}

#endif

////// END analog input code ////////


namespace MozziPrivate {
//// BEGIN AUDIO OUTPUT code ///////
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

// to store backups of timer registers so Mozzi can be stopped and pre_mozzi
// timer values can be restored
static uint8_t pre_mozzi_TCCR1A, pre_mozzi_TCCR1B, pre_mozzi_OCR1A,
    pre_mozzi_TIMSK1;

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
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

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
#if defined(TCCR2A)
static uint8_t mozzi_TCCR2A, mozzi_TCCR2B, mozzi_OCR2A, mozzi_TIMSK2;
#elif defined(TCCR2)
static uint8_t mozzi_TCCR2, mozzi_OCR2, mozzi_TIMSK;
#elif defined(TCCR4A)
static uint8_t mozzi_TCCR4A, mozzi_TCCR4B, mozzi_TCCR4C, mozzi_TCCR4D,
    mozzi_TCCR4E, mozzi_OCR4C, mozzi_TIMSK4;
#endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
static void startAudio() {
  backupPreMozziTimer1();
  Timer1.initializeCPUCycles(
     (F_CPU/MOZZI_AUDIO_RATE)-1, // the -1 here is a result of empirical tests
                           // that showed that it brings the resulting frequency
                           // closer to what is expected.
                           // see: https://github.com/sensorium/Mozzi/pull/202

      PHASE_FREQ_CORRECT); // set period, phase and frequency correct
  TIMSK1 = _BV(TOIE1); // Overflow Interrupt Enable (when not using
                       // Timer1.attachInterrupt())
}

} // namespace MozziPrivate

ISR(TIMER1_OVF_vect, ISR_BLOCK) {
  MozziPrivate::defaultAudioOutput();
}

namespace MozziPrivate {
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_CUSTOM)
static void startAudio() {}  
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
inline void audioOutput(const AudioOutput f)
{
  MOZZI_AUDIO_PIN_1_REGISTER = f.l()+MOZZI_AUDIO_BIAS;
#    if (MOZZI_AUDIO_CHANNELS > 1)
  MOZZI_AUDIO_PIN_2_REGISTER = f.r()+MOZZI_AUDIO_BIAS;
#    endif
}

static void startAudio() {
  backupPreMozziTimer1();

  pinMode(MOZZI_AUDIO_PIN_1, OUTPUT); // set pin to output for audio
#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM) && (MOZZI_PWM_RATE < 32768)    // Formerly known as the - long since deprecated - "STANDARD" mode
  Timer1.initializeCPUCycles(
      (F_CPU/MOZZI_AUDIO_RATE)-1,// the -1 here is a result of empirical tests
                           // that showed that it brings the resulting frequency
                           // closer to what is expected.
                           // see: https://github.com/sensorium/Mozzi/pull/202
      PHASE_FREQ_CORRECT); // set period, phase and frequency correct
#  else // Formerly known as "STANDARD_PLUS" mode
  Timer1.initializeCPUCycles((F_CPU/MOZZI_PWM_RATE)-1, // the -1 here is a result of empirical tests
                                                 // that showed that it brings the resulting frequency
                                                 // closer to what is expected.
                                                 // see: https://github.com/sensorium/Mozzi/pull/202
                             FAST); // fast mode enables higher PWM rate
#  endif
  Timer1.pwm(MOZZI_AUDIO_PIN_1,
             MOZZI_AUDIO_BIAS); // pwm pin, 50% of Mozzi's duty cycle, ie. 0 signal
#  if (MOZZI_AUDIO_CHANNELS > 1)
  pinMode(MOZZI_AUDIO_PIN_2, OUTPUT); // set pin to output for audio
  Timer1.pwm(MOZZI_AUDIO_PIN_2, MOZZI_AUDIO_BIAS);
#  endif
  TIMSK1 = _BV(TOIE1); // Overflow Interrupt Enable (when not using
                       // Timer1.attachInterrupt())
}

} // namespace MozziPrivate

/* Interrupt service routine moves sound data from the output buffer to the
Arduino output register, running at MOZZI_AUDIO_RATE. */
ISR(TIMER1_OVF_vect, ISR_BLOCK) {
#  if (MOZZI_AUDIO_RATE < MOZZI_PWM_RATE) // only update every second ISR, if lower audio rate
  static_assert(2l*MOZZI_AUDIO_RATE == MOZZI_PWM_RATE, "audio rate must the same, or exactly half of the pwm rate!");
  static boolean alternate;
  alternate = !alternate;
  if (alternate) return;
#  endif

  MozziPrivate::defaultAudioOutput();
}

namespace MozziPrivate {
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
inline void audioOutput(const AudioOutput f) {
  // read about dual pwm at
  // http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/
  // sketches at http://wiki.openmusiclabs.com/wiki/PWMDAC,
  // http://wiki.openmusiclabs.com/wiki/MiniArDSP
  // if (!output_buffer.isEmpty()){
  //unsigned int out = output_buffer.read();
  // 14 bit, 7 bits on each pin
  // MOZZI_AUDIO_PIN_1_REGISTER = out >> 7; // B00111111 10000000 becomes
  // B1111111
  // try to avoid looping over 7 shifts - need to check timing or disassemble to
  // see what really happens unsigned int out_high = out<<1; // B00111111
  // 10000000 becomes B01111111 00000000
  // MOZZI_AUDIO_PIN_1_REGISTER = out_high >> 8; // B01111111 00000000
  // produces B01111111 MOZZI_AUDIO_PIN_1_LOW_REGISTER = out & 127;
  /* Atmega manual, p123
  The high byte (OCR1xH) has to be written first.
  When the high byte I/O location is written by the CPU,
  the TEMP Register will be updated by the value written.
  Then when the low byte (OCR1xL) is written to the lower eight bits,
  the high byte will be copied into the upper 8-bits of
  either the OCR1x buffer or OCR1x Compare Register in
  the same system clock cycle.
  */
  MOZZI_AUDIO_PIN_1_REGISTER = (f.l()+MOZZI_AUDIO_BIAS) >> MOZZI_AUDIO_BITS_PER_CHANNEL;
  MOZZI_AUDIO_PIN_1_LOW_REGISTER = (f.l()+MOZZI_AUDIO_BIAS) & ((1 << MOZZI_AUDIO_BITS_PER_CHANNEL) - 1);
}

static void setupTimer2();
static void startAudio() {
  backupPreMozziTimer1();
  // pwm on timer 1
  pinMode(MOZZI_AUDIO_PIN_1, OUTPUT); // set pin to output for audio, use 3.9k resistor
  pinMode(MOZZI_AUDIO_PIN_1_LOW, OUTPUT); // set pin to output for audio, use 499k resistor
  Timer1.initializeCPUCycles(
      F_CPU/125000,
      FAST); // set period for 125000 Hz fast pwm carrier frequency = 14 bits
  Timer1.pwm(MOZZI_AUDIO_PIN_1, 0); // pwm pin, 0% duty cycle, ie. 0 signal
  Timer1.pwm(MOZZI_AUDIO_PIN_1_LOW, 0);
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
  unsigned long period = F_CPU / MOZZI_AUDIO_RATE;
  FrequencyTimer2::setPeriodCPUCycles(period);
  FrequencyTimer2::setOnOverflow(dummy);
  FrequencyTimer2::enable();
}

} // namespace MozziPrivate

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
  MozziPrivate::defaultAudioOutput();
}

//  end of HIFI

namespace MozziPrivate {

#endif

//-----------------------------------------------------------------------------------------------------------------


void stopMozzi() {
  noInterrupts();

  // restore backed up register values
  TCCR1A = pre_mozzi_TCCR1A;
  TCCR1B = pre_mozzi_TCCR1B;
  OCR1A = pre_mozzi_OCR1A;

  TIMSK1 = pre_mozzi_TIMSK1;

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_2PIN_PWM)
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

// Unmodified TimerOne.cpp has TIMER3_OVF_vect.
// Watch out if you update the library file.
// The symptom will be no sound.
// ISR(TIMER1_OVF_vect)
// {
// 	Timer1.isrCallback();
// }
//// END AUDIO OUTPUT code ///////

//// BEGIN Random seeding ////////
#if defined (__AVR_ATmega644P__)

// a less fancy version for gizduino (__AVR_ATmega644P__) which doesn't know INTERNAL
static long longRandom()
{
	return ((long)analogRead(0)+63)*(analogRead(1)+97); // added offsets in case analogRead is 0
}

#else

/*
longRandom(), used as a seed generator, comes from:
http://arduino.cc/forum/index.php/topic,38091.0.html
//  AUTHOR: Rob Tillaart
// PURPOSE: Simple Random functions based upon unreliable internal temp sensor
// VERSION: 0.1
//       DATE: 2011-05-01
//
// Released to the public domain, use at own risk
//
*/
static long longRandom()
{
	//analogReference(INTERNAL);
	unsigned long rv = 0;
	for (uint8_t i=0; i< 32; i++) rv |= ((analogRead(8)+1171) & 1L) << i; // added 1171 in case analogRead is 0
	return rv;
}
#endif

void MozziRandPrivate::autoSeed() {
  ADCSRA &= ~ (1 << ADIE); // adc Disable Interrupt, re-enable at end
  // this attempt at remembering analog_reference stops it working
  // maybe needs a delay after changing analog reference in longRandom (Arduino reference suggests this)
  // because the analog reads return 0
  //uint8_t analog_reference_orig = ADMUX&192; // analog_reference is high 2 bits of ADMUX, store this because longRandom sets it to internal
  x = longRandom();
  y = longRandom();
  z = longRandom();
  //analogReference(analog_reference_orig); // change back to original
  ADCSRA |= (1 << ADIE); // adc re-Enable Interrupt
}

//// END Random seeding ////////
}
