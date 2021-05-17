#pragma once

// Preconditions for mozzi_config.h - Forward declaration of impmlementation calsses
class MozziMBED;
class MozziAVR;
class MozziESP32_I2S;
class MozziESP8266;
class MozziCommon;
class MozziSAMD21;
class MozziSTM32;
class MozziTeensy3;

//enum audio_modes {STANDARD,STANDARD_PLUS,HIFI};
#define STANDARD 0
#define STANDARD_PLUS 1
#define HIFI 2

//enum audio_channels {MONO,STEREO,...};
#define MONO 1
#define STEREO 2

