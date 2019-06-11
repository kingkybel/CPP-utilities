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
#include <typeinfo>

#ifndef DO_TRACE_
#define DO_TRACE_
#endif
#include "traceutil.h"

namespace util
{

    /**
     * Enumeration of stream modes that modify the display of certain values
     * in the util-library.
     */
    enum stream_mode : long
    {
        none_set = 0L, ///< no flags set
        squoted_char = 1L << 0, ///< enclose characters in single quotes
        dquoted_char = 1L << 1, ///< enclose characters in double quotes
        hex_char = 1L << 2, ///< display characters in hexadecimal representation
        squoted_string = 1L << 3, ///< enclose strings in double quotes
        dquoted_string = 1L << 4, ///< enclose strings in double quotes
        squoted_date = 1L << 5, ///< enclose dates in double quotes
        dquoted_date = 1L << 6, ///< enclose dates in double quotes
        alpha_bool = 1L << 7, ///< display booleans as true and false
        round_open_brace = 1L << 8, ///< indicate open intervals with round braces
        symbolic_infinity = 1L << 9, ///< indicate full interval with symbolic infinity "oo"
    };
    const static int stream_mode_xalloc_index = std::ios_base::xalloc();

    /**
     * Enumeration of complements of stream modes that modify the display of
     * certain values in the util-library.
     */
    enum stream_mode_complement : long
    {
        all_set = ~none_set, //< all flags set
        no_squoted_char = ~squoted_char, ///< don't enclose characters in single quotes
        no_dquoted_char = ~dquoted_char, ///< don't enclose characters in double quotes
        no_hex_char = ~hex_char, ///< don't display characters in hexadecimal representation
        no_squoted_string = ~squoted_string, ///< don't enclose strings in single quotes
        no_dquoted_string = ~dquoted_string, ///< don't enclose strings in double quotes
        no_squoted_date = ~squoted_date, ///< don't enclose dates in single quotes
        no_dquoted_date = ~dquoted_date, ///< don't enclose dates in double quotes
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

    class streamManip
    {
        long mode_;
        long aggregate_;
        long alternative_;
        long complement_;

        std::ostream* pOs_;
    public:
        static std::map<std::ostream*, std::pair<size_t, std::ios::fmtflags> > restore_map;
        const static int mode_xindex;
        const static int aggregate_xindex;
        const static int alternative_xindex;
        const static int complement_xindex;
        const static int streamManip_xindex;

        streamManip(std::ostream* pOs = 0,
                    long mode = util::none_set,
                    long aggregate = 0,
                    long alternative = util::all_set,
                    long complement = 0
                    );
        streamManip(const streamManip&) = default;
        streamManip& operator=(const streamManip& rhs) = default;
        streamManip(streamManip&& rhs);
        streamManip& operator=(streamManip&& rhs);
        ~streamManip();

        template<typename T_>
        bool isSet(T_ flag) const
        {
            if (typeid (T_) == typeid (stream_mode))
                return (mode_ & flag) == flag;
            if (typeid (T_) == typeid (stream_mode_aggregate))
                return (aggregate_ & flag) == flag;
            if (typeid (T_) == typeid (stream_mode_alternatives))
                return (alternative_ & flag) == flag;
            if (typeid (T_) == typeid (stream_mode_complement))
                return (complement_ & flag) == flag;
            return false;
        }

        friend streamManip& operator<<(streamManip& lhs, const streamManip& rhs)
        {
            return lhs;
        }

        void set(stream_mode mode)
        {
            mode_ |= mode;
        }

        void unset(stream_mode mode)
        {
            mode_ &= ~((long) mode);
        }

        void set(stream_mode_aggregate mode)
        {
            aggregate_ = mode;
        }

        void unset(stream_mode_aggregate mode)
        {
            aggregate_ &= ~((long) mode);
        }

        void set(stream_mode_alternatives mode)
        {
            alternative_ = mode;
        }

        void unset(stream_mode_alternatives mode)
        {
            alternative_ &= ~((long) mode);
        }

        void set(stream_mode_complement mode)
        {
            complement_ = mode;
        }

        void unset(stream_mode_complement mode)
        {
            complement_ &= ~((long) mode);
        }

        std::ostream& apply(std::ostream& os) const;
        std::ostream& reset(std::ostream& os) const;

    };

    inline std::ostream& operator<<(std::ostream& os, stream_mode sm)
    {
        //        util::streamModeHandler::mode_ |= sm;
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, stream_mode_complement sm)
    {
        //      util::streamConfig::complement_ &= sm;
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, stream_mode_alternatives sm)
    {
        //      util::streamModeHandler::alternative_ = (long) sm;
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, stream_mode_aggregate sm)
    {
        //    util::streamModeHandler::aggregate_ = sm;
        return os;
    }

    inline std::ostream& operator<<(std::ostream& os, const streamManip& sc)
    {
        streamManip* pSC = new streamManip(sc);
        streamManip* old = (streamManip*) os.pword(streamManip::streamManip_xindex);
        delete(old);
        os.pword(streamManip::streamManip_xindex) = pSC;
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
        : width_(0)
        , precision_(0)
        , fill_(0)
        , isFixed_(false)
        , isScientific_(true)
        {
        }

        floatFmt(size_t width,
                 size_t precision = 5,
                 char fill = '0',
                 bool isFixed = true)
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
        if (fmt.isScientific_)
        {
            os << std::ios::scientific;
        }
        else
        {
            os.fill(fmt.fill_);
            os.width(fmt.width_);
            if (fmt.isFixed_)
                os << std::fixed;
        }
        return os;
    }

};

#endif	// NS_UTIL_IOSUTIL_H_INCLUDED

