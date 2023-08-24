/*
 * File Name:   primes.h
 * Description: some simple prime calculation routines
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

#ifndef PRIMES_H_INCLUDED
#define PRIMES_H_INCLUDED

#include <bitset>
#include <cmath>
#include <iostream>
#include <iterator>
#include <limits>
#include <set>
#include <unordered_set>
#include <vector>

namespace util
{
/**
 * Generic ostream - &lt;&lt; operator for vectors.
 */
template<typename T_, typename Alloc_>
inline std::ostream &operator<<(std::ostream &os, const std::vector<T_, Alloc_> &vec)
{
    if(!vec.empty())
    {
        os << "< ";
        std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T_>(os, " | "));
        os << vec.back() << " >";
    }
    else
        os << "< >";

    return (os);
}

/**
 * Generic ostream - &lt;&lt; operator for unordered sets.
 */
template<typename Value, typename Hash, typename Pred, typename Alloc>
inline std::ostream &operator<<(std::ostream &os, const std::unordered_set<Value, Hash, Pred, Alloc> &vec)
{
    if(!vec.empty())
    {
        os << "{~ ";
        for(auto it = vec.begin(); it != vec.end(); it++)
        {
            os << *it << " ";
        }
        os << "~}";
    }
    else
        os << "{~ ~}";

    return (os);
}

/**
 * Create a vector from a given set.
 */
template<typename T_, typename Compare_, typename Alloc_>
inline std::vector<T_, Alloc_> vectorFromSet(const std::set<T_, Compare_, Alloc_> &s)
{
    std::vector<T_, Alloc_> reval;

    if(!s.empty())
        std::copy(s.begin(), s.end(), std::back_inserter(reval));

    return (reval);
}

/**
 * Generic ostream - &lt;&lt; operator for standard (ordered) set.
 */
template<typename T_, typename Compare_, typename Alloc_>
inline std::ostream &operator<<(std::ostream &os, const std::set<T_, Compare_, Alloc_> &s)
{
    if(!s.empty())
    {
        auto vec = vectorFromSet(s);

        os << "{ ";
        std::copy(vec.begin(), vec.end() - 1, std::ostream_iterator<T_>(os, ", "));
        os << vec.back() << " }";
    }
    else
        os << "{ }";

    return (os);
}

#define DEBUG (false)
#define SHOW(v)                                           \
    {                                                     \
        DEBUG &&std::cout << #v << "=" << v << std::endl; \
    }

// template<unsigned long long StepInitLimit_ = 11 >

class prime_checker
{
    private:
    const long double MAX_NUMBER = static_cast<const long double>(std::numeric_limits<unsigned long long>::max());
    const unsigned long long ROOT_MAX_NUMBER = sqrt(MAX_NUMBER);

    public:
    bool divisibleByAny(unsigned long long n, std::vector<unsigned long long> divisors)
    {
        for(auto divisor: divisors)
            if(n != divisor && n % divisor == 0)
                return (true);

        return (false);
    }

    /**
     * We don't need to check all all numbers in [2..sqrt(n)] for primeness of n.
     * It would be sufficient to test for all prime-numbers, however that would
     * mean to keep a record of all primes found so far. As a compromise the
     * following approach is taken in reference to the Sieve of Erathostenes:
     * The first StepInitLimit_ prime numbers are taken to calculate a
     * non-equidistant step size of numbers to be checked.
     * e.g. StepInitLimit_ == 3
     * First 3 primes 2, 3, 5
     * All multiples of 2, 3, 5 do not need to be checked, so step would look
     * like this starting from 3:
     * 2*3*5 = 30
     * multiples of 2, 3, 5 --> then all the spaces that do not need to be checked
     *          1         2         3         4         5         6
     * 123456789012345678901234567890123456789012345678901234567890
     *  x x x x x x x x x x x x x x x
     *     x  x  x  x  x  x  x  x  x
     *          x    x    x    x    x
     *  XXXXX XXX X XXX X XXX XXXXX X
     * o o   o   o o   o o   o     o
     * the array step contains the gap-size between the
     *  2 | 2 | 2 | 4 | 2 | 4 | 2 | 4 | 6
     */
    //    std::vector<unsigned long long> findSteps()
    //    {
    //        std::vector<unsigned long long> rakePrimes;
    //        unsigned long long max = 1;
    //        for (unsigned long long n = 2; rakePrimes.size() < StepInitLimit_; n++)
    //            if (isPrime(n))
    //            {
    //                rakePrimes.push_back(n);
    //                max *= n;
    //            }
    //
    //        step.push_back(2);
    //        unsigned long long current = 3;
    //        for (unsigned long long next = 5; next < max; next += 2)
    //        {
    //            if (!divisibleByAny(next, rakePrimes))
    //            {
    //                step.push_back(next - current);
    //                current = next;
    //            }
    //        }
    //
    //        return step;
    //    }
    public:
    std::vector<unsigned long long>        step;
    std::set<unsigned long long>           memoizedPrimes;
    std::unordered_set<unsigned long long> memoizedNonPrimes;

