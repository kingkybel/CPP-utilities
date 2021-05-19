/*
 * File:		bitConverterTest.cc
 * Description:         Unit tests for cit converter.
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

#include "bitConverterTest.h"

#include <bit_converter.h>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(bitConverterTest);

bitConverterTest::bitConverterTest()
{
}

bitConverterTest::~bitConverterTest()
{
}

void bitConverterTest::setUp()
{
}

void bitConverterTest::tearDown()
{
}

void bitConverterTest::construction_test()
{
    bit_converter defaultBC = 0.0L;
    cout << defaultBC.asBitset().to_string() << endl;
}
