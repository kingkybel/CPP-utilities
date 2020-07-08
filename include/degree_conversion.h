/*
 * File Name:   degree_conversion.h
 * Description: Integer values that can only assume angle values, [0,360],
 *              [-180, 180] degrees, or [0, 2Pi] micro-radians.
 * Copyright (C) 2020 Dieter J Kybelksties
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
 * @date: 2020-06-02
 * @author: Dieter J Kybelksties
 */

#ifndef DEGREE_CONVERSION_H_INCLUDED
#define DEGREE_CONVERSION_H_INCLUDED

#include <iostream>
#include <limits>
#include <exception>
#include <sstream>
#include "constants.h"

namespace util
{

typedef limited_int_traits<int64_t, -179, 180, resolve_modulo, convert_circular_scale> Deg180Traits;
typedef limited_int<int64_t, -179, 180, Deg180Traits> Deg180;

typedef limited_int_traits<int64_t, 0, 359, resolve_modulo, convert_circular_scale> Deg360Traits;
typedef limited_int<int64_t, 0, 359, Deg360Traits> Deg360;

typedef limited_int_traits<int64_t, 0, MICRO_RAD_2PI, resolve_modulo, convert_circular_scale> Rad2PiTraits;
typedef limited_int<int64_t, 0, MICRO_RAD_2PI, Rad2PiTraits> Rad2Pi;

typedef limited_int_traits<int64_t, -1'000'000, 1'000'000, resolve_invalid, convert_scale> MilliMTraits;
typedef limited_int<int64_t, -1'000'000, 1'000'000, MilliMTraits> MilliM;

typedef limited_int_traits<int64_t, -1'000'000'000, 1'000'000'000, resolve_invalid, convert_scale> MicroMTraits;
typedef limited_int<int64_t, -1'000'000'000, 1'000'000'000, MicroMTraits> MicroM;

typedef limited_int_traits<int64_t, 0, 2'000'000, resolve_invalid, convert_scale> MilliM2MillionTraits;
typedef limited_int<int64_t, 0, 2'000'000, MilliM2MillionTraits> MilliM2Million;

}; // namespace util
#endif // DEGREE_CONVERSION_H_INCLUDED

