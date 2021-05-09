/*
 * MetaOscil.h
 *
 * A wrap-up to swap between different oscillators seemlessly.
 *
 *
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



template <uint16_t NUM_TABLE_CELLS, uint16_t UPDATE_RATE, byte N_OSCIL>
  class MetaOscil
{

 public:
  MetaOscil(){}
  
  void addOscil(Oscil<NUM_TABLE_CELLS, UPDATE_RATE>* osc, int cutoff_freq)
  {
    oscillators[current_rank] = osc;
    cutoff_freqs[current_rank] = cutoff_freq;
    current_rank +=1;
  }

  inline
    int8_t next() {return current_osc->next();}

  void setTable(const int8_t * TABLE_NAME, byte rank) {oscillators[rank]->setTable(TABLE_NAME);}

  void setPhase(unsigned int phase) {current_osc->setPhase(phase);}
  
  void setPhase(unsigned int phase, byte rank) {oscillators[rank]->setPhase(phase);}

  void setPhaseFractional(unsigned long phase) {current_osc->setPhaseFractional(phase);}
  
  void setPhaseFractional(unsigned long phase, byte rank) {oscillators[rank]->setPhaseFractional(phase);}

  unsigned long getPhaseFractional() {return current_osc->getPhaseFractional();}

  unsigned long getPhaseFractional(byte rank) {return oscillators[rank]->getPhaseFractional();}

  inline
    int8_t phMod(Q15n16 phmod_proportion) {return current_osc->phMod(phmod_proportion);}

  inline
    void setFreq(int frequency)
  {
    bool fallback = true;
    for (byte i=0; i<N_OSCIL-1; i++)
      {
	if (frequency < cutoff_freqs[i])
	  {
	    fallback = false;
	    oscillators[i]->setPhaseFractional(current_osc->getPhaseFractional());
	    current_osc = oscillators[i];
	    current_osc->setFreq(frequency);
	    break;
	  }
      }
    if (fallback)
      {
	oscillators[N_OSCIL-1]->setPhaseFractional(current_osc->getPhaseFractional());
	current_osc = oscillators[N_OSCIL-1];
	current_osc->setFreq(frequency);
      }
  }

  
  inline
    void setFreq(float frequency)
  {
    bool fallback = true;
    for (byte i=0; i<N_OSCIL-1; i++)
      {
	if ((int)frequency < cutoff_freqs[i])
	  {
	    fallback = false;
	    oscillators[i]->setPhaseFractional(current_osc->getPhaseFractional());
	    current_osc = oscillators[i];
	    current_osc->setFreq(frequency);
	    break;
	  }
      }
    if (fallback)
      {
	oscillators[N_OSCIL-1]->setPhaseFractional(current_osc->getPhaseFractional());
	current_osc = oscillators[N_OSCIL-1];
	current_osc->setFreq(frequency);
      }
  }

  inline
    void setFreq_Q24n8(Q24n8 frequency)
  {
    bool fallback = true;
    for (byte i=0; i<N_OSCIL-1; i++)
      {
	if ((int)(frequency >> 8) < cutoff_freqs[i])
	  {
	    fallback = false;
	    oscillators[i]->setPhaseFractional(current_osc->getPhaseFractional());
	    current_osc = oscillators[i];
	    current_osc->setFreq_Q24n8(frequency);
	    break;
	  }
      }
    if (fallback)
      {
	oscillators[N_OSCIL-1]->setPhaseFractional(current_osc->getPhaseFractional());
	current_osc = oscillators[N_OSCIL-1];
	current_osc->setFreq_Q24n8(frequency);
      }
  }


  inline
    void setFreq_Q16n16(Q16n16 frequency)
  {
    bool fallback = true;
    for (byte i=0; i<N_OSCIL-1; i++)
      {
	if ((int)(frequency >> 16) < cutoff_freqs[i])
	  {
	    fallback = false;
	    oscillators[i]->setPhaseFractional(current_osc->getPhaseFractional());
	    current_osc = oscillators[i];
	    current_osc->setFreq_Q16n16(frequency);
	    break;
	  }
      }
    if (fallback)
      {
	oscillators[N_OSCIL-1]->setPhaseFractional(current_osc->getPhaseFractional());
	current_osc = oscillators[N_OSCIL-1];
	current_osc->setFreq_Q16n16(frequency);
      }
  }

  inline
    int8_t atIndex(unsigned int index) {return current_osc->atIndex(index);}

  inline
    unsigned long phaseIncFromFreq(int frequency) {return current_osc->phaseIncFromFreq(frequency);}

  inline
    void setPhaseInc(unsigned long phaseinc_fractional) {current_osc->setPhaseInc(phaseinc_fractional);}

    

 private:
  Oscil<NUM_TABLE_CELLS, UPDATE_RATE> * oscillators[N_OSCIL];
  Oscil<NUM_TABLE_CELLS, UPDATE_RATE> * current_osc = NULL;
  int cutoff_freqs[N_OSCIL];
  byte current_rank = 0;
  
};


#endif  /* META_OSCIL_H */
