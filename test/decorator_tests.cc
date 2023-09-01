/*
 * File:		decorator_tests.cc
 * Description: Unit tests for stream decoration.
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
#include "decorator.h"
#include "to_string.h"

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

class DecoratorTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(DecoratorTest, bracket_initialisation_test)
{
    auto decInst = decorator<>::instance();
    auto dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "^");
    decInst.clearBrackets();
    dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "");
    decInst.setBracketForKey(util::BracketKey::DEQUE, util::Brackets{util::BracketKey::DEQUE, "new_left", "new_inner", "new_right"});
    dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "new_left");
    decInst.initializeBrackets();
    dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "^");
}

TEST_F(DecoratorTest, int_format_initialisation_test)
{
    auto decInst = decorator<>::instance();
    ASSERT_EQ(decInst.getIntFmt<int8_t>(), util::intFmt::decimal);
    ASSERT_EQ(decInst.getIntFmt<char>(), util::intFmt::print_char);
    ASSERT_EQ(decInst.getIntFmt<unsigned char>(), util::intFmt::decimal);
    decInst.clearIntFormat();
}

TEST_F(DecoratorTest, default_decoration_test)
{
    auto decInst = decorator<>::instance();

    unordered_map<int, char> uMap = {{12, 'a'}, {2, 'b'}, {3, 'c'}, {24, 'd'}, {5, 'e'}, {6, 'f'}};
    cout << uMap << endl;
    auto ordMap = toMap(uMap);
    cout << "1:" << ordMap << endl;

    decInst.setBracketForObject(ordMap, "[left]", "[inner]", "[right]");
    cout << "2:" << ordMap << endl;

    deque<double> deq = {3.1415, 47.11, 1e-10, 2e10, 123.456};
    cout << "deque:" << deq << endl;

    cout << toString('a') << "\t" << toString("Hello") << endl;
    wcout << toWString("a") << endl;
}
