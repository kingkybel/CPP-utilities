/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/bit_converter.h
 * Description: Utility to access bits and bytes of arbitrary objects.
 *
 * Copyright (C) 2023 Dieter J Kybelksties
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

#ifndef NS_UTIL_BIT_CONVERTER_H_INCLUDED
#define NS_UTIL_BIT_CONVERTER_H_INCLUDED

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <limits.h>

namespace util
{
/**
 * @brief a struct to convert any binary object into its bit-pattern
 *
 * @tparam T_ type of the object to convert
 */
template<typename T_>
union bit_converter
{
    using DataType                           = T_;
    static constexpr long long BYTES_IN_CHAR = CHAR_BIT >> 3;
    static constexpr long long BYTES_IN_DATA = (sizeof(DataType) / sizeof(uint8_t)) * BYTES_IN_CHAR;
    static constexpr long long BITS_IN_DATA  = BYTES_IN_DATA << 3;

    DataType data_;
    uint8_t  byte[BYTES_IN_DATA];

    /**
     * @brief Construct a new bit converter object.
     * If ValueT_ does not have a default constructor then parameter data has to be provided.
     *
     * @param data object to convert
     */
    bit_converter(DataType data = T_{}) : data_(data)
    {
    }

    /**
     * @brief Convert from a homogenous initializer list of type ElType_. If the total memory of all objects in the
     * initList exceeds the total memory of the bit converter, then throw a std::length_error
     *
     * @tparam ElType_ type of elements of the  initializer list
     * @param initList the initializer list. all elements need to be of type ElType_
     * @return bit_converter the converter object
     * @throw std::length_error
     */
    template<typename ElType_>
    static bit_converter from(std::initializer_list<ElType_> initList)
    {
        bit_converter   reval;
        const long long numOfBytes = initList.size() * (sizeof(ElType_) / BYTES_IN_CHAR) * sizeof(int8_t);
        if(numOfBytes > BYTES_IN_DATA)
        {
            throw std::length_error("Total size of bytes of given arguments exceeeds actual size of bit_converter");
        }

        union BytesOfEl
        {
            ElType_ el;
            uint8_t byte[sizeof(ElType_)];
        };
        size_t i{0UL};
        size_t byteNum{0UL};
        for(auto v: initList)
        {
            BytesOfEl b{v};
            for(size_t j = 0UL; j < sizeof(ElType_); j++)
            {
                byteNum             = i * sizeof(ElType_) + j;
                reval.byte[byteNum] = (byteNum < numOfBytes) ? b.byte[j] : 0U;
            }
            i++;
        }

        while(byteNum < BYTES_IN_DATA)
            reval.byte[byteNum++] = false;

        return (reval);
    }

    /**
     * @brief cast operator to the DataType
     *
     * @return DataType this cast to the DataType
     */
    operator DataType()
    {
        return (data_);
    }

    /**
     * @brief convert to a bitset
     *
     * @tparam NumberOfBits_ number of bits of the object
     * @param StartBit_ bit where to start the conversion
     * @return std::bitset<NumberOfBits_> the bitset
     */
    template<long long NumberOfBits_ = BITS_IN_DATA>
    std::bitset<NumberOfBits_> asBitset(long long StartBit_ = 0LL) const
    {
        static_assert(NumberOfBits_ > 0, "Number of requested bits needs to be greater than 0.");
        std::bitset<BITS_IN_DATA> allbits(0ULL);

        for(size_t ullCount = BYTES_IN_DATA; ullCount > 0; ullCount--)
        {
            allbits <<= CHAR_BIT;
            allbits |= std::bitset<BITS_IN_DATA>(byte[ullCount - 1]);
        }

        std::bitset<NumberOfBits_> reval;

        for(long long i = 0; i < NumberOfBits_; i++)
        {
            long long allIdx = (i + StartBit_);
            if(allIdx >= 0LL && allIdx < BITS_IN_DATA)
            {
                reval[i] = allbits[allIdx];
            }
        }

        return (reval);
    }

    /**
     * @brief Rotate the bits of the object
     *
     * @param bitsToShift number of positions to shift, negative numbers means shift to right
     */
    void rotate(long long bitsToShift)
    {
        if(bitsToShift != 0LL)
        {
            std::bitset<BITS_IN_DATA> allBits = asBitset();
            bool                      toLeft  = (bitsToShift < 0);

            bitsToShift %= BITS_IN_DATA;

            for(long long shiftCount = 0; shiftCount < std::abs(bitsToShift); shiftCount++)
            {
                bool extremeIsSet = (toLeft) ? allBits[BITS_IN_DATA - 1] : allBits[0];
                if(toLeft)
                {
                    allBits <<= 1;
                    allBits.set(0, extremeIsSet);
                }
                else
                {
                    allBits >>= 1;
                    allBits.set(BITS_IN_DATA - 1, extremeIsSet);
                }
            }

            static constexpr std::bitset<BITS_IN_DATA> mask = (static_cast<unsigned long long>(-1));

            for(size_t ullCount = 0; ullCount < BITS_IN_DATA; ullCount++)
            {
                setBit(ullCount, allBits[ullCount]);
            }
        }
    }

    /**
     * @brief Get a byte from the object
     *
     * @param n number of the byte
     * @return uint8_t& the byte
     */
    uint8_t& operator[](long long n)
    {
        return (byte[n]);
    }

    /**
     * @brief Get a byte from the object
     *
     * @param n number of the byte
     * @return uint8_t the byte
     */
    uint8_t getByte(long long n) const
    {
        return (byte[n]);
    }

    /**
     * @brief Set a byte in the object
     *
     * @param n number of the byte
     * @param val the new byte for position n
     */
    void setByte(long long n, uint8_t val)
    {
        byte[n] = val;
    }

    /**
     * @brief Get the bit value at bitIntex
     *
     * @param bitIndex index of the bit
     * @return true, if bit at the index is set, false otherwise
     */
    bool getBit(long long bitIndex) const
    {
        long long byteIdx = bitIndex / 8;

        uint8_t mask = (1 << (bitIndex % 8));

        return ((byte[byteIdx] & mask) == mask);
    }

    /**
     * @brief Set the bit at bitIntex
     *
     * @param bitIndex index of the bit
     * @param b new bit value
     */
    void setBit(long long bitIndex, bool b = true)
    {
        long long byteIdx = bitIndex >> 3;
        uint8_t   mask    = (1 << (bitIndex % 8));

        if(b == true)
            byte[byteIdx] |= mask;
        else
            byte[byteIdx] &= ~mask;
    }
};
};  // namespace util

#endif  // NS_UTIL_BIT_CONVERTER_H_INCLUDED
