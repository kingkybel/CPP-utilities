/*
 * File Name:   stream_decorate.h
 * Description: Streaming utilities for writing to streams.
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

#ifndef STREAM_DECORATE_H_INCLUDED
#define STREAM_DECORATE_H_INCLUDED

#include "decorator.h"

#include <algorithm>
#include <deque>
#include <iosfwd>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace util
{
template<typename StringT1_, typename StringT2_ = std::string>
StringT1_ convert(const StringT2_ &from)
{
    StringT1_ to;
    using CharT1_ = std::remove_cvref_t<decltype(to.at(0))>;
    using CharT2_ = std::remove_cvref_t<decltype(from.at(0))>;

    static_assert(std::is_integral_v<CharT1_>, "Type CharT1_ must be integral.");
    static_assert(std::is_integral_v<CharT2_>, "Type CharT2_ must be integral.");

    std::transform(from.begin(),
                   from.end(),
                   std::back_inserter(to),
                   [](const auto &c) { return (static_cast<CharT2_>(c)); });

    return (to);
}

/**
 * Convert objects to a string, provided a ostream - &lt;&lt; operator is defined.
 */
template<typename T_>
inline std::string toString(const T_ &v);

/**
 * Convert objects to a wstring, provided a ostream - &lt;&lt; operator is defined.
 */
template<typename T_>
inline std::wstring toWString(const T_ &v);

/**
 * Default decoration: just stream as is with no further adjustment.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename Value_, typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const Value_ &value)
{
    os << value;
}

/**
 * Decorator for boolean values. Apart from configured brackets also can be as 0/1 or true/false.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const bool &value)
{
    auto                    decoConfig = util::decorator<CharT_, Traits_>::instance();
    auto                    bracket    = decoConfig.get(value, util::decorator<CharT_, Traits_>::KEY_CHAR);
    auto                    alpha      = decoConfig.getBoolAlpha();
    std::ios_base::fmtflags f(os.flags());

    if(alpha != util::decorator<CharT_, Traits_>::intFmt::ignore)
    {
        if(alpha == util::decorator<CharT_, Traits_>::intFmt::is_set)
            os << std::boolalpha;
        else
            os << std::noboolalpha;
    }

    os << bracket.left() << value << bracket.right();
    os.flags(f);
}

/**
 * Decorator for integer typed values. Apart from configured brackets also can be streamed as hex/oct/dec.
 *
 * @param os output stream
 * @param key key to get the appropriate brackets
 * @param value the value to decorate
 */
template<typename IntT_, typename CharT_, typename Traits_>
void decorateInt(std::basic_ostream<CharT_, Traits_> &os, const std::basic_string<CharT_, Traits_> &key, IntT_ &value)
{
    auto                    decoConfig = util::decorator<CharT_, Traits_>::instance();
    auto                    bracket    = decoConfig.template getBracket<IntT_>(key);
    auto                    fmt        = decoConfig.template getIntFmt<IntT_>();
    std::ios_base::fmtflags f(os.flags());

    if(fmt != util::decorator<CharT_, Traits_>::intFmt::ignore)
    {
        if(fmt == util::decorator<CharT_, Traits_>::intFmt::hexadecimal)
            os << std::hex;
        else if(fmt == util::decorator<CharT_, Traits_>::intFmt::decimal)
            os << std::dec;
        else if(fmt == util::decorator<CharT_, Traits_>::intFmt::octal)
            os << std::oct;
    }

    os << bracket.left() << value << bracket.right();
    os.flags(f);
}

/**
 * Decorator specialised for 8-bit chars.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const char &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_CHAR, value);
}

/**
 * Decorator specialised for 32-bit chars.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const wchar_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_CHAR, value);
}

/**
 * Decorator specialised for 8-bit ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int8_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator specialised for 16-bit ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int16_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator specialised for 32-bit ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int32_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator specialised for 64-bit ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int64_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator specialised for 8-bit unsigned ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint8_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator specialised for 16-bit unsigned ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint16_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator specialised for 32-bit unsigned ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint32_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator specialised for 64-bit unsigned ints.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint64_t &value)
{
    decorateInt(os, util::decorator<CharT_, Traits_>::KEY_INT, value);
}

/**
 * Decorator for float typed values. Apart from configured brackets also floating point format can be configured.
 *
 * @param os output stream
 * @param key key to get the appropriate brackets
 * @param value the value to decorate
 */
template<typename FloatT_, typename CharT_, typename Traits_>
void decorateFloat(std::basic_ostream<CharT_, Traits_>      &os,
                   const std::basic_string<CharT_, Traits_> &key,
                   FloatT_                                  &value)
{
    auto                    decoConfig = util::decorator<CharT_, Traits_>::instance();
    auto                    bracket    = decoConfig.template getBracket<FloatT_>(key);
    auto                    fmt        = decoConfig.template getFloatFmt<FloatT_>();
    std::ios_base::fmtflags f(os.flags());

    if(fmt.isValid())
        os << fmt;

    os << bracket.left() << value << bracket.right();
    os.flags(f);
}

