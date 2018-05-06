#ifndef AUDIOCONFIGMCP4922_H
#define AUDIOCONFIGMCP4922_H


#include <DAC_MCP49xx.h>
#include <SPI.h>
DAC_MCP49xx dac(DAC_MCP49xx::MCP4922,10);

#define AUDIO_BIAS 2048


#endif
