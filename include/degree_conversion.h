/*
 * File Name:   degree_conversion.h
 * Description: Integer values that can only assume angle values, [0,360],
 *              [-180, 180] degrees, or [0, 2Pi] micro-radians.
 * 
 * Copyright (C) 2023 Dieter J Kybelksties <github@kybelksties.com>
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

#ifndef DEGREE_CONVERSION_H_INCLUDED
#define DEGREE_CONVERSION_H_INCLUDED

#include "constants.h"
#include "limited_int.h"

#include <exception>
#include <iostream>
#include <limits>
#include <sstream>

namespace util
{
using Deg180Traits =
 limited_int_traits<int64_t, DEGREE_180_MIN, DEGREE_180_MAX, resolve_modulo, convert_circular_scale>;
using Deg180 = limited_int<int64_t, DEGREE_180_MIN, DEGREE_180_MAX, Deg180Traits>;

using Deg360Traits =
 limited_int_traits<int64_t, DEGREE_360_MIN, DEGREE_360_MAX, resolve_modulo, convert_circular_scale>;
using Deg360 = limited_int<int64_t, DEGREE_360_MIN, DEGREE_360_MAX, Deg360Traits>;

using Rad2PiTraits =
 limited_int_traits<int64_t, MICRO_RAD_MIN, MICRO_RAD_2PI, resolve_modulo, convert_circular_scale>;
using Rad2Pi = limited_int<int64_t, 0, MICRO_RAD_2PI, Rad2PiTraits>;

using MilliMTraits = limited_int_traits<int64_t, -FACTOR_METER_TO_MILLI_METER, FACTOR_METER_TO_MILLI_METER, resolve_invalid, convert_scale>;
using MilliM       = limited_int<int64_t, -FACTOR_METER_TO_MILLI_METER, FACTOR_METER_TO_MILLI_METER, MilliMTraits>;

using MicroMTraits =
 limited_int_traits<int64_t, -FACTOR_METER_TO_MICRO_METER, FACTOR_METER_TO_MICRO_METER, resolve_invalid, convert_scale>;
using MicroM = limited_int<int64_t, -FACTOR_METER_TO_MICRO_METER, FACTOR_METER_TO_MICRO_METER, MicroMTraits>;

using MilliM2MillionTraits =
 limited_int_traits<int64_t, TWO_MILLION_MM_MIN, TWO_MILLION_MM_MAX, resolve_invalid, convert_scale>;
using MilliM2Million = limited_int<int64_t, TWO_MILLION_MM_MIN, TWO_MILLION_MM_MAX, MilliM2MillionTraits>;

};
// namespace util
#endif  // DEGREE_CONVERSION_H_INCLUDED
