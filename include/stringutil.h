/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/stringutil.h
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

#ifndef NS_UTIL_STRINGUTIL_H_INCLUDED
#define NS_UTIL_STRINGUTIL_H_INCLUDED

#include "to_string.h"
// #define DO_TRACE_
#include "traceutil.h"

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <deque>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace util
{
/**
 * @brief Mutually exclusive enumeration of number-types
 */
enum class NumberClass : int8_t
{
    NONE,  ///< This is not a number (it is a free man!)
    INT,   ///< A signed integer
    UINT,  ///< An unsigned integer
    FLOAT  ///< A floating point number
};

/**
 * @brief Trim-mode enumeration, binary operation possible
 */
enum StripTrimMode : int8_t
{
    FRONT   = 0x01,                  ///< Strip or trim the left-hand-side
    LEFT    = FRONT,                 ///< Strip or trim the left-hand-side
    INSIDE  = 0x02,                  ///< Strip or trim occurrences inside the string
    BACK    = 0x04,                  ///< Strip or trim the right-hand-side
    RIGHT   = BACK,                  ///< Strip or trim the right-hand-side
    OUTSIDE = FRONT | BACK,          ///< Strip or trim the left-hand-side and right-hand-side
    ALL     = FRONT | INSIDE | BACK  ///< Strip or trim all occurences in the string
};

/**
 * @brief Character traits for case-insensitive string type.
 * Inherits all the functions that we don't need to override for
 * case-insensitivity.
 *
 * @tparam CharT_ char-type
 */

/**
 * @brief Create an all-lower-case copy of the given string.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @param str original string
 * @return StringT_ all-lower copy of the string
 */
template<typename StringT_, typename std::enable_if<util::is_std_string<StringT_>::value>::type * = nullptr>
inline StringT_ toLower(StringT_ str)
{
    typedef typename util::is_std_string<StringT_>::char_type char_type;
    auto                                                      reval = StringT_{};
    for(auto iter = str.begin(); iter != str.end(); ++iter)
    {
        auto C = (static_cast<char_type>(towlower(static_cast<wchar_t>(*iter))));
        reval += C;
    }

    return (reval);
}

/**
 * @brief Create an all-upper-case copy of the given string.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @param str original string
 * @return std::basic_string<CharT_, TraitsT_> all-upper copy of the string
 */
template<typename StringT_, typename std::enable_if<util::is_std_string<StringT_>::value>::type * = nullptr>
inline StringT_ toUpper(const StringT_ &str)
{
    typedef typename util::is_std_string<StringT_>::char_type char_type;
    auto                                                      reval = StringT_{};
    for(auto iter = str.begin(); iter != str.end(); ++iter)
    {
        auto C = (static_cast<char_type>(towupper(wchar_t{*iter})));
        reval += C;
    }

    return (reval);
}

// void toUpper(std::string& str, std::string localeStr)
// {
//     //unicode to wide string converter
//     std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

//     //convert to wstring (because std::toupper is not implemented on all platforms for u32string)
//     std::wstring wide = converter.from_bytes(str);

//     std::locale locale;

//     try
//     {
//         locale = std::locale(localeStr);
//     }
//     catch(const std::exception&)
//     {
//         std::cerr << "locale not supported by system: " << localeStr << " (" << getLocaleByLanguage(localeStr) << ")"
//         << std::endl;
//     }

//     auto& f = std::use_facet<std::ctype<wchar_t>>(locale);

//     f.toupper(&wide[0], &wide[0] + wide.size());

//     //convert back
//     str = converter.to_bytes(wide);
// }

/**
 * @brief Strip left and/or right and/or interior of string.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam ConstStringT_ string-type, can be string-like (char[], ...)
 * @param str original string
 * @param stripChars characters to strip
 * @param mode strip mode
 */
template<typename StringT_,
         typename ConstStringT_,
         typename util::is_compatible_string<StringT_, ConstStringT_>::type * = nullptr>
void strip(StringT_ &str, const ConstStringT_ &stripChars = ConstStringT_{0}, StripTrimMode mode = StripTrimMode::ALL)
{
    if(str.empty())
        return;

    bool doStripFront  = ((mode & StripTrimMode::FRONT) == StripTrimMode::FRONT);
    bool doStripBack   = ((mode & StripTrimMode::BACK) == StripTrimMode::BACK);
    bool doStripInside = ((mode & StripTrimMode::INSIDE) == StripTrimMode::INSIDE);

    StringT_                     reval  = "";
    typename StringT_::size_type start  = doStripFront ? str.find_first_not_of(stripChars) : 0;
    typename StringT_::size_type finish = doStripBack ? str.find_last_not_of(stripChars) + 1 : str.size();

    if(start == StringT_::npos)
    {
        str = "";
        return;
    }

    StringT_ stringStripChars{stripChars};
    if(stringStripChars.empty())
        stringStripChars = util::convert<StringT_>(std::string{"\t \r\n"});

    while(start < finish)
    {
        if(!doStripInside)  // if we did not specify to strip inside-matches
            reval.append(&str[start], 1);
        else if(stringStripChars.find(str[start]) == StringT_::npos)
            reval.append(&str[start], 1);

        start++;
    }

    str = reval;
}

/**
 * @brief Trim any of a given set of characters from either end of the given string.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam ConstStringT_ string-type, can be string-like (char[], ...)
 * @param str original string
 * @param trimChars characters to trim
 */
template<typename StringT_,
         typename ConstStringT_,
         typename util::is_compatible_string<StringT_, ConstStringT_>::type * = nullptr>
void trim(StringT_ &str, const ConstStringT_ &trimChars)
{
    strip(str, trimChars, StripTrimMode::OUTSIDE);
}

/**
 * @brief Trim only left any of a given set of characters from either end of the given string.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam ConstStringT_ string-type, can be string-like (char[], ...)
 * @param str original string
 * @param trimChars characters to trim
 */
template<typename StringT_,
         typename ConstStringT_,
         typename util::is_compatible_string<StringT_, ConstStringT_>::type * = nullptr>
inline void trimLeft(StringT_ &str, const ConstStringT_ &trimChars = ConstStringT_{0})
{
    return (strip(str, trimChars, StripTrimMode::LEFT));
}

/**
 * @brief Trim only right any of a given set of characters from either end of the given string.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam ConstStringT_ string-type, can be string-like (char[], ...)
 * @param str original string
 * @param trimChars characters to trim
 */
template<typename StringT_,
         typename ConstStringT_,
         typename util::is_compatible_string<StringT_, ConstStringT_>::type * = nullptr>
inline void trimRight(StringT_ &str, const ConstStringT_ &trimChars = ConstStringT_{0})
{
    return (strip(str, trimChars, StripTrimMode::RIGHT));
}

/**
 * @brief Replace occurrences of chars left and/or right  and/or interior of
 * standard string with replacement char.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam ConstStringT_ string-type, can be string-like (char[], ...)
 * @tparam CharT_ char-type
 * @param str original string
 * @param replChars characters to replace
 * @param repl replacement char
 * @param mode trim-mode
 */
template<typename StringT_,
         typename ConstStringT_,
         typename CharT_,
         typename util::is_compatible_string<StringT_, ConstStringT_>::type * = nullptr>
void replaceChar(StringT_            &str,
                 const ConstStringT_ &replChars   = ConstStringT_{0},
                 CharT_               replaceWith = util::charToChar<CharT_>(char{' '}),
                 StripTrimMode        mode        = StripTrimMode::ALL)
{
    if(str.empty())
        return;

    auto stringReplChars = StringT_{replChars};
    if(stringReplChars.empty())
        stringReplChars = util::convert<StringT_>(std::string{"\n\t \r"});
    if(str.find_first_not_of(stringReplChars) == StringT_::npos)
    {
        str = StringT_(str.size(), replaceWith);
        return;
    }

    size_t firstNonReplChar = 0;

    while(firstNonReplChar < str.size() && stringReplChars.find(str[firstNonReplChar]) != StringT_::npos)
    {
        if((mode & StripTrimMode::FRONT) == StripTrimMode::FRONT)
        {
            str[firstNonReplChar] = replaceWith;
        }

        firstNonReplChar++;
    }

    size_t lastNonReplChar = str.size() - 1;

    while(stringReplChars.find(str[lastNonReplChar]) != StringT_::npos)
    {
        if((mode & StripTrimMode::BACK) == StripTrimMode::BACK)
        {
            str[lastNonReplChar] = replaceWith;
        }
        lastNonReplChar--;
    }

    size_t insideNonReplChar = firstNonReplChar;

    while(insideNonReplChar < lastNonReplChar + 1)
    {
        if((mode & StripTrimMode::INSIDE) == StripTrimMode::INSIDE)
        {
            if(stringReplChars.find(str[insideNonReplChar]) != StringT_::npos)
                str[insideNonReplChar] = replaceWith;
        }

        insideNonReplChar++;
    }
}

/**
 * @brief Replace only left occurrences of replChars.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam ConstStringT_ string-type, can be string-like (char[], ...)
 * @tparam CharT_ char-type
 * @param str original string
 * @param replChars characters to replace
 * @param replaceWith replacement char
 */
template<typename StringT_,
         typename ConstStringT_,
         typename CharT_,
         typename util::is_compatible_string<StringT_, ConstStringT_>::type * = nullptr>
inline void replaceCharLeft(StringT_            &str,
                            const ConstStringT_ &replChars   = ConstStringT_{0},
                            CharT_               replaceWith = util::charToChar<CharT_>(char{' '}))
{
    replaceChar(str, replChars, replaceWith, StripTrimMode::LEFT);
}

/**
 * @brief Replace only right occurrences of replChars.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam ConstStringT_ string-type, can be string-like (char[], ...)
 * @tparam CharT_ char-type
 * @param str original string
 * @param replChars characters to replace
 * @param replaceWith replacement char
 */
template<typename StringT_,
         typename ConstStringT_,
         typename util::is_compatible_string<StringT_, ConstStringT_>::type * = nullptr>
inline void replaceCharRight(StringT_ &str, const StringT_ &stripChars = "\t \r\n", char repl = ' ')
{
    replaceChar(str, stripChars, repl, StripTrimMode::RIGHT);
}

/**
 * @brief Try to scan a string representation of a boolean by most common values
 * like True, on/Off/...
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @param strVal string to parse
 * @param result result of the scan
 * @return true, if the string successfully parsed into true or false, false otherwise
 */
template<typename StringT_, typename std::enable_if<util::is_std_string<StringT_>::value>::type * = nullptr>
bool scanBoolString(const StringT_ &strVal, bool &result)
{
    const static std::map<std::string, bool> VALID_BOOL = {{"true", true},
                                                           {"t", true},
                                                           {"yes", true},
                                                           {"y", true},
                                                           {"1", true},
                                                           {"on", true},
                                                           {"false", false},
                                                           {"f", false},
                                                           {"no", false},
                                                           {"n", false},
                                                           {"0", false},
                                                           {"off", false}};

    auto lower = util::convert<std::string>(util::toLower(strVal));
    auto found = VALID_BOOL.find(lower);

    result = found != VALID_BOOL.end() ? found->second : false;

    return (found != VALID_BOOL.end());
}

template<typename StringT_, typename std::enable_if<util::is_std_string<StringT_>::value>::type * = nullptr>
StringT_ substr_from_to_incl(const StringT_ &str, size_t start, size_t finish)
{
    if(start > static_cast<size_t>(str.size()) || start > finish)
        return StringT_{};
    if(finish > static_cast<size_t>(str.size()))
        finish = StringT_::npos;
    else
        finish = finish - start + 1;
    return str.substr(start, finish);
}

/**
 * @brief  Split a string into a vector of strings using a char sep as separator.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam SeparatorT_ separator type, needs to be eithe CharT_ or string
 * @param str original string
 * @param sep separator character
 * @return std::vector<std::basic_string<CharT_, TraitsT_>> a vector containing the separated sub-strings
 */
template<typename StringT_,
         typename SeparatorT_,
         typename std::enable_if<util::has_std_string_compatible_char<StringT_, SeparatorT_>::value>::type * = nullptr>
std::vector<StringT_> splitIntoVector(const StringT_ &str, SeparatorT_ sep)
{
    std::vector<StringT_>        results;
    typename StringT_::size_type subStrStart = 0UL;
    typename StringT_::size_type sepStart    = str.find(sep);
    typename StringT_::size_type sepLen      = util::string_or_char_size(sep);

    if(sepLen == 0 || sepStart > str.size())
    {
        // separator not found or empty so add the whole string to results and return
        results.emplace_back(str);
        return results;
    }

    bool finished = false;
    while(!finished)
    {
        if(sepStart == subStrStart)
        {
            // we have an empty string at the beginning, or 2 separators are touching
            results.emplace_back(StringT_{});
        }
        else
        {
            results.emplace_back(substr_from_to_incl(str, subStrStart, sepStart - 1));
        }

        // start looking for another separator after this one
        subStrStart = sepStart + sepLen;
        sepStart    = str.find(sep, subStrStart);
        if(sepStart > str.size())
        {
            finished = true;
            results.emplace_back(substr_from_to_incl(str, subStrStart, sepStart - 1));
        }
    }

    return (results);
}

/**
 * @brief  Split a string into a set of strings using a char sep as separator.
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @tparam SeparatorT_ separator type, needs to be eithe CharT_ or string
 * @param str original string
 * @param sep separator character
 * @return std::set<StringT_> a set containing the separated sub-strings
 */
template<typename StringT_,
         typename SeparatorT_,
         typename std::enable_if<util::has_std_string_compatible_char<StringT_, SeparatorT_>::value>::type * = nullptr>
std::set<StringT_> splitIntoSet(const StringT_ &str, SeparatorT_ sep)
{
    std::set<StringT_>           results;
    typename StringT_::size_type subStrStart = 0UL;
    typename StringT_::size_type sepStart    = str.find(sep);
    typename StringT_::size_type sepLen      = util::string_or_char_size(sep);

    if(sepLen == 0 || sepStart > str.size())
    {
        // separator not found or empty so add the whole string to results and return
        results.emplace(str);
        return results;
    }

    bool finished = false;
    while(!finished)
    {
        if(sepStart == subStrStart)
        {
            // we have an empty string at the beginning, or 2 separators are touching
            results.emplace(StringT_{});
        }
        else
        {
            results.emplace(substr_from_to_incl(str, subStrStart, sepStart - 1));
        }

        // start looking for another separator after this one
        subStrStart = sepStart + sepLen;
        sepStart    = str.find(sep, subStrStart);
        if(sepStart > str.size())
        {
            finished = true;
            results.emplace(substr_from_to_incl(str, subStrStart, sepStart - 1));
        }
    }

    return (results);
}

/**
 * @brief Classify a string into one of the classes NONE, INT, UINT, FLOAT.
 * invalid strings have class NONE
 * integral types INT or UINT
 * and valid floating point strings are FLOAT
 * this function is overridden to allow for case-(in-)sensitive strings
 *
 * @tparam StringT_ string-type, must be std::basic_string<...>
 * @param str the string to classify
 * @return NumberClass INT, UINT or FLOAT if the string scans as integer, unsigned or float respectively, NONE otherwise
 */
template<typename StringT_, typename std::enable_if<util::is_string<StringT_>::value>::type * = nullptr>
NumberClass classifyNumberString(const StringT_ &str)
{
    static size_t maxIntLen  = toString(std::numeric_limits<int64_t>::max()).size();
    static size_t maxUintLen = toString(std::numeric_limits<uint64_t>::max()).size();

    if(str.empty() || str.find_first_not_of("0123456789+-.eElL") != StringT_::npos)
        return (NumberClass::NONE);

    if(str.find_first_of(".e") != StringT_::npos)
        return (NumberClass::FLOAT);

    bool isNegative = (str[0] == '-');
    bool isSigned   = (str[0] == '-' || str[0] == '+');

    if(str.size() - (isSigned ? 1 : 0) > maxUintLen)
        return (NumberClass::FLOAT);
    else
    {
        if(str.size() - (isSigned ? 1 : 0) >= maxIntLen)
            return (isNegative ? NumberClass::FLOAT : NumberClass::UINT);
        else
            return (NumberClass::INT);
    }
}

};  // namespace util

#endif  // NS_UTIL_STRINGUTIL_H_INCLUDED
