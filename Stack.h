/*
 * Stack.h
 *
 * Copyright 2013 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */
 
 /** A simple stack, used internally for keeping track of analog input channels as they are read.
 This stack is really just an array with a pointer to the most recent item, and memory is allocated at compile time.
@tparam  T the kind of numbers (or other things) to store.
@tparam NUM_ITEMS the maximum number of items the stack will need to hold. 
 */
template <class T, int NUM_ITEMS>
class Stack
{
private:
	T _array[NUM_ITEMS];
	int top;

public:
	/** Constructor
	*/
	Stack(): top(-1)
	{
	}
	
/** Put an item on the stack.
@param item the thing you want to put on the stack.
*/
	void push(T item)
	{
		if (top< (NUM_ITEMS-1)){
			top++;
			_array[top]=item;
		}
	}

/** Get the item on top of the stack.
@return T the item
*/
	T pop()
	{
		if(top==-1) return -1;
		T item =_array[top];
		top--;
		return item;
	}

};
