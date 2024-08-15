/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/dateutil_tests.cc
 * Description: Unit tests for date utilities.
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

#include "anyutil.h"
#include "bayesutil.h"
#include "csvutil.h"
#include "dateutil.h"
#include "graphutil.h"
#include "statutil.h"
#include "stringutil.h"

#include <boost/assign/std/vector.hpp>
#include <boost/bind.hpp>
#include <gtest/gtest.h>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;
using namespace util;
using namespace util::datescan;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::assign;
using namespace boost::filesystem;

const string filename = "/tmp/test.csv";

class DateUtilTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        resetDateFormats();
        initDateFormats(DateFormatPreference::USA);
    }
};

struct DR
{
    DR(const string &param, const string result, size_t line) : param_(param), result_(result), line_(line)
    {
    }

    bool correctResult()
    {
        bool reval = (result_ == toString(datescan::scanDate(param_)));
        if(!reval)
            cout << " Line " << line_ << ": " << quoted(result_)
                 << " != " << quoted(toString(util::datescan::scanDate(param_))) << " " << ends;
        return reval;
    }
    string param_;
    string result_;
    size_t line_;
};

TEST_F(DateUtilTest, util_date_european_test)
{
    resetDateFormats();
    initDateFormats(DateFormatPreference::European);
    DR scanResults[] = {
     // test all declinated formats
     DR("1967-November-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // "%Y-%B-%d %H:%M:%S"
     DR("1967-Nov-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__),       // "%Y-%b-%d %H:%M:%S"
     DR("67-November-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__),    // "%y-%B-%d %H:%M:%S"
     DR("67-Nov-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__),         // "%y-%b-%d %H:%M:%S"

     DR("November 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // American Format"%B %d %Y %H:%M:%S"
     DR("Nov 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),       // American Format"%b %d %Y %H:%M:%S"
     DR("November 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),    // American Format"%B %d %y %H:%M:%S"
     DR("Nov 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),         // American Format"%b %d %y %H:%M:%S"

     DR("10 November 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // "%d %B %Y %H:%M:%S"
     DR("10 Nov 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),       // "%d %b %Y %H:%M:%S"
     DR("10 November 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),    // "%d %B %y %H:%M:%S"
     DR("10 Nov 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),         // "%d %b %y %H:%M:%S"

     DR("Friday 10 November, 1967 12:34:56",
        "1967-Nov-10 12:34:56",
        __LINE__),  // "%A %d %B, %Y %H:%M:%S"
     DR("Fri 10 November, 1967 12:34:56",
        "1967-Nov-10 12:34:56",
        __LINE__),  // "%a %d %B, %Y %H:%M:%S"
     DR("Friday 10 Nov, 1967 12:34:56",
        "1967-Nov-10 12:34:56",
        __LINE__),                                                       // "%A %d %b, %Y %H:%M:%S"
     DR("Fri 10 Nov, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // "%a %d %b, %Y %H:%M:%S"
     DR("Friday 10 November, 67 12:34:56",
        "2067-Nov-10 12:34:56",
        __LINE__),  // "%A %d %B, %y %H:%M:%S"
     DR("Fri 10 November, 67 12:34:56",
        "2067-Nov-10 12:34:56",
        __LINE__),                                                        // "%a %d %B, %y %H:%M:%S"
     DR("Friday 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),  // "%A %d %b, %y %H:%M:%S"
     DR("Fri 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),     // "%a %d %b, %y %H:%M:%S"

     DR("10/28/1967 12:34:56", "1967-Oct-28 12:34:56", __LINE__),  //  American Format"%m/%d/%Y %H:%M:%S"
     DR("10/28/67 12:34:56", "2067-Oct-28 12:34:56", __LINE__),    //  American Format"%m/%d/%y %H:%M:%S"

     DR("28/11/1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__),  // "%d/%m/%Y %H:%M:%S"
     DR("28/11/67 12:34:56", "2067-Nov-28 12:34:56", __LINE__),    // "%d/%m/%y %H:%M:%S"

     DR("28.11.1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__),  // "%d.%m.%Y %H:%M:%S"
     DR("28.11.67 12:34:56", "2067-Nov-28 12:34:56", __LINE__),    // "%d.%m.%y %H:%M:%S"

     DR("28-11-1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__),  // "%d-%m-%Y %H:%M:%S"
     DR("28-11-67 12:34:56", "2067-Nov-28 12:34:56", __LINE__),    // "%d-%m-%y %H:%M:%S"

     DR("19671110_123456", "1967-Nov-10 12:34:56", __LINE__),  // "%Y%m%d_%H%M%S"
     DR("671110_123456", "2067-Nov-10 12:34:56", __LINE__),    // "%y%m%d_%H%M%S"

     //        // time-only (USES CURRENT-DATE! Need to test separately.)
     //        DR("12:34:56", "2014-Jan-27 12:34:56",__LINE__),                       // "%H:%M:%S" time only
     //        DR("12:34", "2014-Jan-27 12:34:00",__LINE__),                          // "%H:%M" time only

     DR("November 10 1967", "1967-Nov-10 00:00:00", __LINE__),  // "%B %d %Y" American Format
     DR("Nov 10 1967", "1967-Nov-10 00:00:00", __LINE__),       // "%b %d %Y" American Format
     DR("November 10 67", "2067-Nov-10 00:00:00", __LINE__),    // "%B %d %y" American Format
     DR("Nov 10 67", "2067-Nov-10 00:00:00", __LINE__),         // "%b %d %y" American Format

     DR("10 November 1967", "1967-Nov-10 00:00:00", __LINE__),  // "%d %B %Y"
     DR("10 Nov 1967", "1967-Nov-10 00:00:00", __LINE__),       // "%d %b %Y"
     DR("10 November 67", "2067-Nov-10 00:00:00", __LINE__),    // "%d %B %y"
     DR("10 Nov 67", "2067-Nov-10 00:00:00", __LINE__),         // "%d %b %y"

     DR("Friday 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__),  // "%A %d %B, %Y"
     DR("Fri 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__),     // "%a %d %B, %Y"
     DR("Friday 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__),       // "%A %d %b, %Y"
     DR("Fri 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__),          // "%a %d %b, %Y"
     DR("Friday 10 November, 67", "2067-Nov-10 00:00:00", __LINE__),    // "%A %d %B, %y"
     DR("Fri 10 November, 67", "2067-Nov-10 00:00:00", __LINE__),       // "%a %d %B, %y"
     DR("Friday 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__),         // "%A %d %b, %y"
     DR("Fri 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__),            // "%a %d %b, %y"

     DR("10/28/1967", "1967-Oct-28 00:00:00", __LINE__),  // "%m/%d/%Y" American Format
     DR("10/28/19", "2019-Oct-28 00:00:00", __LINE__),    // "%m/%d/%y" American Format
     DR("28/11/1967", "1967-Nov-28 00:00:00", __LINE__),  // "%d/%m/%Y"
     DR("28/11/67", "2067-Nov-28 00:00:00", __LINE__),    // "%d/%m/%y"
     DR("28.11.1967", "1967-Nov-28 00:00:00", __LINE__),  // "%d.%m.%Y"
     DR("28.11.67", "2067-Nov-28 00:00:00", __LINE__),    // "%d.%m.%y"
     DR("28-11-1967", "1967-Nov-28 00:00:00", __LINE__),  // "%d-%m-%Y"
     DR("28-11-67", "2067-Nov-28 00:00:00", __LINE__),    // "%d-%m-%y"
     DR("19671110", "1967-Nov-10 00:00:00", __LINE__),    // "%Y%m%d"
     DR("671110", "6711-Oct-01 00:00:00", __LINE__),      // "%y%m%d"

     // test formats that might require leading zeros
     DR("November 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // American Format"%B %d %Y %H:%M:%S"
     DR("Nov 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),       // American Format"%b %d %Y %H:%M:%S"
     DR("November 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // American Format"%B %d %y %H:%M:%S"
     DR("Nov 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),         // American Format"%b %d %y %H:%M:%S"

     DR("3 November 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // "%d %B %Y %H:%M:%S"
     DR("3 Nov 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),       // "%d %b %Y %H:%M:%S"
     DR("3 November 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // "%d %B %y %H:%M:%S"
     DR("3 Nov 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),         // "%d %b %y %H:%M:%S"

     DR("Friday 3 November, 1967 12:34:56",
        "1967-Nov-03 12:34:56",
        __LINE__),  // "%A %d %B, %Y %H:%M:%S"
     DR("Fri 3 November, 1967 12:34:56",
        "1967-Nov-03 12:34:56",
        __LINE__),                                                         // "%a %d %B, %Y %H:%M:%S"
     DR("Friday 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // "%A %d %b, %Y %H:%M:%S"
     DR("Fri 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),     // "%a %d %b, %Y %H:%M:%S"
     DR("Friday 3 November, 67 12:34:56",
        "2067-Nov-03 12:34:56",
        __LINE__),                                                         // "%A %d %B, %y %H:%M:%S"
     DR("Fri 3 November, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),  // "%a %d %B, %y %H:%M:%S"
     DR("Friday 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // "%A %d %b, %y %H:%M:%S"
     DR("Fri 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),       // "%a %d %b, %y %H:%M:%S"

     DR("3/28/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  //  American Format"%m/%d/%Y %H:%M:%S"
     DR("3/28/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    //  American Format"%m/%d/%y %H:%M:%S"
     DR("10/3/1967 12:34:56", "1967-Mar-10 12:34:56", __LINE__),  //  American Format"%m/%d/%Y %H:%M:%S"
     DR("10/3/67 12:34:56", "2067-Mar-10 12:34:56", __LINE__),    //  American Format"%m/%d/%y %H:%M:%S"
     DR("9/3/1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__),   //  American Format"%m/%d/%Y %H:%M:%S"
     DR("9/3/67 12:34:56", "2067-Mar-09 12:34:56", __LINE__),     //  American Format"%m/%d/%y %H:%M:%S"

     DR("3/11/1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // "%d/%m/%Y %H:%M:%S"
     DR("3/11/67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // "%d/%m/%y %H:%M:%S"
     DR("28/3/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  // "%d/%m/%Y %H:%M:%S"
     DR("28/3/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    // "%d/%m/%y %H:%M:%S"
     DR("9/3/1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__),   // "%d/%m/%Y %H:%M:%S"
     DR("9/3/67 12:34:56", "2067-Mar-09 12:34:56", __LINE__),     // "%d/%m/%y %H:%M:%S"

     DR("3.11.1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // "%d.%m.%Y %H:%M:%S"
     DR("3.11.67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // "%d.%m.%y %H:%M:%S"
     DR("28.3.1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  // "%d.%m.%Y %H:%M:%S"
     DR("28.3.67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    // "%d.%m.%y %H:%M:%S"
     DR("9.3.1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__),   // "%d.%m.%Y %H:%M:%S"
     DR("9.3.67 12:34:56", "2067-Mar-09 12:34:56", __LINE__),     // "%d.%m.%y %H:%M:%S"

     DR("3-11-1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // "%d-%m-%Y %H:%M:%S"
     DR("3-11-67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // "%d-%m-%y %H:%M:%S"
     DR("28-3-1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  // "%d-%m-%Y %H:%M:%S"
     DR("28-3-67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    // "%d-%m-%y %H:%M:%S"
     DR("9-3-1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__),   // "%d-%m-%Y %H:%M:%S"
     DR("9-3-67 12:34:56", "2067-Mar-09 12:34:56", __LINE__),     // "%d-%m-%y %H:%M:%S"
    };
    for(size_t i = 0; i < sizeof(scanResults) / sizeof(DR); i++)
    {
        ASSERT_TRUE(scanResults[i].correctResult());
    }

    resetDateFormats();
    addDateFormat("%H:%M:%S");
    addDateFormat("%H:%M");
    ptime dt = scanDate("12:34:56");
    ASSERT_EQ(string("12:34:56"), toString(dt.time_of_day()));
    dt = scanDate("12:34");
    ASSERT_EQ(string("12:34:00"), toString(dt.time_of_day()));

    resetDateFormats();
    for(size_t i = 0; i < sizeof(scanResults) / sizeof(DR); i++)
    {
        ASSERT_EQ(string("not-a-date-time"), toString(scanDate(scanResults[i].param_)));
    }
    addDateFormat("%H:%Y");
    ASSERT_EQ(string("2013-Jan-01 13:00:00"), toString(scanDate("13:2013")));
}

TEST_F(DateUtilTest, util_date_american_test)
{
    resetDateFormats();
    initDateFormats(DateFormatPreference::USA);
    DR scanResults[] = {
     // test all declinated formats
     DR("1967-November-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // "%Y-%B-%d %H:%M:%S"
     DR("1967-Nov-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__),       // "%Y-%b-%d %H:%M:%S"
     DR("67-November-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__),    // "%y-%B-%d %H:%M:%S"
     DR("67-Nov-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__),         // "%y-%b-%d %H:%M:%S"

     DR("November 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // American Format"%B %d %Y %H:%M:%S"
     DR("Nov 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),       // American Format"%b %d %Y %H:%M:%S"
     DR("November 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),    // American Format"%B %d %y %H:%M:%S"
     DR("Nov 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),         // American Format"%b %d %y %H:%M:%S"

     DR("10 November 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // "%d %B %Y %H:%M:%S"
     DR("10 Nov 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),       // "%d %b %Y %H:%M:%S"
     DR("10 November 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),    // "%d %B %y %H:%M:%S"
     DR("10 Nov 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),         // "%d %b %y %H:%M:%S"

     DR("Friday 10 November, 1967 12:34:56",
        "1967-Nov-10 12:34:56",
        __LINE__),  // "%A %d %B, %Y %H:%M:%S"
     DR("Fri 10 November, 1967 12:34:56",
        "1967-Nov-10 12:34:56",
        __LINE__),  // "%a %d %B, %Y %H:%M:%S"
     DR("Friday 10 Nov, 1967 12:34:56",
        "1967-Nov-10 12:34:56",
        __LINE__),                                                       // "%A %d %b, %Y %H:%M:%S"
     DR("Fri 10 Nov, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__),  // "%a %d %b, %Y %H:%M:%S"
     DR("Friday 10 November, 67 12:34:56",
        "2067-Nov-10 12:34:56",
        __LINE__),  // "%A %d %B, %y %H:%M:%S"
     DR("Fri 10 November, 67 12:34:56",
        "2067-Nov-10 12:34:56",
        __LINE__),                                                        // "%a %d %B, %y %H:%M:%S"
     DR("Friday 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),  // "%A %d %b, %y %H:%M:%S"
     DR("Fri 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__),     // "%a %d %b, %y %H:%M:%S"

     DR("10/28/1967 12:34:56", "1967-Oct-28 12:34:56", __LINE__),  //  American Format"%m/%d/%Y %H:%M:%S"
     DR("10/28/67 12:34:56", "2067-Oct-28 12:34:56", __LINE__),    //  American Format"%m/%d/%y %H:%M:%S"

     DR("28/11/1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__),  // "%d/%m/%Y %H:%M:%S"
     DR("28/11/67 12:34:56", "2067-Nov-28 12:34:56", __LINE__),    // "%d/%m/%y %H:%M:%S"

     DR("28.11.1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__),  // "%d.%m.%Y %H:%M:%S"
     DR("28.11.67 12:34:56", "2067-Nov-28 12:34:56", __LINE__),    // "%d.%m.%y %H:%M:%S"

     DR("28-11-1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__),  // "%d-%m-%Y %H:%M:%S"
     DR("28-11-67 12:34:56", "2067-Nov-28 12:34:56", __LINE__),    // "%d-%m-%y %H:%M:%S"

     DR("19671110_123456", "1967-Nov-10 12:34:56", __LINE__),  // "%Y%m%d_%H%M%S"
     DR("671110_123456", "2067-Nov-10 12:34:56", __LINE__),    // "%y%m%d_%H%M%S"

     //        // time-only (USES CURRENT-DATE! Need to test separately.)
     //        DR("12:34:56", "2014-Jan-27 12:34:56",__LINE__),                       // "%H:%M:%S" time only
     //        DR("12:34", "2014-Jan-27 12:34:00",__LINE__),                          // "%H:%M" time only

     DR("November 10 1967", "1967-Nov-10 00:00:00", __LINE__),  // "%B %d %Y" American Format
     DR("Nov 10 1967", "1967-Nov-10 00:00:00", __LINE__),       // "%b %d %Y" American Format
     DR("November 10 67", "2067-Nov-10 00:00:00", __LINE__),    // "%B %d %y" American Format
     DR("Nov 10 67", "2067-Nov-10 00:00:00", __LINE__),         // "%b %d %y" American Format

     DR("10 November 1967", "1967-Nov-10 00:00:00", __LINE__),  // "%d %B %Y"
     DR("10 Nov 1967", "1967-Nov-10 00:00:00", __LINE__),       // "%d %b %Y"
     DR("10 November 67", "2067-Nov-10 00:00:00", __LINE__),    // "%d %B %y"
     DR("10 Nov 67", "2067-Nov-10 00:00:00", __LINE__),         // "%d %b %y"

     DR("Friday 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__),  // "%A %d %B, %Y"
     DR("Fri 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__),     // "%a %d %B, %Y"
     DR("Friday 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__),       // "%A %d %b, %Y"
     DR("Fri 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__),          // "%a %d %b, %Y"
     DR("Friday 10 November, 67", "2067-Nov-10 00:00:00", __LINE__),    // "%A %d %B, %y"
     DR("Fri 10 November, 67", "2067-Nov-10 00:00:00", __LINE__),       // "%a %d %B, %y"
     DR("Friday 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__),         // "%A %d %b, %y"
     DR("Fri 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__),            // "%a %d %b, %y"

     DR("10/28/1967", "1967-Oct-28 00:00:00", __LINE__),  // "%m/%d/%Y" American Format
     DR("10/28/19", "2019-Oct-28 00:00:00", __LINE__),    // "%m/%d/%y" American Format
     DR("28/11/1967", "1967-Nov-28 00:00:00", __LINE__),  // "%d/%m/%Y"
     DR("28/11/67", "2067-Nov-28 00:00:00", __LINE__),    // "%d/%m/%y"
     DR("28.11.1967", "1967-Nov-28 00:00:00", __LINE__),  // "%d.%m.%Y"
     DR("28.11.67", "2067-Nov-28 00:00:00", __LINE__),    // "%d.%m.%y"
     DR("28-11-1967", "1967-Nov-28 00:00:00", __LINE__),  // "%d-%m-%Y"
     DR("28-11-67", "2067-Nov-28 00:00:00", __LINE__),    // "%d-%m-%y"
     DR("19671110", "1967-Nov-10 00:00:00", __LINE__),    // "%Y%m%d"
     DR("671110", "6711-Oct-01 00:00:00", __LINE__),      // "%y%m%d"

     // test formats that might require leading zeros
     DR("November 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // American Format"%B %d %Y %H:%M:%S"
     DR("Nov 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),       // American Format"%b %d %Y %H:%M:%S"
     DR("November 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // American Format"%B %d %y %H:%M:%S"
     DR("Nov 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),         // American Format"%b %d %y %H:%M:%S"

     DR("3 November 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // "%d %B %Y %H:%M:%S"
     DR("3 Nov 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),       // "%d %b %Y %H:%M:%S"
     DR("3 November 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // "%d %B %y %H:%M:%S"
     DR("3 Nov 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),         // "%d %b %y %H:%M:%S"

     DR("Friday 3 November, 1967 12:34:56",
        "1967-Nov-03 12:34:56",
        __LINE__),  // "%A %d %B, %Y %H:%M:%S"
     DR("Fri 3 November, 1967 12:34:56",
        "1967-Nov-03 12:34:56",
        __LINE__),                                                         // "%a %d %B, %Y %H:%M:%S"
     DR("Friday 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),  // "%A %d %b, %Y %H:%M:%S"
     DR("Fri 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__),     // "%a %d %b, %Y %H:%M:%S"
     DR("Friday 3 November, 67 12:34:56",
        "2067-Nov-03 12:34:56",
        __LINE__),                                                         // "%A %d %B, %y %H:%M:%S"
     DR("Fri 3 November, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),  // "%a %d %B, %y %H:%M:%S"
     DR("Friday 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),    // "%A %d %b, %y %H:%M:%S"
     DR("Fri 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__),       // "%a %d %b, %y %H:%M:%S"

     DR("3/28/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  //  American Format"%m/%d/%Y %H:%M:%S"
     DR("3/28/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    //  American Format"%m/%d/%y %H:%M:%S"
     DR("10/3/1967 12:34:56", "1967-Oct-03 12:34:56", __LINE__),  //  American Format"%m/%d/%Y %H:%M:%S"
     DR("10/3/67 12:34:56", "2067-Oct-03 12:34:56", __LINE__),    //  American Format"%m/%d/%y %H:%M:%S"
     DR("9/3/1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__),   //  American Format"%m/%d/%Y %H:%M:%S"
     DR("9/3/67 12:34:56", "2067-Sep-03 12:34:56", __LINE__),     //  American Format"%m/%d/%y %H:%M:%S"

     DR("3/11/1967 12:34:56", "1967-Mar-11 12:34:56", __LINE__),  // "%d/%m/%Y %H:%M:%S"
     DR("3/11/67 12:34:56", "2067-Mar-11 12:34:56", __LINE__),    // "%d/%m/%y %H:%M:%S"
     DR("28/3/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  // "%d/%m/%Y %H:%M:%S"
     DR("28/3/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    // "%d/%m/%y %H:%M:%S"
     DR("9/3/1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__),   // "%d/%m/%Y %H:%M:%S"
     DR("9/3/67 12:34:56", "2067-Sep-03 12:34:56", __LINE__),     // "%d/%m/%y %H:%M:%S"

     DR("3.11.1967 12:34:56", "1967-Mar-11 12:34:56", __LINE__),  // "%d.%m.%Y %H:%M:%S"
     DR("3.11.67 12:34:56", "2067-Mar-11 12:34:56", __LINE__),    // "%d.%m.%y %H:%M:%S"
     DR("28.3.1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  // "%d.%m.%Y %H:%M:%S"
     DR("28.3.67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    // "%d.%m.%y %H:%M:%S"
     DR("9.3.1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__),   // "%d.%m.%Y %H:%M:%S"
     DR("9.3.67 12:34:56", "2067-Sep-03 12:34:56", __LINE__),     // "%d.%m.%y %H:%M:%S"

     DR("3-11-1967 12:34:56", "1967-Mar-11 12:34:56", __LINE__),  // "%d-%m-%Y %H:%M:%S"
     DR("3-11-67 12:34:56", "2067-Mar-11 12:34:56", __LINE__),    // "%d-%m-%y %H:%M:%S"
     DR("28-3-1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__),  // "%d-%m-%Y %H:%M:%S"
     DR("28-3-67 12:34:56", "2067-Mar-28 12:34:56", __LINE__),    // "%d-%m-%y %H:%M:%S"
     DR("9-3-1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__),   // "%d-%m-%Y %H:%M:%S"
     DR("9-3-67 12:34:56", "2067-Sep-03 12:34:56", __LINE__),     // "%d-%m-%y %H:%M:%S"
    };
    for(size_t i = 0; i < sizeof(scanResults) / sizeof(DR); i++)
    {
        ASSERT_TRUE(scanResults[i].correctResult());
    }
}
