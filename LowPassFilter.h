#ifndef LOWPASSFILTER_H_
#define LOWPASSFILTER_H_

#include "Arduino.h"

//#include "CuttlefishGuts.h"

// adapted from Joe Marshall's 2011
//  low pass filter based on the one from MeeBlip (http://meeblip.noisepages.com)
// used in Joe's OCTOSynth-0.2 http://www.cs.nott.ac.uk/~jqm/?p=605

// a += f*((in-a)+ q*(a-b)
// b+= f* (a-b)

// log table for 128 filter cutoffs
static prog_uchar logCutoffs[128] PROGMEM = {0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x04,0x04,0x04,0x04,0x04,0x05,0x05,0x05,0x05,0x06,0x06,0x06,0x06,0x06,0x06,0x07,0x08,0x08,0x08,0x09,0x09,0x0A,0x0A,0x0A,0x0A,0x0B,0x0C,0x0C,0x0C,0x0C,0x0D,0x0E,0x0F,0x10,0x11,0x12,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1E,0x20,0x21,0x22,0x23,0x24,0x26,0x28,0x2A,0x2C,0x2E,0x30,0x32,0x34,0x36,0x38,0x3A,0x40,0x42,0x44,0x48,0x4C,0x4F,0x52,0x55,0x58,0x5D,0x61,0x65,0x68,0x6C,0x70,0x76,0x7E,0x85,0x8A,0x90,0x96,0x9D,0xA4,0xAB,0xB0,0xBA,0xC4,0xCE,0xD8,0xE0,0xE8,0xF4,0xFF};

class LowPassFilter
{

public:

	LowPassFilter();


	inline
	void setFilterRaw(unsigned char filterF, unsigned char resonance)
	{
		volatile unsigned char tempReg=0,tempReg2=0;
		asm volatile("ldi %[tempReg], 0xff" "\n\t"
		             "sub %[tempReg],%[filtF] " "\n\t"
		             "lsr %[tempReg]" "\n\t"
		             "ldi %[tempReg2],0x04" "\n\t"
		             "add %[tempReg],%[tempReg2]" "\n\t"
		             "sub %[reso],%[tempReg]" "\n\t"
		             "brcc Res_Overflow%=" "\n\t"
		             "ldi %[reso],0x00" "\n\t"
		             "Res_Overflow%=:" "\n\t"
		             "mov %[filtQ],%[reso]" "\n\t"
             :[tempReg] "=&d" (tempReg),[tempReg2] "=&d" (tempReg2),[filtQ] "=&d" (filterQ): [reso] "d" (resonance), [filtF] "d" (filterF) );
	}


	inline
	void setFilter(unsigned char f, unsigned char resonance)
	{
		if(f>127)
			f=127;
		filterF=logCutoffs[f];
		setFilterRaw(filterF,resonance);
	}


