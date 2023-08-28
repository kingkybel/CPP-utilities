/*
 * File:		containert_convert_tests.cc
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
#include "container_convert.h"

#include <deque>
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace util;

class ContainerConvertTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ContainerConvertTest, moveElementsTo_test)
{
    auto vec1 = vector<int>{{1, 2, 4, 8, 16, 32, 64, 128, 256}};
    auto vec2 = vector<int>{{-1, 333}};
    auto gt20 = [](int i) { return i > 20; };
    moveElementsTo(vec1, vec2, gt20);

    auto expVec1 = vector<int>{{1, 2, 4, 8, 16}};
    ASSERT_EQ(vec1, expVec1);
    auto expVec2 = vector<int>{{-1, 333, 32, 64, 128, 256}};
    ASSERT_EQ(vec2, expVec2);

    moveElementsTo(vec1, vec2);
    expVec1 = vector<int>{};
    ASSERT_EQ(vec1, expVec1);
    expVec2 = vector<int>{{-1, 333, 32, 64, 128, 256, 1, 2, 4, 8, 16}};
    ASSERT_EQ(vec2, expVec2);
}

TEST_F(ContainerConvertTest, toVector_test)
{
    auto dblSet = set<double>{{-2.5, -1e5, 0.0, 10.0, 4711.0}};
    auto vec    = toVector(dblSet);
    ASSERT_EQ(vec.size(), dblSet.size());

    for(const auto& el: vec)
        ASSERT_NE(dblSet.find(el), dblSet.end());

    auto chrQue = deque{{'a', 'b', 'c'}};
    auto chrVec = toVector(chrQue);
    auto expVec = vector{{'a', 'b', 'c'}};
    ASSERT_EQ(chrVec, expVec);
}

TEST_F(ContainerConvertTest, toDeque_test)
{
    auto strVec = vector<string>{{"abc", "efg", "hij"}};
    auto que    = toDeque(strVec);
    ASSERT_EQ(que.size(), strVec.size());

    for(size_t i = 0; i < que.size(); i++)
        ASSERT_EQ(que[i], strVec[i]);
}

struct NonPodType
{
    int    i;
    double d;

    friend bool operator<(const NonPodType& lhs, const NonPodType& rhs)
    {
        return lhs.i < rhs.i || (lhs.i == rhs.i && lhs.d < rhs.d);
    }

    friend bool operator==(const NonPodType& lhs, const NonPodType& rhs)
    {
        return lhs.i == rhs.i && lhs.d == rhs.d;
    }
};
namespace std
{
template<>
struct hash<NonPodType>
{
    std::size_t operator()(const NonPodType& v) const
    {
        hash<double> hasher_d;
        hash<int>    hasher_i;

        return (hasher_i(v.i) + hasher_d(v.d));
    }
};
};

TEST_F(ContainerConvertTest, toSet_test)
{
    auto strVec = vector<string>{{"abc", "efg", "hij", "OnlyOnce", "abc", "efg", "hij", "efg", "hij"}};
    auto strSet = toSet(strVec);
    for(const auto& el: strVec)
        ASSERT_NE(strSet.find(el), strSet.end());

    auto nonPodUSet = unordered_set<NonPodType>{{{1, 1.0}, {-1, 17.0}, {20, 1e-2}, {-666, 3.4e5}, {3, 3.1415}}};
    auto nonPodSet  = toSet(nonPodUSet);
    ASSERT_EQ(nonPodSet.size(), nonPodUSet.size());

    for(const auto& el: nonPodUSet)
    {
        ASSERT_NE(nonPodSet.find(el), nonPodSet.end());
    }
}

TEST_F(ContainerConvertTest, toMap_test)
{
    auto strIntMap = unordered_map<string, int>{{"Ground", 1}, {"Floor", 666}, {"Table", -3}};
    auto strIntUMap = toMap(strIntMap);
    ASSERT_EQ(strIntMap.size(), strIntUMap.size());
    for(const auto& kv: strIntMap)
        ASSERT_EQ(strIntUMap[kv.first], kv.second);
}

TEST_F(ContainerConvertTest, toOrderedKeySet_test)
{
    auto strIntUMap = unordered_map<string, int>{{"Ground", 1}, {"Floor", 666}, {"Table", -3}};
    auto strSet = toOrderedKeySet(strIntUMap);
    ASSERT_EQ(strSet.size(), strIntUMap.size());
    for(const auto& kv: strIntUMap)
        ASSERT_NE(strSet.find(kv.first), strSet.end());
}
