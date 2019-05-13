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
 * @date: 2019-04-25
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_LOGVALUE_H_INCLUDED
#define	NS_UTIL_LOGVALUE_H_INCLUDED

#include <cmath>
#include <ostream>
namespace util
{

    class logVal
    {
    private:
        long double val;
        bool isPositive_;
    public:

        logVal(long double logDomainVal, bool isPositive = true)
        : val(logDomainVal)
        , isPositive_(isPositive)
        {

        }

        static logVal fromReal(long double realDomainVal)
        {
            return realDomainVal >= 0.0 ?
                    log(realDomainVal) :
                    logVal(-realDomainVal, false);
        }

        const long double toReal() const
        {
            return isPositive_ ? exp(val) : -exp(val);
        }

        /**
         * Takes two values in the logarithmic domain and adds them together.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return sum of two values converted back into the logarithmic domain
         */
        friend logVal operator+(const logVal& lhs, const logVal& rhs)
        {
            return (lhs.val == -INFINITY) ? rhs :
                    (rhs.val == -INFINITY) ? lhs :
                    (lhs.val > rhs.val) ? lhs.val + log(1 + exp(rhs.val - lhs.val)) :
                    rhs.val + log(1 + exp(lhs.val - rhs.val));
        }

        /**
         * Takes two values in the logarithmic domain and multiplies them.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return product of two values converted back into the logarithmic domain
         */
        friend logVal operator*(const logVal& lhs, const logVal& rhs)
        {
            return (lhs.val == -INFINITY) || (rhs.val == -INFINITY) ?
                    -INFINITY :
                    (lhs.val + rhs.val);
        }

        /**
         * Takes two values in the logarithmic domain and multiplies them.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return product of two values converted back into the logarithmic domain
         */
        friend std::ostream& operator<<(std::ostream& os, const logVal& val)
        {
            os << (val.isPositive_ ? "" : "-") << val.val;
            return os;
        }


    };
}; // namespace util

#endif	// NS_UTIL_LOGVALUE_H_INCLUDED