	/** Filters inValue according to the current frequency and resonance settings.
	*/
	inline
	char next(char inValue)
	{
		// TB the input to the filter is a char * 63 (vol),  ie. -8192 to +8191
		outValue = (int) inValue * 64;
		// at this point outValue = oscillator value
		// it is currently maxed to full volume / 4
		// to allow some headroom for filtering
		//  outValue>>=3;
		// started at 4700
		// 4686

		// TB make these class vars.. so they don't get allocated space every time this runs?
		volatile int tempReg,tempReg2=0;
		volatile unsigned char zeroRegFilt=0;

		// de-volatilisati
		//TB2012 make these class vars?
		unsigned char filtF=filterF;
		unsigned char filtQ=filterQ;
		//TB2012 and do this:
		//filtF=filterF;
		//filtQ=filterQ;

		asm volatile(

		        "sub %A[outVal],%A[filtA]" "\n\t"
		        "sbc %B[outVal],%B[filtA]" "\n\t"
		        "brvc No_overflow1%=" "\n\t"
		        "ldi %A[outVal],0b00000001" "\n\t"
		        "ldi %B[outVal],0b10000000" "\n\t"
		        "No_overflow1%=:" "\n\t"
		        // outVal = (in - filtA), or -1
		        "mov %A[tempReg],%A[filtA]" "\n\t"
		        "mov %B[tempReg],%B[filtA]" "\n\t"
		        "sub %A[tempReg],%A[filtB]" "\n\t"
		        "sbc %B[tempReg],%B[filtB]" "\n\t"
		        "brvc No_overflow3%=" "\n\t"
		        "ldi %A[tempReg],0b00000001" "\n\t"
		        "ldi %B[tempReg],0b10000000" "\n\t"
		        "No_overflow3%=:" "\n\t"
		        // tempReg = (a-b) , or -1
		        "mulsu %B[tempReg],%[filtQ]" "\n\t"
		        "movw %A[tempReg2],r0" "\n\t"
		        // tempReg2 = (HIBYTE(a-b))*Q
		        "mul %A[tempReg],%[filtQ]" "\n\t"
		        "add %A[tempReg2],r1" "\n\t"
		        "adc %B[tempReg2],%[ZERO]" "\n\t"
		        "rol r0" "\n\t"
		        "brcc No_Round1%=" "\n\t"
		        "inc %A[tempReg2]" "\n\t"
		        "No_Round1%=:" "\n\t"
		        // at this point tempReg2 = (a-b)*Q (shifted appropriately and rounded)
		        //      "clc" "\n\t"
		        "lsl %A[tempReg2]" "\n\t"
		        "rol %B[tempReg2]" "\n\t"
		        //     "clc" "\n\t"
		        "lsl %A[tempReg2]" "\n\t"
		        "rol %B[tempReg2]" "\n\t"
		        // tempReg2 = (a-b)*Q*4
		        "add %A[outVal],%A[tempReg2]" "\n\t"
		        "adc %B[outVal],%B[tempReg2]" "\n\t"
		        "brvc No_overflow4%=" "\n\t"
		        "ldi %A[outVal],0b11111111" "\n\t"
		        "ldi %B[outVal],0b01111111" "\n\t"
		        "No_overflow4%=:" "\n\t"
		        // outVal = ((in-a) + (a-b)*(Q>>8)*4) - clipped etc
		        "mulsu %B[outVal],%[filtF]" "\n\t"
		        "movw %A[tempReg],r0" "\n\t"
		        "mul %A[outVal],%[filtF]" "\n\t"
		        "add %A[tempReg],r1" "\n\t"
		        "adc %B[tempReg],%[ZERO]" "\n\t"
		        "rol r0" "\n\t"
		        "brcc No_Round2%=" "\n\t"
		        "inc %A[tempReg]" "\n\t"
		        // tempReg = f* ((in-a) + (a-b)*(Q>>8)*4)
		        "No_Round2%=:" "\n\t"
		        "add %A[filtA],%A[tempReg]" "\n\t"
		        "adc %B[filtA],%B[tempReg]" "\n\t"
		        // A= A+ f* ((in-a) + (a-b)*(Q>>8)*4)
		        "brvc No_overflow5%=" "\n\t"
		        "ldi %A[outVal],0b11111111" "\n\t"
		        "ldi %B[outVal],0b01111111" "\n\t"
		        "No_overflow5%=:" "\n\t"
		        // now calculate B= f* (a - b)

		        "mov %A[tempReg],%A[filtA]" "\n\t"
		        "mov %B[tempReg],%B[filtA]" "\n\t"
		        "sub %A[tempReg],%A[filtB]" "\n\t"
		        "sbc %B[tempReg],%B[filtB]" "\n\t"
		        "brvc No_overflow6%=" "\n\t"
		        "ldi %A[tempReg],0b00000001" "\n\t"
		        "ldi %B[tempReg],0b10000000" "\n\t"
		        "No_overflow6%=:" "\n\t"
		        // tempReg = (a-b)
		        "mulsu %B[tempReg],%[filtF]" "\n\t"
		        "movw %A[tempReg2],r0" "\n\t"
		        "mul %A[tempReg],%[filtF]" "\n\t"
		        "add %A[tempReg2],r1" "\n\t"
		        "adc %B[tempReg2],%[ZERO]" "\n\t"
		        // tempReg2 = f*(a-b)
		        "add %A[filtB],%A[tempReg2]" "\n\t"
		        "adc %B[filtB],%B[tempReg2]" "\n\t"
		        "brvc No_overflow7%=" "\n\t"
		        "ldi %A[filtB],0b11111111" "\n\t"
		        "ldi %B[filtB],0b01111111" "\n\t"
		        "No_overflow7%=:" "\n\t"
		        // now b= b+f*(a-b)
		        "mov %A[outVal],%A[filtB]" "\n\t"
		        "mov %B[outVal],%B[filtB]" "\n\t"

		        // multiply outval by 4 and clip
		        "add %A[outVal],%A[filtB]" "\n\t"
		        "adc %B[outVal],%B[filtB]" "\n\t"
		        "brbs 3, Overflow_End%=" "\n\t"

		        "add %A[outVal],%A[filtB]" "\n\t"
		        "adc %B[outVal],%B[filtB]" "\n\t"
		        "brbs 3, Overflow_End%=" "\n\t"

		        "add %A[outVal],%A[filtB]" "\n\t"
		        "adc %B[outVal],%B[filtB]" "\n\t"
		        "brbs 3, Overflow_End%=" "\n\t"
		        "rjmp No_overflow%=" "\n\t"
		        "Overflow_End%=:" "\n\t"
		        "brbs 2,Overflow_High%=" "\n\t"
		        "ldi %A[outVal],0b00000001" "\n\t"
		        "ldi %B[outVal],0b10000000" "\n\t"
		        "rjmp No_overflow%=" "\n\t"
		        "Overflow_High%=:" "\n\t"
		        "ldi %A[outVal],0b11111111" "\n\t"
		        "ldi %B[outVal],0b01111111" "\n\t"
		        "No_overflow%=:" "\n\t"

		        // TB we aren't sending it to the pwm yet so skip these lines:
		        //char valOut=((unsigned int)(outValue))>>8;
		        //valOut+=128;
		        // OCR2A=(byte)valOut;
		        //"subi %B[outVal],0x80" "\n\t"
		        //"sts 0x00b3,%B[outVal]" "\n\t"

		        // uncomment the lines below to see the register allocations
		        /*
		        "lds %A[filtA],0x01" "\n\t"
		        "lds %B[filtA],0x02" "\n\t"
		        "lds %A[filtB],0x03" "\n\t"
		        "lds %B[filtB],0x04" "\n\t"
		        "lds %[filtQ],0x05" "\n\t"
		        "lds %[filtF],0x06" "\n\t"
		        "lds %A[outVal],0x07" "\n\t"
		        "lds %B[outVal],0x08" "\n\t"
		        "lds %A[tempReg],0x09" "\n\t"
		        "lds %B[tempReg],0x0a" "\n\t"
		        "lds %A[tempReg2],0x0b" "\n\t"
		        "lds %B[tempReg2],0x0c" "\n\t"
		        "lds %[ZERO],0x0d" "\n\t"
		        */

		        // outputs / read/write arguments
        : [filtA] "+&w" (filterA),
		        [filtB] "+&w" (filterB),
		        [tempReg] "=&a" (tempReg),
		        [tempReg2] "=&d" (tempReg2)
				        :
				        [filtQ] "a" (filtQ),
				        [filtF] "a" (filtF),
				        [outVal] "a" (outValue),
				        [ZERO] "d" (zeroRegFilt)
				        // inputs
				        : "r1");


		/*
			outValue*=4;
			// at this point, outValue is a 16 bit signed version of what we want ie. 0 -> 32767, then -32768 -> -1 (0xffff)
			// we want 0->32767 to go to 32768-65535 and -32768 -> -1 to go to 0-32767, then we want only the top byte
			// take top byte, then add 128, then cast to unsigned. The (unsigned int) in the below is to avoid having to shift (ie.just takes top byte)
		  			*/
		//char valOut=((unsigned int)(outValue))>>8;
		//return highByte(outValue);
		//return (char) lowByte(((unsigned int)outValue>>8)); // unsigned to also shift the sign bit
		return highByte(outValue);
	}

private:

	volatile int outValue;
	int filterA;
	int filterB;
	unsigned char filterQ;
	unsigned char filterF;
	//unsigned char filtF;
	//unsigned char filtQ;
	//int tempReg,tempReg2;//=0;
	//unsigned char zeroRegFilt;//=0;

};

#endif /* LOWPASSFILTER_H_ */
