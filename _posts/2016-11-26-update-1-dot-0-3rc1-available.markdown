---
layout: single
title: "Update 1.0.3rc1 available"
date: 2016-11-26 22:16

categories: 
---

release v1.0.3

Mostly bug fixes and maintainance, with update to suit Arduino 1.5+.  Though tests show the 1.0.5 IDE still produces faster code than any speed optimisation levels compiled with the newer versions.
- added library.properties for Arduino 1.5+
- MozziGuts.cpp and .h, and mozzi_config.h: added STEREO_HACK config option, set it to true to try stereo output, also added Stereo_Hack exammple
- README - updated install instructions
- examples/Mozzi_Midi_Input, added note about rx pin for midi in rather than tx as shown on Arduino page.
- ADSR.h - now IDLE phase will always return 0.
- included ADC.h for Teensy 3+ in mozzi_analog.h and removed it from sketches - not sure why this didn't work before (maybe <> instead of "").
- python scripts sin1024_uint8.py and sin8192_uint8.py, changed broken int16_t casts to int, Might have accidentally changed in a global search/replace.
- examples - changed Sinwave_PWM_leds_HIFI to Sinwave_PWM_leds, can't see any need for extra confusion of HFI
- examples - Detuned_Oscil_Wash - added note about compiling for speed, and that Arduino 1.0.5 still produces faster code.
