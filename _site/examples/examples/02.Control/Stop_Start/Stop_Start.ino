/*  Example stopping Mozzi, which restores timers to previous states,
    so Arduino delay() and milleseconds() work, then resuming Mozzi again.

    This may be useful when using sensors or other libraries which need to use
    the same timers as Mozzi.  (Atmel Timer 0, Timer 1, and in HIFI mode, Timer 2).

    Circuit:
    Pushbutton on digital pin D4
       button from the digital pin to +5V (3.3V on Teensy 3.1)
       10K resistor from the digital pin to ground
    Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.com/Mozzi/

		Mozzi documentation/API
		https://sensorium.github.io/Mozzi/doc/html/index.html

		Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2013, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h> // sine table for oscillator

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

#define STOP_PIN 4

boolean pause_triggered, paused = false;;
unsigned int count = 0;

void setup() {
  pinMode(STOP_PIN, INPUT);
  aSin.setFreq(220); // set the frequency
  startMozzi();
  Serial.begin(115200);
}


void updateControl() {
  if (digitalRead(STOP_PIN) == HIGH) {
    pause_triggered = true;
    count = 0;
  }
}


int updateAudio() {
  return aSin.next();
}


void loop() {

  if (pause_triggered) {
    paused = true;
    pause_triggered = false;
    stopMozzi();
  }

  if (paused) {
    Serial.println(count++);
    delay(100);
    if (count > 10) {
      paused = false;
      startMozzi();
    }
  }

  if(!paused) audioHook();
}
