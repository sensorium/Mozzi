---
layout: single
---

Here's a detailed explanation of the parts in a Mozzi sketch. We'll start by
generating audio to produce a plain sine wave, then add control to the sketch to
change the sine wave's frequency and produce vibrato.

## Bare bones example: playing a sine wave

Let's make a minimal Mozzi sketch step by step. The sketch plays a sine wave at
a specified frequency. No great feat, and you can find lots of other Arduino
sketches doing it without Mozzi, but it gives the gist of how a
Mozzi sketch works. You don't need much experience with
Arduino or programming.  

First `#include <Mozzi.h>`. You always need this, as well as headers for any
Mozzi classes, modules or tables used in the sketch.  This time we'll have an oscillator
and a wavetable for the oscillator to play:

{% highlight c++ %}
#include <Mozzi.h> // this makes everything work
#include <Oscil.h>  // a template for an oscillator
#include <tables/sin2048_int8.h>  // a wavetable holding a sine wave
{% endhighlight %}

Next, instantiate the oscillator, using the `Oscil` class just included.  Here is the structure and
the parameters used to declare an `Oscil`.

{% highlight c++ %}
Oscil <table_size, update_rate> name(table_data);
{% endhighlight %}

The oscillator needs to be instantiated using literal numeric values as template
parameters (inside the `< >` brackets). This allows the compiler to do some
calculations at compile time instead of slowing down execution by repeating the
same operations over and over while the program runs.

The table used by an `Oscil` needs to be a power of two, typically at least 256
cells and preferably longer for lower aliasing noise. This Oscil will be
operating as an audio generator, so the update rate will be `MOZZI_AUDIO_RATE`. The
`table_data` is an array which you can find the name of in the table file included
at the top of the sketch.  If you look in Mozzi/tables/sin2048_int8.h, you'll find `SIN2048_DATA`.

So, an audio sine tone oscillator for our sketch is created like this:

{% highlight c++ %}
Oscil <2048, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);
{% endhighlight %}

Now to the program functions.  In Arduino's `setup()` routine goes:

{% highlight c++ %}
startMozzi();
{% endhighlight %}

This sets up one timer to call `updateControl()` at the default rate of 64 Hz, and another
timer which works behind the scenes to send audio samples to the output pin at
the fixed rate of 16384 Hz.

The oscillator frequency can be set in a range of ways, but we'll use an integer:

{% highlight c++ %}
aSin.setFreq(440);
{% endhighlight %}

Now Arduino's `setup()` function looks like this:

{% highlight c++ %}
void setup(){
	startMozzi();
	aSin.setFreq(440);
}
{% endhighlight %}

The next parts of the sketch are `updateControl()` and `updateAudio()`, which
are both required. In this example the frequency has already been set and the
oscillator just needs to be run in `updateAudio()`, using the Oscil's `next()`
method which returns a signed 8 bit value from the oscillator's wavetable. In
this example, the`AudioOutput_t` return value of `updateAudio()` is really just
a zero-centered integer, but could also be configured to hold a stereo sample.

When working with an Arduino Uno/Nano/Pro Mini in the default mode, the output
range may be in the range of -244 to 243, but this differs for different boards
and configurations. To scale the value appropriately in all cases, use
'MonoOutput::from8Bit()' on the 8 bit value coming from the Oscil. (In case you
are wondering, this simply performs an appropriate left or right shift operation,
so it's very fast).

{% highlight c++ %}
void updateControl(){
	// no controls being changed
}

AudioOutput_t updateAudio(){
	return MonoOutput::from8bit(aSin.next());
}
{% endhighlight %}
Finally, `audioHook()` goes in Arduino's `loop()`.
{% highlight c++ %}
void loop(){
	audioHook();
}
{% endhighlight %}

This is where the sound actually gets synthesised, running as fast as possible
to fill the output buffer which gets steadily emptied at Mozzi's audio rate. For
this reason, it's usually best to avoid placing any other code in `loop()`.

It's important to design a sketch with efficiency in mind in terms of what can
be processed in `updateAudio()`, `updateControl()` and `setup()`. Keep `updateAudio()`
lean, put slow changing values in `updateControl()`, and pre-calculate as much as
possible in `setup()`. Control values which directly modify audio synthesis can be
efficiently interpolated with a `Line` object in `updateAudio()` if necessary.

Here's the whole sketch:

{% highlight c++ %}
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>

Oscil <2048, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);

void setup(){
	aSin.setFreq(440);
	startMozzi();
}

void updateControl(){
}

AudioOutput_t updateAudio(){
	return MonoOutput::from8Bit(aSin.next());
}

void loop(){
	audioHook();
}
{% endhighlight %}

***
## Adding a control signal: vibrato

Vibrato can be added to the sketch by periodically changing the frequency of the
audio wave with a low frequency oscillator. The new oscillator can use the same
wave table but this time it's set up to update at control rate. For a smoother sound,
we increase the rate at which `updateControl()` is called, by defining the configuration option
`MOZZI_CONTROL_RATE` as 128 (Hz) rather than the default of 64. Like the audio rate, this must be a
literal number and power of two to allow Mozzi to optimise internal calculations
for run-time speed. Also, like all configuration options, it needs to be set at the
top of the sketch, _before_ the Mozzi includes:

{% highlight c++ %}
#define MOZZI_CONTROL_RATE 128
#include <Mozzi.h>
{% endhighlight %}

The naming convention of using a prefix of `k` for control and `a` for audio rate units
is a personal mnemonic, influenced by Csound.

{% highlight c++ %}
Oscil <2048, MOZZI_CONTROL_RATE> kVib(SIN2048_DATA);
{% endhighlight %}

This time the frequency can be set with a floating point value:

{% highlight c++ %}
kVib.setFreq(6.5f);
{% endhighlight %}

Now, using variables for depth and centre frequency, the vibrato oscillator can
modulate the frequency of the audio oscillator in `updateControl()`.  `kVib.next()`
returns a signed byte between -128 to 127 from the wave table, so `depth` has to
be set proportionately.

{% highlight c++ %}
void updateControl(){
	float vibrato = depth * kVib.next();
	aSin.setFreq(centre_freq+vibrato);
}
{% endhighlight %}

Here's the modified sketch complete with vibrato:

{% highlight c++ %}
#define MOZZI_CONTROL_RATE 128
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>

Oscil <2048, MOZZI_AUDIO_RATE> aSin(SIN2048_DATA);
Oscil <2048, MOZZI_CONTROL_RATE> kVib(SIN2048_DATA);

float centre_freq = 440.0;
float depth = 0.25;

void setup(){
	kVib.setFreq(6.5f);
	startMozzi(MOZZI_CONTROL_RATE);
}

void updateControl(){
	float vibrato = depth * kVib.next();
	aSin.setFreq(centre_freq+vibrato);
}

AudioOutput_t updateAudio(){
	return MonoOutput::from8Bit(aSin.next());
}

void loop(){
	audioHook();
}
{% endhighlight %}

While this example uses floating point numbers, it is best to avoid their use
for intensive audio code which needs to run fast, especially in `updateAudio()`.
When the speed of integer maths is required along with fractional precision, it
is better to use fixed point fractional arithmetic. Mozzi's `fixmath` module has
number types and conversion functions which assist in keeping track of precision
through complex calculations.
