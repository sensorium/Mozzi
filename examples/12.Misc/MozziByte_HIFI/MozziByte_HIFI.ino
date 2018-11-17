/*  
    A cartoon mosquito sound, to demonstrate the Mozzibyte board.
    The Arduino-compatible "Pro Micro" board sent with Mozzibytes 
    needs "Arduino Leonardo" to be set under Arduino>Tools>Board.
    
   Important:
    #define AUDIO_MODE HIFI in mozzi_config.h

    Circuit: Audio output on digital pin 9 and 10 (on a Uno or similar),
    Check the Mozzi core module documentation for others and more detail

                     3.9k
     pin 9  ---WWWW-----|-----output
                    499k|
     pin 10 ---WWWW---- |
                        |
                  4.7n  ==
                        |
                      ground

    Resistors are ±0.5%
    Circuit: Audio output on digital pins 9 and 10 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
    check the README or http://sensorium.github.com/Mozzi/

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

    Tim Barrass 2018, CC by-nc-sa.
*/

#include <WavePacketSample.h>
#include <EventDelay.h>
#include <mozzi_rand.h>
#include <Smooth.h>
#include <Oscil.h> // oscillator template
#include <tables/sin1024_int8.h> // sine table for oscillator

#define CONTROL_RATE 128

#define F_MAX ((int) 750)
#define F_MIN ((int) 150)
#define F_CHANGE ((byte) 60)

#define B_MAX ((int) 500)
#define B_MIN ((int) 10)
#define B_CHANGE ((byte) 200)

#define CF_MAX ((int) 500)
#define CF_MIN ((int) 5)
#define CF_CHANGE ((byte) 40)

#define LEVEL_MAX ((int) 255)
#define LEVEL_MIN ((int) 20)
#define LEVEL_CHANGE ((byte) 50)

#define OFFSET_TIME_MAX_MS ((int) 500)
#define SYNTH_PARAM_MAX_MS ((byte) 200)

Oscil <SIN1024_NUM_CELLS, CONTROL_RATE> kFOffsetTrem(SIN1024_DATA);
Oscil <SIN1024_NUM_CELLS, CONTROL_RATE> kBOffsetTrem(SIN1024_DATA);
Oscil <SIN1024_NUM_CELLS, CONTROL_RATE> kCFOffsetTrem(SIN1024_DATA);
Oscil <SIN1024_NUM_CELLS, CONTROL_RATE> kLevelOffsetTrem(SIN1024_DATA);

WavePacketSample <SINGLE> wavey;

EventDelay kFTime;
EventDelay kBTime;
EventDelay kCFTime;
EventDelay kLevelTime;

EventDelay kFOffsetTime;
EventDelay kBOffsetTime;
EventDelay kCFOffsetTime;
EventDelay kLevelOffsetTime;

int k_ftarget, k_btarget, k_cftarget, k_leveltarget;
int k_foffset_amp, k_boffset_amp, k_cfoffset_amp, k_leveloffset_amp;;

Smooth <int> kSmoothF(0.85f);
Smooth <int> kSmoothBw(0.85);
Smooth <int> kSmoothCf(0.85);
Smooth <long> aSmoothLevel(0.997f);

Smooth <int> kSmoothFOffset(0.99f);
Smooth <int> kSmoothBOffset(0.99f);
Smooth <int> kSmoothCFOffset(0.99f);
Smooth <int> kSmoothLevelOffset(0.99f);

void setup() {
  randSeed();
  //Serial.begin(115200);
  wavey.setTable(SIN1024_DATA);
  kFTime.start(rand(1, 500));
  kBTime.start(rand(1, 500));
  kCFTime.start(rand(1, 500));
  kLevelTime.start(rand(1, 500));
  k_ftarget = rand(F_MIN, F_MAX);
  k_btarget = rand(B_MIN, B_MAX);
  k_cftarget = rand(CF_MIN, CF_MAX);
  k_leveltarget = rand(LEVEL_MIN, LEVEL_MAX);
  kFOffsetTrem.setFreq(7);
  kBOffsetTrem.setFreq(7);
  kCFOffsetTrem.setFreq(7);
  kLevelOffsetTrem.setFreq(7);
  k_foffset_amp = 0; // 0-255 is full range of Oscil, +-char
  k_boffset_amp = 0;
  k_cfoffset_amp = 0;
  k_leveloffset_amp = 0;
  kFOffsetTime.start(rand(200));
  kBOffsetTime.start(rand(200));
  kCFOffsetTime.start(rand(200));
  kLevelOffsetTime.start(rand(200));
  startMozzi(CONTROL_RATE);
}


