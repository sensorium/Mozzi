/*
 * MozziGuts_impl_RP2040.hpp
 *
 * This file is part of Mozzi.
 *
 * Copyright 2022-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

// The main point of this check is to document, what platform & variants this implementation file is for.
#if !(IS_RP2040())
#  error "Wrong implementation included for this platform"
#endif

#include <hardware/dma.h>

namespace MozziPrivate {

////// BEGIN analog input code ////////

#if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_READ_STANDARD)

/** Implementation notes:
 *  - So nobody on the nets seems to have quite figured out, how to run the RP2040 ADC in "regular" asynchronous mode.
 *  - The ADC sports an interrupt, presumably to be triggered when a conversion is done, but how to connect a callback to that?
 *  - There is, however, an official example on a free running ADC writing to DMA (https://github.com/raspberrypi/pico-examples/blob/master/adc/dma_capture/dma_capture.c ; BSD licensed)
 *    We'll abuse that to connect a callback to the DMA channel, instead.
*/

} // namespace MozziPrivate

#include <hardware/adc.h>

namespace MozziPrivate {

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
  for (int i = 0; i < (int) NUM_ANALOG_INPUTS; ++i) {
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

#endif // MOZZI_ANALOG_READ

////// END analog input code ////////


////// BEGIN audio output code //////
#define LOOP_YIELD tight_loop_contents();  // apparently needed, among other things, to service the alarm pool


#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
#include <hardware/pwm.h>

#  if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
inline void audioOutput(const AudioOutput f) {
  pwm_set_gpio_level(MOZZI_AUDIO_PIN_1, f.l()+MOZZI_AUDIO_BIAS);
#    if (MOZZI_AUDIO_CHANNELS > 1)
  pwm_set_gpio_level(MOZZI_AUDIO_PIN_2, f.r()+MOZZI_AUDIO_BIAS);
#    endif
}
#  endif  // MOZZI_OUTPUT_PWM

} // namespace MozziPrivate
#include <pico/time.h>
namespace MozziPrivate {
/** Implementation notes:
 *  - For the time being this port uses a very crude approach to audio output: PWM updated by a hardware timer running at MOZZI_AUDIO_RATE
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
 
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
} // namespace MozziPrivate
#include <I2S.h>
namespace MozziPrivate {
I2S i2s(OUTPUT);

inline bool canBufferAudioOutput() {
  return (i2s.availableForWrite());
}

inline void audioOutput(const AudioOutput f) {

#  if (MOZZI_AUDIO_BITS == 8)
#    if (MOZZI_AUDIO_CHANNELS > 1)
  i2s.write8(f.l(), f.r());
#    else
  i2s.write8(f.l(), 0);
#    endif
  
#  elif (MOZZI_AUDIO_BITS == 16)
#    if (MOZZI_AUDIO_CHANNELS > 1)
  i2s.write16(f.l(), f.r());
#    else
  i2s.write16(f.l(), 0);
#    endif
  
#  elif (MOZZI_AUDIO_BITS == 24)
#    if (MOZZI_AUDIO_CHANNELS > 1)
  i2s.write24(f.l(), f.r());
#    else
  i2s.write24(f.l(), 0);
#    endif
  
#  elif (MOZZI_AUDIO_BITS == 32)
#    if (MOZZI_AUDIO_CHANNELS > 1)
  i2s.write32(f.l(), f.r());
#    else
  i2s.write32(f.l(), 0);
#    endif
#  else
#    error Invalid number of MOZZI_AUDIO_BITS configured
#  endif  

}
#endif 


static void startAudio() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
  // calling analogWrite for the first time will try to init the pwm frequency and range on all pins. We don't want that happening after we've set up our own,
  // so we start off with a dummy call to analogWrite:
  analogWrite(MOZZI_AUDIO_PIN_1, MOZZI_AUDIO_BIAS);
  // Set up fast PWM on the output pins
  // TODO: This is still very crude!
  pwm_config c = pwm_get_default_config();
  pwm_config_set_clkdiv(&c, 1);  // Fastest we can get: PWM clock running at full CPU speed
  pwm_config_set_wrap(&c, 1l << MOZZI_AUDIO_BITS);  // 11 bits output resolution means FCPU / 2048 values per second, which is around 60kHz for 133Mhz clock speed.
  pwm_init(pwm_gpio_to_slice_num(MOZZI_AUDIO_PIN_1), &c, true);
  gpio_set_function(MOZZI_AUDIO_PIN_1, GPIO_FUNC_PWM);
  gpio_set_drive_strength(MOZZI_AUDIO_PIN_2, GPIO_DRIVE_STRENGTH_12MA); // highest we can get
#  if (MOZZI_AUDIO_CHANNELS > 1)
#    if ((MOZZI_AUDIO_PIN_1 / 2) != (MOZZI_AUDIO_PIN_1 / 2))
#      error Audio channel pins for stereo or HIFI must be on the same PWM slice (which is the case for the pairs (0,1), (2,3), (4,5), etc. Adjust MOZZI_AUDIO_PIN_1/2 .
#    endif
  gpio_set_function(MOZZI_AUDIO_PIN_2, GPIO_FUNC_PWM);
  gpio_set_drive_strength(MOZZI_AUDIO_PIN_2, GPIO_DRIVE_STRENGTH_12MA); // highest we can get
#  endif
#endif

#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
  for (audio_update_alarm_num = 0; audio_update_alarm_num < 4; ++audio_update_alarm_num) {
    if (!hardware_alarm_is_claimed(audio_update_alarm_num)) {
      hardware_alarm_claim(audio_update_alarm_num);
      hardware_alarm_set_callback(audio_update_alarm_num, audioOutputCallback);
      break;
    }
  }
  micros_per_update = 1000000l / MOZZI_AUDIO_RATE;
  do {
    next_audio_update = make_timeout_time_us(micros_per_update);
    // See audioOutputCallback(), above. In _theory_ some interrupt stuff might delay us, here, causing us to miss the first beat (and everything that follows)
  } while (hardware_alarm_set_target(audio_update_alarm_num, next_audio_update));

#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
  i2s.setBCLK(MOZZI_I2S_PIN_BCK);
  i2s.setDATA(MOZZI_I2S_PIN_DATA);
  i2s.setBitsPerSample(MOZZI_AUDIO_BITS);
  
#  if (MOZZI_AUDIO_BITS > 16)
  i2s.setBuffers(MOZZI_RP2040_BUFFERS, (size_t) (MOZZI_RP2040_BUFFER_SIZE/MOZZI_RP2040_BUFFERS), 0);
#  else
  i2s.setBuffers(MOZZI_RP2040_BUFFERS, (size_t) (MOZZI_RP2040_BUFFER_SIZE/MOZZI_RP2040_BUFFERS/2), 0);
#  endif
#  if MOZZI_IS(MOZZI_I2S_FORMAT, MOZZI_I2S_FORMAT_LSBJ)
  i2s.setLSBJFormat();
#  endif
  i2s.begin(MOZZI_AUDIO_RATE);
#endif
}

void stopMozzi() {
#if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
  hardware_alarm_set_callback(audio_update_alarm_num, NULL);
#elif MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_I2S_DAC)
  i2s.end();
#endif
  
}
////// END audio output code //////

//// BEGIN Random seeding ////////
void MozziRandPrivate::autoSeed() {
#warning Automatic random seeding is not implemented on this platform
}
//// END Random seeding ////////

} // namespace MozziPrivate

#undef MOZZI_RP2040_BUFFERS
#undef MOZZI_RP2040_BUFFER_SIZE
