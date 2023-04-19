#ifndef AUDIOCONFIGMBED_H
#define AUDIOCONFIGMBED_H

#if not IS_MBED()
#error This header should be included for MBED OS boards, only
#endif

#if (AUDIO_MODE == HIFI)
#error HIFI mode is not available for this CPU architecture (but several high quality output options are available)
#endif

// Audio output options
#define INTERNAL_DAC 1 // output using internal DAC driven via DMA. Output is only possible on the DAC pins (A12, and A13 on the Giga)
#define PDM_VIA_SERIAL 2 // output PDM coded sample on a hardware serial UART. NOTE: currently to be considered experimental. Tune is not correct for all combinatinos of AUDIO_RATE & PDM_RESOLUTION
                         // Also NOTE that you will almost certainly want to use at least some basic RC filter circuit with this mode

// Set output mode
#define MBED_AUDIO_OUT_MODE INTERNAL_DAC

#if (MBED_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
// For use in PDM_VIA_SERIAL, only: Peripheral to use. Note that only the TX channel is actually used, but sine this is a hardware UART, the corresponding
// RX channel needs to be claimed, as well. NOTE: This does not necessarily correspond to the labeling on your board! E.g. SERIAL2_TX is TX1 on the Arduino Giga.
#define PDM_SERIAL_UART_TX_CHANNEL_1 SERIAL2_TX
#define PDM_SERIAL_UART_RX_CHANNEL_1 SERIAL2_RX
#endif

/// User config end. Do not modify below this line

#if (MBED_AUDIO_OUT_MODE == INTERNAL_DAC)
#define AUDIO_BITS 12
#define AUDIO_CHANNEL_1_PIN A13
#define AUDIO_CHANNEL_2_PIN A12
#define BYPASS_MOZZI_OUTPUT_BUFFER true
#elif (MBED_AUDIO_OUT_MODE == PDM_VIA_SERIAL)
#define AUDIO_BITS 16  // well, used internally, at least. The pins will not be able to actually produce this many bits
#define PDM_RESOLUTION 2
#define BYPASS_MOZZI_OUTPUT_BUFFER true
#else
#error Invalid output mode configured in AudioConfigMBED.h
#endif

//#define BYPASS_MOZZI_INPUT_BUFFER true
#define AUDIO_BIAS ((uint16_t) 1<<(AUDIO_BITS-1))

#endif        //  #ifndef AUDIOCONFIGMBED_H
