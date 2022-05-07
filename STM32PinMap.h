/*
 * teensyPinMap.h
 *
 * Copyright 2022 T. Combriat.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef STM32PINMAP_H_
#define STM32PINMAP_H_


#include "mozzi_config.h"



inline uint8_t STM32PinMap(uint8_t pin)
{  
  if (pin > 7) return pin-8;
  else return pin;
}

#endif
