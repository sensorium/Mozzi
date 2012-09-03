#ifndef known_16bit_timers_header_
#define known_16bit_timers_header_

// Wiring-S
//
#if defined(__AVR_ATmega644P__) && defined(WIRING)
  #define TIMER1_A_PIN   5
  #define TIMER1_B_PIN   4
  #define TIMER1_ICP_PIN 6


// Teensy 2.0
//
#elif defined(__AVR_ATmega32U4__) && defined(CORE_TEENSY)
  #define TIMER1_A_PIN   14
  #define TIMER1_B_PIN   15
  #define TIMER1_C_PIN   4
  #define TIMER1_ICP_PIN 22
  #define TIMER1_CLK_PIN 11
  #define TIMER3_A_PIN   9
  #define TIMER3_ICP_PIN 10


// Teensy++ 2.0
#elif defined(__AVR_AT90USB1286__) && defined(CORE_TEENSY)
  #define TIMER1_A_PIN   25
  #define TIMER1_B_PIN   26
  #define TIMER1_C_PIN   27
  #define TIMER1_ICP_PIN 4
  #define TIMER1_CLK_PIN 6
  #define TIMER3_A_PIN   16
  #define TIMER3_B_PIN   15
  #define TIMER3_C_PIN   14
  #define TIMER3_ICP_PIN 17
  #define TIMER3_CLK_PIN 13


// Arduino Mega
//
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  #define TIMER1_A_PIN   11
  #define TIMER1_B_PIN   12
  #define TIMER1_C_PIN   13
  #define TIMER3_A_PIN   5
  #define TIMER3_B_PIN   2
  #define TIMER3_C_PIN   3
  #define TIMER4_A_PIN   6
  #define TIMER4_B_PIN   7
  #define TIMER4_C_PIN   8
  #define TIMER4_ICP_PIN 49
  #define TIMER5_A_PIN   46
  #define TIMER5_B_PIN   45
  #define TIMER5_C_PIN   44
  #define TIMER3_ICP_PIN 48
  #define TIMER3_CLK_PIN 47


// Arduino Uno, Duemilanove, LilyPad, etc
//
#elif defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
  #define TIMER1_A_PIN   9
  #define TIMER1_B_PIN   10
  #define TIMER1_ICP_PIN 8
  #define TIMER1_CLK_PIN 5


// Sanguino
//
#elif defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644__)
  #define TIMER1_A_PIN   13
  #define TIMER1_B_PIN   12
  #define TIMER1_ICP_PIN 14
  #define TIMER1_CLK_PIN 1

#endif

#endif
