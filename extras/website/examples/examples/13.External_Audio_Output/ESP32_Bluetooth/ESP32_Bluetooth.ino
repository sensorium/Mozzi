
/*  Example of simple and stereo synthesis,
    using Mozzi sonification library with output to an A2DP sink
    (i.e. a Bluetooth speaker / headphone).


    IMPORTANT:
    - This example requires the ESP32-A2DP library by Phil Schatzmann:
    https://github.com/pschatzmann/ESP32-A2DP/
    - This example works on ESP32, only
    (Examples for other bluetooth-capable boards will follow, eventually.)
    - You will want to adjust at least the "BLUETOOTH_DEVICE"-define, below!

    Technical notes:
    The BT-lib expects to read samples at its own pace, so we cannot
    just "push" samples at the MOZZI_AUDIO_RATE, but rather they get
    "pulled" via get_data_frames().

    We therefore need a custom buffering scheme, i.e we select 
    MOZZI_OUTPUT_EXTERNAL_CUSTOM as output mode, and keep an own, custom buffer
    (which is visible to Mozzi via the two special functions
    canBufferAudioOutput(), and audioOutput()).

    Note that the BT default sample rate is 44100, which is not one of the powers
    of two that Mozzi likes so much (32768 in this example). Ignoring this would
    "work", but everything would essentially play too fast, resulting in an upward
    frequency shift. To fix this, we use a very crude sample rate adjustment,
    simply by repeating some of the frames to stay in sync.

    Support for Bluetooth A2DP is also provided by the Espressif IDF, so this sketch *could*
    be made to work without an external library. However this is fairly low level,
    and Phil's ESP32-A2DP library does a great job of managing all the scary details.


   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2024-2024 Thomas Friedrichsmeier and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

// before including Mozzi.h, configure external audio output mode:
#include "MozziConfigValues.h"  // for named option values
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_CUSTOM
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO
#define MOZZI_AUDIO_RATE 32768
#define MOZZI_CONTROL_RATE 256 // Hz, powers of 2 are most reliable

#include <Mozzi.h>
#include <Oscil.h>
#include <tables/cos2048_int8.h> // table for Oscils to play
#include <SPI.h>
#include <BluetoothA2DPSource.h>

// devicce to connect to
#define BLUETOOTH_DEVICE "BKH 274"
#define BLUETOOTH_VOLUME 100

// Synthesis part
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCos1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_AUDIO_RATE> aCos2(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kEnv1(COS2048_DATA);
Oscil<COS2048_NUM_CELLS, MOZZI_CONTROL_RATE> kEnv2(COS2048_DATA);

CircularBuffer<AudioOutput> buf;
void audioOutput(const AudioOutput f) {
  buf.write(f);
}

bool canBufferAudioOutput() {
  return !buf.isFull();
}


BluetoothA2DPSource a2dp_source;
const int BT_RATE = 44100;
const int lag_per_frame = BT_RATE-MOZZI_AUDIO_RATE;
int lag = 0;
AudioOutput last_sample;

int32_t get_data_frames(Frame *frame, int32_t frame_count) {
  for (int i = 0; i < frame_count; ++i) {
    lag += lag_per_frame;
    if (lag > BT_RATE) {
      lag -= BT_RATE;
    } else {
      if (!buf.isEmpty()) last_sample = buf.read();
    }
    frame[i].channel1 = last_sample.l();
    frame[i].channel2 = last_sample.r();
  }
  return frame_count;
}

void setup() {
  a2dp_source.set_auto_reconnect(false);
  a2dp_source.start(BLUETOOTH_DEVICE, get_data_frames);  
  a2dp_source.set_volume(BLUETOOTH_VOLUME);

  aCos1.setFreq(440.f);
  aCos2.setFreq(220.f);
  kEnv1.setFreq(0.25f);
  kEnv2.setFreq(0.30f);
  startMozzi();
}

// Carry enveloppes
int env1, env2;

void updateControl() {
  env1 = kEnv1.next();
  env2 = kEnv2.next();
}


AudioOutput_t updateAudio() {
  return StereoOutput::from16Bit(aCos1.next() * env1, aCos2.next() * env2);
}


void loop() {
  audioHook();
}
