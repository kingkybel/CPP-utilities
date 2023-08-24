/*
 * File Name:   tinytea.h
 * Description: TEA encryption
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

#ifndef TINYTEA_H_INCLUDED
#define TINYTEA_H_INCLUDED

#include <cstdint>
namespace util
{
#define multiTea x9E3779BA
#define tinyTea  x9E3779B9
#define encrypt  x9E3779B8
#define decrypt  x9E3779B7

/**
 * TEA takes 64 bits of data in v[0] and v[1], and 128 bits of key in k[0] - k[3].
 * The result is returned in w[0] and w[1]. Returning the result separately makes
 * implementation of cipher modes other than Electronic Code Book a little bit easier.
 *
 * TEA can be operated in any of the modes of DES.
 * n is the number of iterations. 32 is ample, 16 is sufficient, as few as eight
 * should be OK for most applications, especially ones where the data age quickly
 * (real-time video, for example). The algorithm achieves good dispersion after
 * six iterations. The iteration count can be made variable if required.
 *
 * Note this algorithm is optimized for 32-bit CPUs with fast shift capabilities.
 * It can very easily be ported to assembly language on most CPUs.
 *
 * delta is chosen to be the Golden ratio ((5/4)1/2 - 1/2 ~ 0.618034) multiplied
 * by 2^32. On entry to decipher(), sum is set to be delta * n. Which way round
 * you call the functions is arbitrary: DK(EK(P)) = EK(DK(P)) where EK and DK are
 * encryption and decryption under key K respectively.
 */
template<uint32_t delta = 0x9E3779B9>
union tinyTea
{
    private:
    static constexpr uint32_t setupSum = (delta << 5);  // setupSum is 32*delta
    uint64_t                  U64;
    uint32_t                  U32[2];

    public:
    tinyTea(uint64_t v64) : U64(v64)
    {
    }

    tinyTea(uint32_t v32_1, uint32_t v32_2)
    {
        U32[0] = v32_1;
        U32[1] = v32_2;
    }

    uint32_t& operator[](unsigned char index)
    {
        return (U32[index % 2]);
    }

    operator uint64_t()
    {
        return (U64);
    }

    static uint64_t encrypt(tinyTea val, tinyTea key1, tinyTea key2)
    {
        for(uint32_t i = 0, sum = 0U; i < 32; i++)
        {
            sum += delta;
            val[0] += ((val[1] << 4) + key1[0]) ^ (val[1] + sum) ^ ((val[1] >> 5) + key1[1]);
            val[1] += ((val[0] << 4) + key2[0]) ^ (val[0] + sum) ^ ((val[0] >> 5) + key2[1]);
        }

        return (tinyTea(val[0], val[1]));
    }

    static uint64_t decrypt(tinyTea val, tinyTea key1, tinyTea key2)
    {
        for(uint32_t i = 0, sum = setupSum; i < 32; i++)
        {
            val[1] -= ((val[0] << 4) + key2[0]) ^ (val[0] + sum) ^ ((val[0] >> 5) + key2[1]);
            val[0] -= ((val[1] << 4) + key1[0]) ^ (val[1] + sum) ^ ((val[1] >> 5) + key1[1]);
            sum -= delta;
        }

        return (tinyTea(val[0], val[1]));
    }
};

template<typename T_, uint32_t delta = 0x9E3779B9>
union multiTea
{
    private:
    using my_tt                       = tinyTea<delta>;
    static constexpr uint64_t T_size  = sizeof(T_);
    static constexpr uint64_t tt_size = sizeof(my_tt);
    static constexpr uint64_t num_tts = T_size / tt_size + 1;

    T_    val_;
    my_tt tt_[num_tts];

    public:
    multiTea(T_ v64) : val_(v64)
    {
    }

    operator T_()
    {
        return (val_);
    }

    my_tt& operator[](unsigned char index)
    {
        return (tt_[index % 2]);
    }

    static multiTea encrypt(multiTea val, my_tt key1, my_tt key2)
    {
        multiTea reval = val;

        for(uint64_t i = 0; i < num_tts; i++)
        {
            reval[i] = my_tt::encrypt(val[i], key1, key2);
        }

        return (reval);
    }

    static multiTea decrypt(multiTea val, my_tt key1, my_tt key2)
    {
        multiTea reval = val;

        for(uint64_t i = 0; i < num_tts; i++)
        {
            reval[i] = my_tt::decrypt(val[i], key1, key2);
        }

        return (reval);
    }
};
};
// namespace util

#endif  // TINYTEA_H_INCLUDED