void updateControl() {
  
  if (kFOffsetTime.ready())
  {
    if (k_foffset_amp == 0)
    {
      k_foffset_amp = 1 + rand(50);
      kFOffsetTrem.setFreq(3 + rand((byte)6));
    } else {
      k_foffset_amp = 0;
    }
    kFOffsetTime.start(rand(OFFSET_TIME_MAX_MS));
  }

  if (kBOffsetTime.ready())
  {
    if (k_boffset_amp == 0)
    {
      k_boffset_amp = 1 + rand(50);
      kBOffsetTrem.setFreq(1 + rand((byte)8));
    } else {
      k_boffset_amp = 0;
    }
    kBOffsetTime.start(rand(OFFSET_TIME_MAX_MS));
  }

  if (kCFOffsetTime.ready())
  {
    if (k_cfoffset_amp == 0)
    {
      k_cfoffset_amp = 1 + rand(50);
      kCFOffsetTrem.setFreq(1 + rand((byte)8));
    } else {
      k_cfoffset_amp = 0;
    }
    kCFOffsetTime.start(rand(OFFSET_TIME_MAX_MS));
  }

  if (kLevelOffsetTime.ready())
  {
    if (k_leveloffset_amp == 0)
    {
      k_leveloffset_amp = rand(200);
      kLevelOffsetTrem.setFreq(3 + rand((byte)6));
    } else {
      k_leveloffset_amp = 0;
    }
    kLevelOffsetTime.start(rand(OFFSET_TIME_MAX_MS));
  }

  if (kFTime.ready()) {
    k_ftarget += rand(-F_CHANGE, F_CHANGE);
    if (k_ftarget < F_MIN ) k_ftarget += F_CHANGE;
    if (k_ftarget > F_MAX ) k_ftarget -= F_CHANGE;
    kFTime.start(rand(SYNTH_PARAM_MAX_MS));
  }

  if (kBTime.ready()) {
    k_btarget += rand(- B_CHANGE, B_CHANGE);
    if (k_btarget < B_MIN) k_btarget += B_CHANGE;
    if (k_btarget > B_MAX) k_btarget -= B_CHANGE;
    kBTime.start(rand(SYNTH_PARAM_MAX_MS));
  }

  if (kCFTime.ready()) {
    k_cftarget += rand(- CF_CHANGE, CF_CHANGE);
    if (k_cftarget < CF_MIN ) k_cftarget += CF_CHANGE;
    if (k_cftarget > CF_MAX ) k_cftarget -= CF_CHANGE;
    kCFTime.start(rand(SYNTH_PARAM_MAX_MS));
  }

  if (kLevelTime.ready()) {
    k_leveltarget += rand(-LEVEL_CHANGE, LEVEL_CHANGE);
    if (k_leveltarget < LEVEL_MIN ) k_leveltarget += LEVEL_CHANGE;
    if (k_leveltarget > LEVEL_MAX ) k_leveltarget -= LEVEL_CHANGE;
    kLevelTime.start(rand(SYNTH_PARAM_MAX_MS));
  }

  char k_ftrem = kFOffsetTrem.next();
  char k_btrem = kBOffsetTrem.next();
  char k_cftrem = kCFOffsetTrem.next();
  char k_leveltrem = kCFOffsetTrem.next();

  int k_foffset = ((long)k_ftrem * kSmoothFOffset.next(k_foffset_amp)) >> 8;
  int k_boffset = ((long)k_btrem * kSmoothBOffset.next(k_boffset_amp)) >> 8;
  int k_cfoffset = ((long)k_cftrem * kSmoothCFOffset.next(k_cfoffset_amp)) >> 8;
  int k_leveloffset = ((long)k_leveltrem * kSmoothLevelOffset.next(k_leveloffset_amp)) >> 8;

  int f = min(F_MAX, kSmoothF.next(k_ftarget) + k_foffset);
  int b = min (B_MAX, kSmoothBw.next(k_btarget) + k_boffset);
  int cf = min (CF_MAX, kSmoothCf.next(k_cftarget) + k_cfoffset);

  k_leveltarget = min(LEVEL_MAX, k_leveltarget + k_leveloffset);
  k_leveltarget = max(k_leveltarget, 0);

  //Serial.print(f);Serial.print("\t ");Serial.print(b);Serial.print("\t ");Serial.print(cf);Serial.println();
  wavey.set(f, b, cf);
}


int updateAudio() {
  int asig = (aSmoothLevel.next(k_leveltarget) * wavey.next()) >> 8; // for AUDIO_MODE HIFI
  return asig >> 1; // avoid distortion
}


void loop() {
  audioHook(); // required here
}

