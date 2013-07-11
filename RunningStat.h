

// adapted from floating point, http://www.johndcook.com/standard_deviation.html
#include "mozzi_fixmath.h"


template <class T>
class RunningStat
{
public:
	RunningStat() : num_values(0), _mean(0), _previous_mean(0), _previous_variance(0)
	{}



	void clear() {
		num_values = 0;
	}



	void update(T x) {
		num_values++;

		// See Knuth TAOCP vol 2, 3rd edition, page 232
		_mean = _previous_mean + (x - _previous_mean)/num_values;
		_variance = _previous_variance + (x - _previous_mean)*(x - _mean);

		// set up for next iteration
		_previous_mean = _mean;
	}


	T getMean() const {
		return _mean;
	}


	T getVariance() const {
		return ( (num_values > 1) ? _variance/(num_values - 1) : 0.0 );
	}

	
	
	T StandardDeviation() const {
		return sqrt( Variance() );
	}

	

private:
	long num_values;
	T _previous_mean, _mean, _previous_variance, _variance;
};


/** Q7n8 specialisation of RunningStat template*/
template <>
class RunningStat <Q7n8>
{
public:
	RunningStat() : num_values(0), _mean(0), _previous_mean(0), _previous_variance(0)
	{}



	void clear() {
		num_values = 0;
	}



	void update(Q7n8 x) {
		num_values++;

		// See Knuth TAOCP vol 2, 3rd edition, page 232
		// (x - _previous_mean)/num_values will end up as 0 for fixed point before long
		_mean = _previous_mean + (x - _previous_mean)/num_values;
		_variance = _previous_variance + (x - _previous_mean)*(x - _mean);

		// set up for next iteration
		_previous_mean = _mean;
	}


	Q7n8 getMean() const {
		return _mean;
	}


	Q7n8 getVariance() const {
		// _variance/(num_values - 1) will end up as 0 for fixed point before long
		return ( (num_values > 1) ? _variance/(num_values - 1) : 0.0 );
	}


	Q7n8 StandardDeviation() const {
		return Q7n8_sqrt( getVariance() );
	}

	

private:
	long num_values;
	Q7n8 _previous_mean, _mean, _previous_variance, _variance;
};
