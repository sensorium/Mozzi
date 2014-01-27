#include "Arduino.h"

enum options {STANDARD,HIFI};

template <const int N>
class Test
	{}
;

/** specialisation of Test template*/
template <>
class Test <STANDARD>
{
public:
	Test(){
		Serial.println(HELLO);
	}
#if defined(TCCR2A)
#define HELLO ("hello")
unsigned char  pre_mozzi_TCCR2A, pre_mozzi_TCCR2B, pre_mozzi_OCR2A, pre_mozzi_TIMSK2;
#elif defined(TCCR2)
#define OHDEAR ("ohdear")
unsigned char  pre_mozzi_TCCR2, pre_mozzi_OCR2, pre_mozzi_TIMSK;
#elif defined(TCCR4A)
unsigned char  pre_mozzi_TCCR4A, pre_mozzi_TCCR4B, pre_mozzi_TCCR4C, pre_mozzi_TCCR4D, pre_mozzi_TCCR4E, pre_mozzi_OCR4C, pre_mozzi_TIMSK4;
#endif
};

/** specialisation of Test template*/
template <>
class Test <HIFI>
{


public:
	Test(){
		Serial.println(HELLO);
	}

};
