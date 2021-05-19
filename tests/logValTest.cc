/*
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
 * @date: 2019-10-19
 * @author: Dieter J Kybelksties
 */

#include <algorithm>
#include <chrono>
#include <iostream>
#include <logvalue.h>
#include <random>
#include <sstream>
#define DO_TRACE_
#include "logValTest.h"

#include <matrix.h>
#include <stringutil.h>
#include <traceutil.h>

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(logValTest);

logValTest::logValTest()
{
}

logValTest::~logValTest()
{
}

void logValTest::setUp()
{
}

void logValTest::tearDown()
{
}

template<typename RESLT1, typename RESLT2>
string failMessage(const string &oper, long double a, long double b, RESLT1 res1, logVal l_a, logVal l_b, RESLT2 res2)
{
    stringstream ss;
    ss << "Failed: (" << a << ") " << oper << " (" << b << ") == " << res1 << " but (" << l_a << ") " << oper << " ("
       << l_b << ") == " << res2;
    return ss.str();
}

void logValTest::testLogVal()
{
    // construct a trivial random generator engine from a time-based seed:
    unsigned                   seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);

    uniform_real_distribution<long double> distribution(-10000.0L, 10000.0L);
    vector<long double>                    dblVec(100);
    generate(dblVec.begin(), dblVec.end(), [&]() { return distribution(generator); });
    for(size_t i = 0; i < dblVec.size() - 1; i++)
    {
        long double a   = dblVec[i];
        long double b   = dblVec[i + 1];
        logVal      l_a = a;
        logVal      l_b = b;
        CPPUNIT_ASSERT_EQUAL(a == b, l_a == l_b);
        CPPUNIT_ASSERT_EQUAL(a < b, l_a < l_b);
        CPPUNIT_ASSERT_EQUAL(a <= b, l_a <= l_b);
        CPPUNIT_ASSERT_EQUAL(a > b, l_a > l_b);
        CPPUNIT_ASSERT_EQUAL(a >= b, l_a >= l_b);

        auto result   = a + b;
        auto result_l = l_a + l_b;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("+", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        result = a;
        result += b;
        result_l = l_a;
        result_l += l_b;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("+=", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        result++;
        result_l++;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("()++", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        ++result;
        ++result_l;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("++()", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        result   = a - b;
        result_l = l_a - l_b;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("-", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        result--;
        result_l--;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("()--", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        --result;
        --result_l;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("--()", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        result = a;
        result -= b;
        result_l = l_a;
        result_l -= l_b;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("-=", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        result   = a * b;
        result_l = l_a * l_b;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("*", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        result = a;
        result *= b;
        result_l = l_a;
        result_l *= l_b;
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("*=", a, b, result, l_a, l_b, result_l),
                                             result,
                                             result_l.toReal(),
                                             abs(result / 1e8));

        if(b != 0.0L)
        {
            result   = a / b;
            result_l = l_a / l_b;
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("/", a, b, result, l_a, l_b, result_l),
                                                 result,
                                                 result_l.toReal(),
                                                 abs(result / 1e8));

            result = a;
            result /= b;
            result_l = l_a;
            result_l /= l_b;
            CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE(failMessage("/=", a, b, result, l_a, l_b, result_l),
                                                 result,
                                                 result_l.toReal(),
                                                 abs(result / 1e8));
        }
    }

    // test for +/- zero equality
    logVal neg_val = logVal::fromLog(-INFINITY, false);
    logVal pos_val = logVal::fromLog(-INFINITY, true);
    CPPUNIT_ASSERT_EQUAL(pos_val, neg_val);
}
