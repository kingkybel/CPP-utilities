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
namespace util
{
    std::map<std::ostream*, std::pair<size_t, std::ios::fmtflags> > streamModeHandler::ref_count;
    long streamModeHandler::mode = util::none_set;
    long streamModeHandler::aggregate = 0;
    long streamModeHandler::complement = util::all_set;
    long streamModeHandler::alternative = 0;

    std::ostream& streamModeHandler::apply(std::ostream& os)
    {
        if (ref_count[&os].first == 0)
        {
            // save the current state of the given stream
            ref_count[&os].second = os.flags();
        }
        ++(ref_count[&os].first);

        // first get the aggregate as a basis
        long aggMode = aggregate;

        // remove all from aggregated mode that are flagged as removed
        aggMode &= complement;

        // add all explicitly set stream-mode flags
        aggMode |= mode;

        // then get alternatives
        stream_mode_alternatives altMode = (stream_mode_alternatives) alternative;
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
            ref_count[&os].first--;
            if (ref_count[&os].first == 0)
            {
                // restore the state of the stream from the backup location
                os.flags(ref_count[&os].second);
            }
        }
    }
};
