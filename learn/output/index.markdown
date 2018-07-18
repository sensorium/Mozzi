---
layout: single
---

The default AUDIO_MODE since March 2014 is STANDARD_PLUS.  On Atmel-based boards this mode has a pwm carrier rate of 32768 Hz, double the rate for STANDARD mode.
The higher pwm rate is above hearing range, an advance on STANDARD mode's 16384 Hz which can be audible for children and some adults.  
Teensy 3.x boards provide true analog DAC rather than pwm, so Teensy 3's are a great option for cleaner sounding audio and you won't need a pwm carrier filter. These boards also run much faster than the Atmel based Arduinos, so you can generate more complex sounds.  
STANDARD_PLUS can still benefit from a low pass filter, as outlined below for STANDARD mode.


## Output circuit for HIFI mode
Mozzi's HIFI audio mode combines the output of 2 pins in a technique called dual PWM, which is explained in detail at [Open Music Labs](http://www.openmusiclabs.com/learning/digital/pwm-dac/dual-pwm-circuits/).

Here's the circuit for Mozzi, based on the Open Music Labs article.


![Mozzi output circuit for HIFI audio mode](https://farm8.staticflickr.com/7458/10657009473_26c1f478de.jpg)

Teensy 3.1 doesn't use 14 bit pwm HIFI mode, but has 12 bit DAC analog output enabled by default, even in STANDARD and STANDARD_PLUS modes.


#### Advantages of HIFI mode
- Higher resolution sound than STANDARD or STANDARD_PLUS modes.  
- Doesn't need a notch filter on the audio signal because the carrier frequency is out of hearing range (STANDARD_PLUS also doesn't need it).

#### Disadvantages
- Requires 2 pins, 2 resistors and a capacitor, so it's not so quick to set up compared to a rough, direct single-pin output in STANDARD and STANDARD_PLUS modes.

### Pins and where to put the resistors on various boards for HIFI mode  
Boards tested in HIFI mode have an x, though most of these have been tested in STANDARD_PLUS mode
and there's no reason for them not to work in HIFI (unless the pin number is wrong or something).
Any reports are welcome.

{% include boards-tested.markdown %}


### Low pass audio filter
Mozzi's sound output can be filtered to reduce aliasing at high frequencies.
Here's some info about low pass [RC filters](http://electronics.stackexchange.com/questions/34843/how-determine-the-rc-time-constant-in-pwm-digital-to-analog-low-pass-filter) and a [calculator](http://sim.okawa-denshi.jp/en/PWMtool.php) to help choose components.
For Mozzi, an RC filter with a roll-off frequency of just under 6kHz works well, using a 270 Ω resistor and a 100n capacitor, as shown below.   In HIFI mode, this would be on the combined audio output signal rather than straight from pin 9.


![RC circuit](http://farm9.staticflickr.com/8317/7934584004_096300ef0d.jpg)



### Notch filter for STANDARD mode carrier frequency

This may still be useful if you need to use STANDARD mode for some reason, for instance if you need Timer 2 for another process
(making HIFI unavailable), or if your sketch is too intensive for STANDARD_PLUS and you need 10% more juice.

People who have used Mozzi in STANDARD mode with children report that kids find the carrier frequency particularly irritating, however adults are often unable to perceive it at any level!  Many thanks to Andrew McPherson, who sent a schematic and equation for a twin-T notch filter which he used successfully to remove the offending frequency.

![Twin-T Notch Filter](http://farm9.staticflickr.com/8470/8124196839_6f3e506525.jpg)
![equation](http://farm9.staticflickr.com/8472/8124196815_6c02f4fb86.jpg)

Here's a design tool for calculating [Twin-T notch filters.](http://sim.okawa-denshi.jp/en/TwinTCRtool.php)

Andrew also reported that the low-pass and notch filter described here can be used together in series without any problems.

Here's a schematic for the 6kHz low pass and the notch filter using commonly available parts chosen to exactly match STANDARD mode's 16384Hz carrier frequency, using the notch filter design tool.

![Schematic](http://farm9.staticflickr.com/8209/8210496070_4a1b21f8c1_c.jpg)

### Download
Download a Fritzing file with breadboard, schematic and circuit board layouts [here](https://docs.google.com/file/d/0B_eOzePFYDZaSEo4bVJ6NlJnSXM/edit?usp=sharing).
