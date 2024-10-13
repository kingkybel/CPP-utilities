/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/to_string.h
 * Description: string utility functions
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

#ifndef NS_UTIL_TO_STRING_H_INCLUDED
#define NS_UTIL_TO_STRING_H_INCLUDED

#include "decorator.h"
#include "traits.h"
// #define DO_TRACE_
#include "traceutil.h"

#include <sstream>
#include <string>

namespace util
{
/**
 * @brief Convert objects to a string, provided a ostream - &lt;&lt; operator is defined.
 *
 * @tparam Value value-type
 * @param value the value to convert
 * @return std::string a string representation of the value
 */
template <typename Value>
inline std::string toString(Value const &value)
{
    std::stringstream ss;

    util::decorate(ss, value);

    return ss.str();
}

/**
 * @brief Convert objects to a wstring, provided a ostream - &lt;&lt; operator is defined.
 *
 * @tparam Value value-type
 * @param value the value to convert
 * @return std::wstring  a wide string representation of the value
 */
template <typename Value>
inline std::wstring toWString(Value const &value)
{
    std::wstringstream ss;

    decorate(ss, value);

    return ss.str();
}
}; // namespace util

#endif // NS_UTIL_TO_STRING_H_INCLUDED
