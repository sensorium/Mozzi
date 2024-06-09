/*
 * MozziHeadersOnly.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Tim Barrass and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
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
