#ifndef TimerTwo_h
#define TimerTwo_h

namespace TimerTwo {
  extern unsigned period_;
  // timer reset flag
  extern bool reset_;
  // user function
	extern void (*f_)();
  // call f every usec microseconds if reset == false
  // call f after delay of usec microseconds from call return if reset == true
  // max delay is 256*1024 clock cycles or 16,384 microseconds for a 16 MHz CPU
	unsigned char init(unsigned usec, void (*f)(), bool reset = false);
	// period in usec
  inline unsigned period() {return period_;}
	// start calls
	void start();
	// stop calls
	void stop();
}
#endif // TimerTwo
