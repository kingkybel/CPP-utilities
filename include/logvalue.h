/*
 * File:   logvalue.h
 * Author: Dieter J Kybelksties
 *
 * Created on April 30, 2019, 9:22 AM
 */

#ifndef LOGVALUE_H_INCLUDED
#define	LOGVALUE_H_INCLUDED

#include <cmath>
#include <ostream>
namespace util
{

    class logVal
    {
    private:
        long double val;
    public:

        logVal(long double logDomainVal)
        : val(logDomainVal)
        {

        }

        static logVal fromReal(long double realDomainVal)
        {
            return log(realDomainVal);
        }

        const long double toReal() const
        {
            return exp(val);
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
            return (lhs.val == -INFINITY) || (rhs.val == -INFINITY) ? -INFINITY :
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
            os << val.val;
            return os;
        }


    };
};
#endif	// LOGVALUE_H_INCLUDED

