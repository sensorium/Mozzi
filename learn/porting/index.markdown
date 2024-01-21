---
layout: single
---

# Porting old sketches to a new Mozzi

We try to keep Mozzi backwards compatible with your earlier sketches as much as possible, but sometimes that will not be 100% possible. If that is the case,
adjusting your sketch to work with the latest and greatest Mozzi is generally very simple, however. This page shows, how. (Not all of these steps are
even strictly necessary.)

## Porting from Mozzi 1.x to Mozzi 2.0

### All sketches must include Mozzi.h (instead of MozziGuts.h)

In you sketches, replace ```#include <MozziGuts.h>``` with ```#include <Mozzi.h>```. While this adjustment is not currently strictly required, MozziGuts.h may be removed some
time in the future. Also, when you include MozziGuts.h rather than Mozzi.h, the code will assume that your sketch has not been ported, yet, and will omit a few
(currently only minor) optimizations in favour of backwards compatibilty.

Note that previously, some headers, such as Phasor.h included MozziGuts.h, implictly, and so you did not have to do so, yourself. This is no longer the case: Each sketch
must include Mozzi.h.

#### Working with more than one .cpp file

*Only* if your sketch contains one or more .cpp-files *in addition* to your .ino-file, make sure that *exactly* one of the includes Mozzi.h. All further files will
have to ```#include <MozziHeadersOnly.h>```, instead (if Mozzi headers are need at all in that file). Failure to do so will lead to "multiple definition" errors while
linking.

### Configuration





## Older changes (that should have happened before Mozzi 1.1, but might still be missing in really old code)




TODO: complete this file

