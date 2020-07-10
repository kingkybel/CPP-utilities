/*
 * File:		LimitedIntTest.cc
 * Description:         Unit tests for limited_values -templates
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
 * @date: 2020-06-04
 * @author: Dieter J Kybelksties
 */

#include "limitedIntTest.h"
#include <iostream>
#include <string>
#include <sstream>

#include <map>
#include <vector>
#include <set>
#include <algorithm>

#include <limited_int.h>
#include <degree_conversion.h>

#define DO_TRACE_
#include <traceutil.h>
#include <stringutil.h>

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(LimitedValuesTest);

LimitedValuesTest::LimitedValuesTest()
{
}

LimitedValuesTest::~LimitedValuesTest()
{
}

void LimitedValuesTest::setUp()
{
}

void LimitedValuesTest::tearDown()
{
}

template<typename T_, T_ min, T_ max>
void checkAllValidValues()
{
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_scale>  traitsModuloScale;
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_circular_scale> traitsModuloCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_scale> traitsInvalidScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_circular_scale>  traitsInvalidCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_scale> traitsThrowScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_circular_scale>  traitsThrowCircularScale;

    typedef limited_int<T_, min, max, traitsThrowScale> testedTypeThrow;
    typedef limited_int<T_, min, max, traitsInvalidCircularScale> testedTypeInvalid;
    typedef limited_int<T_, min, max, traitsModuloCircularScale> testedTypeModulo;

    for (T_ i = min; i <= max; i++)
    {
        try
        {
            testedTypeThrow v = i;
            CPPUNIT_ASSERT(v.isValid());
            if (i > min)
            {
                v = i - 1;
                CPPUNIT_ASSERT(v.isValid());
            }
        }
        catch (...)
        {
            stringstream ss;
            ss << "Construction of limited_int<"
                    << typeid (testedTypeThrow).name()
                    << " with valid value "
                    << i
                    << " failed.";
            CPPUNIT_FAIL(ss.str());
        }
        try
        {
            testedTypeInvalid v = i;
            CPPUNIT_ASSERT(v.isValid());
            if (i > min)
            {
                v = i - 1;
                CPPUNIT_ASSERT(v.isValid());
            }
        }
        catch (...)
        {
            stringstream ss;
            ss << "Construction of limited_int<"
                    << typeid (testedTypeInvalid).name()
                    << " with valid value "
                    << i
                    << " failed.";
            CPPUNIT_FAIL(ss.str());
        }
        try
        {
            testedTypeModulo v = i;
            CPPUNIT_ASSERT(v.isValid());
            if (i > min)
            {
                v = i - 1;
                CPPUNIT_ASSERT(v.isValid());
            }
        }
        catch (...)
        {
            stringstream ss;
            ss << "Construction of limited_int<"
                    << typeid (testedTypeModulo).name()
                    << " with valid value "
                    << i
                    << " failed.";
            CPPUNIT_FAIL(ss.str());
        }
    }
}

template<typename T_, T_ min, T_ max>
void checkInvalidValues()
{
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_scale>  traitsModuloScale;
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_circular_scale> traitsModuloCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_scale> traitsInvalidScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_circular_scale>  traitsInvalidCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_scale> traitsThrowScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_circular_scale>  traitsThrowCircularScale;

    typedef limited_int<T_, min, max, traitsThrowScale> testedTypeThrow;
    typedef limited_int<T_, min, max, traitsInvalidCircularScale> testedTypeInvalid;
    typedef limited_int<T_, min, max, traitsModuloCircularScale> testedTypeModulo;

    for (T_ i = min - 10; i < min; i++)
    {
        try
        {
            testedTypeThrow v = i;
            stringstream ss;
            ss << "Construction of "
                    << typeid (testedTypeThrow).name()
                    << " with invalid value "
                    << i << " should have thrown, but did not.";
            CPPUNIT_FAIL(ss.str());
        }
        catch (std::exception const & e)
        {
            // OK
        }
    }
    for (T_ i = max + 1; i < max + 10; i++)
    {
        try
        {
            testedTypeThrow v = i;
            stringstream ss;
            ss << "Construction of "
                    << typeid (testedTypeThrow).name()
                    << " with invalid value "
                    << i << " should have thrown, but did not.";
            CPPUNIT_FAIL(ss.str());
        }
        catch (std::exception const & e)
        {
            // OK
        }
    }

    for (T_ i = min - 10; i < min; i++)
    {
        testedTypeInvalid v = i;
        stringstream ss;
        ss << "Construction of "
                << typeid (testedTypeInvalid).name()
                << " with invalid value "
                << i
                << " should have set the value to invalid, but did not.";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), false == v.isValid());
    }
    for (T_ i = max + 1; i < max + 10; i++)
    {
        testedTypeInvalid v = i;
        stringstream ss;
        ss << "Construction of "
                << typeid (testedTypeInvalid).name()
                << " with invalid value "
                << i
                << " should have set the value to invalid, but did not.";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), false == v.isValid());
    }


    for (T_ i = min - 10; i < min; i++)
    {
        testedTypeModulo v = i;
        stringstream ss;
        ss << "Construction of "
                << typeid (testedTypeModulo).name()
                << " with invalid value "
                << i
                << " should have set the value to a valid one, but did not.";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), true == v.isValid());
    }
    for (T_ i = max + 1; i < max + 10; i++)
    {
        T_ const dist = max - min;
        T_ x = i;
        x = ((x - min + dist) % dist) + min - T_(1);
        testedTypeModulo v = i;
        stringstream ss;
        ss << "Construction of "
                << typeid (testedTypeModulo).name()
                << " with invalid value "
                << i
                << " should have set the value to a valid one, but did not."
                << endl
                << "dist=" << dist << " v(" << v << ") - min(" << min << ")+dist(" << dist << ")+" << min << "-1=" << x;
        CPPUNIT_ASSERT_MESSAGE(ss.str(), true == v.isValid() && min <= v.val() && v.val() <= max);
    }
}

