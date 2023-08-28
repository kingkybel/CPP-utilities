/*
 * File Name:   contants.h
 * Description: some constants
 *
 * Copyright (C) 2019 Dieter J Kybelksties
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @date: 2019-05-13
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_CONTANTS_H_INCLUDED
#define NS_UTIL_CONTANTS_H_INCLUDED

#include <cstdint>

// Pi~31415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
const constexpr int64_t MICRO_RAD_PI      = 3'141'592LL;
const constexpr int64_t MICRO_RAD_2PI_MIN = 0LL;
const constexpr int64_t MICRO_RAD_2PI_MAX = MICRO_RAD_PI * 2LL;

const constexpr int64_t NANO_RAD_PI  = 3'141'592'653LL;
const constexpr int64_t NANO_RAD_2PI = NANO_RAD_PI * 2LL;

const constexpr int64_t DEGREE_180_MAX = 180LL;
const constexpr int64_t DEGREE_180_MIN = -179LL;

const constexpr int64_t DEGREE_360_MIN = 0LL;
const constexpr int64_t DEGREE_360_MAX = 359LL;

const constexpr int64_t FACTOR_M_TO_MICRO_METER = 1'000'000LL;
const constexpr int64_t FACTOR_M_TO_MM          = 1'000LL;
const constexpr int64_t TWO_MILLION_MM_MIN      = 0LL;
const constexpr int64_t TWO_MILLION_MM_MAX      = 2'000'000LL;

constexpr int64_t powTwo[] = {
 1LL << 0LL,  1LL << 1LL,  1LL << 2LL,  1LL << 3LL,  1LL << 4LL,  1LL << 5LL,  1LL << 6LL,  1LL << 7LL,
 1LL << 8LL,  1LL << 9LL,  1LL << 10LL, 1LL << 11LL, 1LL << 12LL, 1LL << 13LL, 1LL << 14LL, 1LL << 15LL,
 1LL << 16LL, 1LL << 17LL, 1LL << 18LL, 1LL << 19LL, 1LL << 20LL, 1LL << 21LL, 1LL << 22LL, 1LL << 23LL,
 1LL << 24LL, 1LL << 25LL, 1LL << 26LL, 1LL << 27LL, 1LL << 28LL, 1LL << 29LL, 1LL << 30LL, 1LL << 31LL,
 1LL << 32LL, 1LL << 33LL, 1LL << 34LL, 1LL << 35LL, 1LL << 36LL, 1LL << 37LL, 1LL << 38LL, 1LL << 39LL,
 1LL << 40LL, 1LL << 41LL, 1LL << 42LL, 1LL << 43LL, 1LL << 44LL, 1LL << 45LL, 1LL << 46LL, 1LL << 47LL,
 1LL << 48LL, 1LL << 49LL, 1LL << 50LL, 1LL << 51LL, 1LL << 52LL, 1LL << 53LL, 1LL << 54LL, 1LL << 55LL,
 1LL << 56LL, 1LL << 57LL, 1LL << 58LL, 1LL << 59LL, 1LL << 60LL, 1LL << 61LL, 1LL << 62LL, 1LL << 63LL};

#endif  // NS_UTIL_CONTANTS_H_INCLUDED
