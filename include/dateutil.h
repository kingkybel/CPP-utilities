/*
 * File Name:   dateutil.h
 * Description: date utility functions
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

#ifndef NS_UTIL_DATEUTIL_H_INCLUDED
#define NS_UTIL_DATEUTIL_H_INCLUDED

#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <iterator>
#include <algorithm>
#include <ctime>                    // for struct tm
#include <boost/date_time.hpp>      // gregorian dates/posix time/...

namespace util
{
    namespace val = boost::posix_time;
    namespace datescan
    {

        /**
         *  Date-strings can sometimes be interpreted as as different dates depending
         *  on whether they are interpreted as American or European, for example
         *  9/11/2001 is the 11th of September 2001 in American format, but
         *  the 9th of November in Europe. This enum configures the resolution of
         *  this ambiguity.
         */
        enum class DateFormatPreference
        {
            European, USA
        };
        extern std::vector<std::locale> formats; ///< A sequence of formats a string is tested against.

        /**
         *  Conversion of a boost::posix_time to a time_t.
         */
        std::time_t pt_to_time_t(const val::ptime& pt);

        /**
         *  Returns the seconds elapsed since the epoch (1 Jan 1900)
         */
        time_t seconds_from_epoch(const std::string& s);

        /**
         *  Scan/parse a string into a date.
         */
        val::ptime scanDate(const std::string& s);

        /**
         *  Returns true if the string depicts a time-only format and false otherwise.
         */
        inline bool isTimeOnly(const std::string& s);

        /**
         *  Helper to standardise date-strings to always use 2-digits for month/days.
         */
        std::string addLeadingZeros(const std::string& str);

        /**
         *  Helper to conjugate certain format specifiers' case (\%y<->\%Y,...)
         */
        std::string changeCaseOfFormatFlag(const std::string& fmt, char flag);

        /**
         *  Add a format to the list of valid formats.
         */
        void addDateFormat(const std::string& fmt, std::vector<std::locale>& formatVec = datescan::formats);

        /**
         *  Initialise the list of valid formats to a set of commonly used ones.
         */
        std::vector<std::locale> initDateFormats(DateFormatPreference pref = DateFormatPreference::European,
                                                 std::vector<std::locale>& formatVec = datescan::formats);

        /**
         *  Clear the list of all formats.
         */
        void resetDateFormats(std::vector<std::locale>& fmts = datescan::formats);

        /**
         *  Create a time using year/month/day/hour/minute/seconds/milliseconds
         * all specified as integers. Returns a now-timestamp if all parameters
         * are equal to -1.
         */
        val::ptime toDate(int y = -1,
                         int m = -1,
                         int d = -1,
                         int H = 0,
                         int M = 0,
                         int S = 0,
                         int ms = 0);

        /**
         *  Get a now-timestamp in a specified format.
         */
        std::string timestamp(const std::string& fmt = "%Y-%m-%d_%H:%M:%S.%f");

        /**
         *  Imbues an output format for dates to an output-stream.
         */
        void imbueDateFormat(std::ostream& os, const std::string& fmt);
    };

    /**
     *  Checks whether a time is a valid time.
     */
    inline bool valid(const val::ptime& pt)
    {
        return !pt.is_infinity() &&
                !pt.is_not_a_date_time() &&
                pt.date().year() >= 1900 &&
                pt.date().year() <= 2200;
    }
}; // namespace util

#endif // NS_UTIL_DATEUTIL_H_INCLUDED

