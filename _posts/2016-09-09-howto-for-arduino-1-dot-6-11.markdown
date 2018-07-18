---
layout: single
title: "Howto for Arduino 1.6.11"
date: 2016-09-09 21:48

categories: [ fixes, updates ]
---

Now Mozzi can work at full speed on the latest Arduino IDE.  It works, but now Mozzi needs an update to install smoothly in recent IDE's.  Coming soon, with some other fixes.

The problem for Mozzi on Arduino IDE's after 1.0.5 used to be that sketches were compiled for small size instead of speed, but now the compiler optimations can be edited.

Here's how you do it.  Find Arduino's platform.txt (on OSX you can find it by searching in Users/your_name/Library/Arduino15).  Search and replace -Os with -O2.  Save.

It's explained more thoroughly (for Windows) here:
http://www.instructables.com/id/Arduino-IDE-16x-compiler-optimisations-faster-code/?ALLSTEPS
