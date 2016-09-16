/*
 * Portamento.h
 *
 * Copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

#ifndef PORTAMENTO_H_
#define PORTAMENTO_H_

#include "mozzi_midi.h"
#include "mozzi_fixmath.h"
#include "Line.h"

/** A simple portamento (pitch slide from one note to the next) effect, useful for note-based applications.
*/
template <unsigned int CONTROL_UPDATE_RATE>
class
	Portamento {
	
public:

	/** Constructor.
	 */
	Portamento():
			MICROS_PER_CONTROL_STEP(1000000/CONTROL_UPDATE_RATE)
	{
	}
	
	/** Set how long it will take to slide from note to note, in milliseconds.
	@param milliseconds
	*/
	inline
	void setTime(unsigned int milliseconds){
		//control_steps_per_portamento = ((long)milliseconds*1000)/MICROS_PER_CONTROL_STEP; // more accurate but slower
		control_steps_per_portamento = convertMsecToControlSteps(milliseconds);
	}

	/** Call this at note-on, it initialises the portamento.
	@param note a midi note number, a whole number.
	*/
	inline
	void start(uint8_t note) {
		target_freq = Q16n16_mtof(Q8n0_to_Q16n16(note));
		aPortamentoLine.set(target_freq, control_steps_per_portamento);
		countdown = control_steps_per_portamento;
		portamento_on=true;
	}
	
	/** Call this at note-on, it initialises the portamento.
	@param note a midi note number in Q16n16 fractional format.  This is useful for non-whole note or detuned values.
	*/
	inline
	void start(Q16n16 note) {
		target_freq = Q16n16_mtof(note);
		aPortamentoLine.set(target_freq, control_steps_per_portamento);
		countdown = control_steps_per_portamento;
		portamento_on=true;
	}
	

	/** Use this in updateControl() to provide a frequency to the oscillator it's controlling. 
	For example: 
	myOscil.setFreq_Q16n16(myPortamento.next());
	@return a Q16n16 fractional frequency value, progressing smoothly between successive notes.
	*/
	inline
	Q16n16 next() {
		if (portamento_on==true){
			if(--countdown < 0) {
				// stay level when portamento has finished
				aPortamentoLine.set(target_freq, target_freq, control_steps_per_portamento);
				portamento_on=false;
			}
		}
		return aPortamentoLine.next();
	}
	
	private:

	int countdown;
	int control_steps_per_portamento;
	Q16n16 target_freq;
	bool portamento_on;
	const unsigned int MICROS_PER_CONTROL_STEP;
	Line <Q16n16> aPortamentoLine;

	
	// copied from ADSR.h
	inline
	static const unsigned int convertMsecToControlSteps(unsigned int msec){
		return (uint16_t) (((uint32_t)msec*CONTROL_UPDATE_RATE)>>10); // approximate /1000 with shift
	}

};

/**
@example 05.Control_Filters/Teensy_USB_MIDI_portamento/Teensy_USB_MIDI_portamento.ino
This example demonstrates the Portamento class.
*/

#endif /* PORTAMENTO_H_ */
