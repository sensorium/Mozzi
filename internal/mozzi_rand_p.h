#ifndef MOZZI_RAND_P_H
#define MOZZI_RAND_P_H

namespace MozziPrivate {

class MozziRandPrivate {
friend void randSeed();
friend void randSeed(uint32_t);
friend uint32_t xorshift96();
  static uint32_t x;
  static uint32_t y;
  static uint32_t z;
public:
  static uint32_t xorshift96() {
    //period 2^96-1
    uint32_t t;

    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return z;
  }
  static void autoSeed(); // defined in hardware specific MozziGuts_impl-files
};

inline void randSeed(uint32_t seed) { MozziRandPrivate::x = seed; };

}

#endif