/**
 * Decorator specialised for floats.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const float &value)
{
    decorateFloat(os, util::decorator<CharT_, Traits_>::KEY_FLOAT, value);
}

/**
 * Decorator specialised for doubles.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const double &value)
{
    decorateFloat(os, util::decorator<CharT_, Traits_>::KEY_FLOAT, value);
}

/**
 * Decorator specialised for long doubles.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const long double &value)
{
    decorateFloat(os, util::decorator<CharT_, Traits_>::KEY_FLOAT, value);
}

/**
 * Decorator specialised for std::strings.
 *
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const std::basic_string<CharT_, Traits_> &value)
{
    auto decoConfig = util::decorator<CharT_, Traits_>::instance();
    auto bracket =
     decoConfig.template getBracket<std::basic_string<CharT_, Traits_>>(util::decorator<CharT_, Traits_>::KEY_STRING);

    os << bracket.left() << value << bracket.right();
}

/**
 * Iterate through a container and pushing each elements on the given output stream enclosing in left and right
 * brackets and separated by an inner separator.
 *
 * @param os output stream
 * @param container the forward iterable container
 * @param defaultBracketId  an ID to select a bracket if no special bracket is defined for the container
 */
template<typename ContainerType_, typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_>      &os,
              ContainerType_                            container,
              const std::basic_string<CharT_, Traits_> &defaultBracketId)
{
    auto decoConfig = util::decorator<CharT_, Traits_>::instance();
    auto bracket    = decoConfig.template getBracket<ContainerType_>(defaultBracketId);

    os << bracket.left();

    if(!container.empty())
    {
        auto iter = container.begin();

        while(iter != container.end())
        {
            decorate(os, *iter);
            iter++;

            if(iter != container.end())
                os << bracket.inner();
        }
    }

    os << bracket.right();
}

/**
 * Generic ostream - &lt;&lt; operator for vectors.
 *
 * @param os the output-stream
 * @param vec the vector to stream
 *
 * @return the modified stream
 */
template<typename T_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::vector<T_, Alloc_>       &vec)
{
    decorate(os, vec, util::decorator<CharT_, Traits_>::KEY_VECTOR);

    return (os);
}

/**
 * Generic ostream - &lt;&lt; operator for double ended queues.
 *
 * @param os the output-stream
 * @param dblEndQueue the deque to stream
 *
 * @return the modified stream
 */
template<typename T_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::deque<T_, Alloc_>        &dblEndQueue)
{
    decorate(os, dblEndQueue, util::decorator<CharT_, Traits_>::KEY_DEQUE);

    return (os);
}

/**
 * Generic ostream - &lt;&lt; operator for sets.
 *
 * @param os the output-stream
 * @param sortedSet the set to stream
 *
 * @return the modified stream
 */
template<typename Key, typename Compare, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::set<Key, Compare, Alloc> &sortedSet)
{
    decorate(os, sortedSet, util::decorator<CharT_, Traits_>::KEY_SET);

    return (os);
}

/**
 * Generic ostream - &lt;&lt; operator for unordered sets.
 *
 * @param os the output-stream
 * @param unordSet the unordered_set to stream
 *
 * @return the modified stream
 */
template<typename Value, typename Hash, typename Pred, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_>                &os,
                                                       const std::unordered_set<Value, Hash, Pred, Alloc> &unordSet)
{
    decorate(os, unordSet, util::decorator<CharT_, Traits_>::KEY_UNORDERED_SET);

    return (os);
}

/**
 * Generic ostream - &lt;&lt; operator for pairs.
 *
 * @param os the output-stream
 * @param pair1st2nd the pair to stream
 *
 * @return the modified stream
 */
template<typename T1_, typename T2_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::pair<T1_, T2_>           &pair1st2nd)
{
    using Pair_ = std::pair<T1_, T2_>;

    auto decoConfig = util::decorator<CharT_, Traits_>::instance();
    auto bracket    = decoConfig.template getBracket<Pair_>(decoConfig.KEY_PAIR);

    os << bracket.left();
    decorate(os, pair1st2nd.first);
    os << bracket.inner();
    decorate(os, pair1st2nd.second);
    os << bracket.right();

    return (os);
}

/**
 * Generic ostream - &lt;&lt; operator for unordered maps.
 *
 * @param os the output-stream
 * @param unordMap the unordered_map to stream
 *
 * @return the modified stream
 */
template<typename Key, typename Value, typename Hash, typename Pred, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &
 operator<<(std::basic_ostream<CharT_, Traits_> &os, const std::unordered_map<Key, Value, Hash, Pred, Alloc> &unordMap)
{
    decorate(os, unordMap, util::decorator<CharT_, Traits_>::KEY_UNORDERED_MAP);

    return (os);
}

/**
 * Generic ostream - &lt;&lt; operator for standard (ordered) maps.
 *
 * @param os the output-stream
 * @param sortedMap the map to stream
 *
 * @return the modified stream
 */
template<typename T1_, typename T2_, typename Compare_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_>        &os,
                                                       const std::map<T1_, T2_, Compare_, Alloc_> &sortedMap)
{
    decorate(os, sortedMap, util::decorator<CharT_, Traits_>::KEY_MAP);

    return (os);
}

template<typename StringT_, typename T_>
inline std::string as(const T_ &v)
{
    std::stringstream ss;

    decorate(ss, v);

    return (ss.str());
}

/**
 * Convert objects to a string, provided a ostream - &lt;&lt; operator is defined.
 */
template<typename T_>
inline std::string toString(const T_ &v)
{
    std::stringstream ss;

    decorate(ss, v);

    return (ss.str());
}

/**
 * Convert objects to a wstring, provided a ostream - &lt;&lt; operator is defined.
 */
template<typename T_>
inline std::wstring toWString(const T_ &v)
{
    std::wstringstream ss;

    decorate(ss, v);

    return (ss.str());
}

};  // namespace util

#endif  // STREAM_DECORATE_H_INCLUDED
