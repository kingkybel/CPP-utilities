/*
 * File Name:   stringutil.cc
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

#include <limits>
#include <stringutil.h>

using namespace std;
using namespace util;

namespace util
{
    /**
     * try to classify a string into one of the classes NONE, INT, UINT, FLOAT
     * invalid strings have class NONE
     * integral types INT or UINT
     * and valid floating point strings are FLOAT
     * this function is template'd to allow for case-(in-)sensitive strings
     */
    template<typename T_>
        NumberClass classifyNumberStringT (const T_ &str)
        {
            static size_t maxIntLen = asString (numeric_limits<int64_t>::max ()).size ();
            static size_t maxUintLen = asString (numeric_limits<uint64_t>::max ()).size ();

            if (str.empty () || str.find_first_not_of ("0123456789+-.eElL") != T_::npos)
                return (NONE);

            if (str.find_first_of (".e") != T_::npos)
                return (FLOAT);

            bool isNegative = (str[0] == '-');
            bool isSigned = (str[0] == '-' || str[0] == '+');

            bool reval = ((isdigit (str[0])) || ((str.size () > 1) && isSigned));
            if (str.size () - (isSigned ? 1 : 0) > maxUintLen)
                return (FLOAT);
            else
            {
                if (str.size () - (isSigned ? 1 : 0) >= maxIntLen)
                    return (isNegative ? FLOAT : UINT);
                else
                    return (INT);
            }
        }

    NumberClass classifyNumberString (const string &str)
    {
        return (classifyNumberStringT (str));
    }

    NumberClass classifyNumberString (const ci_string &str)
    {
        return (classifyNumberStringT (str));
    }

    /**
     * ry to scan a string representation of a boolean by most common values
     * like True, on/Off/...
     */
    template<typename T_>
        bool scanBoolStringT (const T_ &strVal, bool &result)
        {
            const static map<ci_string, bool> VALID_BOOL =
            {
            { "true", true },
            { "t", true },
            { "yes", true },
            { "y", true },
            { "1", true },
            { "on", true },
            { "false", false },
            { "f", false },
            { "no", false },
            { "n", false },
            { "0", false },
            { "off", false } };

            auto found = VALID_BOOL.find (ci_string (strVal.c_str ()));

            result = found != VALID_BOOL.end () ? found->second : false;

            return (found != VALID_BOOL.end ());
        }

    bool scanBoolString (const string &strVal, bool &result)
    {
        return (scanBoolStringT (strVal, result));
    }

    bool scanBoolString (const util::ci_string &strVal, bool &result)
    {
        return (scanBoolStringT (strVal, result));
    }

    StripTrimMode operator& (const StripTrimMode lhs, const StripTrimMode &rhs)
    {
        return (static_cast<StripTrimMode> (static_cast<unsigned char> (lhs) & static_cast<unsigned char> (rhs)));
    }

    StripTrimMode operator| (const StripTrimMode lhs, const StripTrimMode &rhs)
    {
        return (static_cast<StripTrimMode> (static_cast<unsigned char> (lhs) | static_cast<unsigned char> (rhs)));
    }

    /**
     * removes all occurrences of stripChars in a string
     * strip "__<a_><bc>__" of trimChars="_<>" would result in abc
     */
    template<typename T_>
        void stripT (T_ &v, const T_ &stripChars, StripTrimMode m)
        {
            if (v.empty ())
                return;

            bool doStripFront = ((m & StripTrimMode::FRONT) == StripTrimMode::FRONT);
            bool doStripBack = ((m & StripTrimMode::BACK) == StripTrimMode::BACK);

            using TT = T_;
            TT reval = "";
            typename TT::size_type start = doStripFront ? v.find_first_not_of (stripChars) : 0;
            typename TT::size_type finish = doStripBack ? v.find_last_not_of (stripChars) + 1 : v.size ();

            if (start == TT::npos)
            {
                v = "";
                return;
            }

            bool doInside = ((m & StripTrimMode::INSIDE) == StripTrimMode::INSIDE);

            while (start < finish)
            {
                if (!doInside)  // if we did not specify to strip inside-matches
                    reval.append (&v[start], 1);
                else if (stripChars.find (v[start]) == TT::npos)
                    reval.append (&v[start], 1);

                start++;
            }

            v = reval;
        }

    void strip (string &v, const string &stripChars, StripTrimMode m)
    {
        stripT (v, stripChars, m);
    }

    void strip (ci_string &v, const ci_string &stripChars, StripTrimMode m)
    {
        stripT (v, stripChars, m);
    }

    /**
     * removes chars from the front and back of a string
     * intended for whitespace, but can be used to remove any set of trimChars
     * trim "__<abc>__" of trimChars="_<>" would result in abc
     */
    template<typename T_>
        void trimT (T_ &v, const T_ &trimChars, StripTrimMode m)
        {
            stripT (v, trimChars,
                    static_cast<StripTrimMode> (((m & StripTrimMode::FRONT) | (m & StripTrimMode::BACK))));
        }

    void trim (string &v, const string &trimChars, StripTrimMode m)
    {
        trimT (v, trimChars, m);
    }
    void trim (ci_string &v, const ci_string &trimChars, StripTrimMode m)
    {
        trimT (v, trimChars, m);
    }

    /**
     * replaces all occurrences of replChars in a string with a repl char
     * eplaceChar "__<a_><bc>__" of replChars="_<>",'#' would result in ###a###bc###
     */
    template<typename T_>
        void replaceCharT (T_ &v, const T_ &replChars, char repl, StripTrimMode m)
        {
            if (v.empty ())
                return;

            if (v.find_first_not_of (replChars) == T_::npos)
            {
                v = T_ (v.size (), repl);
                return;
            }

            size_t firstNonReplChar = 0;

            while (firstNonReplChar < v.size () && replChars.find (v[firstNonReplChar]) != T_::npos)
            {
                if ((m & StripTrimMode::FRONT) == StripTrimMode::FRONT)
                {
                    v[firstNonReplChar] = repl;
                }

                firstNonReplChar++;
            }

            size_t lastNonReplChar = v.size () - 1;

            while (replChars.find (v[lastNonReplChar]) != T_::npos)
            {
                if ((m & StripTrimMode::BACK) == StripTrimMode::BACK)
                {
                    v[lastNonReplChar] = repl;
                }
                lastNonReplChar--;
            }

            size_t insideNonReplChar = firstNonReplChar;

            while (insideNonReplChar < lastNonReplChar + 1)
            {
                if ((m & StripTrimMode::INSIDE) == StripTrimMode::INSIDE)
                {
                    if (replChars.find (v[insideNonReplChar]) != T_::npos)
                        v[insideNonReplChar] = repl;
                }

                insideNonReplChar++;
            }
        }

    void replaceChar (string &v, const string &replChars, char repl, StripTrimMode m)
    {
        replaceCharT (v, replChars, repl, m);
    }

    void replaceChar (ci_string &v, const ci_string &replChars, char repl, StripTrimMode m)
    {
        replaceCharT (v, replChars, repl, m);
    }

    /**
     * splits a string into a vector of strings using a char sep as separator
     */
    template<typename T_>
        vector<T_> splitIntoVectorT (const T_ &str, char sep)
        {
            vector<T_> results;
            typename T_::size_type start = 0;
            typename T_::size_type finish = str.find (sep);
            bool finished = false;

            while (!finished)
            {
                if (finish == T_::npos)
                {
                    results.push_back (str.substr (start));
                    finished = true;
                }
                else
                {
                    results.push_back (str.substr (start, finish - start));
                    start = finish + 1;
                    finish = str.find (sep, start);
                }
            }

            return (results);
        }

    vector<string> splitIntoVector (const string &str, char sep)
    {
        return (splitIntoVectorT (str, sep));
    }

    vector<ci_string> splitIntoVector (const ci_string &str, char sep)
    {
        return (splitIntoVectorT (str, sep));
    }

    /**
     * splits a string into a vector of strings using any one of the chars
     * in sep as separator
     */
    template<typename T_>
        vector<T_> splitIntoVectorT (const T_ &str, const T_ &sep)
        {
            vector<T_> results;
            typename T_::size_type start = 0;
            typename T_::size_type finish = str.find_first_of (sep);
            bool finished = false;

            while (!finished)
            {
                if (finish == T_::npos)
                {
                    results.push_back (str.substr (start));
                    finished = true;
                }
                else
                {
                    results.push_back (str.substr (start, finish - start));
                    start = finish + 1;
                    finish = str.find_first_of (sep, start);
                }
            }

            return (results);
        }

    vector<string> splitIntoVector (const string &str, const string &sep)
    {
        return (splitIntoVectorT (str, sep));
    }

    vector<ci_string> splitIntoVector (const ci_string &str, const ci_string &sep)
    {
        return (splitIntoVectorT (str, sep));
    }

    /**
     * splits a string into a set of strings using a char sep as separator
     */
    template<typename T_>
        set<T_> splitIntoSetT (const T_ &str, char sep)
        {
            set<T_> results;
            typename T_::size_type start = 0;
            typename T_::size_type finish = str.find (sep);
            bool finished = false;

            while (!finished)
            {
                if (finish == T_::npos)
                {
                    results.insert (str.substr (start));
                    finished = true;
                }
                else
                {
                    results.insert (str.substr (start, finish - start));
                    start = finish + 1;
                    finish = str.find (sep, start);
                }
            }

            return (results);
        }

    set<string> splitIntoSet (const string &str, char sep)
    {
        return (splitIntoSetT (str, sep));
    }

    set<ci_string> splitIntoSet (const ci_string &str, char sep)
    {
        return (splitIntoSetT (str, sep));
    }

    /**
     * splits a string into a set of strings using any one of the chars
     * in sep as separator
     */
    template<typename T_>
        set<T_> splitIntoSetT (const T_ &str, const T_ &sep)
        {
            set<T_> results;
            typename T_::size_type start = 0;
            typename T_::size_type finish = str.find_first_of (sep);
            bool finished = false;

            while (!finished)
            {
                if (finish == T_::npos)
                {
                    results.insert (str.substr (start));
                    finished = true;
                }
                else
                {
                    results.insert (str.substr (start, finish - start));
                    start = finish + 1;
                    finish = str.find_first_of (sep, start);
                }
            }

            return (results);
        }

    set<string> splitIntoSet (const string &str, const string &sep)
    {
        return (splitIntoSetT (str, sep));
    }

    set<ci_string> splitIntoSet (const ci_string &str, const ci_string &sep)
    {
        return (splitIntoSetT (str, sep));
    }

}
;
// namespace util
