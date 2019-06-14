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

    template<typename T, typename U = T>
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
    std::map<std::ostream*, std::pair<size_t, std::ios::fmtflags> > streamManip::restore_map;
    const int streamManip::mode_xindex = std::ios_base::xalloc();
    const int streamManip::aggregate_xindex = std::ios_base::xalloc();
    const int streamManip::alternative_xindex = std::ios_base::xalloc();
    const int streamManip::complement_xindex = std::ios_base::xalloc();
    const int streamManip::streamManip_xindex = std::ios_base::xalloc();

    streamManip::streamManip(std::ostream* pOs /*= 0*/,
                             long mode /*= none_set*/,
                             long aggregate /* = 0*/,
                             long alternative /* = all_set*/,
                             long complement /* = 0*/
                             )
    : pOs_(pOs)
    , mode_(mode)
    , aggregate_(aggregate)
    , alternative_(alternative)
    , complement_(complement)
    {
        if (pOs_ != 0)
            apply(*pOs);
    }

    streamManip::~streamManip()
    {
        if (pOs_ != 0)
            reset(*pOs_);
    }

    streamManip::streamManip(streamManip&& rhs)
    : pOs_(std::exchange(rhs.pOs_, (std::ostream*)0))
    , mode_(std::exchange(rhs.mode_, util::none_set))
    , aggregate_(std::exchange(rhs.aggregate_, 0))
    , alternative_(std::exchange(rhs.alternative_, util::all_set))
    , complement_(std::exchange(rhs.complement_, 0))
    {

    }

    streamManip& streamManip::operator=(streamManip&& rhs)
    {
        pOs_ = rhs.pOs_;
        rhs.pOs_ = 0;
        mode_ = std::exchange(rhs.mode_, util::none_set);
        aggregate_ = std::exchange(rhs.aggregate_, 0);
        alternative_ = std::exchange(rhs.alternative_, util::all_set);
        complement_ = std::exchange(rhs.complement_, 0);
    }

    std::ostream& streamManip::apply(std::ostream& os) const
    {
        if (restore_map[&os].first == 0)
        {
            // save the current state of the given stream
            restore_map[&os].second = os.flags();
        }
        ++(restore_map[&os].first);

        return os;
    }

    std::ostream& streamManip::reset(std::ostream& os) const
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