template<typename T_, typename Res_, typename Conv_>
void checkExtremeValues()
{
    constexpr T_ minT_ = std::numeric_limits<T_>::min();
    constexpr T_ maxT_ = std::numeric_limits<T_>::max();

    typedef limited_int_traits<T_, maxT_ - 10, maxT_, Res_, Conv_> testedTypeUprTraits;
    typedef limited_int<T_, maxT_ - 10, maxT_, testedTypeUprTraits> testedTypeUpr;

    typedef limited_int_traits<T_, minT_, minT_ + 10, Res_, Conv_> testedTypeLwrTraits;
    typedef limited_int<T_, minT_, minT_ + 10, testedTypeLwrTraits> testedTypeLwr;

    try
    {
        testedTypeUpr v = maxT_;
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeUpr).name() << " with valid value "
                << maxT_ << " should have succeeded, but did not.";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), true == v.isValid());
        ss.str("");
        ss << typeid (testedTypeUpr).name() << " that includes the std::numeric_limits<T_>::max() should have "
                << "std::numeric_limits<T_>::min() as invalid(), but does not";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), (v.invalid() == std::numeric_limits<T_>::min()));
    }
    catch (std::exception const & e)
    {
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeUpr).name() << " with valid value "
                << maxT_ << " should have succeeded, but did not. Exception:" << e.what();
        CPPUNIT_FAIL(ss.str());
    }

    try
    {
        testedTypeUpr v = maxT_ - 10;
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeUpr).name() << " with valid value "
                << minT_ << " should have succeeded, but did not.";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), true == v.isValid());
        ss.str("");
        ss << typeid (testedTypeUpr).name() << " that includes the std::numeric_limits<T_>::min() should have "
                << "std::numeric_limits<T_>::max() as invalid(), but does not";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), v.invalid() == std::numeric_limits<T_>::min());
    }
    catch (std::exception const & e)
    {
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeUpr).name() << " with valid value "
                << minT_ << " should have succeeded, but did not. Exception:" << e.what();
        CPPUNIT_FAIL(ss.str());
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        testedTypeLwr v = minT_;
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeLwr).name() << " with valid value "
                << minT_ << " should have succeeded, but did not.";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), true == v.isValid());
        ss.str("");
        ss << typeid (testedTypeLwr).name() << " that includes the std::numeric_limits<T_>::min() should have "
                << "std::numeric_limits<T_>::max() as invalid(), but does not";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), (v.invalid() == std::numeric_limits<T_>::max()));
    }
    catch (std::exception const & e)
    {
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeLwr).name() << " with valid value "
                << minT_ << " should have succeeded, but did not. Exception:" << e.what();
        CPPUNIT_FAIL(ss.str());
    }

    try
    {
        testedTypeLwr v = minT_ + 10;
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeLwr).name() << " with valid value "
                << minT_ << " should have succeeded, but did not.";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), true == v.isValid());
        ss.str("");
        ss << typeid (testedTypeUpr).name() << " that includes the std::numeric_limits<T_>::min() should have "
                << "std::numeric_limits<T_>::max() as invalid(), but does not";
        CPPUNIT_ASSERT_MESSAGE(ss.str(), v.invalid() == std::numeric_limits<T_>::max());
    }
    catch (std::exception const & e)
    {
        stringstream ss;
        ss << "Construction of " << typeid (testedTypeLwr).name() << " with valid value "
                << minT_ << " should have succeeded, but did not. Exception:" << e.what();
        CPPUNIT_FAIL(ss.str());
    }

}

