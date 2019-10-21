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
#define NS_UTIL_LOGVALUE_H_INCLUDED

#include <cmath>
#include <ostream>
#include <exception>
#define DO_TRACE_
#include <traceutil.h>
namespace util
{

    class logVal
    {
    private:
        long double val_;
        bool isPositive_;

        logVal(long double logDomainVal, bool isPositive)
        : val_(logDomainVal)
        , isPositive_(isPositive)
        {
        }

    public:

        bool isZero() const
        {
            return val_ == -INFINITY;
        }

        static logVal fromReal(long double realDomainVal)
        {
            return realDomainVal >= 0.0L ?
                logVal(log(realDomainVal), true) :
                logVal(log(-realDomainVal), false);
        }

        static logVal fromLog(long double logDomainVal, bool isPositive = true)
        {
            return logVal(logDomainVal, isPositive);
        }

        logVal(long double realDomainVal)
        : val_(log(realDomainVal >= 0.0L ? realDomainVal : -realDomainVal))
        , isPositive_(realDomainVal >= 0.0L)
        {

        }

        const long double toReal() const
        {
            return isPositive_ ? exp(val_) : -exp(val_);
        }

        /**
         * Takes two values in the logarithmic domain and adds them together.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return sum of two values converted back into the logarithmic domain
         */
        friend logVal operator+(const logVal& lhs, const logVal& rhs)
        {
            long double lhsDbl = lhs.isPositive_ ? exp(lhs.val_) : -exp(lhs.val_);
            long double rhsDbl = rhs.isPositive_ ? exp(rhs.val_) : -exp(rhs.val_);

            return logVal(lhsDbl + rhsDbl);

            // (lhs.val_ > rhs.val_) ? lhs.val_ + log(1 + exp(rhs.val_ - lhs.val_)) :
            // rhs.val_ + log(1 + exp(lhs.val_ - rhs.val_));
        }

        /**
         * Takes two values in the logarithmic domain and adds them together.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return sum of two values converted back into the logarithmic domain
         */
        logVal& operator+=(const logVal& rhs)
        {
            logVal reval = *this+rhs;
            val_ = reval.val_;
            isPositive_ = reval.isPositive_;
            return *this;
        }

        /**
         * Unary minus operator.
         * @param lhs left-hand-side value in logarithmic domain
         * @return negative value of lhs converted back into the logarithmic domain
         */
        friend logVal operator-(const logVal& lhs)
        {
            return fromLog(lhs.val_, !lhs.isPositive_);
        }

        /**
         * Takes two values in the logarithmic domain and adds them together.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return sum of two values converted back into the logarithmic domain
         */
        friend logVal operator-(const logVal& lhs, const logVal& rhs)
        {
            long double lhsDbl = lhs.isPositive_ ? exp(lhs.val_) : -exp(lhs.val_);
            long double rhsDbl = rhs.isPositive_ ? exp(rhs.val_) : -exp(rhs.val_);

            return logVal(lhsDbl - rhsDbl);
        }

        /**
         * Takes two values in the logarithmic domain and multiplies them.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return product of two values converted back into the logarithmic domain
         */
        friend logVal operator*(const logVal& lhs, const logVal& rhs)
        {
            return fromLog(lhs.isZero() || rhs.isZero() ? -INFINITY : (lhs.val_ + rhs.val_),
                           (lhs.isPositive_ == rhs.isPositive_));
        }

        /**
         * Takes two values in the logarithmic domain and divides them.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return quotient of two values converted back into the logarithmic domain
         */
        friend logVal operator/(const logVal& lhs, const logVal& rhs)
        {
            if (rhs.isZero())
            {
                throw std::runtime_error("Attempted division by zero.");
            }
            return fromLog(lhs.isZero() ? -INFINITY : (lhs.val_ - rhs.val_),
                           (lhs.isPositive_ == rhs.isPositive_));
        }

        /**
         * Takes two values in the logarithmic domain and multiplies them.
         * @param rhs right-hand-side value in logarithmic domain
         * @return product of two values converted back into the logarithmic domain
         */
        logVal& operator*=(const logVal& rhs)
        {
            logVal reval = *this*rhs;
            val_ = reval.val_;
            isPositive_ = reval.isPositive_;
            return *this;
        }

        /**
         * Equality comparison operator.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return true if lhs equals rhs, false otherwise
         */
        friend bool operator==(const logVal& lhs, const logVal& rhs)
        {
            return lhs.isPositive_ == rhs.isPositive_ && lhs.val_ == rhs.val_;
        }

        /**
         * Less-than comparison operator.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return true if lhs less than rhs, false otherwise
         */
        friend bool operator<(const logVal& lhs, const logVal& rhs)
        {
            if (lhs.isPositive_ && rhs.isPositive_)
                return lhs.val_ < rhs.val_;
            if (!lhs.isPositive_ && !rhs.isPositive_)
                return lhs.val_ > rhs.val_;

            return rhs.isPositive_;
        }

        /**
         * Less-than-or-equal comparison operator.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return true if lhs less than or equal rhs, false otherwise
         */
        friend bool operator<=(const logVal& lhs, const logVal& rhs)
        {
            if (lhs.isPositive_ && rhs.isPositive_)
                return lhs.val_ <= rhs.val_;
            if (!lhs.isPositive_ && !rhs.isPositive_)
                return lhs.val_ >= rhs.val_;

            return rhs.isPositive_;
        }

        /**
         * Greater-than comparison operator.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return true if lhs greater than rhs, false otherwise
         */
        friend bool operator>(const logVal& lhs, const logVal& rhs)
        {
            if (lhs.isPositive_ && rhs.isPositive_)
                return lhs.val_ > rhs.val_;
            if (!lhs.isPositive_ && !rhs.isPositive_)
                return lhs.val_ < rhs.val_;

            return !rhs.isPositive_;
        }

        /**
         * Less-than-or-equal comparison operator.
         * @param lhs left-hand-side value in logarithmic domain
         * @param rhs right-hand-side value in logarithmic domain
         * @return true if lhs greater than or equal rhs, false otherwise
         */
        friend bool operator>=(const logVal& lhs, const logVal& rhs)
        {
            if (lhs.isPositive_ && rhs.isPositive_)
                return lhs.val_ >= rhs.val_;
            if (!lhs.isPositive_ && !rhs.isPositive_)
                return lhs.val_ <= rhs.val_;

            return !rhs.isPositive_;
        }

        /**
         * Stream operator for logVal.
         * @param os stream to writes to
         * @param val value to be written
         * @return the modified stream
         */
        friend std::ostream& operator<<(std::ostream& os, const logVal& val)
        {
            os << (val.isPositive_ ? "" : "-") << exp(val.val_);
            return os;
        }


    };
}; // namespace util

#endif // NS_UTIL_LOGVALUE_H_INCLUDED

