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

// The main point of this check is to document, what platform & variants this implementation file is for.
#if !(IS_RP2040())
#  error "Wrong implementation included for this platform"
#endif


////// BEGIN analog input code ////////

#define MOZZI_FAST_ANALOG_IMPLEMENTED

/** Implementation notes:
 *  - So nobody on the nets seems to have quite figured out, how to run the RP2040 ADC in "regular" asynchronous mode.
 *  - The ADC sports an interrupt, presumably to be triggered when a conversion is done, but how to connect a callback to that?
 *  - There is, however, an official example on a free running ADC writing to DMA (https://github.com/raspberrypi/pico-examples/blob/master/adc/dma_capture/dma_capture.c ; BSD licensed)
 *    We'll abuse that to connect a callback to the DMA channel, instead.
*/

#include <hardware/adc.h>
#include <hardware/dma.h>

#define getADCReading() rp2040_adc_result
#define channelNumToIndex(channel) channel

inline void adc_run_once () {           // see rp2040 sdk code for adc_read() vs adc_run()
    hw_set_bits(&adc_hw->cs, ADC_CS_START_ONCE_BITS); // vs ADC_CS_START_MANY_BITS
}

uint8_t adcPinToChannelNum(uint8_t pin) {
  if (pin >= 26) pin -= 26;     // allow analog to be called by GP or ADC numbering
  return pin;

}

void adcStartConversion(uint8_t channel) {
  adc_select_input(channel);
  adc_run_once();
  // adc_run(true);
}

void startSecondADCReadOnCurrentChannel() {
  adc_run_once();
  // adc_run(true);
}

void setupFastAnalogRead(int8_t speed) {
  if (speed == FAST_ADC) {
    adc_set_clkdiv(2048);  // Note: arbritray pick
  } else if (speed == FASTER_ADC) {
    adc_set_clkdiv(256);  // Note: arbritray pick
  } else {
    adc_set_clkdiv(0); // max speed
  }
}

void rp2040_adc_queue_handler();

static uint16_t rp2040_adc_result = 0;
int rp2040_adc_dma_chan;
void setupMozziADC(int8_t speed) {
  for (int i = 0;  i < NUM_ANALOG_INPUTS; ++i) {
    adc_gpio_init(i); 
  }

  adc_init();
  adc_fifo_setup(
      true,    // Write each completed conversion to the sample FIFO
      true,    // Enable DMA data request (DREQ)
      1,       // DREQ (and IRQ) asserted when at least 1 sample present
      false,   // Don't want ERR bit
      false    // Keep full sample range
  );
  
  uint dma_chan = dma_claim_unused_channel(true);
  rp2040_adc_dma_chan = dma_chan;
  static dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

  // Reading from constant address, writing to incrementing byte addresses
  channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
  channel_config_set_read_increment(&cfg, false);
  channel_config_set_write_increment(&cfg, false);  // we don't really want a fifo, just keep reading to the same address

  // Pace transfers based on availability of ADC samples
  channel_config_set_dreq(&cfg, DREQ_ADC);

  dma_channel_configure(dma_chan, &cfg,
      &rp2040_adc_result,    // dst
      &adc_hw->fifo,  // src
      1,              // transfer count
      true            // start immediately
  );

  // we want notification, when a sample has arrived
  dma_channel_set_irq0_enabled(dma_chan, true);
  irq_add_shared_handler(DMA_IRQ_0, rp2040_adc_queue_handler, PICO_SHARED_IRQ_HANDLER_DEFAULT_ORDER_PRIORITY);
  irq_set_enabled(DMA_IRQ_0, true);
  dma_channel_start(dma_chan);
}

void rp2040_adc_queue_handler() {
  if (!dma_channel_get_irq0_status(rp2040_adc_dma_chan)) return; // shared handler may get called on unrelated events
  dma_channel_acknowledge_irq0(rp2040_adc_dma_chan); // clear interrupt flag  
  //adc_run(false);  // adc not running continuous
  //adc_fifo_drain(); // no need to drain fifo, the dma transfer did that
  dma_channel_set_trans_count(rp2040_adc_dma_chan, 1, true);  // set up for another read
  advanceADCStep();
}
////// END analog input code ////////


////// BEGIN audio output code //////
#define LOOP_YIELD tight_loop_contents();  // apparently needed, among other things, to service the alarm pool


#if (RP2040_AUDIO_OUT_MODE == PWM_VIA_BARE_CHIP) || (EXTERNAL_AUDIO_OUTPUT == true)
#include <hardware/pwm.h>
#if (EXTERNAL_AUDIO_OUTPUT != true) // otherwise, the last stage - audioOutput() - will be provided by the user
inline void audioOutput(const AudioOutput f) {
  pwm_set_gpio_level(AUDIO_CHANNEL_1_PIN, f.l()+AUDIO_BIAS);
#if (AUDIO_CHANNELS > 1)
  pwm_set_gpio_level(AUDIO_CHANNEL_2_PIN, f.r()+AUDIO_BIAS);
#endif
}
#endif  // #if (EXTERNAL_AUDIO_OUTPUT != true)

#include <pico/time.h>
/** Implementation notes:
 *  - For the time being this port uses a very crude approach to audio output: PWM updated by a hardware timer running at AUDIO_RATE
 *  - Hardware timer isn't fixed, but rather we claim the first unclaimed one
 *  - Quite pleasently, the RP2040 latches PWM duty cycle, so we do not have to worry about updating whilst in the middle of the previous PWM cycle.
 *  - The more simple add_repeating_timer_us has appers to have far too much jitter
 *  - Using DMA transfers, instead of a manual timer, would be much more elegant, but I'll leave that as an exercise to the reader ;-)
 *  - Not to mention PWM output, etc.
 */
