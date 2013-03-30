#ifndef MOZZI_ANALOG_H_
#define MOZZI_ANALOG_H_


#include "mozzi_utils.h"

/** @defgroup analog Mozzi analog input functions for sensors and audio 
*/


void adcEnableInterrupt();
void adcReadAllChannels();
int adcGetResult(unsigned char channel_num);


// hack for Teensy 2 (32u4), pasted from hardware/arduino/variants/leonardo/pins_arduino.h
#if defined(__AVR_ATmega32U4__)
//	__AVR_ATmega32U4__ has an unusual mapping of pins to channels
extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )
#endif

void setupFastAnalogRead();
void disconnectDigitalIn(byte channel_num);
void reconnectDigitalIn(byte channel_num);
void adcDisconnectAllDigitalIns();
void adcReconnectAllDigitalIns();
void adcSetChannel(unsigned char pin);
void adcStartConversion();
void startAnalogRead(unsigned char pin);
int receiveAnalogRead();



#endif /* MOZZI_ANALOG_H_ */
