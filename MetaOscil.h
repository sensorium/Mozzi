/*
 * MetaOscil.h
 *
 * A wrap-up to swap between different oscillators seemlessly, allowing to produce non-aliased sounds by automatically switching between oscillators.
 *
 * This file is part of Mozzi.
 */

#ifndef META_OSCIL_H
#define META_OSCIL_H


#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Oscil.h"
#include "mozzi_fixmath.h"


/**
   MetaOscil is a wrapper for several Oscil. Once constructed it will behave exactly as an Oscil except that it will automatically switch between Oscil depending on the asked frequency. This allows to produce non-aliased sounds by switching between tables with less and less harmonics as the frequency increases.
*/


template<uint16_t NUM_TABLE_CELLS, uint16_t UPDATE_RATE, byte N_OSCIL>
  class MetaOscil
  
{
 public:
  /** Constructor
      Declare a MetaOscil containing any number of Oscil pointers. Every Oscil should have the same TABLE_NUM_CELLS and UPDATE_RATE which are also passed in the MetaOscil constructor. 
      @param N_OSCIL is the number of Oscil contained in the MetaOscil. This cannot be changed after construction. */ 
  template<class... T>  MetaOscil(Oscil<NUM_TABLE_CELLS, UPDATE_RATE>* first, T*... elements):oscillators{first, elements...} {
    current_osc=oscillators[0];};

  MetaOscil(){};
  
  /* Add one oscil to the MetaOscil.
      @param osc is a pointer toward an Oscil
      @param cutoff_freq is the cutoff frequency of this Oscil 
  void addOscil(Oscil<NUM_TABLE_CELLS, UPDATE_RATE>* osc, int cutoff_freq)
  {
    oscillators[current_rank] = osc;
    cutoff_freqs[current_rank] = cutoff_freq;
    if (current_rank == 0) current_osc=oscillators[0];
    current_rank += 1;
  }*/


  /** Set all Oscil of a MetaOscil.
      @param first... is a list of pointers towards several Oscil */ 
  template<typename ... T >  void setOscils(Oscil<NUM_TABLE_CELLS, UPDATE_RATE>* first,T... elements)
    {
      oscillators[current_rank]=first;
      if (current_rank == 0) current_osc=oscillators[0];
      current_rank+=1;
      setOscils(elements...);
      current_rank = 0;
    }

  void setOscils(){};

  
  /** Set all the cutoff frequencies for changing between Oscil. They have to be sorted in increasing values and contain at least N_OSCIL-1 values. Note that the last Oscil will be used by default for frequencies higher than the higher cutoff, hence the last value can be discarded.
      @param first, elements... a set of int cutoff frequencies.*/
  template<typename ... T >  void setCutoffFreqs(int first,T... elements)
    {
      cutoff_freqs[current_rank]=first;
      current_rank+=1;
      setCutoffFreqs(elements...);
      current_rank = 0;
    }

  void setCutoffFreqs() {};

  /** Set or change the cutoff frequency of one Oscil.
      @param rank is the rank of the Oscil.
      @param freq is the cutoff frequency. */
  void setCutoffFreq(int freq, byte rank)
  {
    cutoff_freqs[rank] = freq;
  }
  
  /** Updates the phase according to the current frequency and returns the sample at the new phase position.
      @return the next sample.
  */
  inline
    int8_t next() {return current_osc->next();}

  /** Change the sound table which will be played by the Oscil of rank.
      @param TABLE_NAME is the name of the array in the table ".h" file you're using.
      @param rank is the Oscil.*/
  void setTable(const int8_t * TABLE_NAME, byte rank) {oscillators[rank]->setTable(TABLE_NAME);}

  
  /** Set the phase of the currently playing Oscil.
      @param phase a position in the wavetable.*/
  void setPhase(unsigned int phase) {current_osc->setPhase(phase);}


  /** Set the phase of the currently playing Oscil in fractional format.
      @param phase a position in the wavetable.*/
  void setPhaseFractional(unsigned long phase) {current_osc->setPhaseFractional(phase);}


