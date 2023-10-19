/*
 * File Name:   logvalue.h
 * Description: double values transformed into logarithmic domain
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

#ifndef NS_UTIL_LOGVALUE_H_INCLUDED
#define NS_UTIL_LOGVALUE_H_INCLUDED

#include <cmath>
#include <exception>
#include <ostream>

namespace util
{
class logVal;
long double abs(const logVal &val);

/**
 * @brief Class to store floating point values in the log domain.
 */
class logVal
{
    private:
    long double val_;
    bool        isPositive_;

    /**
     * @brief Private construction of a new log Val object
     *
     * @param logDomainVal value in the log-domain
     * @param isPositive flag to indicate whether the value is positive
     */
    logVal(long double logDomainVal, bool isPositive)
    : val_(logDomainVal)
    , isPositive_(logDomainVal == 0.0L ? true : isPositive)  // @suppress("Direct float comparison")
    {
    }

    public:
    /**
     * @brief Check whether the value is zero.
     *
     * @return true, if so, false  otherwise
     */
    bool isZero() const
    {
        return (val_ == -INFINITY);  // @suppress("Direct float comparison")
    }

    static logVal fromReal(long double realDomainVal)
    {
        return (realDomainVal >= 0.0L ? logVal(log(realDomainVal), true) : logVal(log(-realDomainVal), false));
    }

    /**
     * @brief Convert a value from the log-domain to the real-domain.
     *
     * @param logDomainVal value in the log-domain
     * @param isPositive flag to indicat positive (true) or negative (false)
     * @return logVal the value translated
     */
    static logVal fromLog(long double logDomainVal, bool isPositive = true)
    {
        return (logVal(logDomainVal, isPositive));
    }

    /**
     * @brief Default-construct a new log Val object
     *
     * @param realDomainVal value in the real domain
     */
    logVal(long double realDomainVal = 0.0L)
    : val_(log(realDomainVal >= 0.0L ? realDomainVal : -realDomainVal))
    , isPositive_(realDomainVal >= 0.0L)
    {
    }
    logVal(const logVal &rhs)            = default;
    logVal &operator=(const logVal &rhs) = default;
    ~logVal()                            = default;

    /**
     * @brief Conversion to a long double.
     *
     * @return long double converted value
     */
    long double toReal() const
    {
        return (isPositive_ ? exp(val_) : -exp(val_));
    }

    /**
     * @brief Conversion to a long double.
     *
     * @return long double converted value
     */
    explicit operator long double()
    {
        return (toReal());
    }

    /**
     * @brief Takes two values in the logarithmic domain and adds them together.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     *
     * @return sum of two values converted back into the logarithmic domain
     */
    friend logVal operator+(const logVal &lhs, const logVal &rhs)
    {
        long double lhsDbl = lhs.isPositive_ ? exp(lhs.val_) : -exp(lhs.val_);
        long double rhsDbl = rhs.isPositive_ ? exp(rhs.val_) : -exp(rhs.val_);

        return (logVal(lhsDbl + rhsDbl));
    }

