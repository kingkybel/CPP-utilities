/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/csvutil_test.cc
 * Description: Unit tests for CSV utilities
 *
 * Copyright (C) 2020 Dieter J Kybelksties <github@kybelksties.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @date: 2023-08-25
 * @author: Dieter J Kybelksties
 */

#include "csvutil.h"
#include "dateutil.h"
#include "stringutil.h"
// #define DO_TRACE_
#include "traceutil.h"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include <algorithm>
#include <map>
#include <set>
#include <vector>

using namespace std;
using namespace util;
using namespace util::datescan;
using namespace boost::date_time;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::filesystem;

const string filename = "/tmp/test.csv";

using namespace std;
using namespace util;

class CsvUtilTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
        if(is_regular_file(filename))
        {
            remove(path(filename));
        }
    }
};

TEST_F(CsvUtilTest, test_different_value_types)
{
    initDateFormats();
    {
        CSVAnalyzer csv("Col1,Col2,Col3,Col4", "Text,d,real,ordInal");
        csv << string("abc, 10/11/67, 3.14159265, 5");

        ASSERT_EQ(csv.getString(0, 0), "abc");
        ASSERT_EQ(csv.getDate(1, 0), VAR_DATE(boost::gregorian::date(2067, 11, 10)));
        ASSERT_DOUBLE_EQ(csv.getFloat(2, 0), 3.14159265L);
        ASSERT_EQ(csv.getUint(3, 0), VAR_UINT(5));
    }
}

