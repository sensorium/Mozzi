/*

Parts of this file are drawn from Arduino's source code for analogRead() (https://github.com/arduino/ArduinoCore-renesas/blob/main/cores/arduino/analog.cpp) and part from Renesas' documentation (https://renesas.github.io/fsp/group___a_d_c.html), among other things.
It contains functions to interact with the ADC in order to implement async ADC reads, aka mozziAnalogRead().
*/

#include <analog.h>
#include <analog.cpp>
#include <IRQManager.h>




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
