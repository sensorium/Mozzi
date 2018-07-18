---
layout: single
title: "Tweak for faster code"
date: 2016-11-26 22:19

categories: 
---

***

If you need your synth to run fast, Arduino versions above 1.5 can be tweaked to optimise compiled code for speed instead of small size.  

Find Arduino’s platform.txt (on OSX you can find it by searching in Users/your_name/Library/Arduino15). Search and replace -Os with -O2. Save.

It’s explained more thoroughly (for Windows) [here](http://www.instructables.com/id/Arduino-IDE-16x-compiler-optimisations-faster-code/?ALLSTEPS).

Not sure where it is on Linux at the moment - but then you'd already know, wouldn't you.

If you still need more, Arduino 1.0.5 produces slightly faster code.