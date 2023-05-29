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

#if !(IS_FSP())
#  error "Wrong implementation included for this platform"
#endif

#include <FspTimer.h>


////// BEGIN analog input code ////////

//#define MOZZI_FAST_ANALOG_IMPLEMENTED

#define getADCReading() 0


#define channelNumToIndex(channel) channel
uint8_t adcPinToChannelNum(uint8_t pin) {
  return pin;
}

/** NOTE: Code needed to trigger a conversion on a new channel */
void adcStartConversion(uint8_t channel) {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to trigger a subsequent conversion on the latest channel. If your platform has no special code for it, you should store the channel from
 *  adcStartConversion(), and simply call adcStartConversion(previous_channel), here. */
void startSecondADCReadOnCurrentChannel() {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to set up faster than usual analog reads, e.g. specifying the number of CPU cycles that the ADC waits for the result to stabilize.
 *  This particular function is not super important, so may be ok to leave empty, at least, if the ADC is fast enough by default. */
void setupFastAnalogRead(int8_t speed) {
#warning Fast analog read not implemented on this platform
}

/** NOTE: Code needed to initialize the ADC for asynchronous reads. Typically involves setting up an interrupt handler for when conversion is done, and
 *  possibly calibration. */
void setupMozziADC(int8_t speed) {
#warning Fast analog read not implemented on this platform
}

/* NOTE: Most platforms call a specific function/ISR when conversion is complete. Provide this function, here.
 * From inside its body, simply call advanceADCStep(). E.g.:
 void stm32_adc_eoc_handler() {
 advanceADCStep();
 }
*/
////// END analog input code ////////



//// BEGIN AUDIO OUTPUT code ///////


FspTimer timer;

#if (EXTERNAL_AUDIO_OUTPUT != true) // otherwise, the last stage - audioOutput() - will be provided by the user
FspTimer timer_dac;
#include <dac.h>
#include <FspTimer.h>
#include <r_dtc.h>


/*
Good parts of the following are from analogWave example class
*/

volatile uint32_t pin;
CircularBuffer<uint16_t> output_buffer;
uint8_t dac_bits;
dtc_instance_ctrl_t dtc_ctrl;
transfer_info_t dtc_info;
dtc_extended_cfg_t dtc_cfg_extend;
transfer_cfg_t dtc_cfg;

////////////// DAC CREATION AND FIRST INIT ///////////////////
void dac_creation(pin_size_t pinN) {
  if (IS_DAC(pinN)) {
    auto cfg_dac = getPinCfgs(pinN, PIN_CFG_REQ_DAC);
    pin = cfg_dac[0];
    if (IS_DAC_8BIT(pin)) {
      dac_bits = 8;
      dtc_info.transfer_settings_word_b.size = TRANSFER_SIZE_1_BYTE;
      if (GET_CHANNEL(pin) == 0) {
#ifdef DAC_ADDRESS_8_CH0
        dtc_info.p_dest = (void *)DAC_ADDRESS_8_CH0;
#endif
      } else if (GET_CHANNEL(pin) == 1) {
#ifdef DAC_ADDRESS_8_CH1
        dtc_info.p_dest = (void *)DAC_ADDRESS_8_CH1;
#endif
      }
    } else {
      dac_bits = 12;
      dtc_info.transfer_settings_word_b.size = TRANSFER_SIZE_2_BYTE;
      if (GET_CHANNEL(pin) == 0) {
#ifdef DAC_ADDRESS_12_CH0
        dtc_info.p_dest = (void *)DAC_ADDRESS_12_CH0;
#endif
      } else if (GET_CHANNEL(pin) == 1) {
#ifdef DAC_ADDRESS_12_CH1
        dtc_info.p_dest = (void *)DAC_ADDRESS_12_CH1;
#endif
      }
    }
  }

  dtc_info.transfer_settings_word_b.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED;
  dtc_info.transfer_settings_word_b.repeat_area = TRANSFER_REPEAT_AREA_SOURCE;
  dtc_info.transfer_settings_word_b.irq = TRANSFER_IRQ_END;
  dtc_info.transfer_settings_word_b.chain_mode = TRANSFER_CHAIN_MODE_DISABLED;
  dtc_info.transfer_settings_word_b.src_addr_mode =
    TRANSFER_ADDR_MODE_INCREMENTED;
  dtc_info.transfer_settings_word_b.mode = TRANSFER_MODE_REPEAT;
  dtc_info.p_src = (void const *)NULL;

  dtc_info.num_blocks = 0;
  dtc_info.length = 0;

  dtc_cfg.p_info = &dtc_info;
  dtc_cfg.p_extend = &dtc_cfg_extend;

  dtc_cfg_extend.activation_source = FSP_INVALID_VECTOR;
}

void dac_init() {
  if (IS_DAC_8BIT(pin)) {
#if DAC8_HOWMANY > 0
    if (GET_CHANNEL(pin) < DAC8_HOWMANY) {
      _dac8[GET_CHANNEL(pin)].init();
    }
#endif
  } else {
    if (GET_CHANNEL(pin) < DAC12_HOWMANY) {
      _dac12[GET_CHANNEL(pin)].init();
    }
  }
}

#endif


//////////////// TIMER ////////////////

#if EXTERNAL_AUDIO_OUTPUT == true
void timer_callback_dummy(timer_callback_args_t __attribute__((unused)) *args){defaultAudioOutput();};
#else
//void timer_callback_dummy(timer_callback_args_t __attribute__((unused)) *args){
void timer_callback_dummy(timer_callback_args_t __attribute__((unused)) *args){output_buffer.read();}; // to empty the buffer (the dac does not take care of it), a bit a waste of timer...
#endif

void timer_init() {
  uint8_t type = 0;
  int8_t tindex = FspTimer::get_available_timer(type);

  if (tindex < 0) {
    tindex = FspTimer::get_available_timer(type, true);
  }

  if (tindex >= 0) {
    timer.begin(TIMER_MODE_PERIODIC, type, tindex, AUDIO_RATE, 50.0,timer_callback_dummy);
    timer.setup_overflow_irq();
  }
  
#if EXTERNAL_AUDIO_OUTPUT != true // we need to set up another timer for dac caring
  // note: it is running at the same speed than the other one, but could not manage
  // to get the other one updating the dac and removing the samples from the buffer…
    tindex = FspTimer::get_available_timer(type);

  if (tindex < 0) {
    tindex = FspTimer::get_available_timer(type, true);
  }

  if (tindex >= 0) {
    FspTimer::force_use_of_pwm_reserved_timer();
    timer_dac.begin(TIMER_MODE_PERIODIC, type, tindex, AUDIO_RATE, 50.0);
    timer_dac.setup_overflow_irq();    
     dtc_cfg_extend.activation_source = timer_dac.get_cfg()->cycle_end_irq;
     timer_dac.open();    
#endif
    timer.open();
  }
}


inline void audioOutput(const AudioOutput f) {
  output_buffer.write(f+AUDIO_BIAS);
}
#define canBufferAudioOutput() (!output_buffer.isFull())


static void startAudio() {
#if EXTERNAL_AUDIO_OUTPUT != true
  dac_creation(AUDIO_CHANNEL_1_PIN);
#endif
  timer_init();
#if EXTERNAL_AUDIO_OUTPUT != true
  dac_init();
  R_DTC_Open(&dtc_ctrl, &dtc_cfg);
  R_DTC_Enable(&dtc_ctrl);

  dtc_cfg.p_info->p_src = output_buffer.address();
  dtc_cfg.p_info->length = MOZZI_BUFFER_SIZE;
  R_DTC_Reconfigure(&dtc_ctrl, dtc_cfg.p_info);
  timer_dac.start();
#endif
  timer.start();
  
}

void stopMozzi() {
  timer.stop();
}
//// END AUDIO OUTPUT code ///////
