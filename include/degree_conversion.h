/*
 * File Name:   degree_conversion.h
 * Description: Integer values that can only assume angle values, [0,360],
 *              [-180, 180] degrees, or [0, 2Pi] micro-radians.
 * Copyright (C) 2020 Dieter J Kybelksties
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
 * @date: 2020-06-02
 * @author: Dieter J Kybelksties
 */

#ifndef DEGREE_CONVERSION_H_INCLUDED
#define DEGREE_CONVERSION_H_INCLUDED

#include <iostream>
#include <limits>
#include <exception>
#include <sstream>
#include "constants.h"

namespace util
{
    template<typename T_> class TDeg360;
    template<typename T_> class TDeg180;
    template<typename T_> class TRad2Pi;

    template<typename T_>
    class TDeg180 : public limited_int< T_,
                                        static_cast<T_>(-179),
                                        static_cast<T_>(180),
                                        PolicySetModulo>
    {
        typedef limited_int<T_,
                            static_cast<T_>(-179),
                            static_cast<T_>(180),
                            PolicySetModulo>        Base;
    public:
        TDeg180(T_ val = 0)
        : Base(val)
        {
            static_assert(std::is_integral<T_>::value,
                    "Template TDeg180<> requires integral template parameter");
            static_assert(T_(-180) < T_(0),
                    "Cannot instantiate TDeg180 with unsigned type");
            static_assert(sizeof(T_) > 1,
                    "Cannot instantiate TDeg180 with underlying type too small");
        }
        TDeg180(TDeg360<T_> const & val360);
        TDeg180(TRad2Pi<T_> const & val2Pi);
    };

    template<typename T_>
    class TDeg360 : public limited_int<T_, 0, 359, PolicySetModulo>
    {
        typedef limited_int<T_,
                            static_cast<T_>(0),
                            static_cast<T_>(359),
                            PolicySetModulo>        Base;
    public:
        TDeg360(T_ val = 0)
        : Base(val)
        {
            static_assert(std::is_integral<T_>::value,
                    "Template TDeg360<> requires integral template parameter");
            static_assert(sizeof(T_) > 1,
                    "Cannot instantiate TDeg360 with underlying type too small");
        }

        TDeg360(TDeg180<T_> const & val180)
        : TDeg360((val180.val()+360)%360)
        {
        }
        TDeg360(TRad2Pi<T_> const & val2Pi);
    };

    template<typename T_>
    class TRad2Pi : public limited_int<T_, 0, MICRO_RAD_2PI, PolicySetModulo>
    {
        typedef limited_int<T_,
                            static_cast<T_>(0),
                            static_cast<T_>(MICRO_RAD_2PI),
                            PolicySetModulo>                Base;
    public:
        TRad2Pi(T_ val = 0)
        : Base(val)
        {
            static_assert(std::is_integral<T_>::value,
                    "Template TRad2Pi<> requires integral template parameter");
            static_assert(sizeof(T_) > 2,
                    "Cannot instantiate TRad2Pi with underlying type too small");
        }

        TRad2Pi(TDeg180<T_> const & val180)
        : TRad2Pi((TDeg360<T_>(val180).val()*int64_t(MICRO_RAD_2PI))/360LL)
        {
        }

        TRad2Pi(TDeg360<T_> const & val360)
        : TRad2Pi(int64_t(val360.val())*int64_t(MICRO_RAD_2PI)/360LL)
        {
        }
    };

    template<typename T_>
    TDeg180<T_>::TDeg180(TDeg360<T_> const & val360)
    : TDeg180((val360.val()-360)%180)
    {
    }

    template<typename T_>
    TDeg180<T_>::TDeg180(TRad2Pi<T_> const & val2Pi)
    : TDeg180(TDeg360<T_>(val2Pi))
    {
    }

    template<typename T_>
    TDeg360<T_>::TDeg360(TRad2Pi<T_> const & val2Pi)
    : TDeg360<T_>(T_((long double)val2Pi.val()/(long double)MICRO_RAD_2PI*360.0L))
    {
    }

    typedef TDeg360<int64_t> Deg360;
    typedef TDeg180<int64_t> Deg180;
    typedef TRad2Pi<int64_t> Rad2Pi;

}; // namespace util
#endif // DEGREE_CONVERSION_H_INCLUDED

