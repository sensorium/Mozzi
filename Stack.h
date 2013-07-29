/*
 * Stack.h
 *
 * Copyright 2013 Tim Barrass.
 *
 * This file is part of Mozzi.
 *
 * Mozzi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mozzi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mozzi.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
template <class T, int NUM_ITEMS>
class Stack
{
private:
	T _array[NUM_ITEMS];
	int top;

public:
	Stack(): top(-1)
	{
	}
	

	void push(T item)
	{
		if (top< (NUM_ITEMS-1)){
			top++;
			_array[top]=item;
		}
	}


	T pop()
	{
		if(top==-1) return -1;
		T item =_array[top];
		top--;
		return item;
	}

};
