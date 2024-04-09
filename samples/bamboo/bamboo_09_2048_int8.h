#ifndef BAMBOO_09_2048_H_
#define BAMBOO_09_2048_H_

#include <Arduino.h>
#include "mozzi_pgmspace.h"

#define BAMBOO_09_2048_NUM_CELLS 2048
#define BAMBOO_09_2048_SAMPLERATE 16384

CONSTTABLE_STORAGE(int8_t) BAMBOO_09_2048_DATA []  = {1, 0, 0, 0, -1, 1, -1, 1, 1, -1, 0,
                -3, -14, -44, -89, -91, 6, 125, 121, 31, -29, -40, -22, -25, -39, -16, -9, -4,
                51, 72, 55, 41, 0, -32, -33, -64, -91, -35, 24, 21, 18, 50, 72, 45, 7, -1, -19,
                -77, -90, -31, -7, -1, 33, 43, 54, 71, 42, -5, -30, -45, -62, -60, -41, -7, 16,
                36, 79, 86, 28, -1, 8, -31, -76, -71, -55, -33, 19, 45, 43, 55, 59, 40, 12, -28,
                -64, -75, -60, -23, -2, -2, 30, 77, 70, 38, 25, -5, -48, -58, -60, -59, -35, 0,
                33, 59, 65, 55, 39, 9, -25, -55, -77, -68, -35, -15, 9, 47, 61, 54, 48, 24, -19,
                -55, -65, -54, -30, -12, 1, 21, 47, 60, 56, 28, -3, -24, -43, -47, -34, -33,
                -19, 14, 34, 40, 47, 38, 19, -1, -30, -51, -49, -36, -16, 3, 17, 32, 44, 43, 30,
                7, -27, -40, -33, -32, -28, -7, 11, 26, 41, 41, 30, 11, -11, -26, -28, -33, -28,
                -15, 0, 17, 31, 31, 28, 22, 3, -14, -22, -33, -37, -23, -5, 9, 19, 28, 31, 27,
                10, -10, -26, -33, -30, -21, -12, 0, 12, 24, 32, 29, 11, -7, -13, -21, -28, -23,
                -13, -4, 8, 17, 22, 25, 18, 7, -6, -20, -28, -26, -20, -9, 2, 10, 19, 27, 23, 8,
                -6, -16, -26, -25, -16, -7, 0, 7, 16, 27, 24, 13, -1, -11, -18, -17, -15, -12,
                -3, 6, 14, 21, 21, 13, 3, -6, -13, -19, -20, -13, -3, 5, 7, 13, 16, 14, 9, 3,
                -9, -17, -18, -15, -8, -1, 3, 12, 19, 20, 12, 4, -8, -19, -21, -16, -7, -2, 4,
                14, 18, 14, 9, 3, -5, -12, -18, -17, -12, -6, 0, 7, 12, 14, 11, 5, 0, -9, -17,
                -18, -12, -6, 0, 5, 11, 15, 13, 7, 0, -6, -12, -16, -14, -8, -2, 3, 12, 16, 14,
                7, 0, -5, -11, -16, -15, -10, -2, 4, 9, 11, 11, 9, 3, -4, -10, -15, -15, -8, -1,
                2, 6, 11, 14, 12, 8, 0, -10, -15, -15, -10, -4, 0, 5, 10, 13, 11, 7, 0, -7, -13,
                -17, -14, -6, 1, 6, 10, 13, 10, 6, 2, -5, -12, -17, -12, -6, -1, 4, 8, 11, 11,
                9, 4, -4, -10, -14, -13, -9, -2, 1, 5, 11, 13, 8, 4, 0, -5, -12, -14, -10, -6,
                0, 6, 9, 11, 11, 7, 2, -4, -12, -16, -13, -8, -1, 3, 8, 12, 14, 9, 3, -4, -12,
                -14, -11, -8, -4, 0, 6, 12, 14, 10, 5, -1, -8, -11, -12, -14, -10, 0, 8, 12, 13,
                10, 4, 0, -4, -9, -14, -14, -10, -5, 3, 10, 12, 11, 10, 6, -3, -10, -13, -14,
                -11, -4, 3, 7, 9, 11, 9, 5, 1, -4, -8, -12, -13, -10, -4, 3, 9, 11, 12, 10, 3,
                -2, -7, -13, -15, -12, -5, 3, 7, 10, 12, 10, 6, 0, -7, -10, -12, -13, -9, -2, 5,
                9, 11, 11, 8, 4, -1, -6, -12, -15, -11, -6, 1, 5, 10, 11, 10, 7, 2, -4, -11,
                -13, -14, -11, -3, 4, 8, 11, 12, 9, 3, -3, -9, -14, -14, -11, -6, -1, 5, 10, 12,
                10, 7, 2, -5, -11, -13, -12, -8, -2, 4, 7, 10, 12, 8, 4, -1, -6, -12, -12, -11,
                -6, 0, 6, 10, 10, 10, 7, 0, -4, -8, -12, -13, -8, -1, 2, 7, 11, 10, 7, 2, -2,
                -6, -10, -10, -9, -5, 0, 4, 7, 9, 8, 4, 0, -4, -8, -11, -10, -6, -3, 2, 6, 9, 9,
                6, 2, -1, -5, -10, -10, -7, -3, 1, 4, 7, 9, 6, 3, 1, -3, -8, -9, -7, -4, -2, 1,
                6, 7, 7, 4, 2, -3, -5, -8, -8, -5, -3, 1, 4, 7, 7, 5, 3, 0, -4, -7, -7, -6, -3,
                0, 3, 6, 6, 6, 3, 0, -3, -6, -7, -6, -5, -3, 1, 4, 5, 5, 4, 1, -2, -5, -6, -6,
                -5, -3, 0, 3, 3, 4, 5, 2, 0, -4, -6, -7, -6, -4, -1, 1, 3, 5, 5, 3, 1, -3, -5,
                -6, -5, -4, -2, 1, 3, 5, 4, 3, 1, -2, -4, -6, -6, -6, -2, 0, 2, 3, 4, 4, 2, -1,
                -2, -4, -6, -5, -3, -1, 1, 3, 4, 4, 3, 1, -2, -4, -5, -4, -4, -2, -1, 2, 3, 5,
                5, 3, 0, -3, -5, -5, -4, -3, -1, 2, 4, 4, 3, 3, 1, -2, -4, -5, -5, -3, -1, 1, 2,
                3, 3, 2, 1, -1, -3, -5, -5, -4, -2, 0, 2, 2, 3, 4, 2, 0, -2, -4, -5, -4, -3, -1,
                1, 2, 4, 3, 2, 1, -2, -4, -4, -5, -4, -2, 1, 2, 3, 3, 3, 1, -1, -3, -4, -5, -3,
                -2, -1, 1, 2, 3, 2, 2, 0, -2, -3, -3, -4, -2, -2, 0, 1, 3, 2, 2, 1, -1, -2, -3,
                -4, -3, -2, -1, 1, 2, 3, 2, 1, 0, -2, -3, -3, -3, -2, 0, 1, 2, 2, 2, 1, 0, -1,
                -2, -3, -3, -2, -2, 0, 1, 1, 2, 2, 0, 0, -2, -2, -3, -3, -2, 0, 0, 1, 2, 2, 1,
                1, -1, -2, -3, -3, -2, 0, 0, 1, 2, 2, 1, 0, -1, -2, -3, -3, -2, -2, -1, 1, 2, 2,
                1, 1, 0, -1, -2, -3, -3, -2, -1, 0, 2, 2, 2, 1, 1, -1, -2, -3, -3, -2, -1, -1,
                1, 2, 2, 2, 0, -1, -2, -3, -3, -3, -2, -1, 1, 2, 2, 2, 1, 0, -2, -3, -3, -3, -3,
                0, 1, 1, 2, 2, 1, 0, 0, -2, -3, -3, -3, -2, -1, 1, 2, 2, 2, 1, 0, -2, -3, -3,
                -2, -2, -1, 1, 1, 2, 2, 1, 0, -1, -2, -3, -3, -2, -1, 0, 1, 2, 2, 1, 1, -1, -1,
                -2, -3, -2, -2, -1, 1, 1, 2, 2, 1, -1, -2, -2, -3, -2, -2, -1, 0, 1, 1, 2, 0, 0,
                -1, -2, -2, -2, -2, -1, 0, 0, 1, 1, 0, 0, 0, -1, -2, -2, -2, -2, -1, 0, 1, 1, 0,
                1, 0, -1, -2, -2, -2, -2, -1, 0, 0, 0, 1, 0, 0, -1, -1, -2, -2, -2, -1, 0, 0, 0,
                1, 1, 0, 0, -1, -1, -2, -2, -2, -1, 0, 0, 1, 0, 1, 0, -1, -1, -1, -1, -1, -1,
                -1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1,
                -1, -1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, 0, -1,
                -1, -1, -1, -1, 0, 0, 0, 1, 0, 0, 0, -1, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, -1,
                -1, -1, -2, -1, -1, -1, 0, 0, 0, 0, 0, -1, -1, -1, -2, -1, -1, -1, 0, 0, 0, 0,
                0, 0, -1, -1, -2, -1, -1, -1, -1, 0, 0, 1, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0,
                0, 1, 0, 0, 0, -1, -1, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0,
                0, 0, 0, 0, 0, 0, 0, 0, -1, -2, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -2, -1,
                -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, 0, -1, 0, 0, 0, 0, -1, -1, -1, -1,
                -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, -1, 0, 0, 0, -1, -1, -1,
                -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0,
                -1, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0,
                0, 0, -1, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, 0,
                0, 0, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, 0,
                -1, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1,
                -1, -1, 0, 0, 0, 0, -1, 0, -1, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1,
                -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, -1,
                -1, -1, -1, -1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, -1, 0, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, -1,
                0, -1, -1, -1, -1, -1, 0, 0, -1, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, -1, 0, 0,
                0, 0, 0, -1, 0, 0, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, -1, -1, 0, -1,
                0, 0, 0, -1, 0, 0, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, -1, -1, -1, 0, -1,
                0, 0, 0, 0, 0, 0, 0, 0, -1, -1, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, -1, 0,
                0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, -1, 0, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0,
                -1, 0, 0, -1, 0, 0, 0, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0,
                -1, 0, -1, -1, 0, -1, 0, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, 0, -1, 0, 0, 0,
                -1, 0, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, -1, 0, 0, -1, 0, 0, 0, 0,
                -1, 0, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, -1, 0, -1, 0, 0,
                -1, 0, 0, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 0, 0, -1, -1, -1, -1, 0, 0, 0,
                0, -1, 0, -1, 0, 0, -1, -1, -1, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, -1, 0, 0, 0,
                -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, -1, 0, -1, -1, -1, 0, 0, 0, 0, -1, -1, -1, -1,
                -1, -1, 0, 0, 0, 0, -1, 0, -1, 0, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, -1,
                -1, -1, -1, 0, -1, 0, 0, -1, 0, -1, -1, -1, 0, -1, -1, 0, 0, 0, 0, 0, 0, -1, 0,
                -1, 0, -1, -1, 0, -1, 0, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, 0, -1, 0, -1, 0,
                -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, 0, -1, 0, -1, 0, -1, 0, -1, 0, 0, 0, 0, 0,
                0, 0, 0, -1, -1, -1, 0, 0, -1, 0, -1, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1,
                0, 0, -1, 0, -1, -1, -1, -1, -1, -1, 0, 0, -1, 0, 0, 0, 0, -1, 0, -1, -1, 0, 0,
                -1, 0, -1, 0, -1, 0, -1, -1, -1, 0, -1, 0, 0, -1, 0, 0, 0, -1, 0, -1, 0, -1, -1,
                0, -1, 0, 0, -1, 0, -1, 0, -1, -1, -1, -1, -1, 0, -1, 0, 0, 0, 0, 0, 0, -1, 0,
                0, -1, 0, -1, 0, 0, 0, 0, 0, -1, -1, -1, 0, -1, -1, -1, 0, 0, -1, 0, -1, 0, -1,
                0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, -1, 0,
                -1, 0, -1, 0, -1, 0, -1, 0, -1, 0, 0, 0, -1, 0, -1, 0, 0, 0, -1, 0, -1, 0, -1,
                0, 0, -1, 0, -1, 0, 0, 0, };

#endif /* BAMBOO_09_2048_H_ */
