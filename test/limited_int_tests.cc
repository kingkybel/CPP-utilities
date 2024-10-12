/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/limited_int_tests.cc
 * Description: Unit tests for limited_values -templates
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

#include "degree_conversion.h"
#include "limited_int.h"
#include "to_string.h"
// #define DO_TRACE_
#include "traceutil.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace util;

class LimitedIntTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

template<typename T_, T_ min, T_ max>
void checkAllValidValues()
{
    // typedef limited_int_traits<ValueT_, min, max, resolve_modulo, convert_scale>           traitsModuloScale;
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_circular_scale> traitsModuloCircularScale;
    // typedef limited_int_traits<ValueT_, min, max, resolve_invalid, convert_scale>          traitsInvalidScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_circular_scale> traitsInvalidCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_scale>            traitsThrowScale;
    // typedef limited_int_traits<ValueT_, min, max, resolve_throw, convert_circular_scale>   traitsThrowCircularScale;

    typedef limited_int<T_, min, max, traitsThrowScale>           testedTypeThrow;
    typedef limited_int<T_, min, max, traitsInvalidCircularScale> testedTypeInvalid;
    typedef limited_int<T_, min, max, traitsModuloCircularScale>  testedTypeModulo;

    for(T_ i = min; i <= max; i++)
    {
        try
        {
            testedTypeThrow v = i;
            ASSERT_TRUE(v.isValid());
            if(i > min)
            {
                v = i - 1;
                ASSERT_TRUE(v.isValid());
            }
        }
        catch(...)
        {
            stringstream ss;
            ss << "Construction of limited_int<" << typeid(testedTypeThrow).name() << " with valid value " << i
               << " failed.";
            FAIL() << ss.str();
        }
        try
        {
            testedTypeInvalid v = i;
            ASSERT_TRUE(v.isValid());
            if(i > min)
            {
                v = i - 1;
                ASSERT_TRUE(v.isValid());
            }
        }
        catch(...)
        {
            stringstream ss;
            ss << "Construction of limited_int<" << typeid(testedTypeInvalid).name() << " with valid value " << i
               << " failed.";
            FAIL() << ss.str();
        }
        try
        {
            testedTypeModulo v = i;
            ASSERT_TRUE(v.isValid());
            if(i > min)
            {
                v = i - 1;
                ASSERT_TRUE(v.isValid());
            }
        }
        catch(...)
        {
            stringstream ss;
            ss << "Construction of limited_int<" << typeid(testedTypeModulo).name() << " with valid value " << i
               << " failed.";
            FAIL() << ss.str();
        }
    }
}

template<typename T_, T_ min, T_ max>
void checkInvalidValues()
{
    // typedef limited_int_traits<ValueT_, min, max, resolve_modulo, convert_scale>           traitsModuloScale;
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_circular_scale> traitsModuloCircularScale;
    // typedef limited_int_traits<ValueT_, min, max, resolve_invalid, convert_scale>          traitsInvalidScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_circular_scale> traitsInvalidCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_scale>            traitsThrowScale;
    // typedef limited_int_traits<ValueT_, min, max, resolve_throw, convert_circular_scale>   traitsThrowCircularScale;

    typedef limited_int<T_, min, max, traitsThrowScale>           testedTypeThrow;
    typedef limited_int<T_, min, max, traitsInvalidCircularScale> testedTypeInvalid;
    typedef limited_int<T_, min, max, traitsModuloCircularScale>  testedTypeModulo;

    for(T_ i = min - 10; i < min; i++)
    {
        try
        {
            [[maybe_unused]] testedTypeThrow v = i;
            stringstream                     ss;
            ss << "Construction of " << typeid(testedTypeThrow).name() << " with invalid value " << i
               << " should have thrown, but did not.";
            FAIL() << ss.str();
        }
        catch(std::exception const &e)
        {
            // OK
        }
    }
    for(T_ i = max + 1; i < max + 10; i++)
    {
        try
        {
            [[maybe_unused]] testedTypeThrow v = i;
            stringstream                     ss;
            ss << "Construction of " << typeid(testedTypeThrow).name() << " with invalid value " << i
               << " should have thrown, but did not.";
            FAIL() << ss.str();
        }
        catch(std::exception const &e)
        {
            // OK
        }
    }

    for(T_ i = min - 10; i < min; i++)
    {
        testedTypeInvalid v = i;
        stringstream      ss;
        ss << "Construction of " << typeid(testedTypeInvalid).name() << " with invalid value " << i
           << " should have set the value to invalid, but did not.";
        ASSERT_TRUE(false == v.isValid()) << ss.str();
    }
    for(T_ i = max + 1; i < max + 10; i++)
    {
        testedTypeInvalid v = i;
        stringstream      ss;
        ss << "Construction of " << typeid(testedTypeInvalid).name() << " with invalid value " << i
           << " should have set the value to invalid, but did not.";
        ASSERT_TRUE(false == v.isValid()) << ss.str();
    }

    for(T_ i = min - 10; i < min; i++)
    {
        testedTypeModulo v = i;
        stringstream     ss;
        ss << "Construction of " << typeid(testedTypeModulo).name() << " with invalid value " << i
           << " should have set the value to a valid one, but did not.";
        ASSERT_TRUE(true == v.isValid()) << ss.str();
    }
    for(T_ i = max + 1; i < max + 10; i++)
    {
        T_ const dist      = max - min;
        T_       x         = i;
        x                  = ((x - min + dist) % dist) + min - T_(1);
        testedTypeModulo v = i;
        stringstream     ss;
        ss << "Construction of " << typeid(testedTypeModulo).name() << " with invalid value " << i
           << " should have set the value to a valid one, but did not." << endl
           << "dist=" << dist << " v(" << v << ") - min(" << min << ")+dist(" << dist << ")+" << min << "-1=" << x;
        ASSERT_TRUE(true == v.isValid() && min <= v.val() && v.val() <= max) << ss.str();
    }
}

