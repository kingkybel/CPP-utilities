/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   src/floatingpoint.cc
 * Description: Re-create Solaris double_to_decimal, decimal_to_double, ...
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

#include "floatingpoint.h"

#include "stringutil.h"

#include <cmath>
#include <gmp.h>
#include <gmpxx.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <mpfr.h>
#include <sstream>
#include <string>

namespace util
{
using namespace std;
decimal_record::decimal_record(enum fp_class_type fpclass_,
                               int                sign_,
                               int                exponent_,
                               const std::string  ds_,
                               int                more_,
                               int                ndigits_)
: fpclass(fpclass_)
, sign(sign_)
, exponent(exponent_)
, more(more_)
, ndigits(ndigits_)
{
    memset(ds, 0, DECIMAL_STRING_LENGTH);
    strcpy(ds, ds_.c_str());
}

static const unsigned int defaultNumberOfBits = 128;
bool                      operator==(const decimal_record &lhs, const decimal_record &rhs)
{
    return ((lhs.exponent == rhs.exponent) && (lhs.fpclass == rhs.fpclass) && (lhs.more == rhs.more)
            && (lhs.ndigits == rhs.ndigits) && (lhs.sign == rhs.sign) && (!strcmp(lhs.ds, rhs.ds)));
}
ostream &operator<<(ostream &os, const decimal_record &dr)
{
    os << "decimal_record { ds=" << dr.ds << ", exponent=" << dr.exponent << ", fpclass="
       << (dr.fpclass == fp_zero      ? "fp_zero" :
           dr.fpclass == fp_subnormal ? "fp_subnormal" :
           dr.fpclass == fp_normal    ? "fp_normal" :
           dr.fpclass == fp_infinity  ? "fp_infinity" :
           dr.fpclass == fp_quiet     ? "fp_quiet" :
           dr.fpclass == fp_signaling ? "fp_signaling" :
                                        "<unkown>")
       << ", more=" << dr.more << ", ndigits=" << dr.ndigits << ", sign=" << dr.sign << " }";

    return (os);
}
ostream &operator<<(ostream &os, const decimal_mode &dm)
{
    os << "decimal_mode {" << (dm.df == floating_form ? "floating_form" : "fixed_form") << ", "
       << (dm.rd == fp_nearest  ? "fp_nearest" :
           dm.rd == fp_tozero   ? "fp_tozero" :
           dm.rd == fp_negative ? "fp_negative" :
                                  "fp_positive")
       << ","
       << "ndigits=" << dm.ndigits;

    return (os);
}

// constants we need for the rounding process
static const mpf_class zero(0.0, defaultNumberOfBits);
static const mpf_class one(1.0, defaultNumberOfBits);
static const mpf_class oneHalf(0.5, defaultNumberOfBits);
static const mpf_class ten(10.0, defaultNumberOfBits);

// helper operators for mpf's
bool operator<(const mpf_class &lhs, const mpf_class &rhs)
{
    return (cmp(lhs, rhs) < 0);
}
bool operator==(const mpf_class &lhs, const mpf_class &rhs)
{
    return (cmp(lhs, rhs) == 0);
}
bool operator!=(const mpf_class &lhs, const mpf_class &rhs)
{
    return (!(lhs == rhs));
}
bool operator>(const mpf_class &lhs, const mpf_class &rhs)
{
    return (cmp(lhs, rhs) > 0);
}
bool isNegative(const mpf_class &lhs)
{
    return (sgn(lhs) == -1);
}

// helper: returns a mpf 10^exponent
// accepts negative exponents as well, unlike the mpf-function "pow"
mpf_class ten_to_the_power_of(int exponent)
{
    mpf_class reval(0.0, defaultNumberOfBits);
    mpf_pow_ui(reval.get_mpf_t(), ten.get_mpf_t(), abs(exponent));

    if(exponent < 0)
        mpf_div(reval.get_mpf_t(), one.get_mpf_t(), reval.get_mpf_t());

    return (reval);
}

// strip 0's from the front and end of a string
void remove0(string &str)
{
    util::strip(str, "0");
}

/**
 * round the multi precision float 'val' using the IEEE rounding standards and precision passed in through 'pm'
 */
mpf_class round(mpf_class val, decimal_mode *pm)
{
    bool      negative = isNegative(val);
    int       exponent = 0;
    mpf_class reval    = zero;

    if(pm != nullptr && val != zero)
    {
        reval             = val;
        mpf_class abs_val = abs(val);

        if(abs_val < one)  // the value is between -1.0 and 1.0
        {
            // To find out the exponent exp in the #.######E(exp)
            // representation of the number count the zero's after the "."
            // 0.0000012340002350000 is equivalent to
            // 1.234000235e-6
            // so exponent should be -6
            stringstream ss;
            mp_exp_t     e = 0;

            ss << fixed << setprecision(40) << abs_val.get_str(e);  // @suppress("Avoid magic numbers")

            string            s            = ss.str();
            string::size_type firstNotNull = s.find_first_not_of("0.");

            exponent = (firstNotNull != string::npos) ? (1 - int(firstNotNull)) : 1;
        }
        else
        {
            mpf_class tmp = abs_val;

            while(tmp > one)
            {
                tmp /= ten;  // divide by 10.0 until we are < 1.0
                exponent++;
            }
        }

        int shiftDpl = ((pm->df == floating_form) ? pm->ndigits - exponent : pm->ndigits);

        reval *= ten_to_the_power_of(shiftDpl);

        if(pm->rd == fp_nearest)
        {
            // -10.4 --> -10
            // -12.8 --> -13
            //  10.4 -->  10
            //  12.8 -->  13
            if(negative)
            {
                // reval = trunc(reval - 0.5)
                reval -= oneHalf;
                reval = trunc(reval);
            }
            else
            {
                // reval = trunc(reval + 0.5)
                reval += oneHalf;
                reval = trunc(reval);
            }
        }
        else if(pm->rd == fp_tozero)
        {
            // -10.4 --> -10
            // -12.8 --> -12
            //  10.4 -->  10
            //  12.8 -->  12
            mpf_class toZeroCorrector = ten_to_the_power_of(exponent - 29);

            if(negative)
                toZeroCorrector *= -1.0;

            reval += toZeroCorrector;
            reval = trunc(reval);
        }
        else if(pm->rd == fp_positive)
        {
            // -10.4 --> -10
            // -12.8 --> -12
            //  10.4 -->  11
            //  12.8 -->  13
            reval = ceil(reval);
        }
        else if(pm->rd == fp_negative)
        {
            // -10.4 --> -11
            // -12.8 --> -13
            //  10.4 -->  10
            //  12.8 -->  10
            reval = floor(reval);
        }

        reval /= ten_to_the_power_of(shiftDpl);
    }

    return (reval);
}

//
// DESCRIPTION (from Sun man-pages)
//     The floating_to_decimal functions convert the floating-point
//     value  at  *px  into  a decimal record at *pd, observing the
//     modes specified in *pm and setting exceptions  in  *ps.   If
//     there are no IEEE exceptions, *ps will be zero.
//
//     If *px is zero, infinity, or NaN,  then  only  pd->sign  and
//     pd->fpclass  are  set. Otherwise pd->exponent and pd->ds are
//     also set so that
//
//     (sig)*(pd->ds)*10**(pd->exponent)
//
//     is a correctly rounded approximation to *px, where sig is +1
//     or  -1,  depending upon whether pd->sign is  0 or -1. pd->ds // <DKyb> NGBVal uses 1 as sign
//     has at least one and no  more  than  DECIMAL_STRING_LENGTH-1
//     significant  digits  because  one  character is used to ter-
//     minate the string with a null.
//
//     pd->ds is correctly rounded according to the  IEEE  rounding
//     modes  in  pm->rd.  *ps has fp_inexact set if the result was
//     inexact, and has fp_overflow set if the string  result  does
//     not    fit    in    pd->ds   because   of   the   limitation
//     DECIMAL_STRING_LENGTH.
//
//     If pm->df ==  floating_form,  then  pd->ds  always  contains
//     pm->ndigits  significant  digits.   Thus if *px == 12.34 and
//     pm->ndigits == 8, then  pd->ds  will  contain  12340000  and
//     pd->exponent will contain -6.
//
//     If pm->df == fixed_form  and  pm->ndigits  >=  0,  then  the
//     decimal  value is rounded at pm->ndigits digits to the right
//     of the decimal point. For  example,  if  *px  ==  12.34  and
//     pm->ndigits   ==   1,  then  pd->ds  will  contain  123  and
//     pd->exponent will be set to -1.
//
//     If pm->df == fixed_form and pm->ndigits< 0, then the decimal
//     value  is  rounded at -pm->ndigits digits to the left of the
//     decimal point, and pd->ds is padded with trailing  zeros  up
//     to the decimal point. For example, if *px == 12.34 and pm->n
//     digits == -1, then pd->ds will contain 10  and  pd->exponent
//     will be set to 0.
//
//     When pm->df == fixed_form and the value to be  converted  is
//     large  enough  that the  resulting string would contain more
//     than DECIMAL_STRING_LENGTH-1 digits, then the string  placed
//     in  pd->ds  is  limited  to  exactly DECIMAL_STRING_LENGTH-1
//     digits (by moving the place at which the  value  is  rounded
//     further  left  if need be), pd->exponent is adjusted accord-
//     ingly and the overflow flag is set in *ps.
//
//     pd->more is not used.
//
void quadruple_to_decimal(quadruple *px, decimal_mode *pm, decimal_record *pd, fp_exception_field_type *ps)
{
    mpf_set_default_prec(defaultNumberOfBits);
    if(pd != nullptr && px != nullptr)
    {
        *pd = decimal_record();
        if(*px == numeric_limits<quadruple>::infinity())  // @suppress("Direct float comparison")
        {
            pd->fpclass = fp_infinity;
            return;
        }
        else if(*px == -numeric_limits<quadruple>::infinity())  // @suppress("Direct float comparison")
        {
            pd->fpclass = fp_infinity;
            pd->sign    = 1;
            return;
        }
        else if(*px == numeric_limits<quadruple>::quiet_NaN())  // @suppress("Direct float comparison")
        {
            pd->fpclass = fp_quiet;
            return;
        }
        else if(*px == numeric_limits<quadruple>::signaling_NaN())  // @suppress("Direct float comparison")
        {
            pd->fpclass = fp_signaling;
            return;
        }

        stringstream ss;
        ss << fixed << setprecision(100) << *px;  // @suppress("Avoid magic numbers")

        string            valStr = ss.str();
        mpf_class         val(valStr, defaultNumberOfBits);
        string::size_type pos = valStr.find_first_of(".");

        val        = round(val, pm);
        mp_exp_t e = 0;
        ss.str("");
        ss << fixed << setprecision(100) << val.get_str(e);  // @suppress("Avoid magic numbers")
        valStr = ss.str();

        if(valStr[0] == '-')
        {
            valStr   = valStr.substr(1);
            pd->sign = 1;
        }
        else if(valStr[0] == '+')
        {
            valStr = valStr.substr(1);
        }

        if(valStr.find_last_of(".") == string::npos)
            valStr += ".";

        remove0(valStr);

        if(valStr == ".")
            valStr = "0.0";
        else if(valStr[0] == '.')
            valStr = "0" + valStr;
        else if(valStr[valStr.size() - 1] == '.')
            valStr += "0";

        // now we have a "normalized string"
        // "0.0", "1.0", "2.0", "13413.0"
        // "0.2341", "6536.2352", ...
        // but *NOT* :
        // "0", "000234", "00123.000". "54634.64563456000", ...

        if(valStr == "0.0")
        {
            pd->exponent = 0;
            pd->ndigits  = 0;
            pd->fpclass  = fp_zero;
            strcpy(pd->ds, "0");
        }
        else
        {
            // first find the significant digits
            remove0(valStr);
            pos          = valStr.find_last_of(".");
            pd->exponent = -(valStr.size() - pos - 1);
            valStr       = valStr.substr(0, pos) + (valStr[valStr.size() - 1] != '.' ? valStr.substr(pos + 1) : "");

            //  need to pad with 0-s to have at least the
            // required digits for pm->ndigits decimal-places
            // if we have more post-comma-digits than required, we
            // don't need to do anything, else we need to pad the string
            // out with zeroes (see man-page comments above)
            string::size_type len = valStr.size();
            pd->ndigits           = len;

            int padding = abs(pm->ndigits + pd->exponent);

            if(-pd->exponent < pm->ndigits)
            {
                valStr += string(padding, '0');
                pd->exponent -= padding;
                pd->ndigits = valStr.size();
            }

            if(pm->df == fixed_form && pd->ndigits > pm->ndigits && *px < 1.0L && *px > -1.0)
            {
                valStr = valStr.substr(0, pm->ndigits);
                pd->exponent += (pd->ndigits - pm->ndigits);
                pd->ndigits = pm->ndigits;
            }

            pd->fpclass = fp_normal;
            strcpy(pd->ds, valStr.c_str());
        }
    }
}

//
// DESCRIPTION (from Sun man-pages)
//     These  functions  convert  the  decimal  record  *pd  to   a
//     floating-point  value  *px  observing the rounding direction
//     specified in *pm and setting *ps to  reflect  any  floating-
//     point exceptions that occur.
//
//     When  pd->fpclass  is  fp_zero,  fp_infinity,  fp_quiet,  or
//     fp_signaling,  *px is set to zero, infinity, a quiet NaN, or
//     a signaling NaN, respectively, with the  sign  indicated  by
//     pd->sign. All other fields in *pd are ignored.
//
//     When pd->fpclass is fp_normal or fp_subnormal,  pd->ds  must
//     contain a null-terminated string of one or more ASCII digits
//     representing a non-zero integer m, and pd->ndigits  must  be
//     equal  to  the  length  of this string. Then *px is set to a
//     correctly rounded approximation to
//
//     -1**(pd->sign) * m * 10**(pd->exponent)
//
//     pd->more can be set to a non-zero  value  to  indicate  that
//     insignificant  trailing  digits were omitted from pd->ds. In
//     this case, m is replaced by m  +  delta  in  the  expression
//     above, where delta is some tiny positive fraction.
//
//     The converted value is rounded  according  to  the  rounding
//     direction  specified  in  pm->rd. pm->df and pm->ndigits are
//     not used.
//
//     On exit, *ps contains a bitwise OR of flags corresponding to
//     any floating-point exceptions that occurred. The only possi-
//     ble exceptions are underflow, overflow, and inexact.  If  no
//     floating-point exceptions occurred, *ps is set to zero.
//
void decimal_to_quadruple(quadruple *px, decimal_mode *pm, decimal_record *pd, fp_exception_field_type *ps)
{
    mpf_set_default_prec(defaultNumberOfBits);

    if(pd != nullptr && px != nullptr)
    {
        if(pd->fpclass == fp_infinity)
        {
            if(pd->sign != 0)
                *px = -numeric_limits<quadruple>::infinity();
            else
                *px = numeric_limits<quadruple>::infinity();

            return;
        }
        else if(pd->fpclass == fp_quiet)
        {
            *px = numeric_limits<quadruple>::quiet_NaN();

            return;
        }
        else if(pd->fpclass == fp_signaling)
        {
            *px = numeric_limits<quadruple>::signaling_NaN();

            return;
        }

        string    numStr(pd->ds);
        mpf_class tmp(numStr, defaultNumberOfBits);

        tmp *= ten_to_the_power_of(pd->exponent);

        // some NGBVal operations set the flag to 1 rather than -1
        // so we do a != 0 here
        if(pd->sign != 0)
            tmp *= -1.0;

        tmp = round(tmp, pm);

        stringstream ss;
        mp_exp_t     e = 0;

        ss << fixed << setprecision(100) << tmp.get_str(e);  // @suppress("Avoid magic numbers")
        ss >> *px;
    }
}

};
// namespace util
