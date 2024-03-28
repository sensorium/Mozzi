/*
 * Mozzi.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2012-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
 */


/** @ingroup core 
 * @file Mozzi.h
 *
 * This is the main include file in Mozzi. Almost all sketches using Mozzi will want to include this file @em exactly once.
 *
 * Should your sketch require \ref core Mozzi functions in more than one translation unit (i.e. you have more than one .cpp-file
 * in your sketch itself), only *one* of these shall include this file, while any others shall include \ref MozziHeadersOnly instead.
 * (Failing to heed this advice will lead to "duplicate definition" errors.)
 */

#ifndef MOZZI_H_
#define MOZZI_H_

#include "MozziGuts.h"

#endif