template<typename T_, typename Res_, typename Conv_>
void checkExtremeValues()
{
    constexpr T_ minT_ = std::numeric_limits<T_>::min();
    constexpr T_ maxT_ = std::numeric_limits<T_>::max();

    typedef limited_int_traits<T_, maxT_ - 10, maxT_, Res_, Conv_>  testedTypeUprTraits;
    typedef limited_int<T_, maxT_ - 10, maxT_, testedTypeUprTraits> testedTypeUpr;

    typedef limited_int_traits<T_, minT_, minT_ + 10, Res_, Conv_>  testedTypeLwrTraits;
    typedef limited_int<T_, minT_, minT_ + 10, testedTypeLwrTraits> testedTypeLwr;

    try
    {
        testedTypeUpr v = maxT_;
        stringstream  ss;
        ss << "Construction of " << typeid(testedTypeUpr).name() << " with valid value " << maxT_
           << " should have succeeded, but did not.";
        ASSERT_TRUE(true == v.isValid()) << ss.str();
        ss.str("");
        ss << typeid(testedTypeUpr).name() << " that includes the std::numeric_limits<ValueT_>::max() should have "
           << "std::numeric_limits<ValueT_>::min() as invalid(), but does not";
        ASSERT_TRUE((v.invalid() == std::numeric_limits<T_>::min())) << ss.str();
    }
    catch(std::exception const &e)
    {
        stringstream ss;
        ss << "Construction of " << typeid(testedTypeUpr).name() << " with valid value " << maxT_
           << " should have succeeded, but did not. Exception:" << e.what();
        FAIL() << ss.str();
    }

    try
    {
        testedTypeUpr v = maxT_ - 10;
        stringstream  ss;
        ss << "Construction of " << typeid(testedTypeUpr).name() << " with valid value " << minT_
           << " should have succeeded, but did not.";
        ASSERT_TRUE(true == v.isValid()) << ss.str();
        ss.str("");
        ss << typeid(testedTypeUpr).name() << " that includes the std::numeric_limits<ValueT_>::min() should have "
           << "std::numeric_limits<ValueT_>::max() as invalid(), but does not";
        ASSERT_TRUE(v.invalid() == std::numeric_limits<T_>::min()) << ss.str();
    }
    catch(std::exception const &e)
    {
        stringstream ss;
        ss << "Construction of " << typeid(testedTypeUpr).name() << " with valid value " << minT_
           << " should have succeeded, but did not. Exception:" << e.what();
        FAIL() << ss.str();
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        testedTypeLwr v = minT_;
        stringstream  ss;
        ss << "Construction of " << typeid(testedTypeLwr).name() << " with valid value " << minT_
           << " should have succeeded, but did not.";
        ASSERT_TRUE(true == v.isValid()) << ss.str();
        ss.str("");
        ss << typeid(testedTypeLwr).name() << " that includes the std::numeric_limits<ValueT_>::min() should have "
           << "std::numeric_limits<ValueT_>::max() as invalid(), but does not";
        ASSERT_TRUE((v.invalid() == std::numeric_limits<T_>::max())) << ss.str();
    }
    catch(std::exception const &e)
    {
        stringstream ss;
        ss << "Construction of " << typeid(testedTypeLwr).name() << " with valid value " << minT_
           << " should have succeeded, but did not. Exception:" << e.what();
        FAIL() << ss.str();
    }

    try
    {
        testedTypeLwr v = minT_ + 10;
        stringstream  ss;
        ss << "Construction of " << typeid(testedTypeLwr).name() << " with valid value " << minT_
           << " should have succeeded, but did not.";
        ASSERT_TRUE(true == v.isValid()) << ss.str();
        ss.str("");
        ss << typeid(testedTypeUpr).name() << " that includes the std::numeric_limits<ValueT_>::min() should have "
           << "std::numeric_limits<ValueT_>::max() as invalid(), but does not";
        ASSERT_TRUE(v.invalid() == std::numeric_limits<T_>::max()) << ss.str();
    }
    catch(std::exception const &e)
    {
        stringstream ss;
        ss << "Construction of " << typeid(testedTypeLwr).name() << " with valid value " << minT_
           << " should have succeeded, but did not. Exception:" << e.what();
        FAIL() << ss.str();
    }
}

