// adapted from floating point, http://www.johndcook.com/standard_deviation.html

#ifndef RUNNINGSTAT_H_
#define RUNNINGSTAT_H_

#include "mozzi_fixmath.h"
//#include "MagicNumber.cpp"

class RunningStatFixedPointRing
{

public:
	RunningStatFixedPointRing(unsigned int buflen, int * Mbuf, int * Sbuf) : m_n(0), buf_len(buflen), oldest(0)
	{
		mAdjustArray = Mbuf;
		sAdjustArray = Sbuf;
	}

	void Clear()
	{
		m_n = 0;
		oldest = 0 ;
	}

	void Push(int x)

	{

		m_n++;

		// See Knuth TAOCP vol 2, 3rd edition, page 232
		if (m_n == 1)
		{
			m_oldM = m_newM = x;
			m_oldS = 0;
			oldest = 0;
			mAdjustArray[oldest] = 0;
			sAdjustArray[oldest] = 0;
		}
		else
		{
			// might be better to scale these up because they are small numbers around 1
			m_oldM -= mAdjustArray[oldest];
			m_oldS -= sAdjustArray[oldest];
			x = x<<4; //scale up x for fractional calculations

			//int mAdjust = (x - m_oldM)/(min(m_n,buf_len));  // use magic numbers trick here?
			int mAdjust = (x - m_oldM)/32; // divided by 32, default buf_len
			//int mAdjust = (x - m_oldM)>>1;// >>5; now scaled by 32 // divided by 32, default buf_len
			m_newM = m_oldM + mAdjust;
			//int sAdjust = (x - m_oldM)*(x - m_newM);
			int sAdjust = ((x - m_oldM)*(x - m_newM))>>4; // adjust for multiplying scaled vals
			m_newS = m_oldS + sAdjust;

			// set up for next iteration
			m_oldM = m_newM;
			m_oldS = m_newS;

			mAdjustArray[oldest] = mAdjust;
			sAdjustArray[oldest] = sAdjust;
		}

		// update ringbuf
		oldest++;
		if(oldest>=buf_len) oldest = 0;
	}


	unsigned int NumDataValues() const
	{
		return min(m_n,buf_len);
	}

	int mAdjustArrayOldest() const
	{
		return mAdjustArray[oldest];
	}

	int sAdjustArrayOldest() const
	{
		return sAdjustArray[oldest];
	}

	unsigned int BufferLength() const
	{
		return buf_len;
	}

	int Mean() const
	{
		return (m_n > 0) ? m_newM>>4 : 0;
	}

	long Variance() const
	{
		//return ( (m_n > 1) ? m_newS/(min(m_n,buf_len) - 1) : 0 );
		// but this can give negatives?

		// magicnumbers for /31
		// online calc
		// M = 2216757315   s = 4 - good but needs 32x32--64bit calc
		// arduino calc- bad
		// M = -2078209981 s = 4

		//return ( (m_n > 1) ? m_newS/31 : 0 );
		return ( (m_n > 1) ? ((long)m_newS*2114)>>20 : 0 );
		//541200.516129032 (1/31  x256x256x256)
		//2114.06451612903 (1/31  x256X256)
		//return ( (m_n > 1) ? (((long)m_newS*2216757315)>>4) : 0 );
		//return ( (m_n > 1) ? ((long)((((long long) ((long)m_newS*2216757315))>>4)>>32)) : 0 );
		//return ( (m_n > 1) ? (((long)m_newS*-2078209981)>>4) : 0 );
	}

	int StandardDeviation() const
	{
		return fastSqrt( Variance() );
	}

private:
	unsigned int m_n;
	//long m_oldM, m_newM, m_oldS, m_newS;
	int m_oldM, m_newM;
	int m_oldS, m_newS;
	unsigned int buf_len;
	int * mAdjustArray;
	int * sAdjustArray;
	unsigned int oldest;
};

#endif /* RUNNINGSTAT_H_ */
