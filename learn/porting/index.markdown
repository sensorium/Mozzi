---
layout: single
---

# Porting old sketches to a new Mozzi

We try to keep Mozzi backwards compatible with your earlier sketches as much as possible, but sometimes that will not be 100% possible. If that is the case,
adjusting your sketch to work with the latest and greatest Mozzi is generally very simple, however. This page shows, how. (Not all of these steps are
even strictly necessary.)

## Compatibility level option

If you want Mozzi to be extra strict in complaining about anything that is not the latest and greatest, add
```
#include <MozziConfigValues.h>
#define MOZZI_COMPATIBILITY_LEVEL MOZZI_COMPATIBILITY_LATEST
```
at the top of your sketch (above any other Mozzi headers).

## Porting from Mozzi 1.x to Mozzi 2.0

### All sketches must include Mozzi.h (instead of MozziGuts.h)

In your sketches, replace ```#include <MozziGuts.h>``` with ```#include <Mozzi.h>```. While this adjustment is not currently strictly required, MozziGuts.h may be removed some
time in the future. Also, when you include MozziGuts.h rather than Mozzi.h, the code will assume that your sketch has not been ported, yet, and will omit a few
(currently only minor) optimizations in favour of backwards compatibilty.

Note that previously, some headers, such as Phasor.h included MozziGuts.h, implictly, and so you did not have to do so, yourself. This is no longer the case: Each sketch
must include Mozzi.h.

#### Working with more than one .cpp file

*Only* if your sketch contains one or more .cpp-files *in addition* to your .ino-file, make sure that *exactly* one of the includes Mozzi.h. All further files will
have to ```#include <MozziHeadersOnly.h>```, instead (if Mozzi headers are need at all in that file). Failure to do so will lead to "multiple definition" errors while
linking.

### twi_nonblock

(AVR only): The TWI code is now only compiled if / when twi_nonblock.h is explicitly included in a sketch. Should more than one source .cpp file need to access
twi_nonblock functions, the same procedure as above is needed: Exactly one of these files shall ```#include <twi_nonblock.h>```, while any others shall use
```#include <twi_nonblockHeadersOnly.h>```. If you have no use for these functions, simply don't include the header, and save ~400 bytes of flash.

### Moved / internal headers

Some header files that were never meant to be used directly in user code have been moved to the subdirectory "internal", inside Mozzi. Including these headers,
while still possible, is definitely not recommended. Anything inside might be subject to change without notice.

### MozziPrivate namespace

Some functions have been moved into a new namespace called MozziPrivate. It is strongly recommended not to use any of those, direclty, in your owm code, as
these could change without notice, including subtle changes in semantics that will silently break your code. Should you think you need any of these, it is
recommended to copy their implementation (while again omitting any calls to other MozziPrivate code), *and* letting us know, so we can consider making some
things "public".

(Don't worry, you will not be using any of these functions "by accident", as the namespace would have to be given, explicitly.)

### Removed functions / variables / headers

Some things have been removed for good. If you need any of these, let us know, so we can reconsider:


    pauseMozzi() - used to be declared but not defined. Use stopMozzi(), instead
    unpauseMozzi() - used to be declared but not defined. Use startMozzi(), instead
    Teensy3/4: channel2sc1a -> thought to be unused, removed
    Teensy2: adc_mapping -> hidden away; use adcPinToChannelNum(), as on all other platforms, instead
    removed several inclusions of "WProgram.h". If using Arduino versions < 1.0, you need to update, seriously ;-)
    Since Mozzi (AVR-port) no longer uses Timer 0 since a long time, the corresponding library (utility/TimerZero.h) has now been removed, too. The Arduino functions delay(), millis(), micros() and delayMicroseconds() should now be usable in theory. That said, you should avoid these functions, as they are slow (or even blocking). For measuring time, refer to mozziMicros(). For delaying events, you can use Mozzi's EventDelay() unit instead (not to be confused with AudioDelay()).

### Fixed point arithmethic (Q16n16, etc.)