void LimitedValuesTest::testInstanciation()
{
    checkAllValidValues<int32_t, -10, 10 > ();
    checkAllValidValues<int32_t, 0, 10>();
    checkAllValidValues<int32_t, -10, 10 > ();

    checkAllValidValues<int64_t, -10, 10 > ();
    checkAllValidValues<int64_t, 0, 10>();
    checkAllValidValues<int64_t, -10, 10 > ();

    checkInvalidValues<int32_t, -10, 10 > ();
    checkInvalidValues<int32_t, 0, 10>();
    checkInvalidValues<int32_t, -10, 10 > ();

    checkInvalidValues<int64_t, -10, 10 > ();
    checkInvalidValues<int64_t, 0, 10>();
    checkInvalidValues<int64_t, -10, 10 > ();

    checkExtremeValues<int32_t, resolve_modulo, convert_scale>();
    checkExtremeValues<int32_t, resolve_modulo, convert_circular_scale>();
    checkExtremeValues<int32_t, resolve_invalid, convert_scale>();
    checkExtremeValues<int32_t, resolve_invalid, convert_circular_scale>();
    checkExtremeValues<int32_t, resolve_throw, convert_scale>();
    checkExtremeValues<int32_t, resolve_throw, convert_circular_scale>();

    checkExtremeValues<int64_t, resolve_modulo, convert_scale>();
    checkExtremeValues<int64_t, resolve_modulo, convert_circular_scale>();
    checkExtremeValues<int64_t, resolve_invalid, convert_scale>();
    checkExtremeValues<int64_t, resolve_invalid, convert_circular_scale>();
    checkExtremeValues<int64_t, resolve_throw, convert_scale>();
    checkExtremeValues<int64_t, resolve_throw, convert_circular_scale>();

    // unsigned
    checkAllValidValues<uint32_t, 3, 10>();
    checkAllValidValues<uint32_t, 0, 10>();

    checkAllValidValues<uint64_t, 3, 10>();
    checkAllValidValues<uint64_t, 0, 10>();

    checkInvalidValues<uint32_t, 3, 10>();
    checkInvalidValues<uint32_t, 0, 10>();

    checkInvalidValues<uint64_t, 3, 10>();
    checkInvalidValues<uint64_t, 0, 10>();
}

