/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   src/dateutil.cc
 * Description: date utility functions
 *
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

#include "dateutil.h"

#include "stringutil.h"

#include <array>
#include <sys/time.h>
#include <utility>

namespace util
{
namespace datescan
{
    using namespace std;
    using namespace boost::posix_time;
    using namespace boost::gregorian;
    vector<locale> &formats()
    {
        static auto fmts = vector<locale>();
        return fmts;
    }

    /*
     * boost POSIX library cannot scan string representations with single-digit
     * month/day fields. This function adds leading 0s into the string.
     * 3/8/1989 -> 03/08/1989
     */
    string addLeadingZeros(const string &str)
    {
        string      reval = "";
        const char *pC    = str.c_str();

        while(pC && *pC != 0)
        {
            if(isdigit(*pC))
            {
                if(*pC != '0' && atoi(pC) < 10)  // @suppress("Avoid magic numbers")
                {
                    reval.push_back('0');
                }

                while(isdigit(*pC))
                {
                    reval.push_back(*pC);
                    pC++;
                }
            }
            else
            {
                reval.push_back(*pC);
                pC++;
            }
        }

        return (reval);
    }

    /*
     * convert a POSIX-time into a time_t representation
     */
    time_t pt_to_time_t(const ptime &pt)
    {
        ptime         timet_start(date(1970, 1, 1));
        time_duration diff = pt - timet_start;

        return (diff.ticks() / time_duration::rep_type::ticks_per_second);
    }

    /*
     * check whether we got a time only formatted time-string
     */
    inline bool isTimeOnly(const string &s)
    {
        // timeOnlyFormats are (s == "%H:%M:%S" || s == "%H:%M");
        return (((s.size() == 8) && (s[2] == ':') && (s[5] == ':'))
                || ((s.size() == 5) && (s[2] == ':')));  // @suppress("Avoid magic numbers")
    }

    /*
     * scans a string into a posix time representation using the configured
     * formats
     */
    ptime scanDate(const string &s)
    {
        ptime        reval;
        static ptime nullTime;
        string       withZeros = addLeadingZeros(s);

        for(auto &format: formats())
        {
            istringstream is(withZeros);
            is.imbue(format);
            is >> reval;
            if(reval != nullTime)
            {
                if(isTimeOnly(s))
                {
                    // if we have a "time-only-format" then we need to explicitly
                    // set the day to the current day, as the stream conversion sets the
                    // day to the 1st Jan 1400
                    tm now      = to_tm(second_clock::local_time());
                    tm reval_tm = to_tm(reval);
                    reval =
                     ptime(date(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday),  // @suppress("Avoid magic numbers")
                           time_duration(reval_tm.tm_hour, reval_tm.tm_min, reval_tm.tm_sec));
                }
                break;
            }
        }

        return (reval);
    }

    // seconds elapsed since epoch to the time described by the string

    time_t seconds_from_epoch(const string &s)
    {
        return (pt_to_time_t(scanDate(s)));
    }

    /*
     * iteration function: changes the case of all occurrences
     * of the given format flag, if flag == 'b', all occurrences
     * of %b in the format-string fmt are changed to %B, but
     * occurrences of %B are unchanged
     */
    string setABYFlag(const string &fmt, char flag_letter)
    {
        string flag = "%";
        flag += flag_letter;
        const string changeableFlags = "%a%A%b%B%y%Y";
        string       reval           = fmt;

        if(changeableFlags.find(flag) == string::npos)
            return (reval);

        string::size_type pos = 0;
        while((pos = toLower(reval).find(toLower(flag), pos)) != std::string::npos)
        {
            reval.replace(pos, flag.length(), flag);
            pos += flag.length();  // Move past the replacement
        }

        return (reval);
    }

    void addDateFormat(const string &fmt)
    {
        formats().push_back(locale(locale::classic(), new time_input_facet(fmt)));
    }

