/*
 * MozziStreams.cpp
 *
 * This file provides some alternative means to output data.
 *
 * Copyright 2021 Phil Schatzmann
 *
 * This file is part of Mozzi.
 *
 * Mozzi by Tim Barrass is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 * 
 */


#pragma once
#include "Mozzi.h"

#if STEREO_HACK == true
#error("STEREO_HACK not supported")
#endif

#if __has_include("USBAudio.h")
#include "USBAudio.h"
#define USE_USB_AUDIO 
#endif


/**
 * @brief Provides the Audio data as an Arduino Stream from where you can read the data.
 * Preferrably use readBytes( uint8_t *buffer, size_t length) over the other methods.
 */
class AudioStream : public MozziControl, public Stream {
    public:
        AudioStream(){     
        }

        void start(int control_rate = CONTROL_RATE){
            active = true;
            control_counter_max = AUDIO_RATE / control_rate;
            if (control_counter_max <= 0){
                control_counter_max = 1;
            }
            control_counter = control_counter_max;
        }

        void stop(){
            active = false;
        }   

        virtual int available() {
            // this is unbounded to we just return a big number
            return active ? 1024 : 0;
        };

        /// Reads a single byte - this is a little bit hacky just to support the full Arduino API
        virtual int read() {
            if (!active) return -1;
            uint8_t result = -1;
            switch(tmp_size){
                case 0:  
                    sample = readSample();
                    result = sample_ptr[0];
                    tmp[0] = sample_ptr[1];                    
                    tmp_size = 1;
                    break;
                case 1:
                    result = tmp[0];
                    tmp_size = 0;
                    break;
                case 2:
                    result = tmp[0];
                    tmp[0] = tmp[1];
                    tmp_size = 1;
                    break;
            }
            return result;
        }

        /// peeks a single byte
        virtual int peek() {
            if (!active) return -1;
            if (tmp_size=0){ 
                *tmp_ptr = readSample();
                tmp_size = 2;
            }
            return tmp[0];
        }

        /// Reads te data into a buffer of char
        size_t readBytes( char *buffer, size_t length){
            readBytes((uint8_t*)buffer, length);
        }
        
        /// Optimized Method -> Please preferrabley use this one
        size_t readBytes( uint8_t *buffer, size_t length) { 
            if (!active) return -1;
            int16_t *ptr = (int16_t *) buffer;
            size_t result_samples = length / sizeof(int16_t);
            for (int j=0;j<result_samples;j++){
                ptr[j] = readSample();
            }
            return result_samples * sizeof(int16_t);            
        }
        
        // not implemented
        virtual size_t write(uint8_t) {
        }

    protected:
        bool active;
        int control_counter_max;
        int control_counter;
        uint64_t samples_written_to_buffer;
        uint8_t tmp[2];
        int16_t *tmp_ptr = (int16_t*)tmp;
        int tmp_size;
        int16_t sample;
        uint8_t *sample_ptr = (uint8_t *)sample;

        /// Provides a single sample
        virtual int16_t readSample() {            
            if (--control_counter<0){
                control_counter = control_counter_max;
                ::updateControl();
            }
            int result = (int) ::updateAudio();
            samples_written_to_buffer++;
            return result;
        }
};


#ifdef USE_USB_AUDIO
/**
 * @brief Provides the Audio data for your USB connection 
 */
#define USB_BUFFER_SIZE 1024

class MozziUSBAudio : public MozziControl {
    public:
        MozziUSBAudio() {
        }
        ~MozziUSBAudio(){
            if (audio!=nullptr){
                delete audio;
            }
        }

        void start(int control_rate = CONTROL_RATE) {
            if (audio==nullptr){
                stream.start(control_rate);
                int channels_out = 1;
                audio = new USBAudio(true, 48000, 1, AUDIO_RATE, channels_out);
            }
            active = true;
        }

        void stop() {
            stream.stop();
            active = false;
        }

        void audioHook(){
            if (active && audio!=nullptr && audio->write_ready()) {
                size_t len = stream.readBytes(buffer, USB_BUFFER_SIZE );
                write_usb(buffer, len);
            }
        }

    protected:
        USBAudio *audio = nullptr;
        AudioStream stream;
        uint8_t buffer[USB_BUFFER_SIZE];
        int16_t *samples = (int16_t *) buffer;
        bool active;

        size_t write_usb(uint8_t *buffer, size_t len) {
            return audio->write(buffer,len);   
        }
       
};

#endif