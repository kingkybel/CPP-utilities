/*
 * File:		anyutilTest.cc
 * Description:         Unit tests for string utilities
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

#include "anyutilTest.h"

#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>

#include <stringutil.h>
#include <dateutil.h>
#include <anyutil.h>

using namespace std;
using namespace util;
using namespace util::datescan;

CPPUNIT_TEST_SUITE_REGISTRATION(anyutilTest);

anyutilTest::anyutilTest()
{
}

anyutilTest::~anyutilTest()
{
}

void anyutilTest::setUp()
{
}

void anyutilTest::tearDown()
{
}

void anyutilTest::util_any_test()
{
    //BOOST_TEST_MESSAGE("");
    //BOOST_TEST_MESSAGE("====== Testing any templates functions ========");

    //BOOST_TEST_MESSAGE("boolean values...");
    bool bYes = scanAs<bool>("t");
    CPPUNIT_ASSERT_EQUAL(bYes, true);
    bYes = scanAs<bool>("TrUe");
    CPPUNIT_ASSERT_EQUAL(bYes, true);
    bYes = scanAs<bool>("yEs");
    CPPUNIT_ASSERT_EQUAL(bYes, true);
    bYes = scanAs<bool>("oN");
    CPPUNIT_ASSERT_EQUAL(bYes, true);
    CPPUNIT_ASSERT_EQUAL(bYes, true);

    bool bNo = scanAs<bool>("F");
    CPPUNIT_ASSERT_EQUAL(bNo, false);
    bNo = scanAs<bool>("F");
    CPPUNIT_ASSERT_EQUAL(bNo, false);
    bNo = scanAs<bool>("fAlSe");
    CPPUNIT_ASSERT_EQUAL(bNo, false);
    bNo = scanAs<bool>("ofF");
    CPPUNIT_ASSERT_EQUAL(bNo, false);
    bNo = scanAs<bool>("NO");
    CPPUNIT_ASSERT_EQUAL(bNo, false);

    //BOOST_TEST_MESSAGE("int values...");
    VAR_INT i = scanAs<VAR_INT>("1234");
    CPPUNIT_ASSERT_EQUAL(i, VAR_INT(1234));
    i = scanAs<VAR_INT>("-666");
    CPPUNIT_ASSERT_EQUAL(i, VAR_INT(-666));
    VAR_UINT ui = scanAs<VAR_UINT>("1234");
    CPPUNIT_ASSERT_EQUAL(ui, VAR_UINT(1234));

    //BOOST_TEST_MESSAGE("float values...");
    VAR_FLOAT f = scanAs<VAR_FLOAT>("-1234.3456");
    CPPUNIT_ASSERT_DOUBLES_EQUAL(f, -1234.3456L, VAR_INT(1e-27));
    f = scanAs<VAR_FLOAT>("1234.34e-31");
    CPPUNIT_ASSERT_DOUBLES_EQUAL(f, 1234.34e-31L, VAR_INT(1e-27));

    Var anAny(string("a T_"));
    CPPUNIT_ASSERT(isA<string>(anAny));
    anAny = (VAR_INT) 5L;
    CPPUNIT_ASSERT(isA<VAR_INT>(anAny));

    initDateFormats();
    anAny = scanAsVar<VAR_DATE>("1/11/2012 12:45:21");
    CPPUNIT_ASSERT(anAny == VAR_DATE(toDate(2012, 11, 1, 12, 45, 21)));

    Var a1(VAR_UINT(1234UL));
    Var a2(VAR_UINT(1235UL));
    Var a3(string("A"));
    Var a4(string("B"));
    Var a5(false);
    Var a6(true);

    CPPUNIT_ASSERT(a1 < a2);
    CPPUNIT_ASSERT(a3 < a4);
    CPPUNIT_ASSERT(a5 < a6);
    CPPUNIT_ASSERT(a1 <= a2);
    CPPUNIT_ASSERT(a3 <= a4);
    CPPUNIT_ASSERT(a5 <= a6);
    CPPUNIT_ASSERT(a1 <= a2);
    CPPUNIT_ASSERT(a3 <= a4);
    CPPUNIT_ASSERT(a5 <= a6);
    CPPUNIT_ASSERT(a2 >= a1);
    CPPUNIT_ASSERT(a4 >= a3);
    CPPUNIT_ASSERT(a6 >= a5);
    CPPUNIT_ASSERT(a2 >= a1);
    CPPUNIT_ASSERT(a4 >= a3);
    CPPUNIT_ASSERT(a6 >= a5);

    Var v1 = VAR_UINT(1234UL);
    Var v2 = VAR_UINT(1235UL);
    CPPUNIT_ASSERT(v1 < v2);

    Interval<VAR_FLOAT> itv_0L_1L(0.0L, 1.0L);
    CPPUNIT_ASSERT(itv_0L_1L.contains(0.5));
    Interval<VAR_FLOAT> itv_minus1_2(-1.0L, 2.0L);
    CPPUNIT_ASSERT(itv_0L_1L.isSubIntervalOf(itv_minus1_2));

    VAR_UINT_INTERVAL itv_minus_oo_oo;
    CPPUNIT_ASSERT(itv_minus_oo_oo.contains(VAR_UINT(8)));
    CPPUNIT_ASSERT(VAR_UINT_INTERVAL(2,{infiniteMax}).isSubIntervalOf(itv_minus_oo_oo));
    CPPUNIT_ASSERT(VAR_UINT_INTERVAL(2,{infiniteMin}).isSubIntervalOf(itv_minus_oo_oo));
    CPPUNIT_ASSERT(VAR_UINT_INTERVAL(4ULL, 12344ULL).isSubIntervalOf(itv_minus_oo_oo));
    VAR_FLOAT_INTERVAL itv_5_oo(5.0L,{infiniteMax});
    CPPUNIT_ASSERT(!itv_5_oo.contains(VAR_FLOAT(4.0)));
    CPPUNIT_ASSERT(!VAR_FLOAT_INTERVAL(2.234,{infiniteMax}).isSubIntervalOf(itv_5_oo));
    CPPUNIT_ASSERT(!VAR_FLOAT_INTERVAL(2.234,{infiniteMin}).isSubIntervalOf(itv_5_oo));

    // [c, +oo]
    VAR_CHAR_INTERVAL itv_c_oo = VAR_CHAR_INTERVAL(VAR_CHAR('c'));
    CPPUNIT_ASSERT(itv_c_oo.contains(VAR_CHAR('z')));
    CPPUNIT_ASSERT(!VAR_CHAR_INTERVAL('f',{infiniteMin}).isSubIntervalOf(itv_c_oo));
    CPPUNIT_ASSERT(!VAR_CHAR_INTERVAL('a',{infiniteMax}).isSubIntervalOf(itv_c_oo));
}

template<typename T_>
void util_any_interval_testT(T_ i, T_ j)
{
    //BOOST_TEST_MESSAGE("");
    //BOOST_TEST_MESSAGE("====== Testing any interval functions ========");
    typedef Interval<T_> INTERV;

    CPPUNIT_ASSERT(INTERV() == INTERV());
    CPPUNIT_ASSERT(!(INTERV() < INTERV()));
    set<INTERV> s;
    CPPUNIT_ASSERT(s.empty());
    s.insert(INTERV());
    CPPUNIT_ASSERT(!s.empty());
    CPPUNIT_ASSERT_EQUAL(s.size(), 1UL);
    CPPUNIT_ASSERT(s.find(INTERV()) != s.end());
    typename set<INTERV>::iterator found = s.find(INTERV());
    CPPUNIT_ASSERT_EQUAL(*found, INTERV());
    s.insert(INTERV(i, j));
    CPPUNIT_ASSERT(!s.empty());
    CPPUNIT_ASSERT_EQUAL(s.size(), 2UL);
    CPPUNIT_ASSERT(s.find(INTERV(i, j)) != s.end());
    found = s.find(INTERV(i, j));
    CPPUNIT_ASSERT_EQUAL(*found, INTERV(i, j));
}

void anyutilTest::util_any_interval_test()
{
    util_any_interval_testT<VAR_CHAR>('a', 'z');
    util_any_interval_testT<VAR_INT>(-5, 10);
    util_any_interval_testT<VAR_UINT>(5, 10);
    util_any_interval_testT<VAR_FLOAT>(5.0L, 10.0L);
    util_any_interval_testT<VAR_DATE>(toDate(2014, 1, 24), toDate(2015, 12, 3));
}
