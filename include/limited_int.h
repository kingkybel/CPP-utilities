/*
 * File Name:   limited_int.h
 * Description: Integers than can only assume values between a minimum and a
 *              maximum.
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

#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

//#define DO_TRACE_
#include "traceutil.h"

#include <exception>
#include <iostream>
#include <limits>
#include <sstream>
#include <type_traits>
#include <typeinfo>

namespace util
{
/**
 * Helper class that deals with the resolution of out of bounds issues by
 * applying a modulo operation on the value to move it into the valid range.
 */
struct resolve_modulo
{
    template<typename T_>
    static bool resolve(T_ min, T_ max, T_ &val, T_ invalid)
    {
        const T_ dist = max - min + 1;

        val -= min;
        val %= dist;

        if(val < 0)
            val += dist;

        val += min;

        return (true);
    }
};

/**
 * Helper class that deals with the resolution of out of bounds issues by
 * throwing an exception.
 */
struct resolve_throw
{
    template<typename T_>
    static bool resolve(T_ min, T_ max, T_ &val, T_ invalid)
    {
         std::stringstream ss;

        ss << "resolve_throw::resolve() limited_int<" << typeid(T_).name() << "," << min << "," << max << ">(" << val
           << ") out of range.";
        val = (std::numeric_limits<T_>::min() != min ? std::numeric_limits<T_>::min() : std::numeric_limits<T_>::max());

        throw std::out_of_range(ss.str());
    }
};

/**
 * Helper class that deals with the resolution of out of bounds issues by
 * setting the limited int to the special "invalid" value;.
 */
struct resolve_invalid
{
    template<typename T_>
    static bool resolve(T_ min, T_ max, T_ &val, T_ invalid)
    {
        val = invalid;

        return (false);
    }
};

/**
 * Test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers always false.
 */
template<typename, typename = void>
struct is_out_of_bounds_resolver : std::false_type
{
};

/**
 * Specialised test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers true for resolve_modulo.
 */
template<>
struct is_out_of_bounds_resolver<resolve_modulo> : std::true_type
{
};

/**
 * Specialised test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers true for resolve_invalid.
 */
template<>
struct is_out_of_bounds_resolver<resolve_invalid> : std::true_type
{
};

/**
 * Specialised test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers true for resolve_throw.
 */
template<>
struct is_out_of_bounds_resolver<resolve_throw> : std::true_type
{
};

/**
 * Helper class that deals with the conversion between different limited_int
 * specialisations by scaling the rhs interval onto the lhs interval.
 */
struct convert_scale
{
    template<typename T_, typename LimitedInt2_>
    static T_ convertFrom(T_ min_, T_ max_, const LimitedInt2_ &rhs)
    {
        long double distLhs     = (static_cast<long double>(max_) - static_cast<long double>(min_));
        long double distRhs     = (rhs.max() - rhs.min());
        long double valRhsTo0   = (static_cast<long double>(rhs.val()) - rhs.min());
        long double scaleFactor = distLhs / distRhs;
        long double valLhsTo0   = valRhsTo0 * scaleFactor;
        T_          reval(valLhsTo0 + static_cast<long double>(min_));

        return (reval);
    }
};

/**
 * Helper class that deals with the conversion between different limited_int
 * specialisations by circular scaling the rhs interval onto the lhs interval.
 */
struct convert_circular_scale
{
    template<typename T_, typename LimitedInt2_>
    static T_ convertFrom(T_ min_, T_ max_, const LimitedInt2_ &rhs)
    {
        if(((rhs.min() + rhs.max() > 1) && rhs.min() != 0) || ((min_ + max_ > 1) && min_ != 0))
        {
            std::stringstream ss;
            ss << "convert_circular_scale::convertFrom(" << min_ << "," << max_ << "," << rhs
               << "):can only user circular scale conversion on symmetric around 0 or [0, pos] limited ints";
            throw std::out_of_range(ss.str());
        }

        T_ rhsDist      = rhs.max() - rhs.min();
        T_ rhsValMapped = (rhs.min() < 0 && rhs.val() < 0) ? rhs.val() + rhsDist : rhs.val();
        T_ lhsDist      = max_ - min_;
        T_ scale        = lhsDist / rhsDist;
        T_ lhsValMapped = T_(static_cast<long double>(rhsValMapped) * static_cast<long double>(scale));

        if(min_ < 0)
            lhsValMapped = (lhsValMapped - lhsDist) % lhsDist;

        return (lhsValMapped);
    }
};

