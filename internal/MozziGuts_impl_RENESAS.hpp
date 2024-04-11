/*
 * MozziGuts_impl_RENESAS.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 T. Combriat and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

#if !(IS_RENESAS())
#  error "Wrong implementation included for this platform"
#endif

#include <FspTimer.h>

namespace MozziPrivate {

////// BEGIN analog input code ////////

#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)

#define channelNumToIndex(channel) channel-14  // A0=14
void const *const p_context = 0; // unused but needed for the ADC call
adc_callback_args_t *const p_callback_memory = NULL;  // idem
uint16_t cfg_adc = 0;  // put at 0 for starters but modified by the ADC_container
uint8_t r4_pin; // to store it between calls

void adc_callback(adc_callback_args_t *p_args) {
  advanceADCStep();
}

} // namespace MozziPrivate

#include "MozziGuts_impl_RENESAS_ADC.hpp"

namespace MozziPrivate {

#define getADCReading() readADC(r4_pin)

uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}

void adcStartConversion(uint8_t channel) {
  r4_pin = channel;  // remember for startSecondADCReadOnCurrentChannel()
  startScan(r4_pin);
}

void startSecondADCReadOnCurrentChannel() {
  startScan(r4_pin);
}

void setupFastAnalogRead(int8_t speed) {
  //#warning Fast analog read not implemented on this platform
}

void setupMozziADC(int8_t speed) {
  IRQManager::getInstance().addADCScanEnd(&adc, NULL); // this is needed to change some config inside the ADC, even though we do not give the callback here (doing so crashes the board). The callback is declared to the ADC by: R_ADC_CallbackSet(&(_adc->ctrl), adc_callback, p_context, p_callback_memory); in MozziGuts_impl_RENESAS_ADC.hpp.
}
#endif
  
////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////

/*
The strategy to output sound on this platform differs somehow from what is usually done in Mozzi.
Usually, Mozzi's circular buffer are read from the outputting device (PWM, DAC...) or committed as
a whole (for the MBED platform) and thus emptied. Once a free spot is present, Mozzi fills it with updateAudio().
Here, the DAC works straight from a buffer, outputting samples from it at a fixed rate. 
This is sweet as we can branch it straight to Mozzi's buffer and it will read from it without
further action from us.
The big difference is that it *does not* empty the buffer, nor notify that something has been
read, which is okay for outputting a periodic signal where just a full period is present
in the buffer.
As a consequence we need to artificially empty the buffer at the same rate that the DAC is reading
it.
*/

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
FspTimer timer;
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
CircularBuffer<uint16_t> output_buffer;
} // namespace MozziPrivate
#include "MozziGuts_impl_RENESAS_analog.hpp"
namespace MozziPrivate {
#endif


//////////////// TIMER ////////////////

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_EXTERNAL_TIMED)
void timer_callback_dummy(timer_callback_args_t __attribute__((unused)) *args){defaultAudioOutput();};
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
//void timer_callback_dummy(timer_callback_args_t __attribute__((unused)) *args){
void timer_callback_dummy(timer_callback_args_t __attribute__((unused)) *args){output_buffer.read();}; // to empty the buffer (the dac does not take care of it), a bit a waste of timer...
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
void timer_init() {
  uint8_t type;
  int8_t tindex = FspTimer::get_available_timer(type);

  if (tindex < 0) {
    tindex = FspTimer::get_available_timer(type, true);
  }

  if (tindex >= 0) {
    timer.begin(TIMER_MODE_PERIODIC, type, tindex, MOZZI_AUDIO_RATE, 50.0,timer_callback_dummy);
    timer.setup_overflow_irq();
  }

#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
  // we need to set up another timer for dac caring
  // note: it is running at the same speed than the other one, but could not manage
  // to get the other one updating the dac and removing the samples from the buffer…
  tindex = FspTimer::get_available_timer(type);

  if (tindex < 0) {
    tindex = FspTimer::get_available_timer(type, true);
  }

  if (tindex >= 0) {
    FspTimer::force_use_of_pwm_reserved_timer();
    timer_dac.begin(TIMER_MODE_PERIODIC, type, tindex, MOZZI_AUDIO_RATE, 50.0);
    timer_dac.setup_overflow_irq();
    dtc_cfg_extend.activation_source = timer_dac.get_cfg()->cycle_end_irq;
    timer_dac.open();
  }
#  endif // TODO: This endif used to be two lines up from here (above timer.open), which does not make sense syntactically, for external output
  timer.open();
}
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
inline void audioOutput(const AudioOutput f) {
  output_buffer.write(f+MOZZI_AUDIO_BIAS);
}
#  define canBufferAudioOutput() (!output_buffer.isFull())
#endif

static void startAudio() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
  dac_creation(MOZZI_AUDIO_PIN_1);
#endif
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
  timer_init(); // this need to be done between the DAC creation and initialization in the case where the on-board DAC is used, hence the ugly repetition here.
#endif
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC)
  dac_init();
  R_DTC_Open(&dtc_ctrl, &dtc_cfg);
  R_DTC_Enable(&dtc_ctrl);

  // The following branches the DAC straight on Mozzi's circular buffer.
  dtc_cfg.p_info->p_src = output_buffer.address();
  dtc_cfg.p_info->length = MOZZI_BUFFER_SIZE;
  R_DTC_Reconfigure(&dtc_ctrl, dtc_cfg.p_info);
  timer_dac.start();
#endif
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
  timer.start();
#endif
}

void stopMozzi() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_INTERNAL_DAC, MOZZI_OUTPUT_EXTERNAL_TIMED)
  timer.stop();
#endif
}
//// END AUDIO OUTPUT code ///////

//// BEGIN Random seeding ////////
void MozziRandPrivate::autoSeed() {
#warning Automatic random seeding is not implemented on this platform
}
//// END Random seeding ////////

} // namespace MozziPrivate
