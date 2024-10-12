/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/anyutil_tests.cc
 * Description: Unit tests for any utilities.
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
#include "dateutil.h"
#include <gtest/gtest.h>
#include <map>
#include <set>
#include <string>

using namespace std;
using namespace util;
using namespace util::datescan;

class AnyUtilTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(AnyUtilTest, util_any_test)
{
    bool bYes = scanAs<bool>("t");
    ASSERT_EQ(bYes, true);
    bYes = scanAs<bool>("TrUe");
    ASSERT_EQ(bYes, true);
    bYes = scanAs<bool>("yEs");
    ASSERT_EQ(bYes, true);
    bYes = scanAs<bool>("oN");
    ASSERT_EQ(bYes, true);
    ASSERT_EQ(bYes, true);

    bool bNo = scanAs<bool>("F");
    ASSERT_EQ(bNo, false);
    bNo = scanAs<bool>("F");
    ASSERT_EQ(bNo, false);
    bNo = scanAs<bool>("fAlSe");
    ASSERT_EQ(bNo, false);
    bNo = scanAs<bool>("ofF");
    ASSERT_EQ(bNo, false);
    bNo = scanAs<bool>("NO");
    ASSERT_EQ(bNo, false);

    // BOOST_TEST_MESSAGE("int values...");
    VAR_INT i = scanAs<VAR_INT>("1234");
    ASSERT_EQ(i, VAR_INT(1234));
    i = scanAs<VAR_INT>("-666");
    ASSERT_EQ(i, VAR_INT(-666));
    VAR_UINT ui = scanAs<VAR_UINT>("1234");
    ASSERT_EQ(ui, VAR_UINT(1234));

    // BOOST_TEST_MESSAGE("float values...");
    VAR_FLOAT f = scanAs<VAR_FLOAT>("-1234.3456");
    ASSERT_DOUBLE_EQ(f, -1234.3456L);
    f = scanAs<VAR_FLOAT>("1234.34e-31");
    ASSERT_DOUBLE_EQ(f, 1234.34e-31L);

    Var anAny(string("a ValueT_"));
    ASSERT_TRUE(isA<string>(anAny));
    anAny = (VAR_INT)5L;
    ASSERT_TRUE(isA<VAR_INT>(anAny));

    initDateFormats();
    anAny = scanAsVar<VAR_DATE>("1/11/2012 12:45:21");
    ASSERT_TRUE(anAny == VAR_DATE(toDate(2012, 11, 1, 12, 45, 21)));

    Var a1(VAR_UINT(1234UL));
    Var a2(VAR_UINT(1235UL));
    Var a3(string("A"));
    Var a4(string("B"));
    Var a5(false);
    Var a6(true);

    ASSERT_LT(a1, a2);
    ASSERT_LT(a3, a4);
    ASSERT_LT(a5, a6);
    ASSERT_LE(a1, a2);
    ASSERT_LE(a3, a4);
    ASSERT_LE(a5, a6);
    ASSERT_LE(a1, a2);
    ASSERT_LE(a3, a4);
    ASSERT_LE(a5, a6);
    ASSERT_GE(a2, a1);
    ASSERT_GE(a4, a3);
    ASSERT_GE(a6, a5);
    ASSERT_GE(a2, a1);
    ASSERT_GE(a4, a3);

    Var v1 = VAR_UINT(1234UL);
    Var v2 = VAR_UINT(1235UL);
    ASSERT_LT(v1, v2);

    Interval<VAR_FLOAT> itv_0L_1L(0.0L, 1.0L);
    ASSERT_TRUE(itv_0L_1L.contains(0.5));
    Interval<VAR_FLOAT> itv_minus1_2(-1.0L, 2.0L);
    ASSERT_TRUE(itv_0L_1L.isSubIntervalOf(itv_minus1_2));

    VAR_UINT_INTERVAL itv_minus_oo_oo;
    ASSERT_TRUE(itv_minus_oo_oo.contains(VAR_UINT(8)));
    ASSERT_TRUE(VAR_UINT_INTERVAL(2, {infiniteMax}).isSubIntervalOf(itv_minus_oo_oo));
    ASSERT_TRUE(VAR_UINT_INTERVAL(2, {infiniteMin}).isSubIntervalOf(itv_minus_oo_oo));
    ASSERT_TRUE(VAR_UINT_INTERVAL(4ULL, 12344ULL).isSubIntervalOf(itv_minus_oo_oo));
    VAR_FLOAT_INTERVAL itv_5_oo(5.0L, {infiniteMax});
    ASSERT_TRUE(!itv_5_oo.contains(VAR_FLOAT(4.0)));
    ASSERT_TRUE(!VAR_FLOAT_INTERVAL(2.234, {infiniteMax}).isSubIntervalOf(itv_5_oo));
    ASSERT_TRUE(!VAR_FLOAT_INTERVAL(2.234, {infiniteMin}).isSubIntervalOf(itv_5_oo));

    // [c, +oo]
    VAR_CHAR_INTERVAL itv_c_oo = VAR_CHAR_INTERVAL(VAR_CHAR('c'));
    ASSERT_TRUE(itv_c_oo.contains(VAR_CHAR('z')));
    ASSERT_FALSE(VAR_CHAR_INTERVAL('f', {infiniteMin}).isSubIntervalOf(itv_c_oo));
    ASSERT_FALSE(VAR_CHAR_INTERVAL('a', {infiniteMax}).isSubIntervalOf(itv_c_oo));
}

template<typename T_>
void util_any_interval_testT(T_ i, T_ j)
{
    typedef Interval<T_> INTERV;

    ASSERT_EQ(INTERV(), INTERV());
    ASSERT_FALSE(INTERV() < INTERV());
    set<INTERV> s;
    ASSERT_TRUE(s.empty());
    s.insert(INTERV());
    ASSERT_FALSE(s.empty());
    ASSERT_EQ(s.size(), 1UL);
    ASSERT_TRUE(s.find(INTERV()) != s.end());
    typename set<INTERV>::iterator found = s.find(INTERV());
    ASSERT_EQ(*found, INTERV());
    s.insert(INTERV(i, j));
    ASSERT_FALSE(s.empty());
    ASSERT_EQ(s.size(), 2UL);
    ASSERT_TRUE(s.find(INTERV(i, j)) != s.end());
    found = s.find(INTERV(i, j));
    ASSERT_EQ(*found, INTERV(i, j));
}

TEST_F(AnyUtilTest, util_any_interval_test)
{
    util_any_interval_testT<VAR_CHAR>('a', 'z');
    util_any_interval_testT<VAR_INT>(-5, 10);
    util_any_interval_testT<VAR_UINT>(5, 10);
    util_any_interval_testT<VAR_FLOAT>(5.0L, 10.0L);
    util_any_interval_testT<VAR_DATE>(toDate(2014, 1, 24), toDate(2015, 12, 3));
}
