/*
 * File Name:   iosutil.h
 * Description: IO stream configuration
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
 * @date: 2019-05-22
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_IOSUTIL_H_INCLUDED
#define NS_UTIL_IOSUTIL_H_INCLUDED
#include <iostream>
#include <iomanip>
#include <ios>
#include <map>

#ifndef DO_TRACE_
#define DO_TRACE_
#endif
#include "traceutil.h"

namespace util
{

    struct streamModeHandler
    {
        static std::map<std::ostream*, std::pair<size_t, std::ios::fmtflags> > ref_count;
        static long mode;
        static long aggregate;
        static long alternative;
        static long complement;
        std::ostream& os;

        explicit streamModeHandler(std::ostream& ostr);
        streamModeHandler(const streamModeHandler&) = delete;
        streamModeHandler& operator=(const streamModeHandler&) = delete;
        streamModeHandler(const streamModeHandler&&) = delete;
        streamModeHandler& operator=(const streamModeHandler&&) = delete;

        ~streamModeHandler()
        {
            if (os != 0)
            {
                ref_count[&os].first--;
                if (ref_count[&os].first == 0)
                {
                    // restore the state of the stream from the backup location
                    os.flags(ref_count[&os].second);
                }
            }
        }

    };

    /**
     * Enumeration of stream modes that modify the display of certain values
     * in the util-library.
     */
    enum stream_mode : long
    {
        none_set = 0x0000, ///< no flags set
        squoted_char = 0x0001, ///< enclose characters in single quotes
        hex_char = 0x0002, ///< display characters in hexadecimal representation
        dquoted_string = 0x0004, ///< enclose strings in double quotes
        dquoted_date = 0x0008, ///< enclose dates in double quotes
        alpha_bool = 0x0010, ///< display booleans as true and false
        round_open_brace = 0x0100, ///< indicate open intervals with round braces
        symbolic_infinity = 0x0200, ///< indicate full interval with symbolic infinity "oo"
    };
    const static int stream_mode_xalloc_index = std::ios_base::xalloc();

    /**
     * Enumeration of complements of stream modes that modify the display of
     * certain values in the util-library.
     */
    enum stream_mode_complement : long
    {
        all_set = ~none_set, //< all flags set
        no_quoted_char = ~squoted_char, ///< don't enclose characters in single quotes
        no_hex_char = ~hex_char, ///< don't display characters in hexadecimal representation
        no_quoted_string = ~dquoted_string, ///< don't enclose strings in double quotes
        no_quoted_date = ~dquoted_date, ///< don't enclose dates in double quotes
        no_alpha_bool = ~alpha_bool, ///< don't display booleans as true and false
        no_round_open_brace = ~round_open_brace, ///< don't indicate open intervals with round braces
        no_symbolic_infinity = ~symbolic_infinity, ///< don't indicate full interval with symbolic infinity "oo"
    };
    const static int stream_mode_complement_xalloc_index = std::ios_base::xalloc();

    /**
     * Enumeration of modifiers that can only have one alternative, so bit-operations
     * do not apply.
     */
    enum stream_mode_alternatives : long
    {
        short_float = 0x0001, ///< display floating point values in a short format
        long_float = 0x0002, ///< display floating point values in a longer format
        scientific_float = 0x0004, ///< display floating point values in scientific format
        mask_float = (short_float | long_float | scientific_float)
    };
    const static int stream_mode_alternatives_xalloc_index = std::ios_base::xalloc();

    /**
     * Enumeration of modes that contains combinations of stream_mode values.
     * These are NOT complement-able.
     */
    enum stream_mode_aggregate : long
    {
        reset_stream_mode = 0x0000, ///< blank out all and revert to system defaults
        pure = alpha_bool | hex_char, ///< simple scannable format combination
        standard = alpha_bool | short_float | round_open_brace, ///< standard format combination
        safe = squoted_char | hex_char | dquoted_string | dquoted_date | alpha_bool ///< more complex combination
    };
    const static int stream_mode_aggregate_xalloc_index = std::ios_base::xalloc();

    inline std::ostream& operator<<(std::ostream& os, stream_mode sm)
    {
        TRACE1(util::streamModeHandler::mode);
        util::streamModeHandler::mode |= sm;
        TRACE1(util::streamModeHandler::mode);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, stream_mode_complement sm)
    {
        TRACE1(util::streamModeHandler::complement);
        util::streamModeHandler::complement &= sm;
        TRACE1(util::streamModeHandler::complement);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, stream_mode_alternatives sm)
    {
        TRACE1(util::streamModeHandler::alternative);
        util::streamModeHandler::alternative = (long) sm;
        TRACE1(util::streamModeHandler::alternative);
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, stream_mode_aggregate sm)
    {
        TRACE1(util::streamModeHandler::aggregate);
        util::streamModeHandler::aggregate = sm;
        TRACE1(util::streamModeHandler::aggregate);
        return os;
    }

    /**
     * separate the stream formatting of the util-library from the the
     * formatting of objects outside
     */
    const static int backup_fmtflags_xalloc_index = std::ios_base::xalloc();

    struct floatFmt
    {

        floatFmt()
        : isScientific_(true)
        {
        }

        floatFmt(size_t width,
                 size_t precision = 5,
                 char fill = '0',
                 bool isFixed = false)
        : width_(width)
        , precision_(precision)
        , fill_(fill)
        , isFixed_(isFixed)
        , isScientific_(false)
        {
        }

        size_t width_;
        size_t precision_;
        char fill_;
        bool isFixed_;
        bool isScientific_;
    };

    inline std::ostream& operator<<(std::ostream& os, const floatFmt& fmt)
    {
        TRACE1(fmt.isScientific_)
        if (fmt.isScientific_)
        {
            os << std::ios::scientific;
        }
        else
        {
            TRACE1(fmt.fill_);
            TRACE1(fmt.width_);
            os.fill(fmt.fill_);
            os.width(fmt.width_);
            if (fmt.isFixed_)
                os << std::fixed;
        }
        return os;
    }

};

#endif	// NS_UTIL_IOSUTIL_H_INCLUDED

