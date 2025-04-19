/*
 * CircularBuffer.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2014-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */

/*
Modified from https://en.wikipedia.org/wiki/Circular_buffer
Mirroring version
On 18 April 2014, the simplified version on the Wikipedia page for power of 2 sized buffers 
doesn't work - cbIsEmpty() returns true whether the buffer is full or empty.

April 2025: modified for different buffer sizes under the suggestion
of Meebleeps (https://github.com/sensorium/Mozzi/issues/281)
*/



/** Circular buffer object.  Has a fixed number of cells, set by BUFFER_SIZE.
@tparam ITEM_TYPE the kind of data to store, eg. int, int8_t etc.
@tparam BUFFER_SIZE the size of the circular buffer
*/
template <class ITEM_TYPE, int16_t BUFFER_SIZE>
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
		return (num_buffers_read << COUNT_LSHIFT) + start;
	}
        inline
	ITEM_TYPE * address() {
	  return items;
	}

private:
	ITEM_TYPE items[BUFFER_SIZE];
	uint8_t         start;  /* index of oldest itement              */
	uint8_t         end;    /* index at which to write new itement  */
	uint8_t         s_msb;
	uint8_t         e_msb;
	unsigned long num_buffers_read;
    static constexpr unsigned long COUNT_LSHIFT = 
        (BUFFER_SIZE == 256) ? 8 :
        (BUFFER_SIZE == 128) ? 7 :
        (BUFFER_SIZE == 64) ? 6 :
        (BUFFER_SIZE == 32) ? 5 :
        (BUFFER_SIZE == 16) ? 4 :
        (BUFFER_SIZE == 8) ? 3 :
        (BUFFER_SIZE == 4) ? 2 :
        (BUFFER_SIZE == 2) ? 1 : 0;

  inline
  void cbIncrStart()  {
    start++;
    if (start == BUFFER_SIZE)
      {
	start = 0;
	s_msb ^= 1;
	num_buffers_read++;
      }
  }

  inline
  void cbIncrEnd()
  {
    end++;
    if (end == BUFFER_SIZE)
      {
	end = 0;
	e_msb ^= 1;
      }
  }
  

};



/** Circular buffer object.  Specialization for size of 256.
Note: Lot of duplication but C++ does not allow for specialization of the 
function member only (partial specialization).
@tparam ITEM_TYPE the kind of data to store, eg. int, int8_t etc.
*/
template <class ITEM_TYPE>
class CircularBuffer<ITEM_TYPE, 256>
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
        inline
	ITEM_TYPE * address() {
	  return items;
	}

private:
	ITEM_TYPE items[256];
	uint8_t         start;  /* index of oldest itement              */
	uint8_t         end;    /* index at which to write new itement  */
	uint8_t         s_msb;
	uint8_t         e_msb;
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