absolute_time_t next_audio_update;
uint64_t micros_per_update;
uint audio_update_alarm_num;

void audioOutputCallback(uint) {
  do {
    defaultAudioOutput();
    next_audio_update = delayed_by_us(next_audio_update, micros_per_update);
    // NOTE: hardware_alarm_set_target returns true, if the target was already missed. In that case, keep pushing samples, until we have caught up.
  } while (hardware_alarm_set_target(audio_update_alarm_num, next_audio_update));
}
 
#elif (RP2040_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
#include <I2S.h>
I2S i2s(OUTPUT);


inline bool canBufferAudioOutput() {
  return (i2s.availableForWrite());
}

inline void audioOutput(const AudioOutput f) {

#if (AUDIO_BITS == 8)
#if (AUDIO_CHANNELS > 1)
  i2s.write8(f.l(), f.r());
#else
  i2s.write8(f.l(), 0);
#endif
  
#elif (AUDIO_BITS == 16)
#if (AUDIO_CHANNELS > 1)
  i2s.write16(f.l(), f.r());
#else
  i2s.write16(f.l(), 0);
#endif
  
#elif (AUDIO_BITS == 24)
#if (AUDIO_CHANNELS > 1)
  i2s.write24(f.l(), f.r());
#else
  i2s.write24(f.l(), 0);
#endif
  
#elif (AUDIO_BITS == 32)
#if (AUDIO_CHANNELS > 1)
  i2s.write32(f.l(), f.r());
#else
  i2s.write32(f.l(), 0);
#endif
#else
  #error The number of AUDIO_BITS set in AudioConfigRP2040.h is incorrect
#endif  

  
}
#endif 


static void startAudio() {
#if (RP2040_AUDIO_OUT_MODE == PWM_VIA_BARE_CHIP) || (EXTERNAL_AUDIO_OUTPUT == true) // EXTERNAL AUDIO needs the timers set here
#if (EXTERNAL_AUDIO_OUTPUT != true)
  // calling analogWrite for the first time will try to init the pwm frequency and range on all pins. We don't want that happening after we've set up our own,
  // so we start off with a dummy call to analogWrite:
  analogWrite(AUDIO_CHANNEL_1_PIN, AUDIO_BIAS);
  // Set up fast PWM on the output pins
  // TODO: This is still very crude!
  pwm_config c = pwm_get_default_config();
  pwm_config_set_clkdiv(&c, 1);  // Fastest we can get: PWM clock running at full CPU speed
  pwm_config_set_wrap(&c, 1l << AUDIO_BITS);  // 11 bits output resolution means FCPU / 2048 values per second, which is around 60kHz for 133Mhz clock speed.
  pwm_init(pwm_gpio_to_slice_num(AUDIO_CHANNEL_1_PIN), &c, true);
  gpio_set_function(AUDIO_CHANNEL_1_PIN, GPIO_FUNC_PWM);
  gpio_set_drive_strength(AUDIO_CHANNEL_1_PIN, GPIO_DRIVE_STRENGTH_12MA); // highest we can get
#  if (AUDIO_CHANNELS > 1)
#    if ((AUDIO_CHANNEL_1_PIN / 2) != (AUDIO_CHANNEL_2_PIN / 2))
#      error Audio channel pins for stereo or HIFI must be on the same PWM slice (which is the case for the pairs (0,1), (2,3), (4,5), etc. Adjust AudioConfigRP2040.h .
#    endif
  gpio_set_function(AUDIO_CHANNEL_2_PIN, GPIO_FUNC_PWM);
  gpio_set_drive_strength(AUDIO_CHANNEL_2_PIN, GPIO_DRIVE_STRENGTH_12MA); // highest we can get
#  endif
#endif

  for (audio_update_alarm_num = 0; audio_update_alarm_num < 4; ++audio_update_alarm_num) {
    if (!hardware_alarm_is_claimed(audio_update_alarm_num)) {
      hardware_alarm_claim(audio_update_alarm_num);
      hardware_alarm_set_callback(audio_update_alarm_num, audioOutputCallback);
      break;
    }
  }
  micros_per_update = 1000000l / AUDIO_RATE;
  do {
    next_audio_update = make_timeout_time_us(micros_per_update);
    // See audioOutputCallback(), above. In _theory_ some interrupt stuff might delay us, here, causing us to miss the first beat (and everything that follows)
  } while (hardware_alarm_set_target(audio_update_alarm_num, next_audio_update));

#elif (RP2040_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
  i2s.setBCLK(BCLK_PIN);
  i2s.setDATA(DOUT_PIN);
  i2s.setBitsPerSample(AUDIO_BITS);
  
#if (AUDIO_BITS > 16)
  i2s.setBuffers(BUFFERS, (size_t) (BUFFER_SIZE/BUFFERS), 0);
#else
  i2s.setBuffers(BUFFERS, (size_t) (BUFFER_SIZE/BUFFERS/2), 0);
#endif
#if (LSBJ_FORMAT == true)
  i2s.setLSBJFormat();
#endif
  i2s.begin(AUDIO_RATE);
#endif
}

void stopMozzi() {
#if (RP2040_AUDIO_OUT_MODE == PWM_VIA_BARE_CHIP) || (EXTERNAL_AUDIO_OUTPUT == true)
  hardware_alarm_set_callback(audio_update_alarm_num, NULL);
#elif (RP2040_AUDIO_OUT_MODE == EXTERNAL_DAC_VIA_I2S)
  i2s.end();
#endif
  
}
////// END audio output code //////
