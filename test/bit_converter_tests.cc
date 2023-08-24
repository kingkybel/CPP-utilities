/*
 * File:		bit_converter_tests.cc
 * Description:         Unit tests for bit converter.
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
#include "bit_converter.h"
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <string>

using namespace std;
using namespace util;

class BitConverterTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(BitConverterTest, construction_test)
{
    bit_converter<int64_t> defaultBC{int64_t{0}};
    string                 zero_64 = "0000000000000000000000000000000000000000000000000000000000000000";
    ASSERT_EQ(zero_64, string{defaultBC.asBitset().to_string()});
}
