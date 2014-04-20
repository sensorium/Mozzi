/*
 *  Maps a range of input numbers to an output range, comparing
 *  the results of Mozzi's IntMap object with Arduino map().
 *
 *  Demonstrates IntMap, a fast integer replacement for map().
 *  
 *  Circuit: not required
 *
 *  Mozzi help/discussion/announcements:
 *  https://groups.google.com/forum/#!forum/mozzi-users
 *
 *  Tim Barrass 2014.
 *  This example code is in the public domain.
 */

#include <IntMap.h>


const IntMap testIntMap(0,100,10,1000);

void setup(){
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


