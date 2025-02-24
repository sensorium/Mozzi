## Supported boards, output modes and default pins
Table is not necessarily complete. *Abbreviations explained below the table.* The default output mode is framed with a border in each row.
If stereo is supported, in a mode, the cell has a red/blue background, and (where applicable) the second pin number is given in parentheses (+X).
Check the [hardware section of the API-documentation](https://sensorium.github.io/Mozzi/doc/html/hardware.html) for platform specific notes and (pin) configuration options.

<style>
td[data-mo] { background-color: rgba(0,0,255,.5); } /* Mono output */
td[data-st] { background: linear-gradient(-45deg,rgba(0,0,255,.5) 50%, rgba(255,0,0,.5) 50%); } /* Stereo output */
td[data-md] { border: 2px solid; background-color: rgba(0,0,255,.5); } /* Mono default */
td[data-sd] { border: 2px solid; background: linear-gradient(-45deg,rgba(0,0,255,.5) 50%, rgba(255,0,0,.5) 50%); } /* Stereo default */
</style>

<table border="0">
<thead>
<tr><td><b>Board or family</b>  / <b>Output mode</b>                                            </td><td> PWM-1          </td><td> PWM-2             </td><td> PDM        </td><td>inbuilt DAC </td><td>I2S DAC (native)</td></tr>
</thead>
<tbody>
<tr style="border-top: 1px solid">
    <td><i>ATmega328/168</i>: Uno (R3), Nano, Pro Mini, Duemilanove, Leonardo, etc.             </td><td data-sd>9 (+10) </td><td data-mo>9, 10      </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ATmega32U4</i>: Teensy, Teensy2, 2++ <i>B5/B6 correspond to pins 14/15 in Arduino</i></td><td data-sd>B5 (+B6)</td><td data-mo>B5, B6     </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ATmega2560</i>: Arduino Mega, Freetronics EtherMega, etc.                            </td><td data-sd>11 (+12)</td><td data-mo>11, 12     </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ATmega1284</i>: Sanguino                                                             </td><td data-sd>13 (+12)</td><td data-mo>13, 12     </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td>Teensy3.x - <i>note: DAC Pin number depends on model: A14, A12, or A21</i>              </td><td> -              </td><td> -                 </td><td> -          </td><td data-md>DAC </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td>Teensy4.x                                                                               </td><td data-sd>A8 (+A9)</td><td> -                 </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>LGT8F328P</i>: "Register clone" of the ATmega328, uses the same code in Mozzi        </td><td data-sd>9 (+10) </td><td data-mo>9, 10      </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>SAMD</i>: Arduino Nano 33 Iot, Adafruit Playground Express, Gemma M0 etc.            </td><td> -              </td><td> -                 </td><td> -          </td><td data-md>A0/speaker</td><td> -   </td></tr>
<tr style="border-top: 1px solid">
    <td><i>Renesas Arduino Core</i>: Arduino Uno R4                                             </td><td> -              </td><td> -                 </td><td> -          </td><td data-md>A0  </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>Arduino MBED Core</i>: Arduino Giga (only model tested so far in this family)        </td><td> -              </td><td> -                 </td><td data-mo>SERIAL2TX</td><td data-sd>A13 (+A12)</td><td> -    </td></tr>
<tr style="border-top: 1px solid">
    <td><i>STM32 maple core</i>: Various STM32F1 and STM32F4 boards, "Blue/Black Pill"          </td><td data-sd>PB8 (+PB9)</td><td data-mo>PB8, PB9 </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>STM32duino (STM official) core</i>: Huge range of STM32Fx boards                     </td><td data-sd>PA8 (+PA8)</td><td data-mo>PA8, PA9 </td><td> -          </td><td> -          </td><td> -          </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ESP8266</i>: ESP-01, Wemos D1 mini, etc. <i>note: Beware of erratic pin labels</i>   </td><td> -              </td><td> -                 </td><td data-md>GPIO2</td><td> -         </td><td data-st>yes </td></tr>
<tr style="border-top: 1px solid">
    <td><i>ESP32</i>: that has an internal DAC (only ESP32) <i>note: Beware of vastly different pin labels across board variants</i>  </td><td data-st>15 (+16) </td><td>                 </td><td data-mo>yes </td><td data-sd>GPIO25 (+GPIO26)</td><td data-st>yes</td></tr>
	<tr style="border-top: 1px solid">
    <td><i>ESP32-S/C/H/P</i>: that do not have an internal DAC <i>note: Beware of vastly different pin labels across board variants</i>  </td><td data-sd>15 (+16) </td><td>                 </td><td data-mo>yes </td><td></td><td data-st>yes</td></tr>
<tr style="border-top: 1px solid">
    <td><i>RP2040</i>: Raspberry Pi Pico and friends                                            </td><td data-sd>0 (+1)  </td><td data-mo>0, 1       </td><td> -          </td><td> -          </td><td data-st>yes </td></tr>
</tbody>
</table>

<span style="font-size: .6em">
 - PWM-1: 1-pin PWM mode (`MOZZI_OUTPUT_PWM`)
 - PWM-2: 2-pin PWM mode (`MOZZI_OUTPUT_2PIN_PWM`)
 - PDM: Pulse density modulation, may be either `MOZZI_OUTPUT_PDM_VIA_SERIAL` or `MOZZI_OUTPUT_PDM_VIA_I2S`
 - inbuilt DAC: `MOZZI_OUTPUT_INTERNAL_DAC`
 - I2S DAC (native): native support for externally connected I2S DACs (`MOZZI_OUTPUT_I2S_DAC`). Since this requires several, often
   configurable pins, and is never the default option, no pin numbers are shown in this table.
 - **All** platforms also support "external" output modes (`MOZZI_OUTPUT_EXTERNAL_TIMED` or `MOZZI_OUTPUT_EXTERNAL_CUSTOM`), which allow
   for connecting DACs or other external circuitry.
</span>
