/*
 * mozzi_macros.h
 *
 * This file is part of Mozzi.
 *
 * Copyright 2023-2024 Thomas Friedrichsmeier and the Mozzi Team
 *
 * Mozzi is licensed under the GNU Lesser General Public Licence (LGPL) Version 2.1 or later.
 *
*/

/** This file contains some macros used internally inside Mozzi. These are not meant to be useful in user code. */

#ifndef MOZZI_MACROS_H
#define MOZZI_MACROS_H


// internal dummy that should be distinct from any valid config value
#define MOZZI__INVALID_CONFIG_VALUE 9999976543210

// internal implementation of MOZZI_CHECK_SUPPORTED
#define MOZZI__CHECK_SUPPORTED(X, M, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, ...) \
     static_assert(X != MOZZI__INVALID_CONFIG_VALUE && \
                   (X == A0 || X == A1 || X == A2 || X == A3 || X == A4 || X == A5 || X == A6 || X == A7 || X == A8 || X == A9 || \
                    X == B0 || X == B1 || X == B2 || X == B3 || X == B4 || X == B5 || X == B6 || X == B7 || X == B8 || X == B9), "Compile time option " M " does not support value " #X " on this platform.");

// MSVC needs this indirection for proper __VA_ARGS__ expansion. I case we ever need to support MSVC...
#define MOZZI__MACRO_EVAL(...) __VA_ARGS__

/** @file mozzi_internal_macros
 * compile time check whether the given first value (usually specified as a #define) is among the values specified in subsequent args (up to 20)
 *
 * Example: @code MOZZI_CHECK_SUPPORTED(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_EXTERNAL) @endcode
*/
#define MOZZI_CHECK_SUPPORTED(X, ...) MOZZI__MACRO_EVAL(MOZZI__CHECK_SUPPORTED(X, #X, __VA_ARGS__, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE))

/** @file mozzi_internal_macros
 * Compile time check to complain if the given argument is not a power of two */
#define MOZZI_CHECK_POW2(X) static_assert((X & (X - 1)) == 0, #X " must be a power of two");

#define MOZZI__IS(X, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, B0, B1, B2, B3, B4, B5, B6, B7, B8, B9, ...) \
  ((X == A0) || (X == A1) || (X == A2) || (X == A3) || (X == A4) || (X == A5) || (X == A6) || (X == A7) || (X == A8) || (X == A9) || (X == B0) || (X == B1) || (X == B2) || (X == B3) || (X == B4) || (X == B5) || (X == B6) || (X == B7) || (X == B8) || (X == B9))

/** @file mozzi_internal_macros
 * Short-hand to check if given first value is any of the following values (up to 20).
 *
 * (Orgignally, this macro was intended to also produce an error, should any of the values be non-defined (such as because it's a typo), but alas, the preprocessor would
 * let me have that).
 *
 * Example: @code
 * #if MOZZI_IS_ANY(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM, MOZZI_OUTPUT_2PIN_PWM, MOZZI_OUTPUT_EXTERNAL_TIMED)
 * [...]
 * #endif
 * @endcode
 *
 * See also @ref MOZZI_CHECK_SUPPORTED, which throws an error, if the first value is not among the latter values.
 */
#define MOZZI_IS(X, ...) MOZZI__MACRO_EVAL(MOZZI__IS(X, __VA_ARGS__, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, \
     MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE, MOZZI__INVALID_CONFIG_VALUE))

/** @file mozzi_internal_macros
 * Short-hand for a compile time complaint, if the given define does not have the expected value.
 *
 * Use this to check - and clarify - complex nested logic inside #fidefs.
 *
 * Example: @code
 * #if MOZZI_IS(MOZZI_ANALOG_READ, MOZZI_ANALOG_STANDARD)
 * [long difficult to read logic, with further nested #if's]]
 * #else
 * MOZZI_ASSERT_EQUAL(MOZZI_ANALOG_READ, MOZZI_ANALOG_NONE)
 * [more complex logic]
 * #endif
 * @endcode
 */
#define MOZZI_ASSERT_EQUAL(X, Y) static_assert(X == Y, "Internal error in preprocessor logic: " #X " != " #Y ".");

/** @file mozzi_internal_macros
 * See MOZZI_ASSERT_EQUAL, but reversed */
#define MOZZI_ASSERT_NOTEQUAL(X, Y) static_assert(X != Y, "Internal error in preprocessor logic: " #X " == " #Y ".");


#if __cplusplus >= 201402L
/** @file mozzi_internal_macros
 * Document that a function has been deprecated, and when, if possible giving the user an explanatory message */
#define MOZZI_DEPRECATED(WHEN, WHY) [[deprecated(WHY)]]
#elif defined(__GNUC__) && __has_cpp_attribute(deprecated)
#define MOZZI_DEPRECATED(WHEN, WHY) [[deprecated(WHY)]]
#elif defined(__GNUC__) || defined(__clang__)
#define MOZZI_DEPRECATED(WHEN, WHY) __attribute__((deprecated))
#elif defined(_MSC_VER)
#define MOZZI_DEPRECATED(WHEN, WHY) __declspec(deprecated)
#else
#define MOZZI_DEPRECATED(WHEN, WHY)
#endif

/** @file mozzi_internal_macros
 * Document that a function is not implemented on this platform */
#define MOZZI_UNIMPLEMENTED() MOZZI_DEPRECATED("n/a", "This feature is not implemented on this platform.")

#endif
