/*
 * File Name:   logvalue.h
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

#ifndef PRIMES_H_INCLUDED
#define PRIMES_H_INCLUDED

#include <unordered_set>
#include <vector>
#include <stringutil.h>

template<size_t StepInitLimit_ = 11 >
class prime_checker
{
    static std::vector step;

    static bool isPrime(size_t n, bool useAll = true)
    {
        if (useAll)
        {
            if (n < 2)
                return false;
            if (n == 2)
                return true;
            if (n % 2 == 0)
                return false;
            size_t root = sqrt(n);
            for (size_t i = 3; i < root; i += 2)
            {
                if (n % i == 0)
                    return false;
            }
        }
        return true;
    }

    static void findSteps()
    {
        std::vector<size_t> rakePrimes;
        size_t max = 1;
        for (size_t n = 2; n < StepInitLimit_; n++)
            if (isPrime(n))
            {
                rakePrimes.push_back(n);
                max *= n;
            }

        size_t current = 2;
        for (size_t next = 3; next < max; next += 2)
        {
            bool isNotRaked = true;
            for (auto n : rakePrimes)
                if (next % n != 0)
                    isNotRaked = true;
            if (isNotRaked)
            {
                step.push_back(next - current);
                current = next;
            }
        }
    }

    /**
     * Check whether a number is prime or not.
     */
    static bool isPrime(size_t n, bool useAll = true)
    {
        if (n < 2)
            return false;
        if (n == 2)
            return true;
        if (n % 2 == 0)
            return false;
        size_t root = sqrt(n);
        for (size_t i = 3; i < root; i += 2)
        {
            if (n % i == 0)
                return false;
        }
        return true;
    }


};

#endif // PRIMES_H_INCLUDED

