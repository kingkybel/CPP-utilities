/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/tiny_tea_tests.cc
 * Description: Unit tests for tiny tea
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

#include "tinytea.h"

#include <cmath>
#include <gtest/gtest.h>
#include <stringutil.h>

using namespace std;
using namespace util;

class TinyteaTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TinyteaTest, encryption_test)
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
                ASSERT_EQ(val, dec) << "decrypt the encrypted value should result in the original value";
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
