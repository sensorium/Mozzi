/*
Most of this file is drawn, more or less adapted from the AnalogWave example
for Renesas board from Arduino.
It contains functions to create and start the on-board DAC (and associate timer).
*/

FspTimer timer_dac;
#include <dac.h>
#include <FspTimer.h>
#include <r_dtc.h>


volatile uint32_t pin;
uint8_t dac_bits;
dtc_instance_ctrl_t dtc_ctrl;
transfer_info_t dtc_info;
dtc_extended_cfg_t dtc_cfg_extend;
transfer_cfg_t dtc_cfg;

////////////// DAC CREATION AND FIRST INIT ///////////////////
// DAC creation, will take care of specifying the number of bits according to the
// capacity of the DAC (12 only for now) and set up the transfer mode straight from
// an external buffer.
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


// DAC initialization
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