  /** Get the phase of the currently playin Oscil in fractional format.
      @return position in the wavetable, shifted left by OSCIL_F_BITS (which is 16 when this was written).
  */
  unsigned long getPhaseFractional() {return current_osc->getPhaseFractional();}


  
  /** Returns the next sample given a phase modulation value.
      @param phmod_proportion a phase modulation value given as a proportion of the wave. The
      phmod_proportion parameter is a Q15n16 fixed-point number where the fractional
      n16 part represents almost -1 to almost 1, modulating the phase by one whole table length in
      each direction.
      @return a sample from the table.*/
  inline
    int8_t phMod(Q15n16 phmod_proportion) {return current_osc->phMod(phmod_proportion);}


  /** Set the MetaOsc frequency with an unsigned int.
      @param frequency to play the wave table.*/
  inline
    void setFreq(int frequency, bool apply = true)
  {
    if (frequency < cutoff_freqs[0])  //getting out the extreme cases
      {
	oscillators[0]->setPhaseFractional(current_osc->getPhaseFractional());
	current_osc = oscillators[0];
	current_osc->setFreq(frequency);
      }
    
    else if (frequency > cutoff_freqs[N_OSCIL-1])
      {	    
	oscillators[N_OSCIL-1]->setPhaseFractional(current_osc->getPhaseFractional());
	current_osc = oscillators[N_OSCIL-1];
	current_osc->setFreq(frequency);      
      }
    else  // dichotomic search
      {
	byte low_point = 0, high_point = N_OSCIL-1, mid_point = (N_OSCIL-1)>>1;
	while(low_point != high_point)
	  {
	    if (frequency > cutoff_freqs[mid_point]) low_point = mid_point+1;
	    else if (frequency < cutoff_freqs[mid_point]) high_point = mid_point;
	    else
	      {
		break;
	      }
	    mid_point = (low_point + high_point)>>1;
	  }
	oscillators[mid_point]->setPhaseFractional(current_osc->getPhaseFractional());
	current_osc = oscillators[mid_point];
	if (apply) current_osc->setFreq(frequency);      
      }
      
  }


  /** Set the MetaOsc frequency with a float.
      @param frequency to play the wave table.*/
  inline
    void setFreq(float frequency)
  {
    setFreq((int) frequency, false);
    current_osc->setFreq(frequency);    
  }


  /** Set the MetaOsc frequency with a Q24n8 fixed-point number format.
      @param frequency to play the wave table.*/
  inline
    void setFreq_Q24n8(Q24n8 frequency)
  {
    setFreq((int) (frequency>>8), false);
    current_osc->setFreq_Q24n8(frequency);
  }

  
  /** Set the MetaOsc frequency with a Q16n16 fixed-point number format.
      @param frequency to play the wave table.*/
  inline
    void setFreq_Q16n16(Q16n16 frequency)
  {
    setFreq((int) (frequency>>16), false);
    current_osc->setFreq_Q16n16(frequency);
  }


  /**  Returns the sample at the given table index of the current Oscil.
       @param index between 0 and the table size.The
       index rolls back around to 0 if it's larger than the table size.
       @return the sample at the given table index.
  */
  inline
    int8_t atIndex(unsigned int index) {return current_osc->atIndex(index);}


  /** phaseIncFromFreq() and setPhaseInc() are for saving processor time when sliding between frequencies.
      @param frequency for which you want to calculate a phase increment value.
      @return the phase increment value which will produce a given frequency.*/
  inline
    unsigned long phaseIncFromFreq(int frequency) {return current_osc->phaseIncFromFreq(frequency);}

  /** Set a specific phase increment.
      @param phaseinc_fractional a phase increment value as calculated by phaseIncFromFreq().
  */
  inline
    void setPhaseInc(unsigned long phaseinc_fractional) {current_osc->setPhaseInc(phaseinc_fractional);}

    

 private:
  Oscil<NUM_TABLE_CELLS, UPDATE_RATE> * oscillators[N_OSCIL];
  Oscil<NUM_TABLE_CELLS, UPDATE_RATE> * current_osc = NULL;
  int cutoff_freqs[N_OSCIL];
  byte current_rank = 0;
  
};

/**
@example 06.Synthesis/NonAlias_MetaOscil/NonAlias_MetaOscil.ino
This example demonstrates the Meta_Oscil class.
*/

#endif  /* META_OSCIL_H */
