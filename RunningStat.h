#ifndef RUNNINGSTAT_H
#define RUNNINGSTAT_H



// adapted from http://www.johndcook.com/standard_deviation.html

template <class T>
class RunningStat
{
public:
	RunningStat() : num_values(0), _previous_mean(0), _mean(0), _previous_sigma(0), _sigma(0)
	{}



	void clear() {
		num_values = 0;
	}



	void update(T x) {
		num_values++;
		
		T _delta_mean = x - _previous_mean;
		// See Knuth TAOCP vol 2, 3rd edition, page 232
		_mean = _previous_mean + _delta_mean/num_values;
		_sigma = _previous_sigma + _delta_mean*(x - _mean);

		_variance = _sigma/(num_values - 1);
		
		// set up for next iteration
		_previous_mean = _mean;
	}


	T getMean() const {
		return _mean;
	}


	T getVariance() const {
		return _variance;
	}

	
	
	T getStandardDeviation() const {
		return sqrt(_variance);
	}

	

private:
	unsigned long num_values;
	T _previous_mean, _mean, _previous_sigma, _sigma, _variance;
};

#endif        //  #ifndef RUNNINGSTAT_H