TEST_F(LimitedIntTest, testInstanciation)
{
    checkAllValidValues<int32_t, -10, 10>();
    checkAllValidValues<int32_t, 0, 10>();
    checkAllValidValues<int32_t, -10, 10>();

    checkAllValidValues<int64_t, -10, 10>();
    checkAllValidValues<int64_t, 0, 10>();
    checkAllValidValues<int64_t, -10, 10>();

    checkInvalidValues<int32_t, -10, 10>();
    checkInvalidValues<int32_t, 0, 10>();
    checkInvalidValues<int32_t, -10, 10>();

    checkInvalidValues<int64_t, -10, 10>();
    checkInvalidValues<int64_t, 0, 10>();
    checkInvalidValues<int64_t, -10, 10>();

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
    // typedef limited_int_traits<ValueT_, min, max, resolve_modulo, convert_scale>           traitsModuloScale;
    typedef limited_int_traits<T_, min, max, resolve_modulo, convert_circular_scale> traitsModuloCircularScale;
    // typedef limited_int_traits<ValueT_, min, max, resolve_invalid, convert_scale>          traitsInvalidScale;
    typedef limited_int_traits<T_, min, max, resolve_invalid, convert_circular_scale> traitsInvalidCircularScale;
    typedef limited_int_traits<T_, min, max, resolve_throw, convert_scale>            traitsThrowScale;
    // typedef limited_int_traits<ValueT_, min, max, resolve_throw, convert_circular_scale>   traitsThrowCircularScale;

    typedef limited_int<T_, min, max, traitsThrowScale>           testedTypeThrow;
    typedef limited_int<T_, min, max, traitsInvalidCircularScale> testedTypeInvalid;
    typedef limited_int<T_, min, max, traitsModuloCircularScale>  testedTypeModulo;

    // Memory efficiency-check: assert that limited_ints don't carry any extra baggage
    ASSERT_TRUE(sizeof(T_) == sizeof(testedTypeThrow));
    ASSERT_TRUE(sizeof(T_) == sizeof(testedTypeInvalid));
    ASSERT_TRUE(sizeof(T_) == sizeof(testedTypeModulo));

    try
    {
        for(auto iter = testedTypeThrow::begin(); iter != testedTypeThrow::end(min + 15); iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << e.what();
    }

    try
    {
        for(auto iter = testedTypeThrow::begin(max - 15); iter != testedTypeThrow::end(max - 1); iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << e.what();
    }
    try
    {
        auto iter = testedTypeThrow::begin(max - 15);
        for(; iter != testedTypeThrow::end(max); iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            ASSERT_TRUE(iter->isValid());
        }
        ASSERT_THROW(iter++, std::out_of_range) << typeid(testedTypeThrow).name() << " should have invalid iterator";
    }
    catch(exception &e)
    {
        // expected exception
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for(auto iter = testedTypeInvalid::begin(); iter != testedTypeInvalid::end(min + 15); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Limited int with set-invalid policy should not throw when "
                  "iterator starting at begin and staying in valid range";
    }

    try
    {
        for(auto iter = testedTypeInvalid::begin(max - 15); iter != testedTypeInvalid::end(max - 1); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Limited int with set-invalid policy should not throw when "
                  "iterator reaches end";
    }
    try
    {
        auto iter = testedTypeInvalid::begin(max - 15);
        for(; iter != testedTypeInvalid::end(); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
        ASSERT_TRUE(!(iter->isValid()));
    }
    catch(exception &e)
    {
        FAIL() << toString("Limited int with set-invalid policy should not throw when "
                           "iterator reaches end: ")
                   + e.what();
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for(auto iter = testedTypeModulo::begin(); iter != testedTypeModulo::end(min + 15); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Iterator should always be valid";
    }

    try
    {
        for(auto iter = testedTypeModulo::begin(max - 15); iter != testedTypeModulo::end(max - 1); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << toString("Iterator should always be valid:") + e.what();
    }
    try
    {
        for(auto iter = testedTypeModulo::begin(max - 15); iter != testedTypeModulo::end(max); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
        for(auto iter = testedTypeModulo::begin(max - 15); iter != testedTypeModulo::end(min + 15); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << toString("Iterator should always be valid:") + e.what();
    }

    /////////////////// REVERSE/////////////////////////////////////////////////
    try
    {
        for(auto iter = testedTypeThrow::rbegin(max - 5); iter != testedTypeThrow::rend(max - 10); iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << e.what();
    }

    try
    {
        for(auto iter = testedTypeThrow::rbegin(min + 15); iter != testedTypeThrow::rend(min); iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << e.what();
    }
    try
    {
        auto iter = testedTypeThrow::rbegin(min + 15);
        for(; iter != testedTypeThrow::rend(min); iter++)
        {
            // independent of Policy: whilst iterating through valid range *iter should be valid
            ASSERT_TRUE(iter->isValid());
        }
        iter++;
        FAIL() << "Iterating beyond the min value should throw exception.";
    }
    catch(exception &e)
    {
        // expected exception
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for(auto iter = testedTypeInvalid::rbegin(); iter != testedTypeInvalid::rend(max - 15); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Limited int with set-invalid policy should not throw when "
                  "iterator starting at rbegin and staying in valid range: "
               << e.what();
    }

    try
    {
        for(auto iter = testedTypeInvalid::rbegin(min + 15); iter != testedTypeInvalid::rend(min); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Limited int with set-invalid policy should not throw when "
                  "iterator reaches end: "
               << e.what();
    }
    try
    {
        auto iter = testedTypeInvalid::rbegin(min + 15);
        for(; iter != testedTypeInvalid::rend(); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
        ASSERT_TRUE(!(iter->isValid()));
    }
    catch(exception &e)
    {
        FAIL() << "Limited int with set-invalid policy should not throw when "
                  "iterator reaches end: "
               << e.what();
    }

    ////////////////////////////////////////////////////////////////////////////
    try
    {
        for(auto iter = testedTypeModulo::rbegin(); iter != testedTypeModulo::rend(max - 15); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Iterator should always be valid for modulo resolved limited int type: " << e.what();
    }

    try
    {
        for(auto iter = testedTypeModulo::rbegin(min + 15); iter != testedTypeModulo::rend(min); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Iterator should always be valid for modulo resolved limited int type: " << e.what();
    }
    try
    {
        for(auto iter = testedTypeModulo::rbegin(min + 15); iter != testedTypeModulo::rend(min); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
        for(auto iter = testedTypeModulo::rbegin(min + 15); iter != testedTypeModulo::rend(max - 15); iter++)
        {
            ASSERT_TRUE(iter->isValid());
        }
    }
    catch(exception &e)
    {
        FAIL() << "Iterator should always be valid for modulo resolved limited int type: " << e.what();
    }
}

TEST_F(LimitedIntTest, testIterator)
{
    testIteratorT<int32_t, 0, 100>();
    testIteratorT<int32_t, -50, 42>();
    testIteratorT<int32_t, -150, -42>();
}

TEST_F(LimitedIntTest, testDegreeConversion)
{
    std::vector<Deg180> vec180;
    for(int64_t i = -500; i < 500; i++)
        vec180.push_back(i);
    if(vec180.end()
       != std::find_if(vec180.begin(),
                       vec180.end(),
                       [](Deg180 const &v) { return ((v.val() > Deg180::max()) || (v.val() < Deg180::min())); }))
    {
        FAIL() << "Putting int values into a vector<Deg180> left some Deg180 invalid";
    }
    std::set<Deg180> set360;
    for(int64_t i = -500; i < 500; i++)
        set360.insert(i);
    if(set360.end()
       != std::find_if(set360.begin(),
                       set360.end(),
                       [](Deg360 const &v) { return ((v.val() > Deg360::max()) || (v.val() < Deg360::min())); }))
    {
        FAIL() << "Putting int values into a set<Deg360> left some Deg360 invalid";
    }
    std::map<Deg180, Rad2Pi> map180To2Pi;
    for(int64_t i = -500; i < 500; i++)
        map180To2Pi[i] = Deg180(i);  // should store the 2Pi - value cast from the 180 value

    for(auto m: map180To2Pi)
    {
        Deg180 deg180 = m.first;
        Rad2Pi rad2Pi = m.second;
        if(deg180.val() < Deg180::min() || deg180.val() > Deg180::max())
            FAIL() << "Deg180 values as key of map is invalid:" + toString(deg180);
        if(rad2Pi.val() < Rad2Pi::min() || rad2Pi.val() > Rad2Pi::max())
            FAIL() << "Rad2Pi values as value of map is invalid:" + toString(rad2Pi);
    }
}