    /**
     * @brief Takes two values in the logarithmic domain and adds them together.
     *
     * @param rhs right-hand-side value in logarithmic domain
     *
     * @return sum of two values converted back into the logarithmic domain
     */
    logVal &operator+=(const logVal &rhs)
    {
        logVal reval = *this + rhs;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Increment the value val++.
     *
     * @param dummy dummy-value
     *
     * @return incremented value
     */
    logVal &operator++()
    {
        logVal reval = *this + 1.0L;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Pre-increment the value ++val.
     *
     * @return incremented value
     */
    logVal &operator++(int dummy)
    {
        logVal reval = *this + 1.0L;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Unary minus operator.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @return negative value of lhs converted back into the logarithmic domain
     */
    friend logVal operator-(const logVal &lhs)
    {
        return (fromLog(lhs.val_, !lhs.isPositive_));
    }

    /**
     * @brief Takes two values in the logarithmic domain and subtracts the second
     * from the first.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     *
     * @return sum of two values converted back into the logarithmic domain
     */
    friend logVal operator-(const logVal &lhs, const logVal &rhs)
    {
        long double lhsDbl = lhs.isPositive_ ? exp(lhs.val_) : -exp(lhs.val_);
        long double rhsDbl = rhs.isPositive_ ? exp(rhs.val_) : -exp(rhs.val_);

        return (logVal(lhsDbl - rhsDbl));
    }

    /**
     * @brief Takes two values in the logarithmic domain and subtracts the second
     * from the first.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     *
     * @return sum of two values converted back into the logarithmic domain
     */
    logVal &operator-=(const logVal &rhs)
    {
        logVal reval = *this - rhs;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Decrement the value val--.
     *
     * @param dummy dummy-value
     * @return decremented value
     */
    logVal &operator--()
    {
        logVal reval = *this - 1.0L;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Pre-decrement the value --val.
     *
     * @return decremented value
     */
    logVal &operator--(int dummy)
    {
        logVal reval = *this - 1.0L;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Takes two values in the logarithmic domain and multiplies them.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return product of two values converted back into the logarithmic domain
     */
    friend logVal operator*(const logVal &lhs, const logVal &rhs)
    {
        return (fromLog(lhs.isZero() || rhs.isZero() ? -INFINITY : (lhs.val_ + rhs.val_),
                        (lhs.isPositive_ == rhs.isPositive_)));
    }

    /**
     * @brief Takes two values in the logarithmic domain and multiplies them.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return product of two values converted back into the logarithmic domain
     */
    logVal &operator*=(const logVal &rhs)
    {
        logVal reval = *this * rhs;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Takes two values in the logarithmic domain and divides them.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return quotient of two values converted back into the logarithmic domain
     */
    friend logVal operator/(const logVal &lhs, const logVal &rhs)
    {
        if(rhs.isZero())
        {
            throw std::runtime_error("Attempted division by zero.");
        }

        return (fromLog(lhs.isZero() ? -INFINITY : (lhs.val_ - rhs.val_), (lhs.isPositive_ == rhs.isPositive_)));
    }

    /**
     * @brief Takes two values in the logarithmic domain and divides the first by the second.
     *
     * @param rhs right-hand-side value in logarithmic domain
     * @return quotient of two values converted back into the logarithmic domain
     */
    logVal &operator/=(const logVal &rhs)
    {
        logVal reval = *this / rhs;
        val_         = reval.val_;
        isPositive_  = reval.isPositive_;

        return (*this);
    }

    /**
     * @brief Equality comparison operator.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return true if lhs equals rhs, false otherwise
     */
    friend bool operator==(const logVal &lhs, const logVal &rhs)
    {
        bool reval =
         (lhs.isZero() && rhs.isZero())
         || (lhs.isPositive_ == rhs.isPositive_ && lhs.val_ == rhs.val_);  // @suppress("Direct float comparison")

        return (reval);
    }

    /**
     * @brief Inequality comparison operator.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return true if lhs not equal rhs, false otherwise
     */
    friend bool operator!=(const logVal &lhs, const logVal &rhs)
    {
        return (!(lhs == rhs));
    }

    /**
     * @brief Less-than comparison operator.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return true if lhs less than rhs, false otherwise
     */
    friend bool operator<(const logVal &lhs, const logVal &rhs)
    {
        if(lhs.isPositive_ && rhs.isPositive_)
            return (lhs.val_ < rhs.val_);

        if(!lhs.isPositive_ && !rhs.isPositive_)
            return (lhs.val_ > rhs.val_);

        return (rhs.isPositive_);
    }

    /**
     * @brief Less-than-or-equal comparison operator.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return true if lhs less than or equal rhs, false otherwise
     */
    friend bool operator<=(const logVal &lhs, const logVal &rhs)
    {
        if(lhs.isPositive_ && rhs.isPositive_)
            return (lhs.val_ <= rhs.val_);

        if(!lhs.isPositive_ && !rhs.isPositive_)
            return (lhs.val_ >= rhs.val_);

        return (rhs.isPositive_);
    }

    /**
     * @brief Greater-than comparison operator.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     *
     * @return true if lhs greater than rhs, false otherwise
     */
    friend bool operator>(const logVal &lhs, const logVal &rhs)
    {
        if(lhs.isPositive_ && rhs.isPositive_)
            return (lhs.val_ > rhs.val_);

        if(!lhs.isPositive_ && !rhs.isPositive_)
            return (lhs.val_ < rhs.val_);

        return (!rhs.isPositive_);
    }

    /**
     * @brief Less-than-or-equal comparison operator.
     *
     * @param lhs left-hand-side value in logarithmic domain
     * @param rhs right-hand-side value in logarithmic domain
     * @return true if lhs greater than or equal rhs, false otherwise
     */
    friend bool operator>=(const logVal &lhs, const logVal &rhs)
    {
        if(lhs.isPositive_ && rhs.isPositive_)
            return (lhs.val_ >= rhs.val_);

        if(!lhs.isPositive_ && !rhs.isPositive_)
            return (lhs.val_ <= rhs.val_);

        return (!rhs.isPositive_);
    }

    /**
     * @brief Stream operator for logVal.
     *
     * @param os stream to writes to
     * @param val value to be written
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const logVal &val)
    {
        os << "lv(" << val.toReal() << ") " << (val.isPositive_ ? "+" : "-") << val.val_;
        return (os);
    }
};

/**
 * @brief Absolute value of a logVal.
 *
 * @param val the value
 * @return long double absolute value
 */
inline long double abs(const logVal &val)
{
    return (std::abs(val.toReal()));
}
};  // namespace util

#endif  // NS_UTIL_LOGVALUE_H_INCLUDED
