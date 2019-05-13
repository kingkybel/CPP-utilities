/*
 * File Name:   floatingpoint.h
 * Description: Re-create Solaris double_to_decimal, decimal_to_double, ...
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
 * @date: 2011-12-13
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_FLOATINGPOINT_H_INCLUDED
#define NS_UTIL_FLOATINGPOINT_H_INCLUDED

#include <string>
#include <climits>

namespace util
{
    /**
     * Size of buffer in decimal_record.
     */
    static const int DECIMAL_STRING_LENGTH = 512;

    typedef long double quadruple;

    /**
     * Rounding direction.
     */
    enum fp_direction_type
    {
        fp_nearest = 0, ///< normal mathematical rounding to the neareast number
        fp_tozero = 1, ///< round towards the 0-point, i.e. truncate pos-comma digits
        fp_positive = 2, ///< round towards the next bigger number
        fp_negative = 3 ///< round towards the next smaller number
    };

    /**
     * Levels of precision for floating point numbers.
     */
    enum fp_precision_type
    {
        fp_extended = 0, ///< extended rounding precision
        fp_single = 1, ///< single rounding precision (float)
        fp_double = 2, ///< double rounding precision (double)
        fp_precision_3 = 3 ///< double rounding precision (quadruple)
    };

    /**
     * floating-point classes.
     */
    enum fp_class_type
    {
        fp_zero = 0,
        fp_subnormal = 1,
        fp_normal = 2,
        fp_infinity = 3,
        fp_quiet = 4,
        fp_signaling = 5
    };

    /**
     * exceptions according to bit number
     */
    enum fp_exception_type
    {
        fp_inexact = 0,
        fp_division = 1,
        fp_underflow = 2,
        fp_overflow = 3,
        fp_invalid = 4
    };

    typedef unsigned fp_exception_field_type;

    /**
     * Type of representation
     */
    enum decimal_form
    {
        /**
         * Fortran F format: ndigits specifies number
         * of digits after point; if negative,
         * specifies rounding to occur to left of
         * point.
         */
        fixed_form,

        /**
         * Fortran E format: ndigits specifies number
         *  of significant digits.
         */
        floating_form
    };

    /**
     * How are decimals represented.
     */
    struct decimal_mode
    {
        enum fp_direction_type rd; ///< Rounding direction.
        enum decimal_form df; ///< Format for conversion from binary to decimal.
        int ndigits; ///< Number of digits for conversion.
    };

    typedef char decimal_string[DECIMAL_STRING_LENGTH];

    /**
     * Structure to represent a decimal (float) number.
     */
    struct decimal_record
    {
        explicit decimal_record(enum fp_class_type fpclass_ = fp_zero,
                                int sign_ = 0,
                                int exponent_ = 0,
                                const std::string ds_ = "",
                                int more_ = 0,
                                int ndigits_ = 0);
        enum fp_class_type fpclass;
        int sign;
        int exponent;


        /**
         * Significand - each char contains an ASCII
         *  digit, except the string-terminating
         * ASCII null.
         */
        decimal_string ds;

        /**
         * On conversion from decimal to binary, != 0
         * indicates more non-zero digits following ds.
         */
        int more;

        /**
         * On fixed_form conversion from binary to decimal, contains number of
         *  digits required for ds.
         */
        int ndigits;
    };

    /**
     * Generic ostream - &lh;&lt; operator for decimal_mode.
     */
    std::ostream& operator<<(std::ostream& os, const decimal_mode& dm);

    /**
     * Generic ostream - &lh;&lt; operator for decimal_record.
     */
    std::ostream& operator<<(std::ostream& os, const decimal_record& dr);

    /**
     * Equality for two decimal_records.
     */
    bool operator==(const decimal_record& lhs, const decimal_record& rhs);

    /**
     * Convert a quadruple to a decimal record.
     */
    void quadruple_to_decimal(quadruple *px,
                              decimal_mode *pm,
                              decimal_record *pd,
                              fp_exception_field_type *ps);

    /**
     * Convert a decimal record to a quadruple.
     */
    void decimal_to_quadruple(quadruple *px,
                              decimal_mode *pm,
                              decimal_record *pd,
                              fp_exception_field_type *ps);
}; // namespace util

#endif // NS_UTIL_FLOATINGPOINT_H_INCLUDED

