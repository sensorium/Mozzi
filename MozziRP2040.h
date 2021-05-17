/*
 * MozziRP2040.h
 * 
 * Mozzi Support for the Rasperry Pico for the Arduino Kernels which provide the full Rasperry Pico API
 * ATTENTION: The standard Arduino implementation uses the ARM Mbed! So this would be the wrong place
  
 * Copyright 2012 Tim Barrass.
 * Copyright 2021 Phil Schatzmann.
 * 
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#include "hardware_defines.h"

#if IS_RP2040() 

#include "MozziCommon.h"
#include "AudioConfigRP2040.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/time.h"

// Forward declarations of callbacks
static bool defaultAudioOutputCallbackX(repeating_timer* ptr);

/**
* @brief  Hardware specific implementation class for the Raspberry Pico API - Please note that the standard Arduino implementation is based on MBed and does not
* use this implementation 
*/

class MozziRP2040  {
  friend bool defaultAudioOutputCallbackX(repeating_timer* ptr);

  public:

    void start(int control_rate_hz) {
      setupADC(); 
      Common.startControl(control_rate_hz);
      startAudio();
      Serial.printf("Mozzi started with AUDIO_RATE %u\n", AUDIO_RATE);
    }

    void stop() {
        if (alarm_id!=-1){
          cancel_repeating_timer(&timer);
        }
        stopPWM(pins());
    }

    unsigned long audioTicks() {
      return Common.audioTicks();
    }

    unsigned long mozziMicros() { 
      return Common.mozziMicros();;
    }

    void audioHook()  {
      Common.audioHook();
    }

    const int channels() const {
        return Common.channels();
    }

    const int16_t* pins() {
        return Common.pins();
    }

    void setPin(uint8_t idx, int16_t pin){
        Common.setPin(idx, pin);
    }

#if USE_AUDIO_INPUT == true
    // Provides the analog value w/o delay
    int16_t MozziClass::getAudioInput() { 
      return Common.audio_input;
    }

#endif

  protected:

    uint pwm_slice_num;
    alarm_id_t alarm_id=-1;
    alarm_pool_t *ap;
    repeating_timer_t timer;
    uint8_t max_channel = 0; // last valid channel with a pin defined
    char msg[80];

    //-----------------------------------------------------------------------------------------------------------------
    /// Setup
    //-----------------------------------------------------------------------------------------------------------------

    void startAudio() {
      // this supports all AUDIO_MODE settings
      setupPWM(pins());
      // setup timer for defaultAudioOutput
      setupOutputTimer();
    }


    void setupPWMPin(int pin){
      uint slice = pwm_gpio_to_slice_num(pin);
      uint channel = pwm_gpio_to_channel(pin);

      gpio_set_function(pin, GPIO_FUNC_PWM);
      Serial.printf("Using Pin %d on pico channel %d with slice %d\n",pin, channel, slice);

      // Set period of AUDIO_BITS cycles (0 to AUDIO_BITS inclusive)
      pwm_set_wrap(slice, PWM_CYCLES);
      // Set channel A output high for 0 cycle before dropping
      pwm_set_chan_level(slice, PWM_CHAN_A, 0); // pin 14
      // Set the PWM running
      pwm_set_enabled(slice, true);
    }


    void setupPWM(const int16_t pins[]) {
      // setup clock
      pwm_config cfg = pwm_get_default_config();
      pwm_config_set_clkdiv (&cfg, PWM_CLOCK_DIV);

      // setup pins
      for (int j=0;j<AUDIO_CHANNELS;j++){
        int16_t gpio = pins[j];
        if  (gpio >- 1)  {
            sprintf(msg,"Channel %d -> pin %d", j, gpio);
            Serial.println(msg);
            setupPWMPin(gpio);
            max_channel = j + 1;
        } else {
            sprintf(msg,"Channel %d -> pin %d is already defined", j, gpio);
            Serial.println(msg);
        }
      }
    }

    void stopPWM(const int16_t pins[]){
        // stop all pins
        for (int j=0;j<max_channel;j++){
          int gpio = pins[j];
          if  (gpio >- 1)  {
            uint slice = pwm_gpio_to_slice_num(gpio);
            pwm_set_enabled(slice, false);
          } 
        }
    }

    void setupOutputTimer() {
        uint64_t time = 1000000UL / AUDIO_RATE;
        if (!add_repeating_timer_ms(-time, defaultAudioOutputCallbackX, nullptr, &timer)){
          Serial.println("Error: alarm_pool_add_repeating_timer_us failed; no alarm slots available");
        }
    }


    //-----------------------------------------------------------------------------------------------------------------
    // Input
    //-----------------------------------------------------------------------------------------------------------------

    #if (USE_AUDIO_INPUT == true)

    void  setupADC() {
      adc_init();

      // Make sure GPIO is high-impedance, no pullups etc
      adc_gpio_init(AUDIO_INPUT_PIN);
      // Select ADC input 0 (GPIO26)
      adc_select_input(AUDIO_CHANNEL_IN);
    }

    #elif (USE_AUDIO_INPUT == false)

    void setupADC(){ }

    #endif


    //-----------------------------------------------------------------------------------------------------------------
    // Output
    //-----------------------------------------------------------------------------------------------------------------

    void writePWM(int16_t pin, int16_t value){
      uint slice = pwm_gpio_to_slice_num(pin);
      uint channel = pwm_gpio_to_channel(pin);
      pwm_set_chan_level(slice, channel, value);
    }


    /// Output will always be on both pins!
    void audioOutput(const AudioOutputStorage_t value) {
      const int16_t *pins_ptr = pins();
      #if (AUDIO_MODE == HIFI)
        writePWM(pins_ptr[0], (value+AUDIO_BIAS) & ((1 << AUDIO_BITS_PER_CHANNEL) - 1));
        writePWM(pins_ptr[1], (value+AUDIO_BIAS) >> AUDIO_BITS_PER_CHANNEL);
      #else
        writePWM(pins_ptr[0], value+AUDIO_BIAS);
      #endif
    }

    void audioOutput(const AudioOutput value) {
        // output values to pins
      const int16_t *pins_ptr = pins();
      for (int j=0;j<max_channel;j++){
          AudioOutputStorage_t v = value[j] + AUDIO_BIAS;
          writePWM(pins_ptr[j], v);    
      }
    }
};


//-----------------------------------------------------------------------------------------------------------------
/// Callbacks 
//-----------------------------------------------------------------------------------------------------------------

extern MozziRP2040 Mozzi;

bool defaultAudioOutputCallbackX(repeating_timer* ptr) {
  Mozzi.audioOutput(Common.output_buffer.read());
  return true;
}


#endif  // IS_RP2040