Q16n16, and further typedefs for fixed point arithmetic are still around, with no intention to remove them, soon. However, consider porting such code to the new
FixMath library (a spin-off by the Mozzi team, available at https://github.com/tomcombriat/FixMath), with its SFix and UFix types, which are typesafe and much more
comfortable to use. Functions accepting Q16n16, etc. have gained overloads for these types. The old Q16n16, etc. types are no longer recommended to be used in new code. 

### Configuration

#### Configuration is now "inline", rather than in mozzi_config.h

Read the [configuration tutorial](../configuration/) for a description of how configuration is done in Mozzi 2.0. *Only* if you have customized your
mozzi_config.h, you will now have to adjust. Importantly, configuration is now done per sketch, rather than centrally.

#### Changed config names

The names of most configuration defines have undergone some changes, and in some cases, what used to be a single option has been split, or vice versa.
Importantly, also, configuration defines have been unified across the different platforms as much as possible. For a reference of all available options, see:

  - [Generic Configuration options](Mozzi/doc/html/group__config.html)
  - [Hardware specific info](Mozzi/doc/html/hardware.html)

Here's a list of adjustments needed for some "popular" configuration values:

  - ```#define STEREO_HACK true``` ->  ```#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO```
  - ```#define AUDIO_RATE XY``` -> ```#define MOZZI_AUDIO_RATE XY```
  - ```#define AUDIO_MODE STANDARD_PLUS``` -> just omit this, it's the default (on AVR), anyway
  - ```#define AUDIO_MODE STANDARD``` -> ```#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_PWM``` + ```#define MOZZI_PWM_RATE 16384```
  - ```#define AUDIO_MODE HIFI``` -> ```#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_2PIN_PWM```
  - ```#define EXTERNAL_OUTPUT true``` -> ```#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_EXTERNAL_TIMED```
  - ```#define USE_AUDIO_INPUT true``` -> ```#define MOZZI_AUDIO_INPUT MOZZI_AUDIO_INPUT_STANDARD```

To leave any option at its default value, just omit it!

#### Configuration checks / "This feature is not supported..."-errors

We have added several checks to detect some invalid combinations of configuration options, early. Some of these checks might be too strict. If the compiler
complains on something that you believe should work, please let us know.

Note that *not* getting an error message does *not* guarantee that an option is supported. Please refer to the API documenation linked above for definite
information.

### IS_STM32()

Renamed to ```IS_STM32MAPLE()```, in order to avoid confusion with ```IS_STM32DUINO()```.

### mozziAnalogRead() and getAudioInput()

```mozziAnalogRead()``` and ```getAudioInput()``` (if configured) be used exactly as before, but will produce a compile time warning, that this use is not portable.
The background is that different boards return analog readings at a different resolutions. To silence the warnings, and obtain readings at a defined resolution - let's
assume 12 bits, as an example - you can either add ```#define MOZZI_ANALOG_READ_RESOLUTION 12``` above ```#include <Mozzi.h>``` to configure a specific resolution, or
you can specify the desired resolution in each call as ```uint16_t value = mozziAnalogRead<12>(inputpin);```. Platforms that read analog measures at a different (higher or lower)
resolution, internally, will automatically shift the reading to the desired range. ```getAudioInput()``` respects the same config define, but may also be used with
a template parameter as ```uint16_t value = getAudioInput<12>();```.

The return value of both function has also been changed from ```int``` to ```uint16_t``` (the functions have always been returning only positive values).

### Licence

Starting with Mozzi 2.0, the Mozzi licence has been changed to LGPL version 2.1 or (at your option) any later version of the LGPL. This removes barriers in particular concerning
conflicts of the previous "non-commercial" clause when mixing with GPL'ed code. If you feel a bit lost, what this all means for your project: Probably not very much at all.
The bottom of our main Readme has a short summary of the main points.

## Older changes (that should have happened before Mozzi 1.1, but might still be missing in really old code)

### updateAudio() shall return AudioOutput

Formerly, ```updateAudio()``` was expected to return ```int```, and as a transitory step ```AudioOutput_t```. Instead it shall now return ```AudioOutput```
(which, depending on mono or stereo being configured, is an alias to either MonoOutput or StereoOutput).

### Scale audio output

Formerly, as a final step in ```updateAudio()```, you would scale the output to the hardware range (usually using a ```<<``` or ```>>``` shift). To make it easier
to reuse sketches between different platforms, you should instead use the wrappers ```MonoOutput::from8bit()```, ```MonoOutput::from16bit()``` or ```MonoOutput::fromNbit()```, instead (or their ```StereoOutput``` equivalents). These will perform appropriate shifting, automatically.

E.g.
```
int8_t a = aSin.next();
uint8_t b = master_volume;

return MonoOutput::from16bit((int16_t) a * b);
// previously:
// return ((int16_t) a * b) >> 8;    // convert to 8-bit range, assuming that is the hardware resolution
```
As a special case, sketches that were originally designed for classic Arduino, and return values in the -243..244-range (i.e. "8 bit and a half") should use:

```
return MonoOutput::fromAlmostNbit(9, value);
```

*Only* on classic Arduino, default output mode, this will assume the value still fits into the output range, while everywhere else, it will be shifted as if
it was using 9 bits.

### "STEREO_HACK"

Use ```#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO``` in your configuration, instead. ```updateAudio()``` must then return pairs of L/R-samples
encapsuled in the ```StereoOutput```-class. See e.g. Examples->Mozzi->12.Misc->Stereo.
