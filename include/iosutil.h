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
 * @date: 2014-01-28
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_IOSUTIL_H_INCLUDED
#define NS_UTIL_IOSUTIL_H_INCLUDED
#include <iostream>
#include <map>

namespace util
{

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
    const static int stream_mode_xalloc_index = ios_base::xalloc();

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
    const static int stream_mode_complement_xalloc_index = ios_base::xalloc();

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
    const static int stream_mode_aggregate_xalloc_index = ios_base::xalloc();

    /**
     * Enumeration of modifiers that can only have one alternative, so bit-operations
     * do not apply.
     */
    enum stream_mode_alternatives : long
    {
        reset_float = 0x0000, ///< dreset the floating point display to system default
        short_float = 0x0001, ///< display floating point values in a short format
        long_float = 0x0002, ///< display floating point values in a longer format
        scientific_float = 0x0003 ///< display floating point values in scientific format

    };
    const static int stream_mode_alternatives_xalloc_index = ios_base::xalloc();

    std::ostream& operator<<(std::ostream& os, stream_mode sm)
    {
        os.iword(util::stream_mode) |= sm;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, stream_mode_complement sm)
    {
        os.iword(util::stream_mode) &= sm;
        return os;
    }

    std::ostream& operator<<(std::ostream& os, stream_mode_alternatives sm)
    {

    }

    std::ostream& operator<<(std::ostream& os, stream_mode_aggregate sm)
    {
        os.iword(util::stream_mode) = sm;
        return os;
    }

    /**
     * separate the stream formatting of the util-library from the the
     * formatting of objects outside
     */
    const static int backup_fmtflags_xalloc_index = ios_base::xalloc();

    template<std::ostream* pOS = 0 >
    struct streamModeHandler
    {
        static size_t ref_count = 0;

        streamModeHandler()
        {
            if (pOS != 0)
            {
                if (ref_count == 0)
                {
                    // initialize
                }
                // create/increment the ref-counter for this stream
                ref_count++;
                // reset the current flags on the stream and save the previous state in
                // the backup location
                pOS->iword(util::backup_fmtflags_xalloc_index) = pOS->flags(0);

                // first get the aggregate as a basis
                long aggMode = pOS->iword(stream_mode_aggregate_xalloc_index);

                // then get alternatives
                long altMode = pOS->iword(stream_mode_alternatives);

                // remove all from aggregated mode that are flagged as removed

            }
        }

        ~streamModeHandler()
        {
            if (pOS != 0)
            {
                ref_count--;
                if (ref_count == 0)
                {
                    // restore the state of the stream from the backup location
                    pOS->iword(util::backup_fmtflags_xalloc_index) = pOS->flags(0);
                }
            }
        }

    };

};

#endif	// NS_UTIL_IOSUTIL_H_INCLUDED

