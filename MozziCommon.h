#pragma once
// #include "mozzi_config_defines.h"
// #include "mozzi_config.h"
// #include "AudioConfigAll.h"  // AVR needs to load different config files
#include "CircularBuffer.h"
#include "Mozzi.h"

/**
* Shared functionality between all new implementations
*/

class MozziCommon {
    public:
        MozziCommon() {
            // intialize pins on all channels to to be undefined
            for (int j=0;j<AUDIO_CHANNELS;j++) {
                channel_pins[j] = -1;
            }

            // setup pin numbers
            #ifdef AUDIO_CHANNEL_1_PIN
            channel_pins[0] = AUDIO_CHANNEL_1_PIN;
            #endif

            #if AUDIO_CHANNELS > 1
                #ifdef AUDIO_CHANNEL_1_PIN_HIGH
                    channel_pins[1] = AUDIO_CHANNEL_1_PIN_HIGH;
                #elif defined(AUDIO_CHANNEL_2_PIN)
                    channel_pins[1] = AUDIO_CHANNEL_2_PIN;
                #endif
            #endif
        }

        const int channels() const {
            return AUDIO_CHANNELS;
        }

        const int16_t* pins() {
            return channel_pins;
        }

        void setPin(uint8_t idx, int16_t pin){
            channel_pins[idx] = pin;
        }

        inline bool advanceControlLoop() {
            bool result = false;
            if (!update_control_counter) {
                update_control_counter = update_control_timeout;
                updateControl();
                //adcStartReadCycle();
                result = true;
            } else {
                --update_control_counter;
            }
            return result;
        }

        void startControl(unsigned int control_rate_hz) {
            update_control_timeout = AUDIO_RATE / control_rate_hz;
        }

        unsigned long audioTicks() {
            return output_buffer.count();
        }

        unsigned long mozziMicros() { 
            return audioTicks() * MICROS_PER_AUDIO_TICK;
        }


        void audioHook()  {
            #if (USE_AUDIO_INPUT == true) 
            if (!input_buffer.isEmpty()) {
                audio_input = input_buffer.read();
            }
            #endif

            if (canBufferAudioOutput()) {
                advanceControlLoop();
                bufferAudioOutput(updateAudio());
            }
        }

        bool canBufferAudioOutput(){
            return !output_buffer.isFull();
        } 

        void bufferAudioOutput(const AudioOutput_t f){
            output_buffer.write(f);
        }

        /**
        * Public Shared Data 
        */
        uint16_t update_control_timeout;
        uint16_t update_control_counter;
        int16_t channel_pins[AUDIO_CHANNELS];
        AudioOutputStorage_t audio_input = 0;
        CircularBuffer<AudioOutput_t> output_buffer;
        #if USE_AUDIO_INPUT == true
        CircularBuffer<AudioOutput_t> input_buffer;  
        #endif

};

extern MozziCommon Common;
