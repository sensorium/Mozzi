/** Test case for correct operation of the FLASH_OR_RAM_READ template function.
 *  This should print two identical ascending sequences of numbers to the serial monitor. */

#include <mozzi_pgmspace.h>

CONSTTABLE_STORAGE(byte) a[] = { 1 };
CONSTTABLE_STORAGE(int8_t) b[] = { 2 };
CONSTTABLE_STORAGE(uint8_t) c[] = { 3 };
CONSTTABLE_STORAGE(int16_t) d[] = { 4 };
CONSTTABLE_STORAGE(uint16_t) e[] = { 5 };
CONSTTABLE_STORAGE(int32_t) f[] = { 6 };
CONSTTABLE_STORAGE(uint32_t) g[] = { 7 };
CONSTTABLE_STORAGE(float) h[] = { 8.8 };
CONSTTABLE_STORAGE(double) i[] = { 9.9 };

byte ram_a[] = { 1 };
int8_t ram_b[] = { 2 };
uint8_t ram_c[] = { 3 };
int16_t ram_d[] = { 4 };
uint16_t ram_e[] = { 5 };
int32_t ram_f[] = { 6 };
uint32_t ram_g[] = { 7 };
float ram_h[] = { 8.8 };
double ram_i[] = { 9.9 };

void setup() {
  Serial.begin(9600);
}

void loop() {
   Serial.println("Flash reads");
   Serial.println(FLASH_OR_RAM_READ(a));
   Serial.println(FLASH_OR_RAM_READ(b));
   Serial.println(FLASH_OR_RAM_READ(c));
   Serial.println(FLASH_OR_RAM_READ(d));
   Serial.println(FLASH_OR_RAM_READ(e));
   Serial.println(FLASH_OR_RAM_READ(f));
   Serial.println(FLASH_OR_RAM_READ(g));
   Serial.println(FLASH_OR_RAM_READ(h));
   Serial.println(FLASH_OR_RAM_READ(i));
   Serial.println("RAM reads");
   Serial.println(FLASH_OR_RAM_READ(ram_a));
   Serial.println(FLASH_OR_RAM_READ(ram_b));
   Serial.println(FLASH_OR_RAM_READ(ram_c));
   Serial.println(FLASH_OR_RAM_READ(ram_d));
   Serial.println(FLASH_OR_RAM_READ(ram_e));
   Serial.println(FLASH_OR_RAM_READ(ram_f));
   Serial.println(FLASH_OR_RAM_READ(ram_g));
   Serial.println(FLASH_OR_RAM_READ(ram_h));
   Serial.println(FLASH_OR_RAM_READ(ram_i));
   delay (2000);
}
