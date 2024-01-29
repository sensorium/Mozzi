Porting to Mozzi 2.0

// TODO: These are just short notes taken while working. Needs to be typed up properly, in the end.


changed config names and semantics TODO (incomplete)

audio modes mapping

 - STANDARD: MOZZI_OUTPUT_PWM with MOZZI_PWM_RATE == MOZZI_AUDIO_RATE
 - STANDARD_PLUS: MOZZI_OUTPUT_PWM with MOZZI_PWM_RATE == 32768
 - HIFI: MOZZI_OUTPUT_2PIN_PWM
 - EXTERNAL_AUDIO_OUTPUT (without BYPASS_MOZZI_BUFFER): MOZZI_OUTPUT_EXTERNAL_TIMED
 - EXTERNAL_AUDIO_OUTPUT (with BYPASS_MOZZI_BUFFER): MOZZI_OUTPUT_EXTERNAL_CUSTOM 
 
further
 - USE_AUDIO_INPUT: MOZZI_AUDIO_INPUT
 - IS_STM32() -> IS_STM32MAPLE()

simple renames:
 - AUDIO_RATE: MOZZI_AUDIO_RATE
 - CONTROL_RATE: MOZZI_CONTROL_RATE

all new
 - MOZZI_ANALOG_READS -> allows to disable, explicitly
 - MOZZI_COMPATIBILITY_LEVEL

general:
 - Added many config sanity checks. Some may be too strict, if so please mention

Other removed stuff:
 - pauseMozzi() - was still declared but not defined -> not usable, anyway
 - unpauseMozzi() - was still declared but not defined -> not usable, anyway
 - Teensy3/4: channel2sc1a -> thought to be unused, removed
 - Teensy2: adc_mapping -> hidden away; use adcPinToChannelNum(), as on all other platforms, instead
 - removed several inclusions of "WProgram.h". If using Arduino versions < 1.0, you need to update, seriously ;-)  (TODO many, many, instances of this are still around)
 - Since Mozzi (AVR-port) no longer uses Timer 0 since a long time, the corresponding library (utility/TimerZero.h) has now been removed, too.
   The Arduino functions delay(), millis(), micros() and delayMicroseconds() should now be usable in theory. That said,
   you should avoid these functions, as they are slow (or even blocking). For measuring time, refer
   to mozziMicros(). For delaying events, you can use Mozzi's EventDelay() unit instead (not to be confused with AudioDelay()).

Moved headers:
 - Header files not meant for user inclusion have been moved to "internal"
 - New sketches should include "Mozzi.h", rather than "MozziGuts.h", thereby documenting, they have been written for Mozzi 2.0+