TEST_F(CsvUtilTest, test_header_only)
{
    CSVAnalyzer csv("Col1,Col2,Col3,Col4");
    csv << string("abc, 10/11/67, 3.14159265, 5");

    ASSERT_EQ(csv.getString(0, 0), VAR_STRING("abc"));
    ASSERT_TRUE(csv.getDate(1, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
    ASSERT_DOUBLE_EQ(csv.getFloat(2, 0), 3.14159265L);
    ASSERT_EQ(csv.getInt(3, 0), VAR_INT(5));
}

TEST_F(CsvUtilTest, test_default_construct)
{
    // BOOST_TEST_MESSAGE("Default Construct (no header /types rows)");
    CSVAnalyzer csv;
    // BOOST_TEST_MESSAGE("create default headers and types");
    csv << string("abc, 10/11/67, 3.14159265, -10,0,8,yes,Off,"
                  "999999999999999999,1000000000000000000,9223372036854775807,"
                  "9223372036854775808,10223372036854775807");

    ASSERT_EQ(csv.getString(0, 0), VAR_STRING("abc"));
    ASSERT_TRUE(csv.getDate(1, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
    ASSERT_DOUBLE_EQ(csv.getFloat(2, 0), VAR_FLOAT(3.14159265L));
    ASSERT_EQ(csv.getInt(3, 0), VAR_INT(-10));
    ASSERT_EQ(csv.getInt(4, 0), VAR_INT(0));
    ASSERT_EQ(csv.getInt(5, 0), VAR_INT(8));
    ASSERT_EQ(csv.getBool(6, 0), VAR_BOOL(true));
    ASSERT_EQ(csv.getBool(7, 0), VAR_BOOL(false));
    ASSERT_EQ(csv.getInt(8, 0), VAR_INT(999999999999999999));         // last one converted to  int
    ASSERT_EQ(csv.getUint(9, 0), VAR_UINT(1000000000000000000));      // first one converted to uint
    ASSERT_EQ(csv.getUint(10, 0), VAR_UINT(9223372036854775807));     // max long long
    ASSERT_EQ(csv.getUint(11, 0), VAR_UINT(9223372036854775808UL));   // one bigger
    ASSERT_EQ(csv.getUint(12, 0), VAR_UINT(10223372036854775807UL));  // a lot bigger
}

TEST_F(CsvUtilTest, test_get_sub_csv)
{
    CSVAnalyzer csv;
    csv << string("abc, 10/11/67, 3.14159265, -10,0,8,yes,Off");

    ASSERT_EQ(csv.getString(0, 0), VAR_STRING("abc"));
    ASSERT_TRUE(csv.getDate(1, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
    ASSERT_DOUBLE_EQ(csv.getFloat(2, 0), 3.14159265L);
    ASSERT_EQ(csv.getInt(3, 0), VAR_INT(-10));
    ASSERT_EQ(csv.getInt(4, 0), VAR_INT(0));
    ASSERT_EQ(csv.getInt(5, 0), VAR_INT(8));
    ASSERT_EQ(csv.getBool(6, 0), VAR_BOOL(true));
    ASSERT_EQ(csv.getBool(7, 0), VAR_BOOL(false));

    csv = csv.getSub({1, 2, 5, 6});
    ASSERT_TRUE(csv.getDate(0, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
    ASSERT_DOUBLE_EQ(csv.getFloat(1, 0), 3.14159265L);
    ASSERT_EQ(csv.getInt(2, 0), VAR_INT(8));
    ASSERT_EQ(csv.getBool(3, 0), true);

    csv = csv.getSub({"Column2", "Column6"});
    ASSERT_DOUBLE_EQ(csv.getFloat(0, 0), 3.14159265L);
    ASSERT_EQ(csv.getBool(1, 0), true);
}

TEST_F(CsvUtilTest, test_read_write_to_file)

{
    CSVAnalyzer csv("Col1,Col2,Col3,Col4", "Text,d,real,ordInal");
    csv << string("abc, 10/11/67, 3.14159265, 5");
    csv << string("def, 10/03/74, 1.41421356, 10");

    csv.write(filename);
    {
        std::ifstream  ifs(filename.c_str());
        vector<string> lines;
        string         line;
        while(!ifs.eof())
        {
            getline(ifs, line);
            lines.push_back(line);
        }
        ASSERT_EQ(lines.size(), 5UL);
        ASSERT_EQ(lines[0], VAR_STRING("Col1,Col2,Col3,Col4"));
        ASSERT_EQ(lines[1], VAR_STRING("string,date,float,uint"));
        ASSERT_EQ(lines[2], VAR_STRING("abc,2067-Nov-10 00:00:00,3.14159,5"));
        ASSERT_EQ(lines[3], VAR_STRING("def,2074-Mar-10 00:00:00,1.41421,10"));
    }
    csv.read(filename);
    ASSERT_EQ(csv.columns(), 4UL);
    ASSERT_EQ(csv.lines(), 2UL);
    ASSERT_EQ(csv.header(0), VAR_STRING("Col1"));
    ASSERT_EQ(csv.header(3), VAR_STRING("Col4"));
    ASSERT_EQ(csv.type(1), VAR_STRING("date"));
    ASSERT_EQ(csv.type(2), VAR_STRING("float"));
    ASSERT_EQ(csv.getString("Col1", 1), VAR_STRING("def"));
    ASSERT_DOUBLE_EQ(csv.getFloat("Col3", 0), 3.14159);
    ASSERT_TRUE(csv.getDate("Col2", 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));

    // BOOST_TEST_MESSAGE("write using different delimiter");
    csv.write(filename, " | ");
    {
        std::ifstream  ifs(filename.c_str());
        vector<string> lines;
        string         line;
        while(!ifs.eof())
        {
            getline(ifs, line);
            lines.push_back(line);
        }
        ASSERT_EQ(lines.size(), 5UL);
        ASSERT_EQ(lines[0], VAR_STRING("Col1 | Col2 | Col3 | Col4"));
        ASSERT_EQ(lines[1], VAR_STRING("string | date | float | uint"));
        ASSERT_EQ(lines[2], VAR_STRING("abc | 2067-Nov-10 00:00:00 | 3.14159 | 5"));
        ASSERT_EQ(lines[3], VAR_STRING("def | 2074-Mar-10 00:00:00 | 1.41421 | 10"));
    }

    // BOOST_TEST_MESSAGE("read using different delimiter");
    csv.read(filename, "|");
    ASSERT_EQ(csv.columns(), 4UL);
    ASSERT_EQ(csv.lines(), 2UL);
    ASSERT_EQ(csv.header(0), VAR_STRING("Col1"));
    ASSERT_EQ(csv.header(3), VAR_STRING("Col4"));
    ASSERT_EQ(csv.type(1), VAR_STRING("date"));
    ASSERT_EQ(csv.type(2), VAR_STRING("float"));
    ASSERT_EQ(csv.getString("Col1", 1), VAR_STRING("def"));
    ASSERT_DOUBLE_EQ(csv.getFloat("Col3", 0), 3.14159);
    ASSERT_TRUE(csv.getDate("Col2", 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));

    // BOOST_TEST_MESSAGE("write using different delimiter and without header/types");
    csv.write(filename, " & ", CSVAnalyzer::hasValues);
    {
        std::ifstream  ifs(filename.c_str());
        vector<string> lines;
        string         line;
        while(!ifs.eof())
        {
            getline(ifs, line);
            lines.push_back(line);
        }
        ASSERT_EQ(lines.size(), 3UL);
        ASSERT_EQ(lines[0], VAR_STRING("abc & 2067-Nov-10 00:00:00 & 3.14159 & 5"));
        ASSERT_EQ(lines[1], VAR_STRING("def & 2074-Mar-10 00:00:00 & 1.41421 & 10"));
    }

    // BOOST_TEST_MESSAGE("read using different delimiter and without header/types");
    csv.read(filename, "&", CSVAnalyzer::hasValues);
    ASSERT_EQ(csv.columns(), 4UL);
    ASSERT_EQ(csv.lines(), 2UL);
    ASSERT_EQ(csv.header(0), VAR_STRING("Column0"));
    ASSERT_EQ(csv.header(3), VAR_STRING("Column3"));
    ASSERT_EQ(csv.type(1), VAR_STRING("date"));
    ASSERT_EQ(csv.type(2), VAR_STRING("float"));
    ASSERT_EQ(csv.getString("Column0", 1), VAR_STRING("def"));
    ASSERT_DOUBLE_EQ(csv.getFloat("Column2", 0), 3.14159);
    ASSERT_TRUE(csv.getDate("Column1", 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));

    // BOOST_TEST_MESSAGE("directly write csv using stream");
    {
        // write a sample data file
        std::ofstream ofs(filename.c_str());
        ofs << "Cloud , Rain         , Sprinkler , WetGrass" << endl;
        ofs << "bool  , string       , int       , bool" << endl;
        ofs << "yes   , none         , 5         , yes" << endl;
        ofs << "no    , none         , 1         , no" << endl;
        ofs << "yes   , heavy shower , 2         , yes" << endl;
        ofs << "yes   , light shower , 0         , yes" << endl;
    }
    CSVAnalyzer data;
    data.read(filename);
    ASSERT_EQ(data.columns(), 4UL);
    ASSERT_EQ(data.lines(), 4UL);
    ASSERT_EQ(data.get<string>("Rain", 2), VAR_STRING("heavy shower"));
    ASSERT_EQ(data.get<bool>(3, 2), true);
    ASSERT_TRUE(data.begin("Rain") != data.end("Rain"));
    CSVAnalyzer::COLUMN_TYPE_ITER it        = data.begin("Rain");
    size_t                        lineCount = 0;
    for(; it != data.end("Rain"); it++, lineCount++)
        ;
    // BOOST_TEST_MESSAGE(*it);
    ASSERT_TRUE(it == data.end("Rain"));
    ASSERT_EQ(lineCount, data.lines() + 2);

    vector<string> v;
    v.push_back("Rain");
    v.push_back("Cloud");

    CSVAnalyzer sub = data.getSub(v);
    ASSERT_EQ(sub.columns(), 2UL);
    ASSERT_EQ(sub.lines(), 4UL);
    ASSERT_THROW(sub.get<string>("Sprinkler", 2), index_error);
    ASSERT_TRUE(sub.begin("Rain") != sub.end("Rain"));
    ASSERT_THROW(sub.begin("Sprinkler"), index_error);
    it                  = sub.begin("Rain");
    size_t lineCountSub = 0;
    for(; it != sub.end("Rain"); it++, lineCountSub++)
        ;
    // BOOST_TEST_MESSAGE(*it);
    ASSERT_TRUE(it == sub.end("Rain"));
    ASSERT_EQ(lineCountSub, sub.lines() + 2);
    ASSERT_EQ(lineCountSub, lineCount);

    // BOOST_TEST_MESSAGE("make sure that columns are in the correct order");
    ASSERT_EQ(sub.header(0), VAR_STRING("Rain"));
    ASSERT_EQ(sub.header(1), VAR_STRING("Cloud"));
    ASSERT_EQ(sub.type(0), VAR_STRING("string"));
    ASSERT_EQ(sub.type(1), VAR_STRING("bool"));
    for(size_t line = 0; line < sub.lines(); line++)
    {
        ASSERT_EQ(sub.getString(0, line), sub.getString("Rain", line));
        ASSERT_EQ(sub.getBool(1, line), sub.getBool("Cloud", line));
    }
}
