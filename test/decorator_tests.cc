/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/decorator_tests.cc
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
#include <tuple>
#include <vector>

using namespace std;
using namespace util;

class DecoratorTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
        auto &decInst = decorator<>::instance();
        decInst.clear();
        decInst.initialize();
    }

    void TearDown() override
    {
        auto &decInst = decorator<>::instance();
        decInst.clear();
    }
};

TEST_F(DecoratorTest, bracket_initialisation_test)
{
    auto &decInst       = decorator<>::instance();
    auto  dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "^");
    decInst.clearBrackets();
    dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "");
    decInst.setBracketForKey(util::BracketKey::DEQUE,
                             util::Brackets{util::BracketKey::DEQUE, "new_left", "new_inner", "new_right"});
    dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "new_left");
    decInst.initializeBrackets();
    dequeBrackets = decInst.getBracket(util::BracketKey::DEQUE);
    ASSERT_EQ(dequeBrackets.left(), "^");
}

TEST_F(DecoratorTest, int_format_initialisation_test)
{
    auto &decInst = decorator<>::instance();
    ASSERT_EQ(toString('M'), "'M'");

    auto fmt = intFmt<>{IntBase::hexadecimal};
    decInst.setFormat<char>(fmt);
    ASSERT_EQ(toString('M'), "'4d'");

    decInst.setHexUpper<char>(true);
    ASSERT_EQ(toString('M'), "'4D'");

    decInst.setShowBase<char>(true);
    ASSERT_EQ(toString('M'), "'0x4D'");

    decInst.setWidth<char>(4);
    ASSERT_EQ(toString('M'), "'0x004D'");

    decInst.setFill<char>('-');
    ASSERT_EQ(toString('M'), "'0x--4D'");

    decInst.setBase<char>(IntBase::decimal);
    ASSERT_EQ(toString('M'), "'--77'");

    decInst.setBase<char>(IntBase::octal);
    ASSERT_EQ(toString('M'), "'0o-115'");

    // cout << decInst.showConfig();
    decInst.clearIntFormat();
    // cout << decInst.showConfig();
    ASSERT_EQ(toString('M'), "'M'");

    decInst.initialize();
    ASSERT_EQ(toString(int8_t{127}), "7f");

    decInst.setOctBaseStr<int8_t>("(octal)");
    ASSERT_EQ(toString(int8_t{127}), "(octal)177");
}

TEST_F(DecoratorTest, float_format_initialisation_test)
{
    auto &decInst = decorator<>::instance();
    ASSERT_EQ(toString(0.0L), "0.000000e+00");
    decInst.setBase<long double>(FloatBase::scientific);
    ASSERT_EQ(toString(0.0L), "0.000000e+00");
    decInst.setBase<long double>(FloatBase::fixed);
    decInst.setFill<long double>('*');
    decInst.setWidth<long double>(10);
    decInst.setPrecision<long double>(4);
    ASSERT_EQ(toString(0.0L), "****0.0000");
    decInst.setBase<long double>(FloatBase::hexfloat);
    ASSERT_EQ(toString(0.0L), "0x0p+0");
}

TEST_F(DecoratorTest, container_container_decoration_test)
{
    auto &decInst = decorator<>::instance();
    auto  omap    = map<int, int>{{1, 5}, {2, 3}, {5, 10}};
    ASSERT_EQ(toString(omap), "[(1,5),(2,3),(5,10)]");
    auto oset = set<char>{'z', 't', '6', 'Z'};
    ASSERT_EQ(toString(oset), "{'6','Z','t','z'}");
    auto q = deque<int>{1, 3, 4, 5};
    ASSERT_EQ(toString(q), "^1<-3<-4<-5$");
    decInst.clear();
    ASSERT_EQ(toString(omap), "1 5 2 3 5 10");
    ASSERT_EQ(toString(oset), "6 Z t z");
    ASSERT_EQ(toString(q), "1 3 4 5");
}

