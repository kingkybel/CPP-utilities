/*
 * File:		tinyTeaTest.cc
 * Description:         Unit tests for string utilities
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

#include "tinyTeaTest.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stringutil.h>
#include <tinytea.h>

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(tinyTeaTest);

tinyTeaTest::tinyTeaTest()
{
}

tinyTeaTest::~tinyTeaTest()
{
}

void tinyTeaTest::setUp()
{
}

void tinyTeaTest::tearDown()
{
}

void tinyTeaTest::encryption_test()
{
    vector<uint64_t> keys = {1701ULL, 666ULL, 4711ULL, 42ULL, 1011ULL};
    for(uint64_t val = 0ULL; val < 10000ULL; val += 131ULL)
    {
        for(auto key1: keys)
        {
            for(auto key2: keys)
            {
                uint64_t enc = tinyTea<>::encrypt(val, key1, key2);
                uint64_t dec = tinyTea<>::decrypt(enc, key1, key2);
                CPPUNIT_ASSERT_EQUAL_MESSAGE("decrypt the encrypted value should result in the original value",
                                             val,
                                             dec);
            }
        }
    }

    //    multiTea<long double> enc = multiTea<long double>::encrypt(1234.4112L, "hasdkjghfdskjghlsdk");
    //    cout << (long double)enc << endl;
    //    cout << multiTea<long double>::decrypt(enc, "hasdkjghfdskjghlsdk") << endl;

    //    string strValue = "Unencrypted value";
    //    multiTea<string> encStr = multiTea<string>::encrypt(strValue, "hasdkjghfdskjghlsdk");
    //    cout << (string)encStr << endl;
    //    cout << multiTea<string>::decrypt(encStr, "hasdkjghfdskjghlsdk") << endl;
}
