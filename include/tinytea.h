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

    static void encipher(unsigned long *const data,
                         unsigned long *const encryptedResult,
                         const unsigned long *const key)
    {
        register unsigned long y = data[0];
        register unsigned long z = data[1];
        register unsigned long sum = 0;
        register unsigned long delta = 0x9E3779B9;
        register unsigned long a = key[0];
        register unsigned long b = key[1];
        register unsigned long c = key[2];
        register unsigned long d = key[3];
        register unsigned long n = 32;

        while (n-- > 0)
        {
            sum += delta;
            y += (z << 4) + a ^ z + sum ^ (z >> 5) + b;
            z += (y << 4) + c ^ y + sum ^ (y >> 5) + d;
        }

        encryptedResult[0] = y;
        encryptedResult[1] = z;
    }

    static void decipher(unsigned long *const encryptedData,
                         unsigned long *const decryptedResult,
                         const unsigned long *const key)
    {
        register unsigned long y = encryptedData[0];
        register unsigned long z = encryptedData[1];
        register unsigned long sum = 0xC6EF3720;
        register unsigned long delta = 0x9E3779B9;
        register unsigned long a = key[0];
        register unsigned long b = key[1];
        register unsigned long c = key[2];
        register unsigned long d = key[3];
        register unsigned long n = 32;

        /* sum = delta<<5, in general sum = delta * n */

        while (n-- > 0)
        {
            z -= (y << 4) + c ^ y + sum ^ (y >> 5) + d;
            y -= (z << 4) + a ^ z + sum ^ (z >> 5) + b;
            sum -= delta;
        }

        decryptedResult[0] = y;
        decryptedResult[1] = z;
    }

    static void encipherNew(const unsigned long *const v,
                            unsigned long *const w,
                            const unsigned long * const k)
    {
        register unsigned long y = v[0];
        register unsigned long z = v[1];
        register unsigned long sum = 0;
        register unsigned long delta = 0x9E3779B9;
        register unsigned long n = 32;

        while (n-- > 0)
        {
            y += (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
            sum += delta;
            z += (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
        }

        w[0] = y;
        w[1] = z;
    }

    static void decipherNew(const unsigned long *const v,
                            unsigned long *const w,
                            const unsigned long * const k)
    {
        register unsigned long y = v[0];
        register unsigned long z = v[1];
        register unsigned long sum = 0xC6EF3720;
        register unsigned long delta = 0x9E3779B9;
        register unsigned long n = 32;

        /* sum = delta<<5, in general sum = delta * n */

        while (n-- > 0)
        {
            z -= (y << 4 ^ y >> 5) + y ^ sum + k[sum >> 11 & 3];
            sum -= delta;
            y -= (z << 4 ^ z >> 5) + z ^ sum + k[sum & 3];
        }

        w[0] = y;
        w[1] = z;
    }

};

#endif // TINYTEA_H_INCLUDED

