/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/contants.h
 * Description: some constants
 *
 * Copyright (C) 2023 Dieter J Kybelksties
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
 * @date: 2023-08-28
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_CONTANTS_H_INCLUDED
#define NS_UTIL_CONTANTS_H_INCLUDED

#include <cstdint>

namespace util
{
// Pi~31415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
constexpr int64_t const MICRO_RAD_MIN = 0LL;
constexpr int64_t const MICRO_RAD_PI  = 3'141'592LL;
constexpr int64_t const MICRO_RAD_2PI = MICRO_RAD_PI << 2LL;

constexpr int64_t const NANO_RAD_MIN = 0LL;
constexpr int64_t const NANO_RAD_PI  = 3'141'592'653LL;
constexpr int64_t const NANO_RAD_2PI = NANO_RAD_PI << 2LL;

constexpr int64_t const DEGREE_180_MAX = 180LL;
constexpr int64_t const DEGREE_180_MIN = -179LL;

constexpr int64_t const DEGREE_360_MIN = 0LL;
constexpr int64_t const DEGREE_360_MAX = 359LL;

constexpr int64_t const FACTOR_METER_TO_MICRO_METER = 1'000'000LL;
constexpr int64_t const FACTOR_METER_TO_MILLI_METER = 1'000LL;
constexpr int64_t const TWO_MILLION_MM_MIN          = 0LL;
constexpr int64_t const TWO_MILLION_MM_MAX          = 2'000'000LL;

constexpr int64_t powTwo[] = {1LL << 0LL,  1LL << 1LL,  1LL << 2LL,  1LL << 3LL,  1LL << 4LL,  1LL << 5LL,  1LL << 6LL,
                              1LL << 7LL,  1LL << 8LL,  1LL << 9LL,  1LL << 10LL, 1LL << 11LL, 1LL << 12LL, 1LL << 13LL,
                              1LL << 14LL, 1LL << 15LL, 1LL << 16LL, 1LL << 17LL, 1LL << 18LL, 1LL << 19LL, 1LL << 20LL,
                              1LL << 21LL, 1LL << 22LL, 1LL << 23LL, 1LL << 24LL, 1LL << 25LL, 1LL << 26LL, 1LL << 27LL,
                              1LL << 28LL, 1LL << 29LL, 1LL << 30LL, 1LL << 31LL, 1LL << 32LL, 1LL << 33LL, 1LL << 34LL,
                              1LL << 35LL, 1LL << 36LL, 1LL << 37LL, 1LL << 38LL, 1LL << 39LL, 1LL << 40LL, 1LL << 41LL,
                              1LL << 42LL, 1LL << 43LL, 1LL << 44LL, 1LL << 45LL, 1LL << 46LL, 1LL << 47LL, 1LL << 48LL,
                              1LL << 49LL, 1LL << 50LL, 1LL << 51LL, 1LL << 52LL, 1LL << 53LL, 1LL << 54LL, 1LL << 55LL,
                              1LL << 56LL, 1LL << 57LL, 1LL << 58LL, 1LL << 59LL, 1LL << 60LL, 1LL << 61LL, 1LL << 62LL,
                              1LL << 63LL};

/*
template<uint64_t n>
uint64_t constexpr nth_fibo()
{
    return nth_fibo<n - 1>() + nth_fibo<n - 2>();
}

template<>
uint64_t constexpr nth_fibo<0ULL>()
{
    return 1ULL;
}

template<>
uint64_t constexpr nth_fibo<1ULL>()
{
    return 1ULL;
}

// clang-format off
constexpr uint64_t fibonacci[] = {
 nth_fibo<0ULL>(), nth_fibo<1ULL>(), nth_fibo<2ULL>(), nth_fibo<3ULL>(), nth_fibo<4ULL>(),
 nth_fibo<5ULL>(), nth_fibo<6ULL>(), nth_fibo<7ULL>(), nth_fibo<8ULL>(), nth_fibo<9ULL>(),
 nth_fibo<10ULL>(), nth_fibo<11ULL>(), nth_fibo<12ULL>(), nth_fibo<13ULL>(), nth_fibo<14ULL>(),
 nth_fibo<15ULL>(), nth_fibo<16ULL>(), nth_fibo<17ULL>(), nth_fibo<18ULL>(), nth_fibo<19ULL>(),
 nth_fibo<20ULL>(), nth_fibo<21ULL>(), nth_fibo<22ULL>(), nth_fibo<23ULL>(), nth_fibo<24ULL>(),
 nth_fibo<25ULL>(), nth_fibo<26ULL>(), nth_fibo<27ULL>(), nth_fibo<28ULL>(), nth_fibo<29ULL>(),
 nth_fibo<30ULL>(), nth_fibo<31ULL>(), nth_fibo<32ULL>(), nth_fibo<33ULL>(), nth_fibo<34ULL>(),
 nth_fibo<35ULL>(), nth_fibo<36ULL>(), nth_fibo<37ULL>(), nth_fibo<38ULL>(), nth_fibo<39ULL>(),
 nth_fibo<40ULL>(), nth_fibo<41ULL>(), nth_fibo<42ULL>(), nth_fibo<43ULL>(), nth_fibo<44ULL>(),
 nth_fibo<45ULL>(), nth_fibo<46ULL>(), nth_fibo<47ULL>(), nth_fibo<48ULL>(), nth_fibo<49ULL>(),
 nth_fibo<50ULL>(), nth_fibo<51ULL>(), nth_fibo<52ULL>(), nth_fibo<53ULL>(), nth_fibo<54ULL>(),
 nth_fibo<55ULL>(), nth_fibo<56ULL>(), nth_fibo<57ULL>(), nth_fibo<58ULL>(), nth_fibo<59ULL>(),
 nth_fibo<60ULL>(), nth_fibo<61ULL>(), nth_fibo<62ULL>(), nth_fibo<63ULL>(), nth_fibo<64ULL>(),
 nth_fibo<65ULL>(), nth_fibo<66ULL>(), nth_fibo<67ULL>(), nth_fibo<68ULL>(), nth_fibo<69ULL>()
};
// clang-format on
*/
}; // namespace util

#endif // NS_UTIL_CONTANTS_H_INCLUDED
