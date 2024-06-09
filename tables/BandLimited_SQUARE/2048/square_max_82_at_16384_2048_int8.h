#ifndef SQUARE_MAX_82_AT_16384_2048INT8_H_
#define SQUARE_MAX_82_AT_16384_2048INT8_H_

#include <Arduino.h>
#include "mozzi_pgmspace.h"


#define SQUARE_MAX_82_AT_16384_2048_NUM_CELLS 2048
#define SQUARE_MAX_82_AT_16384_2048_SAMPLERATE 2048

CONSTTABLE_STORAGE(int8_t) SQUARE_MAX_82_AT_16384_2048_DATA [] = 
{
0, 21, 41, 60, 77, 92, 105, 114, 121, 125, 127, 126, 124, 121, 116, 112, 107, 104, 100, 98, 97, 97, 98, 100, 102, 105, 108, 110, 112, 114, 115, 115, 114, 
113, 112, 110, 108, 106, 104, 103, 103, 102, 103, 103, 104, 106, 107, 109, 110, 111, 112, 112, 112, 111, 111, 110, 108, 107, 106, 105, 104, 104, 104, 104, 105, 
106, 107, 108, 109, 110, 110, 111, 111, 111, 110, 109, 109, 108, 107, 106, 105, 105, 105, 105, 105, 106, 107, 108, 108, 109, 110, 110, 110, 110, 110, 109, 109, 
108, 107, 107, 106, 106, 105, 106, 106, 106, 107, 107, 108, 109, 109, 109, 110, 110, 110, 109, 109, 108, 108, 107, 107, 106, 106, 106, 106, 106, 107, 107, 108, 
108, 109, 109, 109, 109, 109, 109, 109, 108, 108, 107, 107, 106, 106, 106, 106, 106, 107, 107, 107, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 107, 
107, 106, 106, 106, 106, 107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 107, 107, 107, 107, 106, 106, 107, 107, 107, 108, 108, 108, 109, 
109, 109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 107, 
107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 
109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 
107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 109, 109, 108, 108, 
108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 
107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 
108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 
107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 
108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 
107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 
108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 
107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 
108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 
108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 
107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 
108, 109, 109, 108, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 
107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 
109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109, 109, 108, 108, 108, 108, 107, 107, 107, 107, 107, 107, 
107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 107, 107, 107, 107, 107, 107, 108, 108, 108, 109, 109, 109, 109, 109, 109, 
108, 108, 108, 107, 107, 107, 106, 106, 107, 107, 107, 107, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 108, 107, 107, 107, 106, 106, 106, 106, 107, 107, 
108, 108, 108, 109, 109, 109, 109, 109, 109, 108, 108, 107, 107, 107, 106, 106, 106, 106, 106, 107, 107, 108, 108, 109, 109, 109, 109, 109, 109, 109, 108, 108, 
107, 107, 106, 106, 106, 106, 106, 107, 107, 108, 108, 109, 109, 110, 110, 110, 109, 109, 109, 108, 107, 107, 106, 106, 106, 105, 106, 106, 107, 107, 108, 109, 
109, 110, 110, 110, 110, 110, 109, 108, 108, 107, 106, 105, 105, 105, 105, 105, 106, 107, 108, 109, 109, 110, 111, 111, 111, 110, 110, 109, 108, 107, 106, 105, 
104, 104, 104, 104, 105, 106, 107, 108, 110, 111, 111, 112, 112, 112, 111, 110, 109, 107, 106, 104, 103, 103, 102, 103, 103, 104, 106, 108, 110, 112, 113, 114, 
115, 115, 114, 112, 110, 108, 105, 102, 100, 98, 97, 97, 98, 100, 104, 107, 112, 116, 121, 124, 126, 127, 125, 121, 114, 105, 92, 77, 60, 41, 21, 0, 
-21, -41, -60, -77, -92, -105, -114, -121, -125, -127, -126, -124, -121, -116, -112, -107, -104, -100, -98, -97, -97, -98, -100, -102, -105, -108, -110, -112, -114, -115, -115, -114, 
-113, -112, -110, -108, -106, -104, -103, -103, -102, -103, -103, -104, -106, -107, -109, -110, -111, -112, -112, -112, -111, -111, -110, -108, -107, -106, -105, -104, -104, -104, -104, -105, 
-106, -107, -108, -109, -110, -110, -111, -111, -111, -110, -109, -109, -108, -107, -106, -105, -105, -105, -105, -105, -106, -107, -108, -108, -109, -110, -110, -110, -110, -110, -109, -109, 
-108, -107, -107, -106, -106, -105, -106, -106, -106, -107, -107, -108, -109, -109, -109, -110, -110, -110, -109, -109, -108, -108, -107, -107, -106, -106, -106, -106, -106, -107, -107, -108, 
-108, -109, -109, -109, -109, -109, -109, -109, -108, -108, -107, -107, -106, -106, -106, -106, -106, -107, -107, -107, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -107, 
-107, -106, -106, -106, -106, -107, -107, -107, -108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -107, -107, -107, -107, -106, -106, -107, -107, -107, -108, -108, -108, -109, 
-109, -109, -109, -109, -109, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -109, -109, -109, -109, -109, -108, -108, -108, -107, -107, -107, -107, 
-107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, 
-109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, 
-107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -109, -109, -108, -108, 
-108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, 
-108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, 
-107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, 
-108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, 
-107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, 
-108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, 
-107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, 
-108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, 
-107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, 
-108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, 
-108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, 
-107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, 
-108, -109, -109, -108, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, 
-107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, 
-109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -108, -109, -109, -109, -109, -108, -108, -108, -108, -107, -107, -107, -107, -107, -107, 
-107, -107, -107, -108, -108, -108, -109, -109, -109, -109, -109, -108, -108, -108, -107, -107, -107, -107, -107, -107, -107, -107, -107, -108, -108, -108, -109, -109, -109, -109, -109, -109, 
-108, -108, -108, -107, -107, -107, -106, -106, -107, -107, -107, -107, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -108, -107, -107, -107, -106, -106, -106, -106, -107, -107, 
-108, -108, -108, -109, -109, -109, -109, -109, -109, -108, -108, -107, -107, -107, -106, -106, -106, -106, -106, -107, -107, -108, -108, -109, -109, -109, -109, -109, -109, -109, -108, -108, 
-107, -107, -106, -106, -106, -106, -106, -107, -107, -108, -108, -109, -109, -110, -110, -110, -109, -109, -109, -108, -107, -107, -106, -106, -106, -105, -106, -106, -107, -107, -108, -109, 
-109, -110, -110, -110, -110, -110, -109, -108, -108, -107, -106, -105, -105, -105, -105, -105, -106, -107, -108, -109, -109, -110, -111, -111, -111, -110, -110, -109, -108, -107, -106, -105, 
-104, -104, -104, -104, -105, -106, -107, -108, -110, -111, -111, -112, -112, -112, -111, -110, -109, -107, -106, -104, -103, -103, -102, -103, -103, -104, -106, -108, -110, -112, -113, -114, 
-115, -115, -114, -112, -110, -108, -105, -102, -100, -98, -97, -97, -98, -100, -104, -107, -112, -116, -121, -124, -126, -127, -125, -121, -114, -105, -92, -77, -60, -41, -21, 
 };

#endif