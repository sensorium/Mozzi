/*
 * EventDelay.h
 *
 * Copyright 2012 unbackwards@gmail.com.
 *
 * This file is part of Cuttlefish.
 *
 * Cuttlefish is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cuttlefish is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cuttlefish.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "EventDelay.h"

EventDelay::EventDelay(unsigned int UPDATE_RATE): micros_per_update(1000000/UPDATE_RATE),counter(0)
{
	;
}

