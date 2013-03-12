/*
  FrequencyTimer2.h - A frequency generator and interrupt generator library
  Author: Jim Studt, jim@federated.com
  Copyright (c) 2007 David A. Mellis.  All right reserved.

  http://www.arduino.cc/playground/Code/FrequencyTimer2

  Version 2 - updated by Paul Stoffregen, paul@pjrc.com
  for compatibility with newer hardware and Arduino 1.0

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <FrequencyTimer2.h>

#include <avr/interrupt.h>

void (*FrequencyTimer2::onOverflow)() = 0;
uint8_t FrequencyTimer2::enabled = 0;

#if defined(TIMER2_COMPA_vect)
ISR(TIMER2_COMPA_vect)
#elif defined(TIMER2_COMP_vect)
ISR(TIMER2_COMP_vect)
#else
#error "This board does not have a hardware timer which is compatible with FrequencyTimer2"
void dummy_function(void)
#endif
{
    static uint8_t inHandler = 0; // protect us from recursion if our handler enables interrupts

    if ( !inHandler && FrequencyTimer2::onOverflow) {
	inHandler = 1;
	(*FrequencyTimer2::onOverflow)();
	inHandler = 0;
    }
}

void FrequencyTimer2::setOnOverflow( void (*func)() )
{
    FrequencyTimer2::onOverflow = func;
#if defined(TIMSK2)
    if ( func) TIMSK2 |= _BV(OCIE2A);
    else TIMSK2 &= ~_BV(OCIE2A);
#elif defined(TIMSK)
    if ( func) TIMSK |= _BV(OCIE2);
    else TIMSK &= ~_BV(OCIE2);
#endif
}

void FrequencyTimer2::setPeriod(unsigned long period)
{
    uint8_t pre, top;
  
    if ( period == 0) period = 1;
    period *= clockCyclesPerMicrosecond();
 
    period /= 2;            // we work with half-cycles before the toggle 
    if ( period <= 256) {
	pre = 1;
	top = period-1;
    } else if ( period <= 256L*8) {
	pre = 2;
	top = period/8-1;
    } else if ( period <= 256L*32) {
	pre = 3;
	top = period/32-1;
    } else if ( period <= 256L*64) {
	pre = 4;
	top = period/64-1;
    } else if ( period <= 256L*128) {
	pre = 5;
	top = period/128-1;
    } else if ( period <= 256L*256) {
	pre = 6;
	top = period/256-1;
    } else if ( period <= 256L*1024) {
	pre = 7;
	top = period/1024-1;
    } else {
	pre = 7;
	top = 255;
    }

#if defined(TCCR2A)
    TCCR2B = 0;
    TCCR2A = 0;
    TCNT2 = 0;
#if defined(ASSR) && defined(AS2)
    ASSR &= ~_BV(AS2);    // use clock, not T2 pin
#endif
    OCR2A = top;
    TCCR2A = (_BV(WGM21) | ( FrequencyTimer2::enabled ? _BV(COM2A0) : 0));
    TCCR2B = pre;
#elif defined(TCCR2)
    TCCR2 = 0;
    TCNT2 = 0;
    ASSR &= ~_BV(AS2);    // use clock, not T2 pin
    OCR2 = top;
    TCCR2 = (_BV(WGM21) | ( FrequencyTimer2::enabled ? _BV(COM20) : 0)  | pre);
#endif
}

unsigned long  FrequencyTimer2::getPeriod()
{
#if defined(TCCR2B)
    uint8_t p = (TCCR2B & 7);
    unsigned long v = OCR2A;
#elif defined(TCCR2)
    uint8_t p = (TCCR2 & 7);
    unsigned long v = OCR2;
#endif
    uint8_t shift;
  
    switch(p) {
      case 0 ... 1:
	shift = 0;
	break;
      case 2:
	shift = 3;
	break;
      case 3:
	shift = 5;
	break;
      case 4:
	shift = 6;
	break;
      case 5:
	shift = 7;
	break;
      case 6:
	shift = 8;
	break;
      case 7:
	shift = 10;
	break;
    }
    return (((v+1) << (shift+1)) + 1) / clockCyclesPerMicrosecond();   // shift+1 converts from half-period to period
}

void FrequencyTimer2::enable()
{
    FrequencyTimer2::enabled = 1;
#if defined(TCCR2A)
    TCCR2A |= _BV(COM2A0);
#elif defined(TCCR2)
    TCCR2 |= _BV(COM20);
#endif
}

void FrequencyTimer2::disable()
{
    FrequencyTimer2::enabled = 0;
#if defined(TCCR2A)
    TCCR2A &= ~_BV(COM2A0);
#elif defined(TCCR2)
    TCCR2 &= ~_BV(COM20);
#endif
}
