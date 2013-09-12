/*  Example swapping between sounds played by a single Oscil
    with Mozzi sonification library.
  
    Demonstrates declaring an Oscil without a table,
    and Oscil::setTable() method.
  
    Created by Tim Barrass 2012.
    This example code is in the public domain.
*/

#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>

// tables need to be the same size
#include <tables/sin512_int8.h> 
#include <tables/saw_analogue512_int8.h>

#define CONTROL_RATE 64

// declare with or without a wavetable, and use setTable() later
Oscil <512, AUDIO_RATE> aOscil; 

// for scheduling table swaps
EventDelay kSwapTablesDelay;

boolean using_sin = true;


void setup(){
  startMozzi(CONTROL_RATE);
  kSwapTablesDelay.set(1000); // 1 second countdown, within resolution of CONTROL_RATE
  aOscil.setFreq(440.f);
}


void updateControl(){
    if(kSwapTablesDelay.ready()){
      if (using_sin){
        aOscil.setTable(SAW_ANALOGUE512_DATA);
        using_sin = false;
      }else{
         aOscil.setTable(SIN512_DATA);
         using_sin = true;
      }
      kSwapTablesDelay.start();
    }
} 


int updateAudio(){
  return aOscil.next();
}


void loop(){
  audioHook(); // required here
}





