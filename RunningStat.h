

// adapted from http://www.johndcook.com/standard_deviation.html


class RunningStat
{
public:
	RunningStat() : num_values(0), _mean(0), _previous_mean(0), _previous_variance(0)
	{}



	void clear() {
		num_values = 0;
	}



	void update(float x) {
		num_values++;

		// See Knuth TAOCP vol 2, 3rd edition, page 232
		_mean = _previous_mean + (x - _previous_mean)/num_values;
		_variance = _previous_variance + (x - _previous_mean)*(x - _mean);

		// set up for next iteration
		_previous_mean = _mean;
	}


	float getMean() const {
		return _mean;
	}


	float getVariance() const {
		return _variance/(num_values - 1);
	}

	
	
	float StandardDeviation() const {
		return sqrt( Variance() );
	}

	

private:
	long num_values;
	float _previous_mean, _mean, _previous_variance, _variance;
};

