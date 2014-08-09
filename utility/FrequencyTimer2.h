#ifndef FREQUENCYTIMER2_IS_IN
#define FREQUENCYTIMER2_IS_IN

/*
  FrequencyTimer2.h - A frequency generator and interrupt generator library
  Author: Jim Studt, jim@federated.com
  Copyright (c) 2007 David A. Mellis.  All right reserved.

  http://www.arduino.cc/playground/Code/FrequencyTimer2

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

/**
MODIFIED by Tim Barrass 2013,2014: see .cpp file
*/

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

/*
// Arduino Mega
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define FREQUENCYTIMER2_PIN  10

// Teensy++
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
#define FREQUENCYTIMER2_PIN  24

// Sanguino
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
#define FREQUENCYTIMER2_PIN  15

// Arduino Uno, Duemilanove, Diecimila, etc
#else
#define FREQUENCYTIMER2_PIN  11
#endif
*/

class FrequencyTimer2
{
  private:
    static uint8_t enabled;
  public:
    static void (*onOverflow)(); // not really public, but I can't work out the 'friend' for the SIGNAL
    
  public:
  	static void setPeriodMicroSeconds(unsigned long);
  	static void setPeriodCPUCycles(unsigned long);
    static unsigned long getPeriod();
    static void setOnOverflow( void (*)() );
    static void enable();
    static void disable();
};

#endif
