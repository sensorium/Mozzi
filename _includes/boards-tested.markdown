
List of supported boards and output modes (not necessarily complete). The default output mode and pin is printed in bold in each
case. In many cases, pins are configurable, however, and also, some platform specific hints may be available. Check the hardware
section of the API documentation (TODO: Add link).

Legend:
 - PWM-1: 1-pin PWM mode (`MOZZI_OUTPUT_PWM`)
 - PWM-2: 2-pin PWM mode (`MOZZI_OUTPUT_2PIN_PWM`)
 - PDM: Pulse density modulation, may be either `MOZZI_OUTPUT_PDM_VIA_SERIAL` or `MOZZI_OUTPUT_PDM_VIA_I2S`
 - inbuilt DAC: `MOZZI_OUTPUT_INTERNAL_DAC`
 - I2S DAC (native): native support for externally connected I2S DACs (`MOZZI_OUTPUT_I2S_DAC`). Since this requires several, often
   configurable pins, and is never the default option, no pin numbers are shown in this table.
 - **All** platforms also all "external" output modes (`MOZZI_OUTPUT_EXTERNAL_TIMED` or `MOZZI_OUTPUT_EXTERNAL_CUSTOM`), which allow
   for connecting DACs or other external circtuity.

+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
|  __Board or family__  / __Output mode__                                      | PWM-1   | PWM-2   | PDM     | inbuilt | I2S DAC |
|                                                                              |         |         |         | DAC     | (native)|
+==============================================================================+=========+=========+=========+=========+=========+
| _ATmega328/168_: Uno (R3), Nano, Pro Mini, Duemilanove, Leonardo, etc.       | **9**   | 9, 10   | -       | -       | -       |
| Stereo:                                                                      | 10      | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _ATmega32U4_: Teensy, Teensy2, 2++                                           | **B5** (14) | B5, B6 | -    | -       | -       |
| Stereo:                                                                      | B6 (15) | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _ATmega2560_: Arduino Mega, Freetronics EtherMega, etc.                      | 11      | 11, 12  | -       | -       | -       |
| Stereo:                                                                      | 12      | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _ATmega1284_: Sanguino                                                       | 13      | 13, 12  | -       | -       | -       |
| Stereo:                                                                      | 12      | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| Teensy3.x - _note: DAC Pin number depends on model: A14, A12, or A21_        | -       | -       | –       | **DAC** | -       |
| Stereo:                                                                      | -       | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _LGT8F328P_: "Register clone" of the ATmega328, uses the same code in Mozzi  | **9**   | 9, 10   | -       | -       | -       |
| Stereo:                                                                      | 10      | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _SAMD_: Arduino Nano 33 Iot, Adafruit Playground Express, Gemma M0 etc.      | -       | -       | -       | A0      | -       |
|                                                                              |         |         |         |(speaker)|         |
| Stereo:                                                                      | -       | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _Renesas Arduino Core_: Arduino Uno R4                                       | -       | -       | -       | A0      | -       |
| Stereo:                                                                      | -       | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _Arduino MBED Core_: Arduino Giga (only model tested so far in this family)  | -       | -       | SERIAL2_TX | **A13** | -    |
| Stereo:                                                                      | -       | -       | -       | A12     | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _STM32 maple core_: Various STM32F1 and STM32F4 boards, "Blue/Black Pill"    | **PB8** | PB8, PB9| -       | -       | -       |
| Stereo:                                                                      | PB9     | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _STM32duino (STM official) core_: Huge range of STM32Fx boards               | **PA8** | PA8, PA9| -       | -       | -       |
| Stereo:                                                                      | PA9     | -       | -       | -       | -       |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _ESP8266_: ESP-01, Wemos D1 mini, etc. _note: Beware of erratic pin labels_  | -       | -       | **GPIO2** | -     | yes     |
| Stereo:                                                                      | -       | -       | -       | -       | yes     |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _ESP32_: _note: Beware of vastly different pin labels across board variants_ | -       | -       | yes     | **GPIO25** | yes  |
| Stereo:                                                                      | -       | -       | -       | GPIO26  | yes     |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+
| _RP2040_: Raspberry Pi Pico and friends                                      | **0**   | 0, 1    | -       | -       | yes     |
| Stereo:                                                                      | 1       | -       | -       | -       | yes     |
+------------------------------------------------------------------------------+---------+---------+---------+---------+---------+

