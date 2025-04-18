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

// TODO: remove this define from here, put a default value in config
#define MOZZI_OUTPUT_BUFFER_SIZE 256

// This is to get the correct cound for audioticks() (there might be a smarter way...)
#if (MOZZI_OUTPUT_BUFFER_SIZE == 256)
#define COUNT_LSHIFT 8
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 128)
#define COUNT_LSHIFT 7
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 64)
#define COUNT_LSHIFT 6
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 32)
#define COUNT_LSHIFT 5
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 16)
#define COUNT_LSHIFT 4
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 8)
#define COUNT_LSHIFT 3
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 4)
#define COUNT_LSHIFT 2
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 2)
#define COUNT_LSHIFT 1
#elif (MOZZI_OUTPUT_BUFFER_SIZE == 1)
#define COUNT_LSHIFT 0
#endif



/** Circular buffer object.  Has a fixed number of cells, set to 256.
@tparam ITEM_TYPE the kind of data to store, eg. int, int8_t etc.
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
		return (num_buffers_read << COUNT_LSHIFT) + start;
	}
        inline
	ITEM_TYPE * address() {
	  return items;
	}

private:
	ITEM_TYPE items[MOZZI_OUTPUT_BUFFER_SIZE];
	uint8_t         start;  /* index of oldest itement              */
	uint8_t         end;    /* index at which to write new itement  */
	uint8_t         s_msb;
	uint8_t         e_msb;
	unsigned long num_buffers_read;

#if (MOZZI_OUTPUT_BUFFER_SIZE == 256)
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
#else // if circular buffer length is != 256, use less efficient version for to manage start/end index buffer index
  inline
  void cbIncrStart()  {
    start++;
    if (start == MOZZI_OUTPUT_BUFFER_SIZE)
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
    if (end == MOZZI_OUTPUT_BUFFER_SIZE)
      {
	end = 0;
	e_msb ^= 1;
      }
  }
#endif
  

};


#undef COUNT_LSHIFT // avoid macro spil
