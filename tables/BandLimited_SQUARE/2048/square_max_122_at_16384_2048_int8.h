#ifndef SQUARE_MAX_122_AT_16384_2048INT8_H_
#define SQUARE_MAX_122_AT_16384_2048INT8_H_

#include <Arduino.h>
#include "mozzi_pgmspace.h"


#define SQUARE_MAX_122_AT_16384_2048_NUM_CELLS 2048
#define SQUARE_MAX_122_AT_16384_2048_SAMPLERATE 2048

CONSTTABLE_STORAGE(int8_t) SQUARE_MAX_122_AT_16384_2048_DATA [] = 
{
0, 14, 28, 42, 55, 67, 79, 89, 98, 106, 113, 118, 122, 125, 126, 127, 127, 125, 123, 121, 118, 115, 112, 109, 106, 104, 101, 100, 98, 97, 97, 97, 98, 
99, 100, 102, 104, 105, 107, 109, 111, 112, 113, 114, 115, 115, 115, 114, 114, 113, 112, 110, 109, 108, 107, 105, 104, 103, 103, 102, 102, 102, 103, 103, 104, 
105, 106, 107, 108, 109, 109, 110, 111, 112, 112, 112, 112, 112, 111, 111, 110, 109, 109, 108, 107, 106, 105, 105, 104, 104, 104, 104, 104, 104, 105, 106, 106, 
107, 108, 108, 109, 110, 110, 110, 111, 111, 111, 111, 110, 110, 109, 109, 108, 108, 107, 107, 106, 106, 105, 105, 105, 105, 105, 105, 106, 106, 106, 107, 108, 
108, 109, 109, 109, 110, 110, 110, 110, 110, 110, 110, 109, 109, 108, 108, 107, 107, 106, 106, 106, 105, 105, 105, 105, 106, 106, 106, 107, 107, 108, 108, 108, 
109, 109, 109, 110, 110, 110, 110, 109, 109, 109, 109, 108, 108, 107, 107, 107, 106, 106, 106, 106, 106, 106, 106, 106, 106, 107, 107, 108, 108, 108, 109, 109, 
109, 109, 109, 109, 109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 106, 106, 106, 106, 106, 106, 106, 106, 107, 107, 107, 107, 108, 108, 108, 109, 109, 109, 
109, 109, 109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 107, 106, 106, 106, 106, 106, 106, 106, 107, 107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 109, 
109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 106, 106, 106, 106, 106, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 
109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 106, 106, 106, 106, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 109, 109, 
108, 108, 108, 108, 107, 107, 107, 107, 107, 106, 106, 106, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 109, 108, 108, 108, 
108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 
107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 107, 107, 
107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 107, 107, 107, 107, 
107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 
107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 
107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 
107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 
107, 107, 107, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 
107, 108, 108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 
108, 108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 
108, 108, 109, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 106, 106, 106, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 
109, 109, 109, 109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 106, 106, 106, 106, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 
109, 109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 106, 106, 106, 106, 106, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 109, 
109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 107, 106, 106, 106, 106, 106, 106, 106, 107, 107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 109, 109, 109, 
109, 109, 108, 108, 108, 107, 107, 107, 107, 106, 106, 106, 106, 106, 106, 106, 106, 107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 109, 109, 109, 109, 109, 
109, 108, 108, 108, 107, 107, 106, 106, 106, 106, 106, 106, 106, 106, 106, 107, 107, 107, 108, 108, 109, 109, 109, 109, 110, 110, 110, 110, 109, 109, 109, 108, 
108, 108, 107, 107, 106, 106, 106, 105, 105, 105, 105, 106, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 110, 110, 110, 110, 110, 109, 109, 109, 108, 108, 
107, 106, 106, 106, 105, 105, 105, 105, 105, 105, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 111, 111, 111, 111, 110, 110, 110, 109, 108, 108, 107, 106, 
106, 105, 104, 104, 104, 104, 104, 104, 105, 105, 106, 107, 108, 109, 109, 110, 111, 111, 112, 112, 112, 112, 112, 111, 110, 109, 109, 108, 107, 106, 105, 104, 
103, 103, 102, 102, 102, 103, 103, 104, 105, 107, 108, 109, 110, 112, 113, 114, 114, 115, 115, 115, 114, 113, 112, 111, 109, 107, 105, 104, 102, 100, 99, 98, 
97, 97, 97, 98, 100, 101, 104, 106, 109, 112, 115, 118, 121, 123, 125, 127, 127, 126, 125, 122, 118, 113, 106, 98, 89, 79, 67, 55, 42, 28, 14, 0, 
-14, -28, -42, -55, -67, -79, -89, -98, -106, -113, -118, -122, -125, -126, -127, -127, -125, -123, -121, -118, -115, -112, -109, -106, -104, -101, -100, -98, -97, -97, -97, -98, 
-99, -100, -102, -104, -105, -107, -109, -111, -112, -113, -114, -115, -115, -115, -114, -114, -113, -112, -110, -109, -108, -107, -105, -104, -103, -103, -102, -102, -102, -103, -103, -104, 
-105, -106, -107, -108, -109, -109, -110, -111, -112, -112, -112, -112, -112, -111, -111, -110, -109, -109, -108, -107, -106, -105, -105, -104, -104, -104, -104, -104, -104, -105, -106, -106, 
-107, -108, -108, -109, -110, -110, -110, -111, -111, -111, -111, -110, -110, -109, -109, -108, -108, -107, -107, -106, -106, -105, -105, -105, -105, -105, -105, -106, -106, -106, -107, -108, 
-108, -109, -109, -109, -110, -110, -110, -110, -110, -110, -110, -109, -109, -108, -108, -107, -107, -106, -106, -106, -105, -105, -105, -105, -106, -106, -106, -107, -107, -108, -108, -108, 
-109, -109, -109, -110, -110, -110, -110, -109, -109, -109, -109, -108, -108, -107, -107, -107, -106, -106, -106, -106, -106, -106, -106, -106, -106, -107, -107, -108, -108, -108, -109, -109, 
-109, -109, -109, -109, -109, -109, -109, -109, -108, -108, -108, -107, -107, -107, -106, -106, -106, -106, -106, -106, -106, -106, -107, -107, -107, -107, -108, -108, -108, -109, -109, -109, 
-109, -109, -109, -109, -109, -109, -108, -108, -108, -107, -107, -107, -107, -106, -106, -106, -106, -106, -106, -106, -107, -107, -107, -107, -108, -108, -108, -109, -109, -109, -109, -109, 
-109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -106, -106, -106, -106, -106, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, 
-109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -106, -106, -106, -106, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -109, -109, 
-108, -108, -108, -108, -107, -107, -107, -107, -107, -106, -106, -106, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -109, -108, -108, -108, 
-108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, 
-107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -107, -107, 
-107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -107, -107, -107, -107, 
-107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, 
-107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, 
-107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, 
-107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, 
-107, -107, -107, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, 
-107, -108, -108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, 
-108, -108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, 
-108, -108, -109, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -106, -106, -106, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, 
-109, -109, -109, -109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -106, -106, -106, -106, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, 
-109, -109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -106, -106, -106, -106, -106, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -109, 
-109, -109, -109, -109, -108, -108, -108, -107, -107, -107, -107, -106, -106, -106, -106, -106, -106, -106, -107, -107, -107, -107, -108, -108, -108, -109, -109, -109, -109, -109, -109, -109, 
-109, -109, -108, -108, -108, -107, -107, -107, -107, -106, -106, -106, -106, -106, -106, -106, -106, -107, -107, -107, -108, -108, -108, -109, -109, -109, -109, -109, -109, -109, -109, -109, 
-109, -108, -108, -108, -107, -107, -106, -106, -106, -106, -106, -106, -106, -106, -106, -107, -107, -107, -108, -108, -109, -109, -109, -109, -110, -110, -110, -110, -109, -109, -109, -108, 
-108, -108, -107, -107, -106, -106, -106, -105, -105, -105, -105, -106, -106, -106, -107, -107, -108, -108, -109, -109, -110, -110, -110, -110, -110, -110, -110, -109, -109, -109, -108, -108, 
-107, -106, -106, -106, -105, -105, -105, -105, -105, -105, -106, -106, -107, -107, -108, -108, -109, -109, -110, -110, -111, -111, -111, -111, -110, -110, -110, -109, -108, -108, -107, -106, 
-106, -105, -104, -104, -104, -104, -104, -104, -105, -105, -106, -107, -108, -109, -109, -110, -111, -111, -112, -112, -112, -112, -112, -111, -110, -109, -109, -108, -107, -106, -105, -104, 
-103, -103, -102, -102, -102, -103, -103, -104, -105, -107, -108, -109, -110, -112, -113, -114, -114, -115, -115, -115, -114, -113, -112, -111, -109, -107, -105, -104, -102, -100, -99, -98, 
-97, -97, -97, -98, -100, -101, -104, -106, -109, -112, -115, -118, -121, -123, -125, -127, -127, -126, -125, -122, -118, -113, -106, -98, -89, -79, -67, -55, -42, -28, -14, 
 };

#endif