/*
 * File Name:   primes.cc
 * Description: double values transformed into logarithmic domain
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
 * @date: 2019-11-17
 * @author: Dieter J Kybelksties
 */

#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

using namespace std;
namespace util
{
    static map<size_t, vector<size_t>, std::greater<size_t> > testSets = {
        {2047u,
            {2u}},
        {1373653u,
            {2u, 3u}},
        {9080191u,
            {31u, 73u}},
        {25326001u,
            {2u, 3u, 5u}},
        {3215031751u,
            {2u, 3u, 5u, 7u}},
        {4759123141u,
            {2u, 7u, 61u}},
        {1122004669633u,
            {2u, 13u, 23u, 1662803u}},
        {2152302898747u,
            {2u, 3u, 5u, 7u, 11u}},
        {3474749660383u,
            {2u, 3u, 5u, 7u, 11u, 13u}},
        {341550071728321,
            {2u, 3u, 5u, 7u, 11u, 13u, 17u}},
        {3825123056546413051u,
            {2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u}},
        //    {18446744073709551616u,
        //        {2u, 3u, 5u, 7u, 11u, 13u, 17u, 19u, 23u, 29u, 31u, 37u}}
    };

    template<typename T_>
    T_ powerMod(T_ b, T_ e, T_ modul)
    {
        long double p = pow((long double) b, (long double) e) / (long double) modul;
        long double rnd = trunc(p);
        long double rest = p - rnd;
        //    cout << b << "^" << e << " % " << modul << " =" << ((size_t) pow(b, e)) % modul << " || " << rest * modul << endl;
        return rest*modul;
    }

    /**
     * Miller-Test:
     *  https://en.wikipedia.org/wiki/Miller%E2%80%93Rabin_primality_test.
     * <code>
    Input: n > 1, an odd integer to be tested for primality
    Output: “composite” if n is composite, “prime” otherwise

    write n as 2r·d + 1 with d odd (by factoring out powers of 2 from n − 1)
    WitnessLoop: for all a in the range [2, min(n−2, ⌊2(ln n)2⌋)]:
       x ← ad mod n
       if x = 1 or x = n − 1 then
          continue WitnessLoop
       repeat r − 1 times:
          x ← x2 mod n
          if x = n − 1 then
             continue WitnessLoop
       return “composite”
    return “prime”
     * </code>
     *
     *
     * if n < 2,047, it is enough to test a = 2;
     * if n < 1,373,653, it is enough to test a = 2 and 3;
     * if n < 9,080,191, it is enough to test a = 31 and 73;
     * if n < 25,326,001, it is enough to test a = 2, 3, and 5;
     * if n < 3,215,031,751, it is enough to test a = 2, 3, 5, and 7;
     * if n < 4,759,123,141, it is enough to test a = 2, 7, and 61;
     * if n < 1,122,004,669,633, it is enough to test a = 2, 13, 23, and 1662803;
     * if n < 2,152,302,898,747, it is enough to test a = 2, 3, 5, 7, and 11;
     * if n < 3,474,749,660,383, it is enough to test a = 2, 3, 5, 7, 11, and 13;
     * if n < 341,550,071,728,321, it is enough to test a = 2, 3, 5, 7, 11, 13, and 17.
     */
    bool isPrimeMiller(size_t n)
    {
        if (n == 2)
            return true;
        if (n < 2 || n % 2 == 0)
            return false;
        // n is odd and n>=3

        // write n as 2^r·d + 1 with d odd (by factoring out powers of 2 from n − 1)
        size_t d = n - 1;
        size_t r = 0;
        while (d % 2 == 0)
        {
            d /= 2;
            r++;
        }
        //    cout << "n=" << n << " d=" << d << " r=" << r << " 2^r*d+1=" << pow(2, r) * d + 1 << endl;
        vector<size_t> aVec;
        for (auto aTestSet : testSets)
            if (n < aTestSet.first)
            {
                aVec = aTestSet.second;
                //            cout << n << "<" << aTestSet.first << ":";
                //            copy(aVec.begin(), aVec.end(), ostream_iterator<size_t>(cout, ", "));
                //            cout << endl;
            }
        //    cout << "\tCHOSEN: ";
        //    copy(aVec.begin(), aVec.end(), ostream_iterator<size_t>(cout, ", "));
        //    cout << endl;

        //write n as 2^r·d + 1 with d odd (by factoring out powers of 2 from n − 1)
        //WitnessLoop: for all a in the range [2, min(n−2, ⌊2(ln n)2⌋)]:
        //   x ← a^d mod n
        //   if x = 1 or x = n − 1 then
        //      continue WitnessLoop
        //   repeat r − 1 times:
        //      x ← x^2 mod n
        //      if x = n − 1 then
        //         continue WitnessLoop
        //   return “composite”
        //return “prime”
        for (auto a : aVec)
        {
            //size_t x = ((size_t) pow(a, d)) % n;
            size_t x = (size_t) powerMod(a, d, n);

            if (x != 1 && x != n - 1)
            {
                bool skip = false;
                for (size_t rIt = 0; rIt < r - 1 && !skip; rIt++)
                {
                    x = (x * x) % n;
                    if (x == n - 1)
                    {
                        skip = true;
                    }
                }
                if (!skip)
                    return false;
            }
        }
        return true;
    }
}; // namespace util