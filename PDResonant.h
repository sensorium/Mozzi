/*
 * PDResonant.h
 *
 * This implementation copyright 2012 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */



#include <mozzi_midi.h>
#include <ADSR.h>
#include <Oscil.h>
#include <Phasor.h>
// wavetable for oscillator:
#include <tables/sin2048_int8.h>

/**  
		 PDResonant is a simple midi instrument using Phase distortion used to simulate resonant filter, based on
		 https://en.wikipedia.org/wiki/Phase_distortion_synthesis.
		 
		 The class shows how the Mozzi Phasor class
		 can be used to generate an index into a wavetable, and an ADSR
		 is used to modulate the effect by modifying the Phasor frequency and sync.
		 More complex phase distortion effects could be developed by using
		 precalculated tables, or calcuating tables on the fly using a double buffer,
		 or a line-breakpoint model, a sort of hybridPhasor-Line object.
*/

class PDResonant
{

public:

	/** Constructor.
	*/
	PDResonant():
			PDM_SCALE(0.05)
	{
		aOsc.setTable(SIN2048_DATA);
		aAmpEnv.setADLevels(255, 255);
		aAmpEnv.setTimes(50, 300, 60000, 1000);
		kResonantFreqEnv.setADLevels(255,100);
	}

	/**  Play a note in response to midi input.  Params copied from MIDI library HandleNoteOn().
	@param channel is the midi channel
	@param pitch is the frequency in Hz
	@param velocity you know what it is
	*/
	void noteOn(byte channel, byte pitch, byte velocity)
	{
		kResonantFreqEnv.noteOn();
		aAmpEnv.noteOn();
		freq = mtof(pitch);
		aBaseCounter.setFreq(freq); // gets modulated in updateControl()
		aResonanceFreqCounter.setFreq(freq);
	}


	/**  Stop a note in response to midi input.    Params copied from MIDI library HandleNoteOff()
	@param channel is the midi channel
	@param pitch is the frequency in Hz
	@param velocity you know what it is
	*/
	void noteOff(byte channel, byte pitch, byte velocity)
	{
		aAmpEnv.noteOff();
		kResonantFreqEnv.noteOff();
	}


	/** Set the resonant filter sweep parameters.
	@param attack ADSR attack
	@param decay ADSR decay
	*/
	void setPDEnv(int attack, int decay)
	{
		// sustain and release timesare hardcoded here but don't need to be
		kResonantFreqEnv.setTimes(attack, decay, 60000, 1000);
		kResonantFreqEnv.update();

		float resonance_freq = freq + ((float)freq * ((float)kResonantFreqEnv.next()*PDM_SCALE));
		aResonanceFreqCounter.setFreq(resonance_freq);
	}


	/** Update the filter sweep.  Use this in updateControl().
	*/
	void update()
	{
		aAmpEnv.update();
		kResonantFreqEnv.update();
		// change freq of resonant freq counter, following the envelope
		float resonance_freq = freq + ((float)freq * ((float)kResonantFreqEnv.next()*PDM_SCALE));
		aResonanceFreqCounter.setFreq(resonance_freq);
	}

	/**  Produce the audio output.  This goes in updateAudio().
	*/
	int next()
	{
		static byte previous_base_counter;
		byte base_counter = aBaseCounter.next()>>24;

		// reset resonance counter (wiki b.)
		if (base_counter<previous_base_counter) aResonanceFreqCounter.set(0);
		previous_base_counter= base_counter;

		// index (phase) needs to end up as 11bit to match 2048 wavetable size
		unsigned int index = aResonanceFreqCounter.next()>>21; // 11 bits fits 2048 cell sin table

		// amp ramp smooths the jump when aResonanceFreqCounter is reset (wiki d.)
		byte amp_ramp = 255-base_counter;

		// wiki e., with amp envelope added
		return ((long)aAmpEnv.next() * amp_ramp * aOsc.atIndex(index))>>16;

		// return ((index>>3)*amp_ramp)>>8; // this also sounds good - squelchy sawtooth
	}


private:
	const float PDM_SCALE;
	byte amp;
	int freq;

	Phasor <AUDIO_RATE> aBaseCounter;
	Phasor <AUDIO_RATE> aResonanceFreqCounter;

	Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aOsc;
	ADSR <CONTROL_RATE, AUDIO_RATE> aAmpEnv;
	ADSR <CONTROL_RATE, CONTROL_RATE> kResonantFreqEnv;

};
