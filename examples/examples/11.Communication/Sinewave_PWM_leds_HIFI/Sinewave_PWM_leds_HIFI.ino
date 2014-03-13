/*  Example playing a sinewave 
    and changing the brightness of 3 leds
    using Mozzi sonification library.
  
    Demonstrates the use of Oscil for audio and control,
    and a way to do PWM on digital pins without disrupting audio.
    Control oscillators are used here to modulate the brightness of the leds
    between 0-255.
    The technique is explained here:
     http://playground.arduino.cc/Main/PWMallPins
    With AUDIO_RATE at 16384 Hz, this gives a 64 Hz pwm duty cycle for the LEDs.
    If there is visible flicker, the resolution of the pwm could be made lower,
    or the AUDIO_RATE could be increased to 32768 Hz, if the
    cpu isn't too busy.
  
    IMPORTANT: this sketch requires Mozzi/mozzi_config.h to be
    be changed from STANDARD mode to HIFI.
    In Mozz/mozzi_config.h, change
    #define AUDIO_MODE STANDARD
    //#define AUDIO_MODE HIFI
    to
    //#define AUDIO_MODE STANDARD
    #define AUDIO_MODE HIFI
  
  
    Circuit: Audio output on digital pin 9 and 10 (on a Uno or similar),
    Check the Mozzi core module documentation for others and more detail
  
                     3.9k 
     pin 9  ---WWWW-----|-----output
                    499k           |
     pin 10 ---WWWW---- |
                                       |
                             4.7n  ==
                                       |
                                   ground
  
    Resistors are ±0.5%  Measure and choose the most precise 
    from a batch of whatever you can get.  Use two 1M resistors
    in parallel if you can't find 499k.
    Alternatively using 39 ohm, 4.99k and 470nF components will 
    work directly with headphones.

    LEDs:
    Red led from pin 3 through a 1.5k resistor to ground
    Green led from pin 4 through a 1.5k resistor to ground
    Blue led from pin 5 through a 1.5k resistor to ground
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012-13.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// LED pins, use 0-7 for PORTD, otherwise change to PORTB in updateRGB
const byte RED_PIN = 3;
const byte GREEN_PIN = 4;
const byte BLUE_PIN = 5;

byte red_brightness, green_brightness, blue_brightness;

// control oscillators using sinewaves to modulate LED brightness
Oscil <SIN2048_NUM_CELLS, CONTROL_RATE> kRed(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, CONTROL_RATE> kGreen(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, CONTROL_RATE> kBlue(SIN2048_DATA);

// audio oscillator
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);


void updateRGB(byte r, byte g, byte b){
  // stagger pwm counter starts to try to reduce combined flicker
  static byte red_count=0;
  static byte green_count=85;
  static byte blue_count=170;
// PORTD maps to Arduino digital pins 0 to 7
// http://playground.arduino.cc/Learning/PortManipulation
  (red_count++ >= r) ? PORTD &= ~(1 << RED_PIN) : PORTD |= (1 << RED_PIN);
  (green_count++ >= g) ? PORTD &= ~(1 << GREEN_PIN) : PORTD |= (1 << GREEN_PIN);
  (blue_count++ >= b) ? PORTD &= ~(1 << BLUE_PIN) : PORTD |= (1 << BLUE_PIN);
}


void setup(){
  pinMode(RED_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);
  // set led brightness modulation frequencies
  kRed.setFreq(0.21f);
  kGreen.setFreq(0.3f);
  kBlue.setFreq(0.27f);
  // set audio oscil frequency
  aSin.setFreq(440); 
  startMozzi(); // uses the default control rate of 64, defined in mozzi_config.h
}


void updateControl(){
  red_brightness = 128 + kRed.next();
  green_brightness = 128 + kGreen.next();
  blue_brightness = 128 + kBlue.next();
}


int updateAudio(){
  updateRGB(red_brightness, green_brightness, blue_brightness);
  // this would make more sense with a higher resolution signal
  // but still benefits from using HIFI to avoid the 16kHz pwm noise
  return aSin.next()<<6; // 8 bits scaled up to 14 bits
}


void loop(){
  audioHook(); // required here
}


