---
layout: single
toc: true
---

When Mozzi cam to life around 2012 the world was simple: Only one CPU family was supported, and only a single output mode. That quickly grew to several different output options,
and today Mozzi runs on a wide variety of different MCUs with very different hardware capabilities.  

Your first difficulty will thus be to understand what output mode your MCU supports, and what pin to expect the output on (TODO: create an up-to-date matrix for boards output modes and pins, and link to that).
However, to keep things somewhat simple, on each platform the default is to output audio on a single pin, and so we'll look at that, first.

## Single pin output circuits

Here, "single pin" means one output pin per audio channel, so it could also be two for stereo output. In that case, obviously, you need the same circuit twice.
{: .notice--info}

### Output using a true hardware DAC

Things are easy, if your MCU comes with a true hardware DAC, which output a true analog signal. In that case, all you need to worry about is whether the output is capable
of providing enough output current. I.e. you may need to follow up with a generic amplifier circuit to drive low impedance speakers, but that's it.

Boards that fall into this category include Teensy 3.1, Arduino Giga, and ESP32. Note however, that for the latter two, the output resolution is only 8 bits.

### Modulated output (PWM or PDM)

Most boards to not feature a hardware DAC, however. In this case, an analog signal needs to be emulated by quickly toggling a digital pin high or low. This works quite well,
but importantly, there may be a high-pitch noise, at the carrier frequency, which may be in the audible range. Using a higher carrier frequency - if possible - may mitigate
this to some degree (TODO: link to config and hardware API documentation). However, for best quality it will often make sense to add a low-pass filter:

### Low pass audio filter
Here's some info about low pass [RC filters](https://electronics.stackexchange.com/questions/34843/how-determine-the-rc-time-constant-in-pwm-digital-to-analog-low-pass-filter) and a [calculator](https://sim.okawa-denshi.jp/en/PWMtool.php) to help choose components.
For Mozzi, an RC filter with a roll-off frequency of just under 6kHz works well, using a 270 Ω resistor and a 100n capacitor, as shown below.

![RC circuit](https://farm9.staticflickr.com/8317/7934584004_096300ef0d.jpg)

### Notch filter for 16384 Hz carrier frequency

First versions of Mozzi used a 16384 Hz PWM carrier frequency on AVR CPUs (originally known as the "STANDARD" mode). This was soon doubled,
but if you want to squeeze out a few % more CPU power, or you want to trade a lower frequency for a higher output signal resolution, you may
once again end up with audible artifacts. In particular children will typically find a 16786 Hz carrier frequency irritating, while adults
are often unable to perceive it at any level!

Simply put, using pulse width modulation (PWM), if you want 12 bits of output resolution, i.e. 4096 distrinc signal levels, a digital output
pin will have to be held high (or low) for up to that many CPU cycles, so this will generally require a clock on the order of at least 100 Mhz
in oder to push the carrier frequency outside the audible range. See configuration options `MOZZI_PWM_RATE`, and `M̀OZZI_AUDIO_BITS`,
or `MOZZI_PDM_RESOLUTION` if you want to tweak this tradeoff.
{: .notice--info}

Many thanks to Andrew McPherson, who sent a schematic and equation for a twin-T notch filter which he used successfully to remove the offending 16384 Hz frequency.

![Twin-T Notch Filter](https://farm9.staticflickr.com/8470/8124196839_6f3e506525.jpg)
![equation](https://farm9.staticflickr.com/8472/8124196815_6c02f4fb86.jpg)

Here's a design tool for calculating [Twin-T notch filters.](https://sim.okawa-denshi.jp/en/TwinTCRtool.php)

Andrew also reported that the low-pass and notch filter described here can be used together in series without any problems.

Here's a schematic for the 6kHz low pass and the notch filter using commonly available parts chosen to exactly match a 16384Hz carrier frequency, using the notch filter design tool.

![Schematic](https://farm9.staticflickr.com/8209/8210496070_4a1b21f8c1_c.jpg)


## Output circuit for 2-pin-PWM ("HIFI") mode

Mozzi's MOZZI_OUTPUT_2PIN_PWM ("HIFI") audio mode combines the output of 2 pins in a technique called dual PWM, which is explained in detail at [Open Music Labs](https://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/).

This output mode needs a differnt and more complex output circuitry than the single pin modes, and using the regular single pin circuit for "HIFI" will result in
terribly distorted sound. In case of difficiculties (but also in general), it is very much recommended to try the default output mode, first, and switch to any
other mode only once you've verified that works. Don't go straight for "HIFI" just because is sounds promising.
{: notice--warning}

Here's the circuit for Mozzi, based on the Open Music Labs article.

![Mozzi output circuit for 2-pin PWM audio mode](https://farm8.staticflickr.com/7458/10657009473_26c1f478de.jpg)

Not all boards support the 2-pin PWM mode, but some are capable of quite good output resolution in their default mode.

### Loe-pass filtering
The dual PWM mode, too, can profit from filtering out the carrier frequency, as detailed above for the single-pin PWM mode. In 2-pin PWM mode, the filter
would be applied on the combined audio output signal rather than straight from pin 9.

### Advantages of HIFI mode
- Higher resolution sound than single pin PWM modes.  
- Generally doesn't need a notch filter on the audio signal because the carrier frequency is out of hearing range.

### Disadvantages
- Requires 2 pins, 2 resistors and a capacitor, so it's not so quick to set up compared to a rough, direct single-pin output in default mode.

### Download
Download a Fritzing file with breadboard, schematic and circuit board layouts [here](https://docs.google.com/file/d/0B_eOzePFYDZaSEo4bVJ6NlJnSXM/edit?usp=sharing).


## Output to external DACs (or custom circuitry)

On a few platforms (including ESP8266 and ESP32), Mozzi has inbuilt support for interfacing with certain DAC ICs using the I2S interface. However,
on all platforms, you can provide custom output routines, which allow you to output to anything from hand-made resistor ladders to purchased dedicated ICs. To enable this,
take a look at the configuration options `MOZZI_OUTPUT_EXTERNAL_TIMED` and `MOZZI_OUTPUT_EXTERNAL_CUSTOM` (TODO: add link). Mozzi also ships with a whole section
of examples for connecting to external components, including MCP4922 and PT8211 DACs (File->Examples->Mozzi->13.External_Audio_Output).

TODO: Describe some circuits here, so sketches can link here.
