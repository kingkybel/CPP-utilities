/*
 * File Name:   ci_string.h
 * Description: Case-insensitive string definition
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

#ifndef NS_UTIL_CI_STRING_H_INCLUDED
#define NS_UTIL_CI_STRING_H_INCLUDED

#include "stringutil.h"
#include "to_string.h"
#include "traits.h"
// #define DO_TRACE_
#include "traceutil.h"

namespace util
{

template<typename CharT_ = char>
struct ci_char_traits : public std::char_traits<CharT_>
{
    /**
     * @brief Equality of two characters ignoring their case.
     *
     * @param c1 first character
     * @param c2 second character
     * @return true, if c1 == c2 (ignoring their case), false otherwise
     */
    static bool eq(CharT_ c1, CharT_ c2)
    {
        wchar_t C1 = towupper(wchar_t{c1});
        wchar_t C2 = towupper(wchar_t{c2});
        return (C1 == C2);
    }

    /**
     * @brief Non-equality of two characters ignoring their case.
     *
     * @param c1 first character
     * @param c2 second character
     * @return true, if c1 != c2 (ignoring their case), false otherwise
     */
    static bool ne(CharT_ c1, CharT_ c2)
    {
        wchar_t C1 = towupper(wchar_t{c1});
        wchar_t C2 = towupper(wchar_t{c2});
        return (C1 != C2);
    }

    /**
     * @brief Less-than of two characters ignoring their case.
     *
     * @param c1 first character
     * @param c2 second character
     * @return true, if c1 &lt; c2 (ignoring their case), false otherwise
     */
    static bool lt(CharT_ c1, CharT_ c2)
    {
        wchar_t C1 = towupper(wchar_t{c1});
        wchar_t C2 = towupper(wchar_t{c2});
        return (C1 < C2);
    }

    /**
     * @brief Returns the index of the first different char. This index is
     * multiplied by -1, if s1 &lt; s2 and 0 if s1==s2 up to n-th char or if
     * the first character == '\0'
     *
     * @param s1 char-pointer to first string
     * @param s2  char-pointer to second string
     * @param n number of chars to compare
     * @return int index of the first different char
     */
    static int compare(const CharT_ *s1, const CharT_ *s2, size_t n)
    {
        if(n == 0UL)
            return (0);

        if(s1 == nullptr)
            return (s2 == nullptr ? 0 : -1);

        long i = 0;

        while((i < (long)n) && (*s1) && (*s2) && (eq(*s1, *s2)))
        {
            s1++;
            s2++;
            i++;
        }

        return (i == (long)n ? 0 : lt(*s1, *s2) ? (-i - 1) : (eq(*s1, *s2) ? 0 : i + 1));
    }

    /**
     * @brief Returns the position of char a in string s of length n.
     *
     * @param s char-pointer to a string
     * @param n length of the string
     * @param c character to search for
     * @return const char* pointer to the first occurrence of c in the string s, or nullptr if it cannot be found
     */
    static const char *find(const CharT_ *s, int n, char c)
    {
        const CharT_ *reval = nullptr;
        while(n-- > 0 && reval == nullptr)
        {
            if(towupper(wchar_t{*s}) == towupper(wchar_t{c}))
                reval = s;

            ++s;
        }

        return (reval);
    }
};

/**
 * @brief Case-insensitive @c char string type.
 */
using ci_string = std::basic_string<char, ci_char_traits<char>, std::allocator<char>>;
/**
 * @brief Case-insensitive @c wchar_t string type.
 */
using ci_wstring = std::basic_string<wchar_t, ci_char_traits<wchar_t>, std::allocator<wchar_t>>;

#ifdef _GLIBCXX_USE_CHAR8_T
/**
 * @brief Case-insensitive  @c char8_t
 */
using ci_u8string = std::basic_string<char8_t, ci_char_traits<char8_t>, std::allocator<char8_t>>;
#endif

#if __cplusplus >= 201103L
/**
 * @brief Case-insensitive @c char16_t string type.
 */
using ci_u16string = std::basic_string<char16_t, ci_char_traits<char16_t>, std::allocator<char16_t>>;

/**
 * @brief Case-insensitive @c char32_t string type.
 */
using ci_u32string = std::basic_string<char32_t, ci_char_traits<char32_t>, std::allocator<char32_t>>;
#endif

};

