---
layout: single
title: "v1.0.3rc2 Teensy bugfix"
date: 2016-12-11 17:11

categories: releases, bugfix
---
release v1.0.3rc2

- Teensy now compiles, bit of a hack added 
	|| defined(TEENSYDUINO)
	to all Teensy conditional compile lines like this:
	#if defined(__MK20DX128__) || defined(__MK20DX256__) || defined(TEENSYDUINO)  // teensy 3, 3.1
- examples, removed commented ADC.h line from all examples, Teensy doesn't seem to need it anymore