/**
 * Default test whether a type is an "limited-int-converter" for
 * static checks. This default delivers always false.
 */
template<typename, typename = void>
struct is_limited_int_converter : std::false_type
{
};

/**
 * Specialised test whether a type is an "limited-int-converter" for
 * static checks. This delivers true for convert_circular_scale.
 */
template<>
struct is_limited_int_converter<convert_circular_scale> : std::true_type
{
};

/**
 * Specialised test whether a type is an "limited-int-converter" for
 * static checks. This delivers true for convert_scale.
 */
template<>
struct is_limited_int_converter<convert_scale> : std::true_type
{
};

/**
 * Traits for limited_int's.
 *
 * @param INT_ underlying integer type/class
 * @param min_ the minimal value of the limited_int
 * @param max the maximal value of the limited_int
 * @param Res_ type that deals with out of range [min,max] resolution
 * @param Conv_ type that deals with conversion between limited_int types
 */
template<typename INT_, INT_ min_, INT_ max_, typename Res_ = resolve_modulo, typename Conv_ = convert_scale>
struct limited_int_traits
{
    using Resolver  = Res_;
    using Converter = Conv_;
    static constexpr INT_ invalid_ =
     (min_ != std::numeric_limits<INT_>::min() ? std::numeric_limits<INT_>::min() : std::numeric_limits<INT_>::max());

    constexpr static INT_ invalid()
    {
        static_assert(is_limited_int_converter<Converter>::value, "invalid limited_int_converter");
        static_assert(is_out_of_bounds_resolver<Resolver>::value, "invalid out_of_bounds_resolver");

        return (invalid_);
    }

    static bool withinBounds(const INT_ &val)
    {
        return ((val >= min_) && (val <= max_) && (min_ < max_));
    }

    static bool apply(INT_ &val)
    {
        if(!withinBounds(val))
        {
            return (Resolver::resolve(min_, max_, val, invalid()));
        }

        return (true);
    }

    template<typename LimitedInt_>
    static INT_ convertFrom(const LimitedInt_ &rhs)
    {
        return (Converter::convertFrom(min_, max_, rhs));
    }

    template<typename LimitedInt_, typename SummandType_>
    static LimitedInt_ nth_next(const LimitedInt_ &val, const SummandType_ &n, bool isReverse)
    {
        constexpr bool is_int_unsigned = std::is_unsigned<LimitedInt_>();
        constexpr bool is_summand_signed = std::is_signed<SummandType_>();
        if constexpr(is_int_unsigned && is_summand_signed)
        {  //
            n = -n;
            return isReverse ? (val.val() + n) : (val.val() - n);
        }
        return isReverse ? (val.val() - n) : (val.val() + n);
    }
};

/** Forward declared iterator type. */
template<typename LimitedIntTag>
class limited_int_iterator;

template<typename T_,
         T_ min_          = std::numeric_limits<T_>::min() + 1,
         T_ max_          = std::numeric_limits<T_>::max(),
         typename Traits_ = limited_int_traits<T_, min_, max_, resolve_modulo, convert_scale>>
struct limited_int
{
    private:
    explicit limited_int(bool dummy) : val_(Traits_::invalid_)
    {
    }

    T_ val_ = min_;

    public:
    using TraitsType = Traits_;
    using iterator   = limited_int_iterator<limited_int<T_, min_, max_, Traits_>>;

    limited_int(T_ val = min_) : val_(val)
    {
        static_assert(true == std::is_integral<T_>::value, "limited_int<> needs integral type as template parameter");
        static_assert(min_ < max_, "limited_int<> min needs to be smaller than max");
        static_assert((min_ != std::numeric_limits<T_>::min()) || (max_ != std::numeric_limits<T_>::max()),
                      "either min or max must be not equal numeric_limits min() and max()");

        Traits_::apply(val_);
    }

    template<typename T2_, T2_ min2_, T2_ max2_, typename Traits2_>
    limited_int(const limited_int<T2_, min2_, max2_, Traits2_> &rhs)
    {
        val_ = Traits_::convertFrom(rhs);
    }

