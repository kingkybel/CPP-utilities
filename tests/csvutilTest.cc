/*
 * File:		csvutilTest.cc
 * Description:         Unit tests for CSV utilities
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
 * @date: 2020-06-17
 * @author: Dieter J Kybelksties
 */

#include "csvutilTest.h"
#include <iostream>
#include <string>
#include <sstream>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include <map>
#include <vector>
#include <set>
#include <algorithm>

#include <stringutil.h>
#include <dateutil.h>
#include <csvutil.h>

//#define DO_TRACE_
#include <traceutil.h>

using namespace std;
using namespace util;
using namespace util::datescan;
using namespace boost::date_time;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::filesystem;

const string filename = "/tmp/test.csv";

CPPUNIT_TEST_SUITE_REGISTRATION(csvutilTest);

csvutilTest::csvutilTest()
{
}

csvutilTest::~csvutilTest()
{
}

void csvutilTest::setUp()
{
}

void csvutilTest::tearDown()
{
}

void csvutilTest::util_csv_test()
{
    initDateFormats();
    //BOOST_TEST_MESSAGE("");
    //BOOST_TEST_MESSAGE("====== Testing csv functions ========");
    {
        //BOOST_TEST_MESSAGE("Construct with header and type-rows");
        //BOOST_TEST_MESSAGE("Also check whether the case insensitive type-strings work");
        CSVAnalyzer csv("Col1,Col2,Col3,Col4", "Text,d,real,ordInal");
        csv << string("abc, 10/11/67, 3.14159265, 5");

        CPPUNIT_ASSERT(csv.getString(0, 0) == "abc");
        CPPUNIT_ASSERT(csv.getDate(1, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat(2, 0), 3.14159265L, 0.000000001);
        CPPUNIT_ASSERT_EQUAL(csv.getUint(3, 0),VAR_UINT(5));
    }
    {
        //BOOST_TEST_MESSAGE("Construct with header row only");
        CSVAnalyzer csv("Col1,Col2,Col3,Col4");
        //BOOST_TEST_MESSAGE("create types (guessed from field-value)");
        csv << string("abc, 10/11/67, 3.14159265, 5");

        CPPUNIT_ASSERT_EQUAL(csv.getString(0, 0), VAR_STRING("abc"));
        CPPUNIT_ASSERT(csv.getDate(1, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat(2, 0), 3.14159265L, 0.000000001);
        CPPUNIT_ASSERT_EQUAL(csv.getInt(3, 0), VAR_INT(5));
    }
    {
        //BOOST_TEST_MESSAGE("Default Construct (no header /types rows)");
        CSVAnalyzer csv;
        //BOOST_TEST_MESSAGE("create default headers and types");
        csv << string("abc, 10/11/67, 3.14159265, -10,0,8,yes,Off,"
                      "999999999999999999,1000000000000000000,9223372036854775807,"
                      "9223372036854775808,10223372036854775807");

        CPPUNIT_ASSERT_EQUAL(csv.getString(0, 0), VAR_STRING("abc"));
        CPPUNIT_ASSERT(csv.getDate(1, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat(2, 0), VAR_FLOAT(3.14159265L), 0.000000001L);
        CPPUNIT_ASSERT_EQUAL(csv.getInt(3, 0), VAR_INT(-10));
        CPPUNIT_ASSERT_EQUAL(csv.getInt(4, 0), VAR_INT(0));
        CPPUNIT_ASSERT_EQUAL(csv.getInt(5, 0), VAR_INT(8));
        CPPUNIT_ASSERT_EQUAL(csv.getBool(6, 0), VAR_BOOL(true));
        CPPUNIT_ASSERT_EQUAL(csv.getBool(7, 0), VAR_BOOL(false));
        CPPUNIT_ASSERT_EQUAL(csv.getInt(8, 0), VAR_INT(999999999999999999)); // last one converted to  int
        CPPUNIT_ASSERT_EQUAL(csv.getUint(9, 0), VAR_UINT(1000000000000000000)); // first one converted to uint
        CPPUNIT_ASSERT_EQUAL(csv.getUint(10, 0), VAR_UINT(9223372036854775807)); // max long long
        CPPUNIT_ASSERT_EQUAL(csv.getUint(11, 0), VAR_UINT(9223372036854775808UL)); // one bigger
        CPPUNIT_ASSERT_EQUAL(csv.getUint(12, 0), VAR_UINT(10223372036854775807UL)); // a lot bigger
    }
    {
        //BOOST_TEST_MESSAGE("Get sub-csv");
        CSVAnalyzer csv;
        //BOOST_TEST_MESSAGE("create default headers and types");
        csv << string("abc, 10/11/67, 3.14159265, -10,0,8,yes,Off");

        CPPUNIT_ASSERT_EQUAL(csv.getString(0, 0), VAR_STRING("abc"));
        CPPUNIT_ASSERT(csv.getDate(1, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat(2, 0), 3.14159265L, 0.000000001);
        CPPUNIT_ASSERT_EQUAL(csv.getInt(3, 0), VAR_INT(-10));
        CPPUNIT_ASSERT_EQUAL(csv.getInt(4, 0), VAR_INT(0));
        CPPUNIT_ASSERT_EQUAL(csv.getInt(5, 0), VAR_INT(8));
        CPPUNIT_ASSERT_EQUAL(csv.getBool(6, 0), VAR_BOOL(true));
        CPPUNIT_ASSERT_EQUAL(csv.getBool(7, 0), VAR_BOOL(false));

        csv = csv.getSub(1, 2, 5, 6);
        CPPUNIT_ASSERT(csv.getDate(0, 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat(1, 0), 3.14159265L, 0.000000001);
        CPPUNIT_ASSERT_EQUAL(csv.getInt(2, 0), VAR_INT(8));
        CPPUNIT_ASSERT_EQUAL(csv.getBool(3, 0), true);

        csv = csv.getSub("Column2", "Column6");
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat(0, 0), 3.14159265L, 0.000000001);
        CPPUNIT_ASSERT_EQUAL(csv.getBool(1, 0), true);
    }
    {
        //BOOST_TEST_MESSAGE("Read/Write csv to file");
        CSVAnalyzer csv("Col1,Col2,Col3,Col4", "Text,d,real,ordInal");
        csv << string("abc, 10/11/67, 3.14159265, 5");
        csv << string("def, 10/03/74, 1.41421356, 10");

        //BOOST_TEST_MESSAGE("write in default format");
        csv.write(filename);
        {
            std::ifstream ifs(filename.c_str());
            vector<string> lines;
            string line;
            while (!ifs.eof())
            {
                getline(ifs, line);
                lines.push_back(line);
            }
            CPPUNIT_ASSERT_EQUAL(lines.size(), 5UL);
            CPPUNIT_ASSERT_EQUAL(lines[0], VAR_STRING("Col1,Col2,Col3,Col4"));
            CPPUNIT_ASSERT_EQUAL(lines[1], VAR_STRING("string,date,float,uint"));
            CPPUNIT_ASSERT_EQUAL(lines[2], VAR_STRING("abc,2067-Nov-10 00:00:00,3.14159,5"));
            CPPUNIT_ASSERT_EQUAL(lines[3], VAR_STRING("def,2074-Mar-10 00:00:00,1.41421,10"));
        }
        //BOOST_TEST_MESSAGE("read in default format");
        csv.read(filename);
        CPPUNIT_ASSERT_EQUAL(csv.columns(), 4UL);
        CPPUNIT_ASSERT_EQUAL(csv.lines(), 2UL);
        CPPUNIT_ASSERT_EQUAL(csv.header(0), VAR_STRING("Col1"));
        CPPUNIT_ASSERT_EQUAL(csv.header(3), VAR_STRING("Col4"));
        CPPUNIT_ASSERT_EQUAL(csv.type(1), VAR_STRING("date"));
        CPPUNIT_ASSERT_EQUAL(csv.type(2), VAR_STRING("float"));
        CPPUNIT_ASSERT_EQUAL(csv.getString("Col1", 1), VAR_STRING("def"));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat("Col3", 0), 3.14159, 0.000001);
        CPPUNIT_ASSERT(csv.getDate("Col2", 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));

        //BOOST_TEST_MESSAGE("write using different delimiter");
        csv.write(filename, " | ");
        {
            std::ifstream ifs(filename.c_str());
            vector<string> lines;
            string line;
            while (!ifs.eof())
            {
                getline(ifs, line);
                lines.push_back(line);
            }
            CPPUNIT_ASSERT_EQUAL(lines.size(), 5UL);
            CPPUNIT_ASSERT_EQUAL(lines[0], VAR_STRING("Col1 | Col2 | Col3 | Col4"));
            CPPUNIT_ASSERT_EQUAL(lines[1], VAR_STRING("string | date | float | uint"));
            CPPUNIT_ASSERT_EQUAL(lines[2], VAR_STRING("abc | 2067-Nov-10 00:00:00 | 3.14159 | 5"));
            CPPUNIT_ASSERT_EQUAL(lines[3], VAR_STRING("def | 2074-Mar-10 00:00:00 | 1.41421 | 10"));
        }

        //BOOST_TEST_MESSAGE("read using different delimiter");
        csv.read(filename, "|");
        CPPUNIT_ASSERT_EQUAL(csv.columns(), 4UL);
        CPPUNIT_ASSERT_EQUAL(csv.lines(), 2UL);
        CPPUNIT_ASSERT_EQUAL(csv.header(0), VAR_STRING("Col1"));
        CPPUNIT_ASSERT_EQUAL(csv.header(3), VAR_STRING("Col4"));
        CPPUNIT_ASSERT_EQUAL(csv.type(1), VAR_STRING("date"));
        CPPUNIT_ASSERT_EQUAL(csv.type(2), VAR_STRING("float"));
        CPPUNIT_ASSERT_EQUAL(csv.getString("Col1", 1), VAR_STRING("def"));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat("Col3", 0), 3.14159, 0.000001);
       CPPUNIT_ASSERT(csv.getDate("Col2", 0) == VAR_DATE(boost::gregorian::date(2067, 11, 10)));

        //BOOST_TEST_MESSAGE("write using different delimiter and without header/types");
        csv.write(filename, " & ", CSVAnalyzer::hasValues);
        {
            std::ifstream ifs(filename.c_str());
            vector<string> lines;
            string line;
            while (!ifs.eof())
            {
                getline(ifs, line);
                lines.push_back(line);
            }
            CPPUNIT_ASSERT_EQUAL(lines.size(), 3UL);
            CPPUNIT_ASSERT_EQUAL(lines[0], VAR_STRING("abc & 2067-Nov-10 00:00:00 & 3.14159 & 5"));
            CPPUNIT_ASSERT_EQUAL(lines[1], VAR_STRING("def & 2074-Mar-10 00:00:00 & 1.41421 & 10"));
        }

        //BOOST_TEST_MESSAGE("read using different delimiter and without header/types");
        csv.read(filename, "&", CSVAnalyzer::hasValues);
        CPPUNIT_ASSERT_EQUAL(csv.columns(), 4UL);
        CPPUNIT_ASSERT_EQUAL(csv.lines(), 2UL);
        CPPUNIT_ASSERT_EQUAL(csv.header(0), VAR_STRING("Column0"));
        CPPUNIT_ASSERT_EQUAL(csv.header(3), VAR_STRING("Column3"));
        CPPUNIT_ASSERT_EQUAL(csv.type(1), VAR_STRING("date"));
        CPPUNIT_ASSERT_EQUAL(csv.type(2), VAR_STRING("float"));
        CPPUNIT_ASSERT_EQUAL(csv.getString("Column0", 1), VAR_STRING("def"));
        CPPUNIT_ASSERT_DOUBLES_EQUAL(csv.getFloat("Column2", 0), 3.14159, 0.000001);
        CPPUNIT_ASSERT(csv.getDate("Column1", 0) ==VAR_DATE(boost::gregorian::date(2067, 11, 10)));

        //BOOST_TEST_MESSAGE("directly write csv using stream");
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
        CPPUNIT_ASSERT_EQUAL(data.columns(), 4UL);
        CPPUNIT_ASSERT_EQUAL(data.lines(), 4UL);
        CPPUNIT_ASSERT_EQUAL(data.get<string>("Rain", 2), VAR_STRING("heavy shower"));
        CPPUNIT_ASSERT_EQUAL(data.get<bool>(3, 2), true);
        CPPUNIT_ASSERT(data.begin("Rain") != data.end("Rain"));
        CSVAnalyzer::COLUMN_TYPE_ITER it = data.begin("Rain");
        size_t lineCount = 0;
        for (; it != data.end("Rain"); it++, lineCount++);
            //BOOST_TEST_MESSAGE(*it);
        CPPUNIT_ASSERT(it == data.end("Rain"));
        CPPUNIT_ASSERT_EQUAL(lineCount, data.lines() + 2);

        vector<string> v;
        v.push_back("Rain");
        v.push_back("Cloud");

        CSVAnalyzer sub = data.getSub(v);
        CPPUNIT_ASSERT_EQUAL(sub.columns(), 2UL);
        CPPUNIT_ASSERT_EQUAL(sub.lines(), 4UL);
        CPPUNIT_ASSERT_THROW(sub.get<string>("Sprinkler", 2), index_error);
        CPPUNIT_ASSERT(sub.begin("Rain") != sub.end("Rain"));
        CPPUNIT_ASSERT_THROW(sub.begin("Sprinkler"), index_error);
        it = sub.begin("Rain");
        size_t lineCountSub = 0;
        for (; it != sub.end("Rain"); it++, lineCountSub++);
            //BOOST_TEST_MESSAGE(*it);
        CPPUNIT_ASSERT(it == sub.end("Rain"));
        CPPUNIT_ASSERT_EQUAL(lineCountSub, sub.lines() + 2);
        CPPUNIT_ASSERT_EQUAL(lineCountSub, lineCount);

        //BOOST_TEST_MESSAGE("make sure that columns are in the correct order");
        CPPUNIT_ASSERT_EQUAL(sub.header(0), VAR_STRING("Rain"));
        CPPUNIT_ASSERT_EQUAL(sub.header(1), VAR_STRING("Cloud"));
        CPPUNIT_ASSERT_EQUAL(sub.type(0), VAR_STRING("string"));
        CPPUNIT_ASSERT_EQUAL(sub.type(1), VAR_STRING("bool"));
        for (size_t line = 0; line < sub.lines(); line++)
        {
            CPPUNIT_ASSERT_EQUAL(sub.getString(0, line), sub.getString("Rain", line));
            CPPUNIT_ASSERT_EQUAL(sub.getBool(1, line), sub.getBool("Cloud", line));
        }
    }
    if (is_regular_file(filename))
    {
        remove(path(filename));
    }
}

