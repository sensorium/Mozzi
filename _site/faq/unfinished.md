[7. What's the difference between STANDARD and HIFI modes?](#7)
<a id="q7"></a>
##What's the difference between STANDARD and HIFI modes? {% render_partial _partials/link-to-top.markdown %}

##How can I read serial, I2C and sensors without disrupting audio?
Some sensors have libraries which rely on Arduino's Wire functions, 
which block the processor with delays and loops while waiting for responses and other timing things.
That disrupts smooth audio.  
For I2C communication, Mozzi has mozzi_twowire.h, initiated by Marlje, 
which splits the Wire functions into request and recieve functions.  
There is also a separate library available which does the same thing, called NBWire library, 
which might be more developed or better maintained.  Here's a  [discussion](http://forum.arduino.cc/index.php/topic,70705.0.html) on the arduino forum, and [download](http://telobot.com/downloads/NBWire10.zip).
The sad pat is that if you want to use a pre-exisiting library made for your sensor, for example Adafruit_INA219, you'll probably
have to adapt it to use the non-blocking code, unless someone else already has, like Adafruit_INA219_nonblock.

NBSerial library http://forum.arduino.cc/index.php?topic=70623.0 http://telobot.com/downloads/NBSerial25.zip



##Why can't I use the Arduino time functions?

##I don't get it

##Can you make it work on X board/processor?

##Can you add X feature?

##Why do I have to dig into the Mozzi/mozzi_config.h file to set options?

##Why is the audio sample rate set at 16384 (or optionally 32768 in HIFI mode?)