    prime_checker()
    {
        //        step = findSteps();
        //        SHOW(step);
    }

    //    /**
    //     * Check whether a number is prime or not.
    //     */
    //    bool isPrime(unsigned long long n, bool useAll = true)
    //    {
    //        if (memoizedPrimes.find(n) != memoizedPrimes.end())
    //            return true;
    //
    //        // If the number was memoized as non-prime then don't bother with a lengthy
    //        // calculation
    //        if (!memoizedNonPrimes.empty() &&
    //            (memoizedNonPrimes.find(n) != memoizedNonPrimes.end()))
    //                return false;
    //
    //        if (n < 2)
    //            return false;
    //
    //        if (n == 2)
    //        {
    //            memoizedPrimes.insert(2);
    //            return true;
    //        }
    //        if (n % 2 == 0)
    //            return false;
    //
    //        unsigned long long root = sqrt(n);
    //        unsigned long long sizeOfSteps = step.size();
    //        // start from index "1" to avoid irregularity at prime "2"
    //        unsigned long long currentStepIndex = 1;
    //        for (unsigned long long divisor = 3;
    //             divisor < root;
    //             divisor += step[currentStepIndex],
    //             currentStepIndex = (currentStepIndex + 1) % sizeOfSteps)
    //        {
    //            if (n % divisor == 0)
    //                return false;
    //        }
    //
    //        // add some of those non-primes to the non-prime memo which would take a
    //        // long time to calculate, e.g. squares of primes as these have to go
    //        // through the maximum numbers of checks
    //
    //        if (n <= ROOT_MAX_NUMBER)
    //        {
    //            memoizedNonPrimes.insert(n * n);
    //        }
    //
    //        auto it = memoizedPrimes.rbegin();
    //
    //        if (n <= ROOT_MAX_NUMBER && memoizedPrimes.size() > 1)
    //        {
    //            memoizedNonPrimes.insert(*it * n);
    //        }
    //
    //        if (memoizedPrimes.size() > 2)
    //        {
    //            it++;
    //            memoizedNonPrimes.insert(*it * n);
    //        }
    //
    //        if (memoizedPrimes.size() > 3)
    //        {
    //            it++;
    //            memoizedNonPrimes.insert(*it * n);
    //        }
    //
    //        memoizedPrimes.insert(n);
    //        SHOW(memoizedNonPrimes);
    //        SHOW(memoizedPrimes);
    //        SHOW(" ");
    //
    //        return true;
    //    }

    static constexpr unsigned long long SIZE_ULL      = 2'000'000;  // 22'347'494;
    static constexpr unsigned long long ROOT_SIZE_ULL = sqrt(SIZE_ULL) + 1;
    std::bitset<SIZE_ULL>               sieve;
    unsigned long long                  partialSieveSize = 2;

    bool makePartialSieve()
    {
        sieve = ~std::bitset<SIZE_ULL>(0LL);  // all true
        for(size_t p = 2; p < ROOT_SIZE_ULL;)
        {
            for(size_t n = p + p; n <= SIZE_ULL; n += p)
                sieve[n] = false;
            p++;
            while(p < ROOT_SIZE_ULL && sieve[p] == false)
                p++;

            if(static_cast<long double>(p) * static_cast<long double>(partialSieveSize)
               < static_cast<long double>(ROOT_SIZE_ULL))
                partialSieveSize *= p;
        }

        return (true);
    }

    bool isPrime(unsigned long long n)
    {
        // static bool madePartial = makePartialSieve();

        if(n < SIZE_ULL)
            return (sieve[n]);

        if(sieve[n % partialSieveSize] == false)
            return (false);

        // unsigned long long currentStepIndex = 1;

        for(unsigned long long divisor = 2; divisor < ROOT_SIZE_ULL; divisor++)
        {
            if(sieve[divisor % partialSieveSize] && n % divisor == 0)
                return (false);
        }

        return (true);
    }
};
};

#endif  // PRIMES_H_INCLUDED
