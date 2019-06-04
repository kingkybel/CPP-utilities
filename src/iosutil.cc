/*
 * File Name:   iosutil.cc
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
#include "iosutil.h"
#include <utility>

#if !defined(__GXX_EXPERIMENTAL_CXX0X) || __cplusplus >= 201103L
namespace std
{

    template<class T, class U = T>
    T exchange(T& obj, U&& new_value)
    {
        T old_value = std::move(obj);
        obj = std::forward<U>(new_value);
        return old_value;
    }
}
#endif

namespace util
{
    std::map<std::ostream*, std::pair<size_t, std::ios::fmtflags> > streamModeHandler::restore_map;
    const int streamModeHandler::mode_xindex = std::ios_base::xalloc();
    const int streamModeHandler::aggregate_xindex = std::ios_base::xalloc();
    const int streamModeHandler::alternative_xindex = std::ios_base::xalloc();
    const int streamModeHandler::complement_xindex = std::ios_base::xalloc();
    const int streamModeHandler::stream_mode_xindex = std::ios_base::xalloc();

    streamModeHandler::streamModeHandler(long mode /*= none_set*/,
                                         long aggregate /* = 0*/,
                                         long alternative /* = all_set*/,
                                         long complement /* = 0*/
                                         )
    : mode_(mode)
    , aggregate_(aggregate)
    , alternative_(alternative)
    , complement_(complement)
    {

    }

    streamModeHandler::streamModeHandler(streamModeHandler&& rhs)
    : mode_(std::exchange(rhs.mode_, util::none_set))
    , aggregate_(std::exchange(rhs.aggregate_, 0))
    , alternative_(std::exchange(rhs.alternative_, util::all_set))
    , complement_(std::exchange(rhs.complement_, 0))
    {

    }

    streamModeHandler& streamModeHandler::operator=(streamModeHandler&& rhs)
    {
        mode_ = std::exchange(rhs.mode_, util::none_set);
        aggregate_ = std::exchange(rhs.aggregate_, 0);
        alternative_ = std::exchange(rhs.alternative_, util::all_set);
        complement_ = std::exchange(rhs.complement_, 0);
    }

    std::ostream& streamModeHandler::apply(std::ostream& os)
    {
        if (restore_map[&os].first == 0)
        {
            // save the current state of the given stream
            restore_map[&os].second = os.flags();
        }
        ++(restore_map[&os].first);

        // first get the aggregate as a basis
        long aggMode = aggregate_;

        // remove all from aggregated mode that are flagged as removed
        aggMode &= complement_;

        // add all explicitly set stream-mode flags
        aggMode |= mode_;

        // then get alternatives
        stream_mode_alternatives altMode = (stream_mode_alternatives) alternative_;
        TRACE1((long) altMode);

        if (altMode == scientific_float)
        {
            os << floatFmt();
        }
        else if (altMode == long_float)
        {
            os << floatFmt(10, 10);
        }
        else if (altMode == short_float)
        {
            os << floatFmt(10, 10);
        }
        return os;
    }

    std::ostream& streamModeHandler::reset(std::ostream& os)
    {
        if (os != 0)
        {
            restore_map[&os].first--;
            if (restore_map[&os].first == 0)
            {
                // restore the state of the stream from the backup location
                os.flags(restore_map[&os].second);
            }
        }
    }
};
