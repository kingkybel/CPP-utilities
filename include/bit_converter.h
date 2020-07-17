/*
 * File Name:   bit_converter.h
 * Description: Utility to access bits and bytes of arbitrary objects.
 * Copyright (C) 2019 Dieter J Kybelksties
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
 * @date: 2020-07-17
 * @author: Dieter J Kybelksties
 */

#if !defined(NS_UTIL_BIT_CONVERTER_H_INCLUDED)
#define NS_UTIL_BIT_CONVERTER_H_INCLUDED

#include <cstdlib>
#include <iostream>
#include <bitset>
#include <algorithm>
#include <limits.h>
#include <initializer_list>
namespace util
{

   template<typename T_>
   union bit_converter
   {
      typedef T_ DataType;
      static constexpr long long BYTES_IN_DATA = sizeof (DataType) / sizeof (uint8_t);
      static constexpr long long BITS_IN_DATA = BYTES_IN_DATA << 3;
      static constexpr long long BYTES_IN_CHAR = CHAR_BIT >> 3;
      static constexpr long long LONG_LONGS_IN_DATA =
            std::max(1UL, sizeof (DataType) / sizeof (unsigned long long));
      static constexpr size_t BITS_IN_LONG_LONG = sizeof (unsigned long long) * 8;

      DataType data_;
      unsigned long long ll[LONG_LONGS_IN_DATA];
      uint8_t byte[sizeof (long double) / sizeof (uint8_t)];

      bit_converter(DataType data = T_())
      : data_(data)
      {
      }

      template<typename FT_>
      static bit_converter from(std::initializer_list<FT_> ullList)
      {
         bit_converter reval;
         const long long numOfBytes =
               ullList.size() * (sizeof (FT_) / BYTES_IN_CHAR) * sizeof (int8_t);
         long long i = 0LL;
         for (auto v : ullList)
         {
            reval.byte[i] = (i < numOfBytes) ? v : 0U;
            i++;
         }
         while (i < BYTES_IN_DATA)
            reval.byte[i++] = false;
         return reval;
      }

      template <long long NumberOfBits_ = BITS_IN_DATA>
      std::bitset<NumberOfBits_> asBitset(long long StartBit_ = 0LL) const
      {
         std::bitset<BITS_IN_DATA> allbits(0ULL);
         for (size_t ullCount = LONG_LONGS_IN_DATA; ullCount > 0; ullCount--)
         {
            allbits <<= (ullCount * (sizeof (unsigned long long) * 8));
            allbits |= std::bitset<BITS_IN_DATA>(ll[ullCount - 1]);
         }

         std::bitset<NumberOfBits_> reval;
         for (long long i = 0; i < NumberOfBits_; i++)
         {
            if (i + StartBit_ < BITS_IN_DATA)
            {
               reval[i] = allbits[i + StartBit_];
            }
         }

         return reval;
      }

      void rotate(long long bitsToShift)
      {
         if (bitsToShift != 0LL)
         {
            std::bitset<BITS_IN_DATA> allBits = asBitset();

            bool toLeft = (bitsToShift < 0);
            bitsToShift %= BITS_IN_DATA;
            for (long long shiftCount = 0; shiftCount < abs(bitsToShift); shiftCount++)
            {
               bool extremeIsSet = (toLeft) ? allBits[BITS_IN_DATA - 1] : allBits[0];
               if (toLeft)
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

            static constexpr std::bitset<BITS_IN_DATA> mask = ((unsigned long long) (-1));
            for (size_t ullCount = 0; ullCount < LONG_LONGS_IN_DATA; ullCount++)
            {
               ll[ullCount] = ((allBits >> (ullCount * BITS_IN_LONG_LONG)) & mask).to_ullong();
            }
         }
      }

      uint8_t getByte(long long n) const
      {
         return byte[n];
      }

      void setByte(long long n, uint8_t val)
      {
         byte[n] = val;
      }

      bool getBit(long long bitIndex) const
      {
         return asBitset()[bitIndex];
      }

      void setBit(long long bitIndex, bool b = true)
      {
         int8_t mask = (1 << (bitIndex % 8));
         if (b == true)
            byte[bitIndex / 8] |= mask;
         else
            byte[bitIndex / 8] &= ~mask;
      }
   };
}; // namespace util

#endif // NS_UTIL_BIT_CONVERTER_H_INCLUDED
