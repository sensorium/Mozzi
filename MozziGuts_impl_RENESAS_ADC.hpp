/*

Parts of this file are drawn from Arduino's source code for analogRead() (https://github.com/arduino/ArduinoCore-renesas/blob/main/cores/arduino/analog.cpp) and part from Renesas' documentation (https://renesas.github.io/fsp/group___a_d_c.html), among other things.
It contains functions to interact with the ADC in order to implement async ADC reads, aka mozziAnalogRead().
*/

#include <analog.h>
//#include <analog.cpp>
#include <IRQManager.h>


/** VERBATIM from Arduino's analog.cpp
 */
#define MAX_ADC_CHANNELS   29
static uint16_t analog_values_by_channels[MAX_ADC_CHANNELS] = {0};
static void ADC_irq_cbk(adc_callback_args_t * cb_data);
static ADC_Container adc(0,ADC_irq_cbk);
static ADC_Container adc1(1,ADC_irq_cbk);

static ADCIrqCbk_f scan_complete_cbk = nullptr;
static ADCIrqCbk_f scan_complete_b_cbk = nullptr;
static ADCIrqCbk_f window_compare_a_cbk = nullptr;
static ADCIrqCbk_f window_compare_b_cbk = nullptr;

static void readAllGroupA(ADC_Container *_adc) {
  for(int i = 0; i < MAX_ADC_CHANNELS; i++) {
    if(_adc->channel_cfg.scan_mask & (1 << i)) {
      //is the channel active -> yes, read it
      R_ADC_Read(&(_adc->ctrl), (adc_channel_t)i, analog_values_by_channels + i);
    }
  }
}

static void readAllGroupB(ADC_Container *_adc) {
  for(int i = 0; i < MAX_ADC_CHANNELS; i++) {
    if(_adc->channel_cfg.scan_mask_group_b & (1 << i)) {
      //is the channel active -> yes, read it
      R_ADC_Read(&(_adc->ctrl), (adc_channel_t)i, analog_values_by_channels + i);
    }
  }
}


static void ADC_irq_cbk(adc_callback_args_t * cb_data) {
  if(cb_data->event == ADC_EVENT_SCAN_COMPLETE) {
    if(scan_complete_cbk != nullptr) {
      if(cb_data->unit == 0) {
        readAllGroupA(&adc);
      }
      else if(cb_data->unit == 1) {
        readAllGroupA(&adc1);
      }
      scan_complete_cbk(cb_data->unit);
    }
  }
  else if(cb_data->event == ADC_EVENT_SCAN_COMPLETE_GROUP_B) {
    if(scan_complete_b_cbk != nullptr) {
      if(cb_data->unit == 0) {
        readAllGroupB(&adc);
      }
      else if(cb_data->unit == 1) {
        readAllGroupB(&adc1);
      }
      scan_complete_b_cbk(cb_data->unit);
    }
  }
  else if(cb_data->event == ADC_EVENT_WINDOW_COMPARE_A) {
    if(window_compare_a_cbk != nullptr) {
      window_compare_a_cbk(cb_data->unit);
    }
  }
  else if(cb_data->event == ADC_EVENT_WINDOW_COMPARE_B) {
    if(window_compare_b_cbk != nullptr) {
      window_compare_b_cbk(cb_data->unit);
    }
  }
}

/* -------------------------------------------------------------------------- */
static ADC_Container *get_ADC_container_ptr(int32_t pin, uint16_t &cfg) {
/* -------------------------------------------------------------------------- */  
  ADC_Container *rv = nullptr;
  auto cfg_adc = getPinCfgs(pin, PIN_CFG_REQ_ADC);
  if(cfg_adc[0] > 0 ) {
    if(IS_ADC1(cfg_adc[0])) {
      rv = &adc1;
    }
    else {
      rv = &adc;
    }
  }
  cfg = cfg_adc[0];
  return rv;

}

/* END of verbatim
 */
//////////////////// ADC //////////////

void startScan(int pin)
{
  int32_t adc_idx = digitalPinToAnalogPin(pin);
  ADC_Container *_adc = get_ADC_container_ptr(adc_idx, cfg_adc);
  _adc->cfg.mode = ADC_MODE_SINGLE_SCAN;
  pinPeripheral(digitalPinToBspPin(adc_idx), (uint32_t)IOPORT_CFG_ANALOG_ENABLE);
  _adc->channel_cfg.scan_mask |= (1 << GET_CHANNEL(cfg_adc));
  R_ADC_Open(&(_adc->ctrl), &(_adc->cfg));
  R_ADC_CallbackSet(&(_adc->ctrl), adc_callback, p_context, p_callback_memory);
  R_ADC_ScanCfg(&(_adc->ctrl), &(_adc->channel_cfg));
  R_ADC_ScanStart(&(_adc->ctrl));
}

uint16_t readADC(int pin)
{
  uint16_t result;
  int32_t adc_idx = digitalPinToAnalogPin(pin);
  ADC_Container *_adc = get_ADC_container_ptr(adc_idx, cfg_adc);
  R_ADC_Read(&(_adc->ctrl), (adc_channel_t)GET_CHANNEL(cfg_adc), &result);
  return result;
}
