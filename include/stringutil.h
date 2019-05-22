/*
 * File Name:   stringutil.h
 * Description: string utility functions
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
 * @date: 2014-01-28
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_STRINGUTIL_H_INCLUDED
#define NS_UTIL_STRINGUTIL_H_INCLUDED

#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <vector>
#include <deque>
#include <iterator>
#include <algorithm>
#include <ctime>    // for struct tm
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include "iosutil.h"

#if defined DO_TRACE_

#define TRACE0 { std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << std::endl; }
#define TRACE1(v1) { std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " " << #v1 << "=" << v1 << std::endl; }
// multi process versions for forked processes
#define PTRACE0 { std::cout << __FILE__ << ":" << __LINE__ << " " << __FUNCTION__ << " PID=" << getpid() << std::endl; }
#define PTRACE1(v1) { std::cout << __FILE__ << ":" << __LINE__  << " " << __FUNCTION__ << " PID=" << getpid() << " " << #v1 << "=" << v1 << std::endl; }

#else

#define TRACE0
#define TRACE1(v1)
// multi process versions for forked processes
#define PTRACE0
#define PTRACE1(v1)

#endif //DO_TRACE_

namespace util
{

    enum NumberClass
    {
        NONE, ///< This is not a number (it is a free man!)
        INT, ///< A signed integer
        UINT, ///< An unsigned integer
        FLOAT ///< A floating point number
    };

    enum StripTrimMode
    {
        FRONT = 0x01, ///< Strip or trim the left-hand-side
        LEFT = FRONT, ///< Strip or trim the left-hand-side
        INSIDE = 0x02, ///< Strip or trim occurences inside the string
        BACK = 0x04, ///< Strip or trim the right-hand-side
        RIGHT = BACK, ///< Strip or trim the right-hand-side
        OUTSIDE = FRONT | BACK, ///< Strip or trim the left-hand-side and right-hand-side
        ALL = FRONT | INSIDE | BACK ///< Strip or trim all occurences in the string
    };

    /**
     * Convert objects to a string, provided a ostream - &lt;&lt; operator is defined.
     */
    template <typename T_>
    inline std::string asString(const T_& v)
    {
        std::stringstream ss;
        streamModeHandler smh(ss);
        ss << v;
        return ss.str();
    }

    /**
     * Generic ostream - &lt;&lt; operator for vectors.
     */
    template<typename T_, typename Alloc_>
    inline std::ostream& operator<<(std::ostream& os, const std::vector<T_, Alloc_>& vec)
    {
        if (!vec.empty())
        {
            os << "< ";
            std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T_>(os, " | "));
            os << vec.back() << " >";
        }
        else
            os << "< >";
        return os;
    }

    /**
     * Generic ostream - &lt;&lt; operator for double ended queues.
     */
    template<typename T_, typename Alloc_>
    inline std::ostream& operator<<(std::ostream& os, const std::deque<T_, Alloc_>& vec)
    {
        if (!vec.empty())
        {
            os << "(* ";
            std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T_>(os, " < "));
            os << vec.back() << " *)";
        }
        else
            os << "(* *)";
        return os;
    }

    /**
     * Generic ostream - &lt;&lt; operator for unordered sets.
     */
    template<typename Value, typename Hash, typename Pred, typename Alloc>
    inline std::ostream& operator<<(std::ostream& os,
                                    const std::unordered_set<Value, Hash, Pred, Alloc>& vec)
    {
        if (!vec.empty())
        {
            os << "{~ ";
            for (auto it = vec.begin(); it != vec.end(); it++)
            {
                os << *it << " ";
            }
            os << "~}";
        }
        else
            os << "{~ ~}";
        return os;
    }

    /**
     * Generic ostream - &lt;&lt; operator for pairs.
     */
    template<typename T1_, typename T2_>
    inline std::ostream& operator<<(std::ostream& os, const std::pair<T1_, T2_>& p)
    {
        os << "(" << p.first << "->" << p.second << ")";
        return os;
    }

    /**
     * Generic ostream - &lt;&lt; operator for unordered maps.
     */
    template<typename Key, typename Value, typename Hash, typename Pred, typename Alloc>
    inline std::ostream& operator<<(std::ostream& os,
                                    const std::unordered_map<Key, Value, Hash, Pred, Alloc>& m)
    {
        if (!m.empty())
        {
            os << "{~ ";
            for (auto it = m.begin(); it != m.end(); it++)
            {
                os << *it << " ";
            }
            os << "~}";
        }
        else
            os << "{~ ~}";
        return os;
    }

    /**
     * Generic ostream - &lt;&lt; operator for standard (ordered) maps.
     */
    template<typename T1_, typename T2_, typename Compare_, typename Alloc_>
    inline std::ostream& operator<<(std::ostream& os, const std::map<T1_, T2_, Compare_, Alloc_>& m)
    {
        if (!m.empty())
        {
            os << "[ ";
            for (auto it = m.begin(); it != m.end(); it++)
            {
                os << *it << " ";
            }
            os << "]";
        }
        else
            os << "[ ]";
        return os;
    }

    /**
     * Create a vector from a given set.
     */
    template<typename T_, typename Compare_, typename Alloc_>
    inline std::vector<T_, Alloc_> vectorFromSet(const std::set<T_, Compare_, Alloc_>& s)
    {
        std::vector<T_, Alloc_> reval;
        if (!s.empty())
            std::copy(s.begin(), s.end(), std::back_inserter(reval));
        return reval;
    }

    /**
     * Generic ostream - &lt;&lt; operator for standard (ordered) set.
     */
    template<typename T_, typename Compare_, typename Alloc_>
    inline std::ostream& operator<<(std::ostream& os, const std::set<T_, Compare_, Alloc_>& s)
    {
        if (!s.empty())
        {
            typename std::vector<T_, Alloc_> vec = vectorFromSet(s);
            os << "{ ";
            std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T_>(os, ", "));
            os << vec.back() << " }";
        }
        else
            os << "{ }";
        return os;
    }

    /**
     * Create a set from a given vector (will remove duplicates).
     */
    template<typename T_, typename Alloc_>
    inline std::set<T_> setFromVector(const std::vector<T_, Alloc_>& vec)
    {
        std::set<T_> reval;
        if (!vec.empty())
            std::copy(vec.begin(), vec.end(), std::inserter(reval, reval.begin()));
        return reval;
    }

    /**
     * Create a standard (ordered) set from a given unordered set.
     */
    template<typename Value,
    typename Hash,
    typename Pred,
    typename Alloc>
    inline std::set<Value> setFromUnordered(const std::unordered_set<Value, Hash, Pred, Alloc>& uSet)
    {
        std::set<Value> reval;
        if (!uSet.empty())
        {
            for (auto it = uSet.begin(); it != uSet.end(); it++)
            {
                reval.insert(*it);
            }
        }
        return reval;
    }

    /**
     * Create an enclosed string copy of out-stream-able object.
     */
    template <typename T_>
    inline std::string enclosed(const T_& v, const std::string& brace = "\"")
    {
        std::stringstream ss;
        ss << brace << v << brace;
        return ss.str();
    }

    /**
     * Create a string copy of out-stream-able object enclosed by custom braces.
     */
    template <typename T_>
    inline std::string eclosed(const T_& v,
                               const std::string& leftBrace,
                               const std::string& rightBrace
                               )
    {
        std::stringstream ss;
        ss << leftBrace << v << rightBrace;
        return ss.str();
    }

    /**
     * Create a string copy of out-stream-able object enclosed by quote(").
     */
    template <typename T_>
    inline std::string quoted(const T_& v)
    {
        return enclosed(v, "\"");
    }

    /**
     * Create a string copy of out-stream-able object enclosed by single quote(').
     */
    template <typename T_>
    inline std::string squoted(const T_& v)
    {
        return enclosed(v, "'");
    }

    /**
     * Create a string copy of out-stream-able object enclosed by braces({ and }).
     */
    template <typename T_>
    inline std::string braced(const T_& v)
    {
        return enclosed(v, "{", "}");
    }

    /**
     * Create a string copy of out-stream-able object enclosed by
     *  brackets([ and ]).
     */
    template <typename T_>
    inline std::string bracketed(const T_& v)
    {
        return enclosed(v, "[", "]");
    }

    /**
     * Create a string copy of out-stream-able object enclosed by angled
     *  braces(\< and \>).
     */
    template <typename T_>
    inline std::string angled(const T_& v)
    {
        return enclosed(v, "<", ">");
    }

    /**
     * Create a string copy of out-stream-able object enclosed by round braces.
     */
    template <typename T_>
    inline std::string roundBraced(const T_& v)
    {
        return enclosed(v, "(", ")");
    }

    /**
     * Character traits for case-insensitive string type.
     * Inherits all the functions that we don't need to override for
     * case-insensitivity.
     */
    struct ci_char_traits : public std::char_traits<char>
    {

        /**
         * Equality of two characters ignoring their case.
         */
        static bool eq(char c1, char c2)
        {
            return toupper(c1) == toupper(c2);
        }

        /**
         * Non-equality of two characters ignoring their case.
         */
        static bool ne(char c1, char c2)
        {
            return toupper(c1) != toupper(c2);
        }

        /**
         * Less-than of two characters ignoring their case.
         */
        static bool lt(char c1, char c2)
        {
            return toupper(c1) < toupper(c2);
        }

        /**
         * Returns the 1-based index of the first different char. This index is
         * multiplied by -1, if s1 \< s2 and 0 if s1==s2 up to n-th char or if
         * the first character == '\0'
         */
        static int compare(const char* s1, const char* s2, size_t n)
        {
            if (n == 0)
                return 0;
            if (s1 == 0)
                return s2 == 0 ? 0 : -1;
            size_t i = 0;
            while (i < n && *s1 && *s2 && eq(*s1, *s2))
            {
                s1++;
                s2++;
                i++;
            }

            return i == n ? 0 : lt(*s1, *s2) ? -i - 1 : eq(*s1, *s2) ? 0 : i + 1;
        }

        /**
         * Returns the position of char a in string s of length n.
         */
        static const char* find(const char* s, int n, char a)
        {
            const char* reval = 0;
            while (n-- > 0 && reval == 0)
            {
                if (toupper(*s) == toupper(a))
                    reval = s;
                ++s;
            }
            return reval;
        }
    };

    /**
     * Case-insensitive string.
     */
    typedef std::basic_string<char, ci_char_traits> ci_string;

    /**
     * Trim left and/or right outside of standard string.
     */
    void trim(std::string& v,
              const std::string& trimChars = "\t \r\n",
              StripTrimMode m = OUTSIDE);

    /**
     * Trim left and/or right outside of case insensitive string.
     */
    void trim(ci_string& v,
              const ci_string& trimChars = "\t \r\n",
              StripTrimMode m = OUTSIDE);

    /**
     * Trim left and/or right  and/or interior of standard string.
     */
    void strip(std::string& v,
               const std::string& stripChars = "\t \r\n",
               StripTrimMode m = ALL);

    /**
     * Trim left and/or right  and/or interior of case insensitive string.
     */
    void strip(ci_string& v,
               const ci_string& stripChars = "\t \r\n",
               StripTrimMode m = ALL);

    /**
     * Replace occurrences of chars left and/or right  and/or interior of
     * standard string with replacement char.
     */
    void replaceChar(std::string& v,
                     const std::string& stripChars = "\t \r\n",
                     char repl = ' ', StripTrimMode m = ALL);

    /**
     * Replace occurrences of chars left and/or right  and/or interior of case
     *  insensitive string with replacement char.
     */
    void replaceChar(ci_string& v,
                     const ci_string& stripChars = "\t \r\n",
                     char repl = ' ',
                     StripTrimMode m = ALL);

    /**
     * Try to convert a string-representation into a bool-value.
     */
    bool scanBoolString(const std::string& strVal, bool& result);

    /**
     * Try to convert a string-representation into a bool-value.
     */
    bool scanBoolString(const ci_string& strVal, bool& result);

    /**
     * Shortcut to trim only left occurrences of trimChars.
     */
    inline void trimLeft(std::string& v, const std::string& trimChars = "\t \r\n")
    {
        return trim(v, trimChars, LEFT);
    }

    /**
     * Shortcut to strip only left occurrences of trimChars.
     */
    inline void trimLeft(ci_string& v, const ci_string& trimChars = "\t \r\n")
    {
        return trim(v, trimChars, LEFT);
    }

    /**
     * Generic ostream - &lt;&lt; operator for case-insensitive strings.
     */
    inline std::ostream& operator<<(std::ostream& os, const ci_string& str)
    {
        os << str.c_str();
        return os;
    }

    /**
     * Shortcut to trim only right occurrences of trimChars.
     */
    inline void trimRight(std::string& v, const std::string& trimChars = "\t \r\n")
    {
        return trim(v, trimChars, RIGHT);
    }

    /**
     * Shortcut to trim only right occurrences of trimChars.
     */
    inline void trimRight(ci_string& v, const ci_string& trimChars = "\t \r\n")
    {
        return trim(v, trimChars, RIGHT);
    }

    /**
     * Shortcut to strip only left occurrences of stripChars.
     */
    inline void stripLeft(std::string& v,
                          const std::string& stripChars = "\t \r\n")
    {
        return strip(v, stripChars, LEFT);
    }

    /**
     * Shortcut to strip only left occurrences of stripChars.
     */
    inline void stripLeft(ci_string& v,
                          const ci_string& stripChars = "\t \r\n")
    {
        return strip(v, stripChars, LEFT);
    }

    /**
     * Shortcut to strip only right occurrences of stripChars.
     */
    inline void stripRight(std::string& v,
                           const std::string& stripChars = "\t \r\n")
    {
        return strip(v, stripChars, RIGHT);
    }

    /**
     * Shortcut to strip only right occurrences of stripChars.
     */
    inline void stripRight(ci_string& v,
                           const ci_string& stripChars = "\t \r\n")
    {
        return strip(v, stripChars, RIGHT);
    }

    /**
     * Shortcut to replace only left occurrences of stripChars.
     */
    inline void replaceCharLeft(std::string& v,
                                const std::string& stripChars = "\t \r\n",
                                char repl = ' ')
    {
        replaceChar(v, stripChars, repl, LEFT);
    }

    /**
     * Shortcut to replace only left occurrences of stripChars.
     */
    inline void replaceCharLeft(ci_string& v,
                                const ci_string& stripChars = "\t \r\n",
                                char repl = ' ')
    {
        replaceChar(v, stripChars, repl, LEFT);
    }

    /**
     * Shortcut to replace only right occurrences of stripChars.
     */
    inline void replaceCharRight(std::string& v,
                                 const std::string& stripChars = "\t \r\n",
                                 char repl = ' ')
    {
        replaceChar(v, stripChars, repl, RIGHT);
    }

    /**
     * Shortcut to replace only right occurrences of stripChars.
     */
    inline void replaceCharRight(ci_string& v,
                                 const ci_string& stripChars = "\t \r\n",
                                 char repl = ' ')
    {
        replaceChar(v, stripChars, repl, RIGHT);
    }

    /**
     * Create an all-lower-case copy of standard string.
     */
    inline std::string toLower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), (char(*)(char))tolower);
        return str;
    }

    /**
     * Create an all-lower-case copy of case-insensitive string.
     */
    inline std::string toLower(ci_string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), (char(*)(char))tolower);
        return str.c_str();
    }

    /**
     * Create an all-upper-case copy of standard string.
     */
    inline std::string toUpper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), (char(*)(char))toupper);
        return str;
    }

    /**
     * Create an all-upper-case copy of case-insensitive string.
     */
    inline std::string toUpper(ci_string str)
    {
        std::transform(str.begin(),
                       str.end(),
                       str.begin(),
                       (char(*)(char))toupper);
        return str.c_str();
    }

    /**
     * Split a string into a vector of strings using a char sep as separator.
     */
    std::vector<std::string> splitIntoVector(const std::string& str, char sep);

    /**
     * Split a string into a vector of strings using a char sep as separator.
     */
    std::vector<std::string> splitIntoVector(const std::string& str,
                                             const std::string& sep);

    /**
     * Split a case-insensitive string into a vector of case-insensitive using a
     *  char sep as separator.
     */
    std::vector<ci_string> splitIntoVector(const ci_string& str, char sep);

    /**
     * Split a case-insensitive string into a vector of case-insensitive using a
     *  char sep as separator.
     */
    std::vector<ci_string> splitIntoVector(const ci_string& str,
                                           const ci_string& sep);

    /**
     * Split a string into a set of strings using a char sep as separator.
     */
    std::set<std::string> splitIntoSet(const std::string& str, char sep);

    /**
     * Split a string into a set of strings using a char sep as separator.
     */
    std::set<std::string> splitIntoSet(const std::string& str,
                                       const std::string& sep);

    /**
     * Split a case-insensitive string into a set of strings using a char sep as
     *  separator.
     */
    std::set<ci_string> splitIntoSet(const ci_string& str, char sep);

    /**
     * Split a case-insensitive string into a set of strings using a char sep as
     * separator.
     */
    std::set<ci_string> splitIntoSet(const ci_string& str, const ci_string& sep);

    /**
     * Classify a string into one of the classes NONE, INT, UINT, FLOAT.
     * invalid strings have class NONE
     * integral types INT or UINT
     * and valid floating point strings are FLOAT
     * this function is overridden to allow for case-(in-)sensitive strings
     */
    NumberClass classifyNumberString(const std::string& str);

    /**
     * Case-insensitive version of NumberClass
     * classifyNumberString(const std::string& str).
     */
    NumberClass classifyNumberString(const ci_string& str);

}; // namespace util

namespace std
{

    /**
     * Override the standard hash function for case insensitive strings to
     * enable as elements in hash containers
     */
    template<>
    struct hash<util::ci_string>
    : public unary_function<util::ci_string, size_t>
    {

        std::size_t operator()(const util::ci_string& s) const
        {
            hash<string> hasher;
            return hasher(util::toLower(s));
        }
    };
}; // namespace std

#endif // NS_UTIL_STRINGUTIL_H_INCLUDED

