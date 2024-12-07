/* Maps a range of input numbers to an output range, comparing
   the results of Mozzi's IntMap object with Arduino map().

   Demonstrates IntMap, a fast integer replacement for map().

   Circuit: not required

   Mozzi documentation/API
   https://sensorium.github.io/Mozzi/doc/html/index.html

   Mozzi help/discussion/announcements:
   https://groups.google.com/forum/#!forum/mozzi-users

   Copyright 2014-2024 Tim Barrass and the Mozzi Team

   Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
*/

#include <IntMap.h>

const IntMap testIntMap(0,100,10,1000);

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200);
  int n;

  int i = -2000;
  while(i<5000){
    n = testIntMap(i);
    Serial.print("testIntMap: \t");
    Serial.print(n, DEC);

    n = map(i,0,100,10,1000);
    Serial.print("\t map: \t");
    Serial.println(n,DEC);
    i += 7;
  }
}

void loop(){
}
