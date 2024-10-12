/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/heap_tests.cc
 * Description: Unit tests for heap templates.
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
#include "heap.h"

#include <cstdlib>
#include <gtest/gtest.h>
#include <string>

using namespace std;
using namespace util;

class HeapTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(HeapTest, heap_test)
{
    heap<int32_t, std::less<int32_t>> hipp;  // max-heap: biggest element at top
    ASSERT_TRUE(hipp.empty());
    ASSERT_EQ(hipp.size(), 0UL);
    ASSERT_EQ(hipp.top(), nullptr);

    hipp.insert(42);
    ASSERT_FALSE(hipp.empty());
    ASSERT_EQ(hipp.size(), 1UL);
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 42);

    hipp.pop();
    ASSERT_TRUE(hipp.empty());
    ASSERT_EQ(hipp.size(), 0UL);
    ASSERT_EQ(hipp.top(), nullptr);

    auto elements = vector<int32_t>{5, 10, 7, 1, 1, 10};
    for(const auto& element: elements)
        hipp.insert(element);

    ASSERT_FALSE(hipp.empty());
    ASSERT_EQ(hipp.size(), elements.size());
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 10);

    // cout << hipp << endl;
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 10);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 7);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 5);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 1);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 1);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_TRUE(hipp.empty());
    ASSERT_EQ(hipp.size(), 0UL);
    ASSERT_EQ(hipp.top(), nullptr);
}

TEST_F(HeapTest, std_heap_test)
{
    std_heap<int32_t, std::less<int32_t>> hipp;  // max-heap: biggest element at top
    ASSERT_TRUE(hipp.empty());
    ASSERT_EQ(hipp.size(), 0UL);
    ASSERT_EQ(hipp.top(), nullptr);

    hipp.insert(42);
    ASSERT_FALSE(hipp.empty());
    ASSERT_EQ(hipp.size(), 1UL);
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 42);

    hipp.pop();
    ASSERT_TRUE(hipp.empty());
    ASSERT_EQ(hipp.size(), 0UL);
    ASSERT_EQ(hipp.top(), nullptr);

    auto elements = vector<int32_t>{5, 10, 7, 1, 1, 10};
    for(const auto& element: elements)
        hipp.insert(element);

    ASSERT_FALSE(hipp.empty());
    ASSERT_EQ(hipp.size(), elements.size());
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 10);

    // cout << hipp << endl;
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 10);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 7);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 5);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 1);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_NE(hipp.top(), nullptr);
    ASSERT_EQ(*hipp.top(), 1);
    hipp.pop();
    // cout << hipp << endl;
    ASSERT_TRUE(hipp.empty());
    ASSERT_EQ(hipp.size(), 0UL);
    ASSERT_EQ(hipp.top(), nullptr);
}