TEST_F(DecoratorTest, container_bracket_configuration_test)
{
    auto &decInst = decorator<>::instance();

    auto vec = vector<int>{};
    ASSERT_EQ(toString(vec), "<>");
    vec.emplace_back(1701);
    ASSERT_EQ(toString(vec), "<1701>");
    vec.emplace_back(1702);
    ASSERT_EQ(toString(vec), "<1701,1702>");
    // changing brackets for a BracketKey channges brackets for all objects that match the key
    decInst.setBracketForKey(BracketKey::VECTOR, "|| ", " | ", " ||");
    ASSERT_EQ(toString(vec), "|| 1701 | 1702 ||");

    auto cvec = vector<char>{'a', 'b', 'c'};
    ASSERT_EQ(toString(cvec), "|| 'a' | 'b' | 'c' ||");
    // changing brackets for a specific object should not change the brackets for the BracketKey
    decInst.setBracketForObject(cvec, "++ ", " * ", " ++");
    ASSERT_EQ(toString(cvec), "++ 'a' * 'b' * 'c' ++");
    ASSERT_EQ(toString(vec), "|| 1701 | 1702 ||");

    decInst.clearBrackets();
    ASSERT_EQ(toString(vec), "1701 1702");
    ASSERT_EQ(toString(cvec), "a b c");
}

TEST_F(DecoratorTest, tuple_decoration_test)
{
    auto                       &decInst = decorator<>::instance();
    tuple<long, string, double> tup{123L, string{"abc"}, double{666.0}};
    ASSERT_EQ(toString(tup), "(123,\"abc\",6.660000e+02)");
    decInst.clear();
    ASSERT_EQ(toString(tup), "123 abc 666");
}

TEST_F(DecoratorTest, multi_container_decoration_test)
{
    auto &decInst = decorator<>::instance();
    auto  mm      = multimap<int, char>{{1, 'a'}, {2, 'b'}, {2, 'B'}, {3, 'c'}};
    ASSERT_EQ(toString(mm), "[#(1,'a'),(2,'b'),(2,'B'),(3,'c')#]");
    auto           umm = unordered_multimap<int, char>{{1, 'a'}, {2, 'b'}, {2, 'B'}, {3, 'c'}};
    vector<string> pairs{"(1,'a')", "(2,'b')", "(2,'B')", "(3,'c')"};
    for(const auto &p: pairs)
        ASSERT_NE(toString(umm).find(p), string::npos)
         << "pair '" << p << "' wasn't found in '" << toString(umm) << "'";

    auto ms = multiset<string>{"abc", "def", "ghi", "def"};
    ASSERT_EQ(toString(ms), "{#\"abc\",\"def\",\"def\",\"ghi\"#}");

    auto           ums = unordered_multiset<string>{"abc", "def", "ghi", "def"};
    vector<string> ums_els{"\"abc\"", "\"def\",\"def\"", "\"ghi\""};
    for(const auto &s: ums_els)
        ASSERT_NE(toString(ums).find(s), string::npos)
         << "string '" << s << "' was not found in '" << toString(ums) << "'";

    // do the same with reset decorator
    decInst.clear();
    ASSERT_EQ(toString(mm), "1 a 2 b 2 B 3 c");
    vector<string> nodec_pairs{"1 a", "2 b", "2 B", "3 c"};
    for(const auto &p: nodec_pairs)
        ASSERT_NE(toString(umm).find(p), string::npos)
         << "pair '" << p << "' wasn't found in '" << toString(umm) << "'";
    ASSERT_EQ(toString(ms), "abc def def ghi");
    vector<string> nodec_ums_els{"abc", "def def", "ghi"};
    for(const auto &s: nodec_ums_els)
        ASSERT_NE(toString(ums).find(s), string::npos)
         << "string '" << s << "' was not found in '" << toString(ums) << "'";
}
