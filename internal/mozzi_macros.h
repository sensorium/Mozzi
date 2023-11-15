/** This file contains some macros used internally inside Mozzi. These are not meant to be useful in user code. */

#ifndef MOZZI_MACROS_H
#define MOZZI_MACROS_H


// internal dummy that should be distinct from any valid config value
#define MOZZI__INVALID_CONFIG_VALUE 9999976543

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

/** Compile time check to complain if the given argument is not a power of two */
#define MOZZI_CHECK_POW2(X) static_assert((X & (X - 1)) == 0, #X " must be a power of two");

/** Simply a way to check if X equal Y, realiably (at compile time). The advantage of this macro is that it will produce an error, in case one or both are defined empty
 * (such as because of a programmer's typo)
 *
 * TODO: Gahh, this doesn't work. I thought it did.
 *
 * Example: @code
 * #if MOZZI_IS(MOZZI_AUDIO_MODE, MOZZI_OUTPUT_PWM)
 * [...]
 * #endif
 * @endcode
 */
#define MOZZI_IS(X, Y) (X == Y)

/** Short-hand for a compile time complaint, if the given define does not have the expected value.
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
#define MOZZI_ASSERT_EQUAL(X, Y) static_assert(X == Y, "Internal error in #if-statement: " #X " != " #Y ".");

/** See MOZZI_ASSERT_EQUAL, but reversed */
#define MOZZI_ASSERT_NOTEQUAL(X, Y) static_assert(X != Y, "Internal error in #if-statement: " #X " == " #Y ".");

#endif
