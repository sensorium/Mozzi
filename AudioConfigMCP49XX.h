#ifndef AUDIOCONFIGMCP49XX_H
#define AUDIOCONFIGMCP49XX_H

/**
  Config file in order to use Mozzi with a true DAC MCP4922.
  In order to use this, you will the MCP49XX library that can be found here:
  https://github.com/exscape/electronics/tree/master/Arduino/Libraries/DAC_MCP49xx written by Thomas Backman.
  As PortWrite is enabled in order to gain speed, use Pin10 for CS SPI pin.
  The type of DAC can be changed in the constructor, beware to adapt the AUDIO_BIAS to the number of bits you are using.
 */


#include <DAC_MCP49xx.h>
#include <SPI.h>
DAC_MCP49xx dac(DAC_MCP49xx::MCP4922,10);
#define AUDIO_BIAS 2048  //we are using 12bits
#endif
