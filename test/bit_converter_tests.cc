/*
 * File:		bit_converter_tests.cc
 * Description: Unit tests for bit converter.
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

TEST_F(BitConverterTest, from_construction_test)
{
    auto   result  = bit_converter<int64_t>::from({char{0}});
    string zero_64 = "0000000000000000000000000000000000000000000000000000000000000000";
    ASSERT_EQ(zero_64, string{result.asBitset().to_string()});
    result = bit_converter<int64_t>::from({char{0}, char{0}, char{0}, char{0}, char{0}, char{0}, char{0}, char{0}});
    ASSERT_EQ(zero_64, string{result.asBitset().to_string()});
    result = bit_converter<int64_t>::from({int16_t{0}, int16_t{0}, int16_t{0}, int16_t{0}});
    ASSERT_EQ(zero_64, string{result.asBitset().to_string()});
    result = bit_converter<int64_t>::from({uint32_t{0}, uint32_t{0}});
    ASSERT_EQ(zero_64, string{result.asBitset().to_string()});

    string expected{"0000000000000100000000000000001100000000000000100000000000000001"};
    result = bit_converter<int64_t>::from({int16_t{1}, int16_t{2}, int16_t{3}, int16_t{4}});
    ASSERT_EQ(expected, string{result.asBitset().to_string()});
}

TEST_F(BitConverterTest, set_and_get_test)
{
    bit_converter<int64_t> bitConv{int64_t{0}};
    for(size_t i = 0UL; i < 64; i++)
        ASSERT_FALSE(bitConv.getBit(i));
    for(size_t i = 0UL; i < 8; i++)
        ASSERT_EQ(bitConv.getByte(i), u_char{0});

    bitConv.setBit(34);
    for(size_t i = 0UL; i < 64; i++)
        if(i != 34)
            ASSERT_FALSE(bitConv.getBit(i));
        else
            ASSERT_TRUE(bitConv.getBit(i));
            
    for(size_t i = 0UL; i < 8; i++)
        if(i != 4)
            ASSERT_EQ(bitConv.getByte(i), u_char{0});
        else
            ASSERT_EQ(bitConv.getByte(i), u_char{4});

    bitConv.setBit(34, false);
    for(size_t i = 0UL; i < 64; i++)
        ASSERT_FALSE(bitConv.getBit(i));
    for(size_t i = 0UL; i < 8; i++)
        ASSERT_EQ(bitConv.getByte(i), u_char{0});

    bitConv.setByte(4, uint8_t{4});
    for(size_t i = 0UL; i < 64; i++)
        if(i != 34)
            ASSERT_FALSE(bitConv.getBit(i));
        else
            ASSERT_TRUE(bitConv.getBit(i));
            
    for(size_t i = 0UL; i < 8; i++)
        if(i != 4)
            ASSERT_EQ(bitConv.getByte(i), u_char{0});
        else
            ASSERT_EQ(bitConv.getByte(i), u_char{4});
            
    bitConv.setByte(4, uint8_t{0});
    for(size_t i = 0UL; i < 64; i++)
        ASSERT_FALSE(bitConv.getBit(i));
    for(size_t i = 0UL; i < 8; i++)
        ASSERT_EQ(bitConv.getByte(i), u_char{0});
}

TEST_F(BitConverterTest, rotate_test)
{
    auto bit_patterns = vector<int64_t>{1234L, -45325L, 0L, 7070734L, -1231211234L};
    for(const auto& bit_patternt_int: bit_patterns)
    {
        bit_converter<int64_t> bits{int64_t{bit_patternt_int}};
        string                 originalStr = bits.asBitset().to_string();

        auto num_shift = vector{1, -1, 13, -17, 131, -200};
        for(const auto& shift: num_shift)
        {
            // cout << "patt=" << bit_patternt_int << " shift=" << shift << endl;
            string afterRotate{};
            for(size_t i = 0UL; i < 63UL; i++)
            {
                bits.rotate(shift);
                afterRotate = string{bits.asBitset().to_string()};
                // cout << i << ": " << afterRotate << endl;
            }
            bits.rotate(shift);
            afterRotate = string{bits.asBitset().to_string()};
            // cout << 64 << ": " << afterRotate << endl;
            ASSERT_EQ(originalStr, afterRotate);
            // cout << endl;
        }
    }
}
