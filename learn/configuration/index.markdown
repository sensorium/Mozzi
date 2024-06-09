---
layout: single
---

# Configuring Mozzi

You want to bend Mozzi to your will, but just how do you set configuration options, and how to find out, which options are available on your hardware?

Before changing anything, it is always recommended to test a new project with the default options. These get the most amount of testing, are simple,
and known to be supported. Starting to tweak options before you've tested your basic code makes it harder to debug in case of problems.
{.notice--info}

## Setting configuration options

When searching the internet / looking at older sketches, you may come across the advice to adjust ```mozzi_config.h```. This is outdated in Mozzi 2.0,
and also many options have been renamed for more consistency.
{.notice--info}

Since Mozzi 2.0, setting configuration options can be done on a sketch-by-sketch basis, by simply adding them at the top of your sketch. For an example,
let's configure our sketch to produce audio samples at 32768 Hz (this is actually the default on some platforms, but not on AVR), and in stereo:

```
// No other (Mozzi-)include above this line
#include <MozziConfigValues.h>              // for named option values

// The actual configuration goes here. You only need to mention values that you want to change
// from their default values.
#define MOZZI_AUDIO_RATE 32768
#define MOZZI_AUDIO_CHANNELS MOZZI_STEREO

#include <Mozzi.h>
```

That's all!

In the ```config``` subdirectory of your Mozzi installation, you will also find some configuration examples. You can either copy settings from these files, or
even use them directly, like this:

```
// No other (Mozzi-)include above this line
#include <config/config_example_rp2040_i2s_pt8211_mono.h>
#include <Mozzi.h>
```

### Working with more than one .cpp file

Should you be working with more than one source file (i.e. you've added one or more .cpp-files in addition to the main .ino-file in Arduino), make sure that:

  - Excatly one of your source files should have the line ```#include <Mozzi.h>```. All other files have to use ```#include <MozziHeadersOnly.h>``` (only if
    Mozzi headers are needed in that source file, of course).
  - Configuration options must be **identical** across all source files in a project. One way to assure this is to create a custom header file in your project
    that contains all Mozzi options.

### Make your sketch work cross platforms

Not all options are available on all boards. If you want to set an option only for a specific platform, you can make use of the ```ÌS_XYZ()``` macros:

```
// No other (Mozzi-)include above this line
#include <MozziConfigValues.h>

#if IS_ESP32()
#define MOZZI_AUDIO_MODE MOZZI_OUTPUT_I2S_DAC
#define MOZZI_I2S_PIN_BCK 22
//[...]
#endif

#include <Mozzi.h>
```

## Available configuration options

For an up-to-date list of configuration options available in Mozzi, you will have to look at two places in the API documentation, because not all options are
supported on all platforms:

  - [Generic Configuration options](Mozzi/doc/html/group__config.html)
  - [Hardware specific info](Mozzi/doc/html/hardware.html)
