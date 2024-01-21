/*
 * MozziHeadersOnly.h
 *
 * Copyright 2024, Tim Barrass and the Mozzi team
 *
 * This file is part of Mozzi.
 *
 * Mozzi is licensed under a Creative Commons
 * Attribution-NonCommercial-ShareAlike 4.0 International License.
 *
 */

/** @ingroup core 
 * @file MozziHeadersOnly.h
 *
 * This file provides declarations of the \ref core Mozzi functions, but no implementation. Use this only, if you have more than one
 * translation unit in your project (i.e. you have more than one .cpp-file in your sketch itself). Otherwise include \ref Mozzi.h, instead.
 *
 * (Failure to head this advice will lead to "symbol XY undefined" errors.).
 */

#ifndef MOZZI_HEADERS_ONLY_H_
#define MOZZI_HEADERS_ONLY_H_

#define _MOZZI_HEADER_ONLY
#include "MozziGuts.h"

#endif
