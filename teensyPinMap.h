/*
 * teensyPinMap.h
 *
 * Copyright 2021 T. Combriat.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef TEENSYPINMAP_H_
#define TEENSYPINMAP_H


#include "mozzi_config.h"



inline uint8_t teensyPinMap(uint8_t pin)
{  
  if (pin < 24) return pin-14;  // common to all teensys
  
#if defined (__MK20DX128__)  // Teensy 3.0
  return pin - 24;
  
#elif defined (__MK20DX256__) // Teensy 3.1/3.2

  switch (pin)
    {
    case 34:
      return 10;
    case 35:
      return 11;
    case 36:
      return 12;
    case 37:
      return 13;    
    case 40:
      return 14;
    case 26:
      return 15;
    case 27:
      return 16;
    case 28:
      return 17;
    case 29:
      return 18;
    case 30:
      return 19;
    case 31:
      return 20;
    }




#elif defined (__MKL26Z64__) //TeensyLC
  return pin-14;


#elif defined(__MK64FX512__) || defined(__MK66FX1M0__)  // Teensy 3.5//3.6
  switch (pin)
    {
    case 64:
      return 10;
    case 65:
      return 11;
    case 31:
      return 12;
    case 32:
      return 13;
    case 33:
      return 14;
    case 34:
      return 15;
    case 35:
      return 16;
    case 36:
      return 17;
    case 37:
      return 18;
    case 38:
      return 19;
    case 39:
      return 20;
    case 66:
      return 21;
    case 67:
      return 22;
    case 49:
      return 23;
    case 50:
      return 24;
    case 68:
      return 25;
    case 69:
      return 26;   
    }

#elif defined ARDUINO_TEENSY40
  return pin-14;

#elif defined ARDUINO_TEENSY41
  if (pin< 28) return pin-14;
  return pin-24;
#endif


  
}





#endif
