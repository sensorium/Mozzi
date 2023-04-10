#ifndef AUDIOCONFIGSTM32_H
#define AUDIOCONFIGSTM32_H

#if not IS_STM32DUINO()
#error This header should be included for STM32 (stm32duino.com core), only
#endif

// Audio output pin. If you want to change this, make sure to also set AUDIO_PWM_TIMER to whichever timer is responsible for your PWM pin, and set the other timers to non-conflicting values
#define AUDIO_CHANNEL_1_PIN PA8  // Note: PB8 does not appear to be available as a PWM pin with this core.
// The timer used for running the audio update loop. This must _not_ be the same timer responsible for PWM on the output pins! NOTE: Timer 3 appears to clash with SPI DMA transfers under some circumstances
#define AUDIO_UPDATE_TIMER TIM2

#if (AUDIO_MODE == HIFI)
// Second out pin for HIFI mode. This must be on the same timer as AUDIO_CHANNEL_1_PIN!
// Note that by default we are not using adjacent pins. This is to leave the "Serial1" pins available (often used for upload/communication with Arduino IDE). If you don't need that, PA9 is a good choice.
#define AUDIO_CHANNEL_1_PIN_HIGH PA9
// Total audio bits.
#define AUDIO_BITS 14
#define AUDIO_BITS_PER_CHANNEL 7
#else
// The more audio bits you use, the slower the carrier frequency of the PWM signal. 10 bits yields ~ 70kHz on a 72Mhz CPU (which appears to be a reasonable compromise)
#define AUDIO_BITS 10
#define AUDIO_BITS_PER_CHANNEL AUDIO_BITS
#if (AUDIO_CHANNELS > 1)
// Second out pin for stereo mode. This must be on the same timer as AUDIO_CHANNEL_1_PIN!
#define AUDIO_CHANNEL_2_PIN PA9
#endif
#endif

#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))

#endif        //  #ifndef AUDIOCONFIGSTM32_H