    [[nodiscard]] bool isValid() const
    {
        return (val_ != Traits_::invalid());
    }

    static constexpr limited_int min()
    {
        return (min_);
    }

    static limited_int invalid()
    {
        static limited_int<T_, min_, max_, Traits_> invalidLimitedInt(false);

        return (invalidLimitedInt);
    }

    static constexpr limited_int max()
    {
        return (max_);
    }

    T_ val() const
    {
        return (val_);
    }

    operator T_() const
    {
        return (val());
    }

    static iterator begin(limited_int start = limited_int::min())
    {
        return (iterator(start, false));
    }

    static iterator end(limited_int finish = limited_int::invalid())
    {
        return (iterator(finish, false));
    }

    static iterator rbegin(limited_int rstart = limited_int::max())
    {
        return (iterator(rstart, true));
    }

    static iterator rend(limited_int rfinish = limited_int::invalid())
    {
        return (iterator(rfinish, true));
    }

    /**
     * global stream operator defined as friend within the template body.
     */
    friend std::ostream &operator<<(std::ostream &os, const limited_int &i)
    {
        if(!i.isValid())
            os << "<INV>";
        else
            os << i.val();
        os << " "
           << "[" << static_cast<T_>(i.min()) << "," << static_cast<T_>(i.max()) << "]";

        return (os);
    }
};

/**
 * Templated random access Iterator on a limited_int type.
 * @param LimitedInt the limited int to iterate over.
 */
template<typename LimitedInt>
class limited_int_iterator
{
    public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = LimitedInt;
    using difference_type   = LimitedInt;
    using pointer           = LimitedInt *;
    using reference         = LimitedInt &;

    private:
    using IntType = decltype(LimitedInt::invalid().val());

    static const LimitedInt getEnd(LimitedInt start, bool isReverse)
    {
        LimitedInt reval = LimitedInt::invalid();

        try
        {
            reval = start + (isReverse ? +1 : -1);
        }
        catch(...)
        {
            //
        }

        return (reval);
    }

    void makeStep(IntType n_step, bool isReverse)
    {
        iterEl_ = LimitedInt::TraitsType::nth_next(iterEl_, n_step, isReverse);
    }

    public:
    limited_int_iterator(LimitedInt start, bool isReverse)
    : iterEl_(start)
    , end_(getEnd(start, isReverse))
    , isReverse_(isReverse)
    {
    }

    limited_int_iterator(const limited_int_iterator<LimitedInt> &) = default;

    limited_int_iterator &operator++()
    {
        makeStep(1, isReverse_);
        return (*this);
    }

    limited_int_iterator operator++(int)
    {
        limited_int_iterator<LimitedInt> tmp(*this);
                                         operator++();

        return (tmp);
    }

    limited_int_iterator &operator+=(IntType step)
    {
        makeStep(step, isReverse_);

        return (*this);
    }

    limited_int_iterator &operator--()
    {
        makeStep(-1, isReverse_);

        return (*this);
    }

    limited_int_iterator operator--(int)
    {
        limited_int_iterator<LimitedInt> tmp(*this);
                                         operator--();

        return (tmp);
    }

    limited_int_iterator &operator-=(IntType step)
    {
        makeStep(-step, isReverse_);

        return (*this);
    }

    friend inline bool operator==(const limited_int_iterator<LimitedInt> &lhs,
                                  const limited_int_iterator<LimitedInt> &rhs)
    {
        return (lhs.iterEl_ == rhs.iterEl_);
    }

    friend inline bool operator!=(const limited_int_iterator<LimitedInt> &lhs,
                                  const limited_int_iterator<LimitedInt> &rhs)
    {
        return (!(lhs.iterEl_ == rhs.iterEl_));
    }

    LimitedInt &operator*()
    {
        return (iterEl_);
    }

    LimitedInt *operator->()
    {
        return (&iterEl_);
    }

    friend std::ostream &operator<<(std::ostream &os, const limited_int_iterator<LimitedInt> &i)
    {
        os << "&*" << i.iterEl_;

        return (os);
    }

    private:
    LimitedInt       iterEl_;
    const LimitedInt end_;
    const bool       isReverse_;
};
};
// namespace util

#endif  // LIMITED_INT_H_INCLUDED