namespace std
{
/**
 * @brief Overload the standard hash function for case insensitive strings to
 * enable ci_strings as elements in hash containers
 */
template<>
struct hash<util::ci_string>
{
    std::size_t operator()(const util::ci_string &s) const
    {
        std::hash<std::string> hasher;
        std::string            strLower = util::convert<std::string>(util::toLower(s));
        TRACE1(strLower);
        return hasher(strLower);
    }
};

/**
 * @brief Overload the standard hash function for wide case insensitive strings to
 * enable ci_wstrings as elements in hash containers
 */
template<>
struct hash<util::ci_wstring>
{
    std::size_t operator()(const util::ci_wstring &s) const
    {
        std::hash<std::wstring> hasher;
        std::wstring            strLower = util::convert<std::wstring>(util::toLower(s));
        return hasher(strLower);
    }
};

/**
 * @brief Generic Out-stream - &lt;&lt; operator for util::ci_string's.
 *
 * @param os the ostream for output
 * @param str the util::ci_string to output
 *
 * @return the modified stream
 */
inline std::ostream &operator<<(std::ostream &os, const util::ci_string &str)
{
    os << str.c_str();

    return (os);
}

/**
 * @brief Generic Out-stream - &lt;&lt; operator for util::ci_wstring's.
 *
 * @param os the ostream for output
 * @param str the util::ci_wstring to output
 *
 * @return the modified stream
 */
inline std::wostream &operator<<(std::wostream &os, const util::ci_wstring &str)
{
    os << str.c_str();

    return (os);
}

#ifdef _GLIBCXX_USE_CHAR8_T

/**
 * @brief Overload the standard hash function for wide case insensitive strings to
 * enable ci_u8strings as elements in hash containers
 */
template<>
struct hash<util::ci_u8string>
{
    std::size_t operator()(const util::ci_u8string &s) const
    {
        std::hash<std::u8string> hasher;
        std::u8string            strLower = util::convert<std::u8string>(util::toLower(s));
        return hasher(strLower);
    }
};

/**
 * @brief Generic Out-stream - &lt;&lt; operator for util::ci_u8tring's.
 *
 * @param os the ostream for output
 * @param str the util::ci_u8string to output
 *
 * @return the modified stream
 */
inline std::basic_ostream<char8_t, std::char_traits<char8_t>> &
 operator<<(std::basic_ostream<char8_t, std::char_traits<char8_t>> &os, const util::ci_u8string &s)
{
    os << s.c_str();

    return (os);
}
#endif

#if __cplusplus >= 201103L

/**
 * @brief Overload the standard hash function for wide case insensitive strings to
 * enable ci_u16strings as elements in hash containers
 */
template<>
struct hash<util::ci_u16string>
{
    std::size_t operator()(const util::ci_u16string &s) const
    {
        std::hash<std::u16string> hasher;
        std::u16string            strLower = util::convert<std::u16string>(util::toLower(s));
        return hasher(strLower);
    }
};

/**
 * @brief Generic Out-stream - &lt;&lt; operator for util::ci_u16tring's.
 *
 * @param os the ostream for output
 * @param str the util::ci_u16string to output
 *
 * @return the modified stream
 */
inline std::basic_ostream<char16_t, std::char_traits<char16_t>> &
 operator<<(std::basic_ostream<char16_t, std::char_traits<char16_t>> &os, const util::ci_u16string &s)
{
    os << s.c_str();

    return (os);
}

/**
 * @brief Overload the standard hash function for wide case insensitive strings to
 * enable ci_u32strings as elements in hash containers
 */
template<>
struct hash<util::ci_u32string>
{
    std::size_t operator()(const util::ci_u32string &s) const
    {
        std::hash<std::u32string> hasher;
        std::u32string            strLower = util::convert<std::u32string>(util::toLower(s));
        return hasher(strLower);
    }
};

/**
 * @brief Generic Out-stream - &lt;&lt; operator for util::ci_u32tring's.
 *
 * @param os the ostream for output
 * @param str the util::ci_u32string to output
 *
 * @return the modified stream
 */
inline std::basic_ostream<char32_t, std::char_traits<char32_t>> &
 operator<<(std::basic_ostream<char32_t, std::char_traits<char32_t>> &os, const util::ci_u32string &s)
{
    os << s.c_str();

    return (os);
}
#endif
};  // namespace std

#endif // NS_UTIL_CI_STRING_H_INCLUDED
