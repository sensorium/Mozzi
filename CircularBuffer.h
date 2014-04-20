/*
Modified from https://en.wikipedia.org/wiki/Circular_buffer
Mirroring version
On 18 April 2014, the simplified version on the Wikipedia page for power of 2 sized buffers 
doesn't work - cbIsEmpty() returns true whether the buffer is full or empty.
*/

/** Circular buffer object.  Has a fixed number of cells, set to 256.
@tparam ITEM_TYPE the kind of data to store, eg. int, char etc.
*/
template <class ITEM_TYPE>
class CircularBuffer
{

public:
	/** Constructor
	*/
	CircularBuffer(): start(0),end(0),s_msb(0),e_msb(0)
	{
	}

	inline
	bool isFull() {
		return end == start && e_msb != s_msb;
	}

	inline
	bool isEmpty() {
		return end == start && e_msb == s_msb;
	}

	inline
	void write(ITEM_TYPE in) {
		items[end] = in;
		//if (isFull()) cbIncrStart(); /* full, overwrite moves start pointer */
		cbIncrEnd();
	}

	inline
	ITEM_TYPE read() {
		ITEM_TYPE out = items[start];
		cbIncrStart();
		return out;
	}

	inline
	unsigned long count() {
		return (num_buffers_read << 8) + start;
	}

private:
	ITEM_TYPE items[256];
	unsigned char         start;  /* index of oldest itement              */
	unsigned char         end;    /* index at which to write new itement  */
	unsigned char         s_msb;
	unsigned char         e_msb;
	unsigned long num_buffers_read;


	inline
	void cbIncrStart() {
		start++;
		if (start == 0) {
			s_msb ^= 1;
			num_buffers_read++;
		}
	}

	inline
	void cbIncrEnd() {
		end++;
		if (end == 0) e_msb ^= 1;
	}

};
