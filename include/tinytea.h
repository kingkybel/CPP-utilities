/*
 * File Name:   tinytea.h
 * Description: TEA encryption
 *
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
 * This particular algorithm is in the public domain:
 * http://143.53.36.235:8080/source.htm
 *
 * @date: 2019-10-22
 * @author: Dieter J Kybelksties
 */

#ifndef TINYTEA_H_INCLUDED
#define TINYTEA_H_INCLUDED

#include <stdint.h>

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
 * Note this algorithm is optimised for 32-bit CPUs with fast shift capabilities.
 * It can very easily be ported to assembly language on most CPUs.
 *
 * delta is chosen to be the Golden ratio ((5/4)1/2 - 1/2 ~ 0.618034) multiplied
 * by 2^32. On entry to decipher(), sum is set to be delta * n. Which way round
 * you call the functions is arbitrary: DK(EK(P)) = EK(DK(P)) where EK and DK are
 * encryption and decryption under key K respectively.
 */
struct tinyTea
{
    static constexpr uint64_t lwr32 = (uint64_t(uint32_t(-1)));
    static constexpr uint64_t upr32 = (lwr32 << 32);
    static constexpr uint32_t delta = 0x9E3779B9; // a key schedule constant

public:
    static uint64_t encrypt(uint64_t val, uint64_t key1, uint64_t key2)
    {
        uint32_t v0 = (val & upr32) >> 32;
        uint32_t v1 = (val & lwr32);
        uint32_t sum = 0U;
        uint32_t k0 = (key1 & upr32) >> 32;
        uint32_t k1 = (key1 & lwr32);
        uint32_t k2 = (key2 & upr32) >> 32;
        uint32_t k3 = (key2 & lwr32);

        for (uint32_t i = 0; i < 32; i++)
        { // basic cycle start
            sum += delta;
            v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
            v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        } // end cycle

        return (uint64_t(v0) << 32) | uint64_t(v1);
    }

    static uint64_t decrypt(uint64_t val, uint64_t key1, uint64_t key2)
    {
        uint32_t v0 = (val & upr32) >> 32;
        uint32_t v1 = (val & lwr32);
        uint32_t sum = 0xC6EF3720;
        uint32_t k0 = (key1 & upr32) >> 32;
        uint32_t k1 = (key1 & lwr32);
        uint32_t k2 = (key2 & upr32) >> 32;
        uint32_t k3 = (key2 & lwr32);
        for (uint32_t i = 0, sum = 0xC6EF3720; i < 32; i++) // set up; sum is 32*delta
        { // basic cycle start
            v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
            v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
            sum -= delta;
        } // end cycle

        return (uint64_t(v0) << 32) | uint64_t(v1);
    }
};

#endif // TINYTEA_H_INCLUDED

