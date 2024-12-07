---
layout: single
---


# Sounds from Sensors with Mozzi                  

## 1. Parts list

![Parts](images/fig1.jpg)

_Figure 1: Parts used in the tutorial_  


Arduino Nano or clone  
Knob  
Switch  
LDR x 2  
Piezo  
5.1K resistor X 2  
10K resistor  
1M resistor  
Red wire x 3  
Black or White wire x 4  
Yellow or Orange wire x 4  
Blue wire  
Audio socket  
Audio cable  
USB cable    

WARNING: This tutorial assumes working with a classic 5V Arduino Nano (V3.0). For other boards, the placement of the pins
may be vastly different. Also, be aware that other boards may require other voltages (e.g., only 3.3V), and following
these instructions, blindly, could easily destroy them.
{: .notice--danger}

---

## 2. Arduino

Push the Nano into the breadboard with top left leg of the USB end at __c1__.
Plug a mini USB cable into the Nano and connect it to your computer.

_Figure 2: Nano on breadboard_  

![Nano](images/fig2.jpg "Figure 2: Nano on breadboard")

* Download the [Arduino application](https://www.arduino.cc).
* Download and install [FTDI drivers](https://www.ftdichip.com/Drivers/VCP.htm) so Arduino can access the USB port.
* Start the Arduino application and set it up for a Nano clone.
_Arduino➞Tools➞Board➞Arduino Nano w/ Atmega 328_
* Set it up to communicate through the USB port.
_Arduino➞Tools➞Serial Port➞usbserial_

Let's test it by uploading a program into the Nano through the USB serial port.  _Arduino➞File➞Examples➞Blink_ and press the ➜ _(upload)_ button to upload the program to the Nano.

The red light on your nano should be blinking once per second.
Edit the code to make the light blink twice as fast by changing the delay from 1000ms to 500ms.  Yay, it works.  

---

## 3. Mozzi

### Software

OK - now the Arduino is working we are ready to install Mozzi.

* The easiest installation option nowadays is to install Mozzi via the Library Manager in your Arduino application: 
  _Arduino➞Sketch➞Include Library➞Library Manager_ type "Mozzi" into the search field, then click "install".
* If you want the latest and greatest development version of Mozzi, instead, you can get [Mozzi from Github](https://sensorium.github.io/Mozzi/).
  The are a lot of step-by-step tutorials on how to install directly from Github on the internet, but be prepared
  that the procedure is slightly more involved.
* Using either installation method, you should now be able to see Mozzi in the list under
_Arduino➞Sketch➞Import Library_.

---

### Wiring

When we wire things together on the breadboard let's all use a common wire colour protocol.

+V ⇒ RED  
-V ⇒ BLACK or WHITE  
INPUTS ⇒ YELLOW or ORANGE  
AUDIO ⇒  BLUE

Unplug the USB cable so that there is no power, just in case you plug a wire into the wrong place.

First let's set up the power rails.  

_Figure 3: 5V and GND pins connected to +V and -V rails_  

![Power](images/fig3.jpg)

* Unplug the USB cable (!)  
* Push a black wire into __b14__ which is __GND__ on the Nano.   
* Push the other end into the __-V__ rail on the left of the board at row __14__.  
* Push another black wire into the top of the left hand __-V__ rail  and connect it across the board to the top of the right hand  __-V__ rail.  
* Push a red wire into __b12__ that connects to the __5V__ pin on the Nano.
* Push the other end into the left  __+V__ rail at row __14__.
* Push another red wire into the top left  __+V__ rail and connect it to the top of the right  __+V__ rail.

Now let's add a headphone socket so we can listen to sounds synthed on the Nano.

_Figure 4: Audio socket connected to PWM Audio Output Pin D9_  

![Audio](images/fig4.jpg)

* Push the headphone socket into the bottom right hand corner of the breadboard at __g26-g30__.  
* Push a blue wire into __h4__ that connects to the audio output pin __D9__ on the Nano.
* Push the other end of the blue wire into __f26__ which is the tip of the audio socket.
* Push a black wire into  f28  to connect the __GND__ on the audio socket and push the other end into the righthand  __-V__ rail.

Connect the audio cable to the audio output socket and the other end to the audio input on your computer.

---

### Audio set up

You can listen to the sound on your computer by downloading and installing [Audacity](https://www.audacityteam.org/download/), a cross platform audio editor.

In Audacity,

* set the _Input_ to __Built-in Input, 1 (Mono)__.
* in the _Project Rate (Hz)_ box, type __16384__.  This is Mozzi's sample rate and will help show your waveforms clearly, otherwise they'll appear as scrambled, aliased Pulse Width Modulated square waves.
  (only applicable to classic Arduino boards).

Some new laptops have a single audio socket that requires a 4-pole audio input cable. If you don't have the cable then just listen to the audio through headphones which will be a bit noisy but good enough for learning….  

WARNING: Directly attaching a headphone will not work with all boards, and could destroy your MCU, if you exceed the current rating of the output pins. Using a simple (high input impedance) headphone
amplifier or an _active_ speaker is generally a safer choice.
{: .notice--danger}

---

### Testing Mozzi

Let's test that it works with an example from the Mozzi library.

* _Arduino➞File➞Examples➞Mozzi➞01.Basics➞Sinewave_
* Connect the USB cable to your Nano and upload the code.
* Press the Record button in Audacity to hear the sound and adjust the input level slider to avoid distortion.
* For a more interesting example of the sounds that Mozzi can make,
_Arduino➞File-Examples➞Mozzi➞09.Delays➞ReverbTank_STANDARD_

Try some of the other examples in the filters, delays, synthesis and sample folders to get a feel for what can be done.

---

## 4. Sensors

Now let's move on the next stage of sensing some input.


### A Knob
A knob (variable resistor, potentiometer or pot) has a middle connection which can be moved across a resistor, changing the resistance between the sweeping pin and each end.

Let's make a circuit using a knob as a sensor.

_Figure 5: Knob as input to Analog Pin A0_  

![Knob](images/fig5.jpg)

* Unplug the USB cable to unpower the Nano.
* Push the 3 legs of the knob into the board at __j18, j20 and j22__, with the rotating part facing right.
* Connect a red wire from one end of the pot at __f18__ to the __+V__ rail.
* Add an orange wire from the centre pin of the pot at __f20__ to __b4__ which connects to analog input pin __A0__ of the Nano.
* Add a black wire from the remaining leg of the potentiometer at __f22__ to the __-V__ rail.

Now the voltage on the analog input will go up and down as the knob is turned.  Let's use the knob to change the sound volume in a Mozzi sketch.
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Volume_Knob_

This program reads the voltage on Analog pin 0 as an integer from 0-1023.  

* Click the Monitor button (magnifying glass) on the top right of the Arduino programming environment to read the value.
* In the monitor window, select __115200__ baud to match the baud rate setup in the code.   Now a stream of numbers should scroll down the monitor, varying between 0-1023 as you rotate the knob.

---

### Light Dependent Resistors

A Light Dependent Resistor (LDR) changes resistance with the light level. Let's use that to control an FM synthesis algorithm.

First, we'll make the sensor circuit using a LDR and a 5.1k Resistor (green-brown-black-brown) as a voltage ladder as input to the Analog Input on the Nano.

_Figure 6: LDR + resistor as input to Analog Pin A1_  

![LDR](images/fig6.jpg)

* Unplug the USB cable to unpower the Nano.
* Push one leg of the LDR into __a20__ and the other leg into the __+V__ rail.
* Connect an orange wire from the LDR at __b20__ to __b5__ which connects to the analog pin __A1__ of the Nano.
* Push one leg of the 5.1k resistor into __a5__ and the other leg into the __-V__ rail.

Now the voltage on the analog input will go up and down with the light level.  The resistor stops high current draw on the USB port when the resistance of the LDR goes low.

Let's test the LDR in a Mozzi sketch.  
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Volume_Knob_LightLevel_Frequency_

This program adds the LDR to the previous example, reading the voltage on Analog pin 1 to change the frequency of the sinewave.  Use the Serial Monitor in the Arduino IDE to have a look at the numbers from the LDR.  They should range somewhere between about 300-1000.  Feel free to calibrate the sensor by changing the number mapping from the LDR to the oscillator frequency in the sketch.

Let's try using the knob and LDR to modulate FM synthesis parameters.
The knob sets the fundamental (or _carrier_) frequency and the LDR changes the modulation width, which affects the brightness of the sound.
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Knob_LightLevel_FMsynth_

Wow.  Try to stay calm.  We're gonna make it twice as fun by adding another light level input to the FM synth.

_Figure 7: LDR + resistor as input to Analog Pin A2_  

![LDR+resistor](images/fig7.jpg)

* Unplug the Nano to depower it.
* Push the leg of another LDR into __a25__ and connect the other leg to   the __+V__ rail as before.
* Add an orange wire from the LDR at __b25__ to __b6__, which connects to Nano Analog Input __A2__.
* Push one leg of the 5.1k resistor into __a6__ and the other leg into the __-V__ rail.

Now upload the dual LDR + knob input FM synth into the Nano.
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Knob_LightLevel_x2_FMsynth_

The three inputs will produce a range of vibratos and timbre modulations that are great fun to mess around with (depending on how excitable you are).  In the sketch, you can calibrate your own mappings from the sensors to the synthesis wherever you see “calibrate” in the comments.

The same inputs can be used to play lots of different kinds of synthesis.  Here's another sketch using the knob and two LDRs.  This one makes vocal-like sounds using wavepacket synthesis.
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞LDR_x2_Knob_WavePacket_

---

### Piezo

A piezo creates a varying voltage when you squeeze it.  We'll use it as a sensor by measuring the voltage the piezo produces across a 1 megaOhm resistor (brown-black-black-yellow).

_Figure 8: Piezo + resistor as input to Analog Pin A3_  

![Piezo](images/fig8.jpg)

* Unplug the Nano to depower it.
* It can be a good idea to wrap some sticky tape around the piezo where the wires are soldered on, to protect the fragile connections.
* Plug the red wire of the piezo into __a30__ and the black wire into the __-V__ rail.
* Add an orange wire from the piezo at __b30__ to __b7__ which connects to Nano analog input __A3__.
* Push one leg of the 1 megaOhm resistor into __a7__ and the other leg into the __-V__ rail.

First, let's get a feel for the piezo's response with a simple patch.
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Piezo_Frequency_

Press on the piezo, tap it and scrape it to hear what it does.  Notice it has a very fast response, especially a sharp attack and decay when it's knocked.  There are a couple of commented-out modifications in the sketch which can be uncommented to play around with the dynamics of the piezo signal.

Now we'll use the sharp attack of the piezo to trigger a recorded audio sample to play, and for fun we can twist the knob to change the playback rate.
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Piezo_SampleTrigger_

The next sketch uses pressure on the piezo to scrub through a sample by changing the starting position.  It's crude, but you get the idea...
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Piezo_SampleScrubber_

---

### Button

So far all of the sensor inputs have been analog.  Now we'll add a digital input with a Button and a 10k Resistor (brown-black-black-red).

_Figure 9: Button + resistor as input to Digital Pin D4_  

![Button](images/fig9.jpg)

* Unplug the Nano to depower it.
* Push the button into the breadboard with one leg in __j9__ which connects to digital input __D4__, and the other leg in the __+V__ rail.
* Add the 10k resistor from __h9__ to the __-V__ rail.

When the button is pressed, the digital pin is connected to __+V__, and the voltage at the pin will be high.  When the button is not pressed, the pin is pulled to __-V__ through the resistor and will be read as low by the Nano.

Let's use the button to change the pitch of a sample trigger by the piezo.  
_Arduino➞File➞Examples➞Mozzi➞03.Sensors➞Piezo_Switch_Pitch_

You can think of a better use for a switch than that!

---

## 5. Go to town

Now we've got some experience using sensors to generate sounds, it's time to expand the frontiers of sensor-based microsynthesis!

---

## 6. Resources

[Mozzi Programming Reference](https://sensorium.github.io/Mozzi/doc/html/index.html)  
[Output circuits for cleaner sounding audio](https://sensorium.github.io/Mozzi/learn/output/)  
[Tips for effective Mozzi programming](https://sensorium.github.io/Mozzi/learn/hints/)  

---

Tim Barrass and Stephen Barrass 2013. This work is licensed under the Creative Commons Attribution-NonCommercial- ShareAlike 3.0 Unported License. To view a copy of this license, visit https://creativecommons.org/licenses/by-nc-sa/3.0/.  
![Creative Commons by-nc-sa logo](images/by-nc-sa.png)  
