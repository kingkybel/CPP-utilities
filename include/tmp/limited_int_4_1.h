/*
 * File Name:   limited_int.h
 * Description: Integers than can only assume values between a minimum and a
 *              maximum.
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

#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

#include <iostream>
#include <limits>
#include <exception>
#include <sstream>

struct PolicySetInvalid
{

    template<typename T_>
    static bool apply(T_ const & min, T_ const & max, T_ & val)
    {
        return ((val >= min) && (val <= max));
    }
};

struct PolicySetModulo
{

    template<typename T_>
    static bool apply(T_ const & min, T_ const & max, T_ & val)
    {
        if ((val > max) || (val < min))
        {
            T_ const dist = max - min + 1;
            val -= min;
            val %= dist;
            if (val < 0)
                val += dist;
            val += min;
        }
        return true;
    }
};

template < typename T_,
T_ min_ = std::numeric_limits<T_>::min() + 1,
T_ max_ = std::numeric_limits<T_>::max(),
typename Policy_ = PolicySetModulo
>
struct limited_int
{
private:
    T_ val_ = min_;
    constexpr static T_ invalid_ = (min_ != std::numeric_limits<T_>::min() ?
                                    std::numeric_limits<T_>::min() :
                                    std::numeric_limits<T_>::max());

public:

    limited_int(T_ val = min_)
    {
        static_assert(true == std::is_integral<T_>::value,
                      "limited_int<> needs integral type as template parameter");
        static_assert(min_ < max_,
                      "limited_int<> min needs to be smaller than max");
        static_assert(std::numeric_limits<T_>::min() != min_
                      || std::numeric_limits<T_>::max() != max_,
                      "limited_int<> cannot extend from numeric limit min() to max().");

        if (false == Policy_::apply(min_, max_, val))
        {
            val = invalid_;
        }
        val_ = val;
    }

    static constexpr limited_int min()
    {
        return min_;
    }

    static constexpr limited_int max()
    {
        return max_;
    }

    static constexpr limited_int invalid()
    {
        return invalid_;
    }

    limited_int isInvalid() const
    {
        return val_ == invalid_;
    }

    T_ val() const
    {
        return val_;
    }

    operator T_() const
    {
        return val();
    }

    friend std::ostream & operator<<(std::ostream & os,
                                     limited_int<T_, min_, max_, Policy_> const & i)
    {
        if (i.isInvalid())
            os << "<limited_int::invalid>";
        else
            os << i.val();
        os << " "
                << "["
                << (T_) i.min()
                << ","
                << (T_) i.max()
                << "]";
        return os;
    }

};

using namespace std;

typedef limited_int<long, -10, 15 > Neg10Pos15;
typedef limited_int<long, -10, 15, PolicySetInvalid> Neg10Pos15Inv;

void execute()
{
    Neg10Pos15 neg10Pos15;
    cout << "initializedValue=" << neg10Pos15 << endl;

    for (long x = -20; x < 20; x++)
    {
        neg10Pos15 = x;
        long y = neg10Pos15;
        cout << x << ":\tlimited_int=" << neg10Pos15 << "\tlong=" << y << endl;
    }
#if SHOW_COMPILE_ERROR == 1
    limited_int<double> dbl;
#endif
#if SHOW_COMPILE_ERROR == 2
    limited_int<long, 10, 2> ll;
#endif
#if SHOW_COMPILE_ERROR == 3
    limited_int<char, std::numeric_limits<char>::min(), std::numeric_limits<char>::max() > cc;
#endif

    Neg10Pos15Inv neg10Pos15Inv = 20;
    cout << "initializedValue=" << neg10Pos15Inv << endl;

}


#endif // LIMITED_INT_H_INCLUDED

