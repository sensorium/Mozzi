---
layout: single
title: "Teensy 3.0-3.1 port progress"
date: 2014-06-20 21:30

categories: ports
---

This is an update regarding work in progress on the port to Teensy 3.0 and 3.1,
to let donors to the project know where the work is up to.  It's taking a bit longer than anticipated,
but I want to assure people who are waiting that the work is well underway.  However, I won't be able to 
spend time on the port for the next couple of weeks, so the wait is going to be a bit longer yet.
I'm aiming to finish it in the second half of July.

Where it's at:  
Most of the demo sketches work, but HIFI output and some functions haven't been implemented yet.
Audio output on pin A14/DAC, in STANDARD or STANDARD_PLUS audio modes - these are identical on the Teensy 3 series as
the output is via DAC rather than PWM.  Non-blocking mozziAnalogRead() is implemented using the excellent ADC library 
created by Pedro Villanueva, which be downloaded from https://github.com/pedvide/ADC.
The ADC library is only being used very simply in the initial Mozzi port, but it opens the way to fine control
of ADC in future development.  Audio input hasn't been tested yet.

Some of the changes at this time which will affect users include:  
- #include <ADC.h> is required at the top of every Teensy 3.0/3.1 sketch.  
- uint8_t and int8_t are used instead of unsigned char and char, where char etc. don't work with Teensy 3.0/3.1.  
- Serial baud rate for monitoring in the IDE needs to be set to 9600.  

