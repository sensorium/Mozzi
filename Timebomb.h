#ifndef TIMEBOMB_H_
#define TIMEBOMB_H_

#include "Arduino.h"

class Timebomb {

public:
	Timebomb(unsigned int update_rate);
	
	inline
	void set(unsigned int millisToWait) {
  		howManyChecks = ((long)millisToWait*1000)/microsPerCheck;
	}
	
	inline
	void store(unsigned int millisToWait) {
  		storedHowManyChecks = ((long)millisToWait*1000)/microsPerCheck;
	}
	
	inline
	void reset() { howManyChecks = storedHowManyChecks; }
	
	inline
	boolean boom() { return --howManyChecks<0; }
	
private:
	long howManyChecks; // long so even at a control rate of 2048 you can have >15seconds
	long storedHowManyChecks;
	const unsigned int microsPerCheck;

};

#endif /* TIMEBOMB_H_ */