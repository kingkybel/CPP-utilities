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
#include "dateutil.h"
#include "stringutil.h"

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
using namespace util::datescan;

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

TEST_F(DecoratorTest, default_decoration_test)
{
   vector<int> intVec = {1, 5, 2, 5, 7, 1, 4, 8};

    cout << intVec << endl;

    auto intSet = toSet(intVec);

    cout << intSet << endl;

#ifdef DEMONSTRATE_COMPILATION_ERROR
    // Following will not compile which is what we want
    struct x  // x is neither comparable by "==" nor "<"
    {
    };

    vector<x> xVec;

    auto xSet = toSet(xVec);  // this requires comparability
#endif

    unordered_set<string> strUSet = {"All", "is", "quiet", "on", "the", "western", "front"};
    cout << strUSet << endl;

    auto strSet = toSet(strUSet);
    cout << strSet << endl;

    unordered_map<int, char> uMap = {{12, 'a'}, {2, 'b'}, {3, 'c'}, {24, 'd'}, {5, 'e'}, {6, 'f'}};
    cout << uMap << endl;

    auto ordMap = toMap(uMap);
    cout << "1:" << ordMap << endl;
    decorator<char>::instance().setBracket(ordMap, "[left]", "[inner]", "[right]");
    cout << "2:" << ordMap << endl;

    deque<double> deq = {3.1415, 47.11, 1e-10, 2e10, 123.456};
    cout << deq << endl;

    cout << toString(char('a')) << "\t" << toString(string("Hello")) << endl;
    wcout << toWString(wchar_t('a')) << L"\t" << toWString(wstring(L"Hello")) << endl;
}
