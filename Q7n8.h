#ifndef Q7n8_H_
#define Q7n8_H_


// types
#define Q7n8 int
//#define Q0n16 unsigned int


// macros to save runtime calculations
#define Q7n8_FIX1 256
//#define Q0n16_FIX1 65535


// Type conversions:
#define Q7n8_int2fix(a)   (((int)(a))<<8)            //Convert char to fix. a is a char
#define Q7n8_fix2int(a)   ((signed char)((a)>>8))    //Convert fix to char. a is an int
#define Q7n8_float2fix(a) ((int)((a)*256.f))         //Convert float to fix. a is a float
#define Q7n8_fix2float(a) ((float)(a)/256.f)         //Convert fix to float. a is an int

//#define Q7n8_keepFixedInRange(a) (a>>8) // shift into range after multiplying

// Fast fixed point multiply
#define Q7n8_multfix(a,b)          	  \
({                                \
int prod, val1=a, val2=b ;        \
__asm__ __volatile__ (            \
	"muls %B1, %B2	\n\t"         \
	"mov %B0, r0 \n\t"	         \
	"mul %A1, %A2\n\t"	         \
	"mov %A0, r1   \n\t"         \
	"mulsu %B1, %A2	\n\t"         \
	"add %A0, r0  \n\t"           \
	"adc %B0, r1 \n\t"           \
	"mulsu %B2, %A1	\n\t"         \
	"add %A0, r0 \n\t"           \
	"adc %B0, r1  \n\t"          \
	"clr r1  \n\t" 		         \
	: "=&d" (prod)               \
	: "a" (val1), "a" (val2)      \
	    );                        \
    prod;                        \
})

// based on:
/*
#define FMULS8(v1, v2)                \
({                                    \
    uint8_t res;                      \
    uint8_t val1 = v1;                \
    uint8_t val2 = v2;                \
    __asm__ __volatile__              \
    (                                 \
        "fmuls $1, $2"         "\n\t" \
        "mov $0, r1"           "\n\t" \
        "clr r1"               "\n\t" \
        : "=&d" (res)                 \
        : "a" (val1), "a" (val2)      \
    );                                \
    res;                              \
}) */
/*
int divfix(int nn, int dd)
begin
    int x, d ;
    signed char count, neg ;
    count = 0;
    neg = 0 ;
    d = dd ;

    // only works with + numbers
    if (d & 0x8000)
    begin
        neg = 1;
        d = -d ;
    end

    // range reduction
    while (d>0x0100)
    begin
        --count ;
        d >>= 1 ;
    end

    while (d<0x0080)
    begin
        ++count ;
        d <<= 1 ;
    end

    // Newton interation
    x = 0x02ea - (d<<1) ;
    x = multfix(x, 0x0200-multfix(d,x));
    //x = multfix(x, 0x0200-multfix(d,x));


    // range expansion
    if (count>0)  x = x<<count ;
    else if (count<0) x = x>>(-count) ;

    // fix sign
    if (neg==1) x=-x;

    //form ratio
    x = multfix(x,nn) ;

    return x ;
end

//========================================================
int sqrtfix(int aa)
begin

    int a;
    char nextbit, ahigh;
    int root, p ;
    a = aa;
    ahigh = a>>8 ;
    //
    // range sort to get integer part and to
    // check for weird bits near the top of the range
    if (ahigh >= 0x40)     //bigger than 64?
    begin
        if (a > 0x7e8f)    //>=126.562 = 11.25^2
        begin
            root = 0x0b40;  // 11
            nextbit = 0x10 ;
        end
        else if (ahigh >= 0x79)    //>=121
        begin
            root = 0x0b00;  // 11
            nextbit = 0x40 ;
        end
        else if (ahigh >= 0x64)    //>=100
        begin
            root = 0x0a00;  // 10
            nextbit = 0x80 ;
        end
        else if (ahigh >= 0x51)    //>=81
        begin
            root = 0x0900;  // 9
            nextbit = 0x80 ;
        end
        else   //64
        begin
            root = 0x0800;      //8
            nextbit = 0x80 ;
        end
    end
    else if  (ahigh >= 0x10)  //16    //smaller than 64 and bigger then 16
    begin
        if (ahigh >= 0x31)  //49
        begin
            root = 0x0700;      //7
            nextbit = 0x80 ;
        end
        else if (ahigh >= 0x24)  //36
        begin
            root = 0x0600;      //6
            nextbit = 0x80 ;
        end
        else if (ahigh >= 0x19)  //25
        begin
            root = 0x0500;      //5
            nextbit = 0x80 ;
        end
        else //16
        begin
            root = 0x0400;      //4
            nextbit = 0x80 ;
        end
    end
    else       //smaller than 16
    begin
         if (ahigh >= 0x09)  //9
        begin
            root = 0x0300;      //3
            nextbit = 0x80 ;
        end
        else if (ahigh >= 0x04)  //4
        begin
            root = 0x0200;      //2
            nextbit = 0x80 ;
        end
        else if (ahigh >= 0x01)  //1
        begin
            root = 0x0100;      //1
            nextbit = 0x80 ;
        end
        else     //less than one
        begin
            root = 0;
            nextbit = 0x80 ;
        end
    end
    // now get the low order bits
    while (nextbit)
    begin
		root = nextbit + root;
		p =  multfix(root,root);
        if (p >= a) root = root - nextbit ;
    	nextbit = nextbit>>1 ;
    end
    return root ;
end
*/
/*
//from octosynth:

    // multiply 2 16 bit numbers together and shift 8 without precision loss
    // requires assembler really
    volatile unsigned char zeroReg=0;
    volatile unsigned int multipliedCounter=oscillators[c].phaseStep;
    asm volatile
    (
      // high bytes mult together = high  byte
      "ldi %A[outVal],0" "\n\t"
      "mul %B[phaseStep],%B[pitchBend]" "\n\t"
      "mov %B[outVal],r0" "\n\t"
      // ignore overflow into r1 (should never overflow)
      // low byte * high byte -> both bytes
      "mul %A[phaseStep],%B[pitchBend]" "\n\t"
      "add %A[outVal],r0" "\n\t"
      // carry into high byte
      "adc %B[outVal],r1" "\n\t"
      // high byte* low byte -> both bytes
      "mul %B[phaseStep],%A[pitchBend]" "\n\t"
      "add %A[outVal],r0" "\n\t"
      // carry into high byte
      "adc %B[outVal],r1" "\n\t"
      // low byte * low byte -> round
      "mul %A[phaseStep],%A[pitchBend]" "\n\t"
      // the adc below is to round up based on high bit of low*low:
      "adc %A[outVal],r1" "\n\t"
      "adc %B[outVal],%[ZERO]" "\n\t"
      "clr r1" "\n\t"
      :[outVal] "=&d" (multipliedCounter)
      :[phaseStep] "d" (oscillators[c].phaseStep),[pitchBend] "d"( pitchBendMultiplier),[ZERO] "d" (zeroReg)
      :"r1","r0"
      );
    oscillators[c].phaseStep=multipliedCounter;

    */
#endif /* Q7n8_H_ */