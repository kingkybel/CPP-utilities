/*
 * File Name:   dateutil.cc
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

#include <sys/time.h>
#include <stringutil.h>
#include <dateutil.h>

namespace util
{
    namespace datescan
    {
        using namespace std;
        using namespace boost::posix_time;
        using namespace boost::gregorian;
        vector<locale> formats = initDateFormats();

        /*
         *  boost POSIX library cannot scan string representations with single-digit
         * month/day fields. This function adds leading 0s into the string.
         * 3/8/1989 -> 03/08/1989
         */
        string addLeadingZeros(const string& str)
        {
            string reval = "";
            const char* pC = str.c_str();
            while (pC && *pC != 0)
            {
                if (isdigit(*pC))
                {
                    if (*pC != '0' && atoi(pC) < 10)
                    {
                        reval.push_back('0');
                    }
                    while (isdigit(*pC))
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
            return reval;
        }

        /*
         *  convert a POSIX-time into a time_t representation
         */
        time_t pt_to_time_t(const ptime& pt)
        {
            ptime timet_start(date(1970, 1, 1));
            time_duration diff = pt - timet_start;
            return diff.ticks() / time_duration::rep_type::ticks_per_second;

        }

        /*
         *  check whether we got a time only formatted time-string
         */
        inline bool isTimeOnly(const string& s)
        {
            // timeOnlyFormats are (s == "%H:%M:%S" || s == "%H:%M");
            return (s.size() == 8 && s[2] == ':' && s[5] == ':') ||
                    (s.size() == 5 && s[2] == ':');
        }

        /*
         *  scans a string into a posix time representation using the configured
         * formats
         */
        ptime scanDate(const string& s)
        {
            ptime reval;
            static ptime nullTime;
            string withZeros = addLeadingZeros(s);
            for (size_t i = 0; i < formats.size(); ++i)
            {
                istringstream is(withZeros);
                is.imbue(formats[i]);
                is >> reval;
                if (reval != nullTime)
                {
                    if (isTimeOnly(s))
                    {
                        // if we have a "time-only-format" then we need to explicitly
                        // set the day to the current day, as the stream conversion sets the
                        // day to the 1st Jan 1400
                        tm now = to_tm(second_clock::local_time());
                        tm reval_tm = to_tm(reval);
                        reval = ptime(date(now.tm_year + 1900,
                                           now.tm_mon + 1,
                                           now.tm_mday),
                                      time_duration(reval_tm.tm_hour,
                                                    reval_tm.tm_min,
                                                    reval_tm.tm_sec));
                    }
                    break;
                }
            }
            return reval;
        }

        // seconds elapsed since epoch to the time described by the string

        time_t seconds_from_epoch(const string& s)
        {
            return pt_to_time_t(scanDate(s));
        }

        /*
         *  iteration function: changes the case of all occurrences
         * of the given format flag, if flag == 'b', all occurrences
         * of %b in the format-string fmt are changed to %B, but
         * occurrences of %B are unchanged
         */
        string changeCaseOfFormatFlag(const string& fmt, char flag)
        {
            const string changeableFlags = "aAbByY";
            string reval = fmt;
            if (changeableFlags.find(flag) == string::npos)
                return reval;

            string fullFormatFlag = "%";
            fullFormatFlag += flag;
            string::size_type pos = 0;
            do
            {
                pos = reval.find(fullFormatFlag, pos);
                if (pos != string::npos)
                {
                    if (islower(reval[pos + 1]))
                        reval[pos + 1] = (char) toupper(reval[pos + 1]);
                    else
                        reval[pos + 1] = (char) tolower(reval[pos + 1]);
                }
            }
            while (pos != string::npos);
            return reval;
        }

        void addDateFormat(const string& fmt, vector<locale>& formatVec)
        {
            formatVec.push_back(locale(locale::classic(), new time_input_facet(fmt)));
        }

        /*
         *  create all the facets for the different date-formats we might come
         *  across this is a subset of the actually possible formats starting
         * with a set of base-formats we "declinate" additional formats by
         * switching parts from long to short representations some formats are
         * ambiguous and can give wrong or no results like "%m/%d/%Y %H:%M:%S"
         * and "%d/%m/%Y %H:%M:%S" one being the preferred format in the US the
         * other one from Europe the string "02/05/2014 12:34:56" would resolve
         * to the 5th February in US and the 2nd May in Europe the parameter
         * pref defaults to European
         */
        vector<locale> initDateFormats(DateFormatPreference pref, vector<locale>& fmts)
        {
            resetDateFormats(fmts);

            struct FmtType
            {

                FmtType(const string& fmt, bool american)
                : fmt_(fmt)
                , american_(american)
                {
                }
                string fmt_;
                bool american_;
            };
            FmtType timeFormats[] = {
                                     // Formats that include a time component
                                     FmtType("%Y-%B-%d %H:%M:%S", false), // 1967-November-10 12:34:56
                                     FmtType("%B %d %Y %H:%M:%S", true), // "November 10 1967 12:34:56" American Format
                                     FmtType("%d %B %Y %H:%M:%S", false), // "10 November 1967 12:34:56"
                                     FmtType("%A %B %d, %Y %H:%M:%S", true), // "Friday November 10, 1967 12:34:56" American Format
                                     FmtType("%A %d %B, %Y %H:%M:%S", false), // "Friday 10 November, 1967 12:34:56"
                                     FmtType("%m/%d/%Y %H:%M:%S", true), // "10/28/1967 12:34:56" American Format
                                     FmtType("%d/%m/%Y %H:%M:%S", false), // "28/11/1967 12:34:56"
                                     FmtType("%Y%m%d_%H%M%S", false), // "19671110_123456"
                                     FmtType("%H:%M:%S", false), // "12:34:56" time only
                                     FmtType("%H:%M", false), // "12:34" time only

                                     // Formats that only have a date component
                                     FmtType("%B %d %Y", true), // "November 10 1967" American Format
                                     FmtType("%d %B %Y", false), // "10 November 1967"
                                     FmtType("%A %d %B, %Y", false), // "Friday 10 November, 1967"
                                     FmtType("%m/%d/%Y", true), // "10/28/1967" American Format
                                     FmtType("%d/%m/%Y", false), // "28/11/1967"
                                     FmtType("%Y%m%d", false) // "19671110"
            };

            vector<string> disambiguatedFormats;
            for (size_t i = 0; i < sizeof (timeFormats) / sizeof (FmtType); i++)
            {
                if (pref == DateFormatPreference::European)
                {
                    if (timeFormats[i].american_ == true)
                    {
                        // swap American and European format
                        disambiguatedFormats.push_back(timeFormats[i + 1].fmt_);
                        disambiguatedFormats.push_back(timeFormats[i].fmt_);
                        i++;
                    }
                    else
                    {
                        disambiguatedFormats.push_back(timeFormats[i].fmt_);
                    }
                }
                else
                {
                    disambiguatedFormats.push_back(timeFormats[i].fmt_);
                }
            }
#if defined DEBUG_
            for (size_t i = 0; i < disambiguatedFormats.size(); i++)
                cout << "Disambiguated Format :" << disambiguatedFormats[i] << endl;
#endif
            for (size_t i = 0; i < disambiguatedFormats.size(); i++)
            {
                string fmt = disambiguatedFormats[i];
                // add all permutations of the format string that
                // contain flags %a,%A, %b, %B, %y, %Y
                const int y_max = fmt.find("%Y") != string::npos ? 2 : 1;
                const int a_max = fmt.find("%A") != string::npos ? 2 : 1;
                const int b_max = fmt.find("%B") != string::npos ? 2 : 1;
                bool finished = false;
                for (int y = 0; !finished && y < y_max; y++) // try first all where the year has 4 digits
                {
                    for (int a = 0; !finished && a < a_max; a++) // First "Monday", then "Mon"
                    {
                        for (int b = 0; !finished && b < b_max; b++) // First "February" then "Feb"
                        {
                            addDateFormat(fmt, fmts);
#if defined DEBUG_
                            cout << "Declinated Format :" << fmt << endl;
#endif
                            fmt = changeCaseOfFormatFlag(fmt, b == 0 ? 'B' : 'b');
                        }
                        fmt = changeCaseOfFormatFlag(fmt, a == 0 ? 'A' : 'a');
                    }
                    fmt = changeCaseOfFormatFlag(fmt, y == 0 ? 'Y' : 'y');
                }
            }

            return fmts;
        }

        void resetDateFormats(vector<locale>& fmts)
        {
            fmts.resize(0);
        }

        void imbueDateFormat(ostream& os, const string& fmt)
        {
            time_facet *facet = new time_facet(fmt.c_str());
            os.imbue(locale(os.getloc(), facet));
        }

        string dateString(const ptime& dt, const string& fmt)
        {
            stringstream ss;
            imbueDateFormat(ss, fmt);
            ss << dt;
            return ss.str();
        }

        string timestamp(const string& fmt)
        {
            return dateString(toDate(-1, -1, -1, -1, -1, -1, -1), fmt);
        }

        ptime toDate(int y, int m, int d, int H, int M, int S, int ms)
        {
            ptime now = microsec_clock::local_time();
            ptime reval = ptime(date(y < 0 ? now.date().year_month_day().year : greg_year(y),
                                     m < 0 ? now.date().year_month_day().month : greg_month(m),
                                     d < 0 ? now.date().year_month_day().day : greg_day(d)),
                                time_duration(H < 0 ? now.time_of_day().hours() : H,
                                              M < 0 ? now.time_of_day().minutes() : M,
                                              S < 0 ? now.time_of_day().seconds() : S,
                                              ms < 0 ? now.time_of_day().fractional_seconds() : ms));
            return reval;
        }

    }; // namespace datescan
}; // namespace util
