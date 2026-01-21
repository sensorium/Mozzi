#ifndef SOFT_CLIP_H
#define SOFT_CLIP_H

#include "AudioOutput.h"

template<unsigned int SMOOTHING, typename T = AudioOutputStorage_t>
class SoftClip
{
  private:
    const unsigned int max_threshold = (unsigned int) SMOOTHING * PI / 2;
    T lookup_table[(unsigned int) (SMOOTHING * PI / 2)];
    T max_value;

  public:
    SoftClip(T max_val)
    {
      max_value = max_val;
      for (unsigned int i = 0; i < max_threshold; i++)
      {
        lookup_table[i] = SMOOTHING * sin(1.*i / SMOOTHING);
      }
    }

    inline T next(T in)
    {
      if (abs(in) < max_value - SMOOTHING) return in;
      else if (in > 0)
      {
        if (in < (T) (max_value - SMOOTHING + max_threshold)) return lookup_table[in - max_value + SMOOTHING] + max_value - SMOOTHING;
        else return max_value;
      }
      else
      {
        if (-in < (T) (max_value - SMOOTHING + max_threshold)) return -lookup_table[-in - max_value + SMOOTHING] - ( max_value - SMOOTHING);
        else return -max_value;
      }
    }
    void setMaxValue(T max_val) {
      max_value = max_val;
    }

    T getMaxValue() {
      return max_value;
    }

};



#endif