    /*
     * create all the facets for the different date-formats we might come
     * across this is a subset of the actually possible formats starting
     * with a set of base-formats we "declinate" additional formats by
     * switching parts from long to short representations some formats are
     * ambiguous and can give wrong or no results like "%m/%d/%Y %H:%M:%S"
     * and "%d/%m/%Y %H:%M:%S" one being the preferred format in the US the
     * other one from Europe the string "02/05/2014 12:34:56" would resolve
     * to the 5th February in US and the 2nd May in Europe.
     * @parameter pref resolution preference, defaults to European
     */
    vector<locale> &initDateFormats(DateFormatPreference pref)
    {
        auto &fmts = formats();
        fmts.clear();

        struct FmtType
        {
            string fmt_;
            bool   american_;
        };

        // in the following array every American format string (indicated by "true") is immediately followed by
        // the equivalent string in Europen format.
        FmtType rawFormats[] = {
         // Formats that include a time component
         {"%Y-%B-%d %H:%M:%S", false},      // 1967-November-10 12:34:56
         {"%B %d %Y %H:%M:%S", true},       // "November 10 1967 12:34:56" American Format
         {"%d %B %Y %H:%M:%S", false},      // "10 November 1967 12:34:56"
         {"%A %B %d, %Y %H:%M:%S", true},   // "Friday November 10, 1967 12:34:56" American Format
         {"%A %d %B, %Y %H:%M:%S", false},  // "Friday 10 November, 1967 12:34:56"
         {"%m/%d/%Y %H:%M:%S", true},       // "10/28/1967 12:34:56" American Format
         {"%d/%m/%Y %H:%M:%S", false},      // "28/11/1967 12:34:56"
         {"%Y%m%d_%H%M%S", false},          // "19671110_123456"
         {"%H:%M:%S", false},               // "12:34:56" time only
         {"%H:%M", false},                  // "12:34" time only

         // Formats that only have a date component
         {"%B %d %Y", true},       // "November 10 1967" American Format
         {"%d %B %Y", false},      // "10 November 1967"
         {"%A %d %B, %Y", false},  // "Friday 10 November, 1967"
         {"%m/%d/%Y", true},       // "10/28/1967" American Format
         {"%d/%m/%Y", false},      // "28/11/1967"
         {"%Y%m%d", false}         // "19671110"
        };

        // now order the formats according to preference:
        // - first try European, or
        // - first try American
        vector<string> disambiguatedFormats;
        size_t         num_formats = sizeof(rawFormats) / sizeof(FmtType);
        for(size_t i = 0; i < num_formats; i++)
        {
            if(pref == DateFormatPreference::European)
            {
                if(rawFormats[i].american_ == true)
                {
                    // swap American and European format
                    disambiguatedFormats.push_back(rawFormats[i + 1].fmt_);
                    disambiguatedFormats.push_back(rawFormats[i].fmt_);
                    i++;
                }
                else
                {
                    disambiguatedFormats.push_back(rawFormats[i].fmt_);
                }
            }
            else
            {
                disambiguatedFormats.push_back(rawFormats[i].fmt_);
            }
        }
#if defined DEBUG_
        for(size_t i = 0; i < disambiguatedFormats.size(); i++)
            cout << "Disambiguated Format :" << disambiguatedFormats[i] << endl;
#endif
        // now declinate all the formats:
        // if there is a Weekday-flag (%a, %A) make sure the "long" weekday (Monday before Mon) is tried
        // before the short one, etc.
        for(auto fmt: disambiguatedFormats)
        {
            // add all permutations of the format string that
            // contain flags %a,%A, %b, %B, %y, %Y
            for(const auto &y: {'Y', 'y'})  // try first all where the year has 4 digits
            {
                fmt = setABYFlag(fmt, y);
                for(const auto &a: {'A', 'a'})  // First "Monday", then "Mon"
                {
                    fmt = setABYFlag(fmt, a);
                    for(const auto b: {'B', 'b'})  // First "February" then "Feb"
                    {
                        fmt = setABYFlag(fmt, b);
                        addDateFormat(fmt);
#if defined DEBUG_
                        cout << "Declinated Format :" << fmt << endl;
#endif
                    }
                }
            }
        }

        return (fmts);
    }

    vector<locale> &resetDateFormats()
    {
        formats().resize(0);
        return formats();
    }

    void imbueDateFormat(ostream &os, const string &fmt)
    {
        auto *facet = new time_facet(fmt.c_str());

        os.imbue(locale(os.getloc(), facet));
    }

    string dateString(const ptime &dt, const string &fmt)
    {
        stringstream ss;

        imbueDateFormat(ss, fmt);
        ss << dt;

        return (ss.str());
    }

    string timestamp(const string &fmt)
    {
        return (dateString(toDate(-1, -1, -1, -1, -1, -1, -1), fmt));
    }

    ptime toDate(int y, int m, int d, int H, int M, int S, int ms)
    {
        ptime now   = microsec_clock::local_time();
        ptime reval = ptime(date(y < 0 ? now.date().year_month_day().year : greg_year(y),
                                 m < 0 ? now.date().year_month_day().month : greg_month(m),
                                 d < 0 ? now.date().year_month_day().day : greg_day(d)),
                            time_duration(H < 0 ? now.time_of_day().hours() : H,
                                          M < 0 ? now.time_of_day().minutes() : M,
                                          S < 0 ? now.time_of_day().seconds() : S,
                                          ms < 0 ? now.time_of_day().fractional_seconds() : ms));

        return (reval);
    }

};  // namespace datescan
};  // namespace util
