#ifndef MOZZI_ANALOG_H_
#define MOZZI_ANALOG_H_


#include "mozzi_utils.h"

#if USE_AUDIO_INPUT
#warning "Using analog pin 0 for audio input.  No other analog channels are available while using audio."
#endif

/** @defgroup analog Mozzi analog input functions for sensors and audio 
*/

void adcEnableInterrupt();
void setupFastAnalogRead();
void disconnectDigitalIn(byte channel_num);
void reconnectDigitalIn(byte channel_num);
void adcDisconnectAllDigitalIns();
void adcReconnectAllDigitalIns();
void adcSetChannel(unsigned char pin);
void adcStartConversion();
void adcStartConversion(unsigned char pin);
//void startAnalogRead(unsigned char pin);
//int receiveAnalogRead();
int adcGetResult();

/*
Currently, if audio input is enabled it uses analog input channel 0 and doesn't
allow other channels to be used, to avoid interference between the strict audio
rate sampling and any control analog conversions which could be in progress when
the audio needs to be sampled. This might be solved in future, but workable
solutions for control inputs include RC circuits on digital pins, switches, or
serial connections (which also may require extra solutions for asynchronous
operation). There is a fork of Mozzi on github (posted on the mozzi-users list)
which includes a 2-wire async serial adaptation. It would be good eventually to
merge that with the main repo.
*/
#if !USE_AUDIO_INPUT
void adcReadAllChannels();
int adcGetResult(unsigned char channel_num);
#endif


// hack for Teensy 2 (32u4), pasted from hardware/arduino/variants/leonardo/pins_arduino.h
#if defined(__AVR_ATmega32U4__)
//	__AVR_ATmega32U4__ has an unusual mapping of pins to channels
extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )
#endif


#endif /* MOZZI_ANALOG_H_ */