template<typename T_, T_ min, T_ max>
void testIteratorT()
{
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_scale>  traitsModuloScale;
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_circular_scale> traitsModuloCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_scale> traitsInvalidScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_circular_scale>  traitsInvalidCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_scale> traitsThrowScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_circular_scale>  traitsThrowCircularScale;

    typedef limited_int<T_, min, max, traitsThrowScale> testedTypeThrow;
    typedef limited_int<T_, min, max, traitsInvalidCircularScale> testedTypeInvalid;
    typedef limited_int<T_, min, max, traitsModuloCircularScale> testedTypeModulo;

    // Memory efficiency-check: assert that limited_ints don't carry any extra baggage
    CPPUNIT_ASSERT(sizeof (T_) == sizeof (testedTypeThrow));
    CPPUNIT_ASSERT(sizeof (T_) == sizeof (testedTypeInvalid));
    CPPUNIT_ASSERT(sizeof (T_) == sizeof (testedTypeModulo));

    try
    {
        for (auto iter = testedTypeThrow::begin();
             iter != testedTypeThrow::end(min + 15);
             iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(e.what());
    }

    try
    {
        for (auto iter = testedTypeThrow::begin(max - 15);
             iter != testedTypeThrow::end(max - 1);
             iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(e.what());
    }
    try
    {
        auto iter = testedTypeThrow::begin(max - 15);
        for (;
             iter != testedTypeThrow::end(max);
             iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            CPPUNIT_ASSERT(iter->isValid());
        }
        iter++;
        CPPUNIT_FAIL("Iterating beyond the max value should throw exception.");
    }
    catch (exception & e)
    {
        // expected exception
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for (auto iter = testedTypeInvalid::begin();
             iter != testedTypeInvalid::end(min + 15);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL("Limited int with set-invalid policy should not throw when "
                     "iterator starting at begin and staying in valid range");
    }

    try
    {
        for (auto iter = testedTypeInvalid::begin(max - 15);
             iter != testedTypeInvalid::end(max - 1);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL("Limited int with set-invalid policy should not throw when "
                     "iterator reaches end");
    }
    try
    {
        auto iter = testedTypeInvalid::begin(max - 15);
        for (;
             iter != testedTypeInvalid::end();
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
        CPPUNIT_ASSERT(!(iter->isValid()));
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(asString("Limited int with set-invalid policy should not throw when "
                              "iterator reaches end: ") + e.what());
    }


    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for (auto iter = testedTypeModulo::begin();
             iter != testedTypeModulo::end(min + 15);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL("Iterator should always be valid");
    }

    try
    {
        for (auto iter = testedTypeModulo::begin(max - 15);
             iter != testedTypeModulo::end(max - 1);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(asString("Iterator should always be valid:") + e.what());
    }
    try
    {
        for (auto iter = testedTypeModulo::begin(max - 15);
             iter != testedTypeModulo::end(max);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
        for (auto iter = testedTypeModulo::begin(max - 15);
             iter != testedTypeModulo::end(min + 15);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(asString("Iterator should always be valid:") + e.what());
    }

    /////////////////// REVERSE/////////////////////////////////////////////////
    try
    {
        for (auto iter = testedTypeThrow::rbegin();
             iter != testedTypeThrow::rend(max - 15);
             iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(e.what());
    }

    try
    {
        for (auto iter = testedTypeThrow::rbegin(min + 15);
             iter != testedTypeThrow::rend(min);
             iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(e.what());
    }
    try
    {
        auto iter = testedTypeThrow::rbegin(min + 15);
        for (;
             iter != testedTypeThrow::rend(min);
             iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            CPPUNIT_ASSERT(iter->isValid());
        }
        iter++;
        CPPUNIT_FAIL("Iterating beyond the min value should throw exception.");
    }
    catch (exception & e)
    {
        // expected exception
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for (auto iter = testedTypeInvalid::rbegin();
             iter != testedTypeInvalid::rend(max - 15);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL("Limited int with set-invalid policy should not throw when "
                     "iterator starting at rbegin and staying in valid range");
    }

    try
    {
        for (auto iter = testedTypeInvalid::rbegin(min + 15);
             iter != testedTypeInvalid::rend(min);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL("Limited int with set-invalid policy should not throw when "
                     "iterator reaches end");
    }
    try
    {
        auto iter = testedTypeInvalid::rbegin(min + 15);
        for (;
             iter != testedTypeInvalid::rend();
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
        CPPUNIT_ASSERT(!(iter->isValid()));
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL("Limited int with set-invalid policy should not throw when "
                     "iterator reaches end");
    }


    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for (auto iter = testedTypeModulo::rbegin();
             iter != testedTypeModulo::rend(max - 15);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL("Iterator should always be valid");
    }

    try
    {
        for (auto iter = testedTypeModulo::rbegin(min + 15);
             iter != testedTypeModulo::rend(min);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(asString("Iterator should always be valid:") + e.what());
    }
    try
    {
        for (auto iter = testedTypeModulo::rbegin(min + 15);
             iter != testedTypeModulo::rend(min);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
        for (auto iter = testedTypeModulo::rbegin(min + 15);
             iter != testedTypeModulo::rend(max - 15);
             iter++)
        {
            CPPUNIT_ASSERT(iter->isValid());
        }
    }
    catch (exception & e)
    {
        CPPUNIT_FAIL(asString("Iterator should always be valid:") + e.what());
    }
}

void LimitedValuesTest::testIterator()
{
    testIteratorT<int32_t, 0, 100>();
    testIteratorT<int32_t, -50, 42 > ();
    testIteratorT<int32_t, -150, -42 > ();
}

void LimitedValuesTest::testDegreeConversion()
{
    std::vector<Deg180> vec180;
    for (int64_t i = -500; i < 500; i++)
        vec180.push_back(i);
    if (vec180.end() != std::find_if(vec180.begin(),
                                     vec180.end(),
                                     [](Deg180 const &v)
                                     {
                                         return ((v.val() > Deg180::max())
                                                 || (v.val() < Deg180::min()));
                                     }))
{
    CPPUNIT_FAIL("Putting int values into a vector<Deg180> left some Deg180 invalid");
}
    std::set<Deg180> set360;
    for (int64_t i = -500; i < 500; i++)
        set360.insert(i);
    if (set360.end() != std::find_if(set360.begin(),
                                     set360.end(),
                                     [](Deg360 const &v)
                                     {
                                         return ((v.val() > Deg360::max())
                                                 || (v.val() < Deg360::min()));
                                     }))
{
    CPPUNIT_FAIL("Putting int values into a set<Deg360> left some Deg360 invalid");
}
    std::map<Deg180, Rad2Pi> map180To2Pi;
    for (int64_t i = -500; i < 500; i++)
        map180To2Pi[i] = Deg180(i); // should store the 2Pi - value cast from the 180 value

    for (auto m : map180To2Pi)
    {
        Deg180 deg180 = m.first;
        Rad2Pi rad2Pi = m.second;
        if (deg180.val() < Deg180::min() || deg180.val() > Deg180::max())
            CPPUNIT_FAIL("Deg180 values as key of map is invalid:" + asString(deg180));
        if (rad2Pi.val() < Rad2Pi::min() || rad2Pi.val() > Rad2Pi::max())
            CPPUNIT_FAIL("Rad2Pi values as value of map is invalid:" + asString(rad2Pi));
    }
}
