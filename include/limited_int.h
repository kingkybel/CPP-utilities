/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/limited_int.h
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

// #define DO_TRACE_
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
 * @brief Helper class that deals with the resolution of out of bounds issues by
 * applying a modulo operation on the value to move it into the valid range.
 */
struct resolve_modulo
{
    /**
     * @brief Resolve the out of bounds issue by mapping the given value "val" to the valid interval by using modulo
     * operator.
     *
     * @tparam T_ integer type
     * @param min minimum value
     * @param max maximum value
     * @param val the value that is out of bounds
     * @param invalid the value to use as "invalid"
     * @return true always, as resolution will always work
     */
    template <typename T_>
    static bool resolve(T_ min, T_ max, T_ &val, T_ invalid)
    {
        const T_ dist = max - min + 1;

        val -= min;
        val %= dist;

        if (val < 0)
        {
            val += dist;
        }

        val += min;

        return true;
    }
};

/**
 * @brief Helper class that deals with the resolution of out of bounds issues by
 * throwing an exception.
 */
struct resolve_throw
{
    /**
     * @brief Resolve the out of bounds issue by throwing an out-of-range exception.
     *
     * @tparam T_ integer type
     * @param min minimum value
     * @param max maximum value
     * @param val the value that is out of bounds
     * @param invalid the value to use as "invalid" - not used
     * @return never returns a value, always throws an exception
     */
    template <typename T_>
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
 * @brief Helper class that deals with the resolution of out of bounds issues by
 * setting the limited int to the special "invalid" value.
 */
struct resolve_invalid
{
    /**
     * @brief Resolve the out of bounds issue by setting the value to a special invalid value.
     *
     * @tparam T_ integer type
     * @param min minimum value - unused
     * @param max maximum value - unused
     * @param val the value that is out of bounds
     * @param invalid the value to use as "invalid"
     * @return always false
     */
    template <typename T_>
    static bool resolve(T_ min, T_ max, T_ &val, T_ invalid)
    {
        val = invalid;

        return false;
    }
};

/**
 * @brief Test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers always false.
 */
template <typename, typename = void>
struct is_out_of_bounds_resolver : std::false_type
{
};

/**
 * @brief Specialised test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers true for resolve_modulo.
 */
template <>
struct is_out_of_bounds_resolver<resolve_modulo> : std::true_type
{
};

/**
 * @brief Specialised test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers true for resolve_invalid.
 */
template <>
struct is_out_of_bounds_resolver<resolve_invalid> : std::true_type
{
};

/**
 * @brief Specialised test whether a type is an "out-of-bounds-resolver" for
 * static checks. This default delivers true for resolve_throw.
 */
template <>
struct is_out_of_bounds_resolver<resolve_throw> : std::true_type
{
};

/**
 * @brief Helper class that deals with the conversion between different limited_int
 * specialisations by scaling the rhs interval onto the lhs interval.
 */
struct convert_scale
{
    /**
     * @brief Convert a limited_int value to a new limited int range.
     *
     * @tparam T_ integer typr
     * @tparam LimitedInt2_ limited _int type to convert from
     * @param min_ new minimum
     * @param max_ new maximum
     * @param rhs limited_int value to convert
     * @return ValueT_ the converted value
     */
    template <typename T_, typename LimitedInt2_>
    static T_ convertFrom(T_ min_, T_ max_, LimitedInt2_ const &rhs)
    {
        long double distLhs     = (static_cast<long double>(max_) - static_cast<long double>(min_));
        long double distRhs     = (rhs.max() - rhs.min());
        long double valRhsTo0   = (static_cast<long double>(rhs.val()) - rhs.min());
        long double scaleFactor = distLhs / distRhs;
        long double valLhsTo0   = valRhsTo0 * scaleFactor;
        T_          reval(valLhsTo0 + static_cast<long double>(min_));

        return reval;
    }
};

/**
 * @brief Helper class that deals with the conversion between different limited_int
 * specialisations by circular scaling the rhs interval onto the lhs interval.
 */
struct convert_circular_scale
{
    /**
     * @brief Convert a limited_int value to a new limited int range if possible, otherwise throw an out-of-range
     * exception.
     *
     * @tparam T_ integer type
     * @tparam LimitedInt2_ limited _int type to convert from
     * @param min_ new minimum
     * @param max_ new maximum
     * @param rhs limited_int value to convert
     * @return ValueT_ the converted value
     */
    template <typename T_, typename LimitedInt2_>
    static T_ convertFrom(T_ min_, T_ max_, LimitedInt2_ const &rhs)
    {
        if (((rhs.min() + rhs.max() > 1) && rhs.min() != 0) || ((min_ + max_ > 1) && min_ != 0))
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

        if (min_ < 0)
        {
            lhsValMapped = (lhsValMapped - lhsDist) % lhsDist;
        }

        return lhsValMapped;
    }
};

/**
 * @brief Default test whether a type is an "limited-int-converter" for
 * static checks. This default delivers always false.
 */
template <typename, typename = void>
struct is_limited_int_converter : std::false_type
{
};

/**
 * @brief Specialised test whether a type is an "limited-int-converter" for
 * static checks. This delivers true for convert_circular_scale.
 */
template <>
struct is_limited_int_converter<convert_circular_scale> : std::true_type
{
};

/**
 * @brief Specialised test whether a type is an "limited-int-converter" for
 * static checks. This delivers true for convert_scale.
 */
template <>
struct is_limited_int_converter<convert_scale> : std::true_type
{
};

/**
 * @brief Traits for limited_int's.
 *
 * @param INT_ underlying integer type/class
 * @param min_ the minimal value of the limited_int
 * @param max the maximal value of the limited_int
 * @param Res_ type that deals with out of range [min,max] resolution
 * @param Conv_ type that deals with conversion between limited_int types
 */
template <typename INT_, INT_ min_, INT_ max_, typename Res_ = resolve_modulo, typename Conv_ = convert_scale>
struct limited_int_traits
{
    using Resolver  = Res_;
    using Converter = Conv_;
    static constexpr INT_ invalid_ =
        (min_ != std::numeric_limits<INT_>::min() ? std::numeric_limits<INT_>::min() : std::numeric_limits<INT_>::max()
        );

    /**
     * @brief Return the unique invalid value for the limited_int.
     *
     * @return constexpr INT_
     */
    constexpr static INT_ invalid()
    {
        static_assert(is_limited_int_converter<Converter>::value, "invalid limited_int_converter");
        static_assert(is_out_of_bounds_resolver<Resolver>::value, "invalid out_of_bounds_resolver");

        return invalid_;
    }

    /**
     * @brief Check wheter  value is within the boundaries of the limited_int.
     *
     * @param val value to check
     * @return true, if value is within the bounds, false otherwise
     */
    static bool withinBounds(const INT_ &val)
    {
        return (val >= min_) && (val <= max_) && (min_ < max_);
    }

    /**
     * @brief Set the value after using the out-of-bounds - resolver first.
     *
     * @param val the value to apply
     * @return true always, errors will be handled by the applicable resolver.
     */
    static bool apply(INT_ &val)
    {
        if (!withinBounds(val))
        {
            return Resolver::resolve(min_, max_, val, invalid());
        }

        return true;
    }

    /**
     * @brief Convert a limited_int of different type to this type/
     *
     * @tparam LimitedInt_ limited_int type
     * @param rhs the other limited int
     * @return INT_ the value in this limited_int type
     */
    template <typename LimitedInt_>
    static INT_ convertFrom(LimitedInt_ const &rhs)
    {
        return Converter::convertFrom(min_, max_, rhs);
    }

    /**
     * @brief Get the value of the given value incremented/decremented by n.
     *
     * @tparam LimitedInt_ limited_int type
     * @tparam SummandType_ inter type of the summand
     * @param val the value
     * @param n distance of next value
     * @param isReverse whether we are looking in negative direction (true) or not (false)
     * @return LimitedInt_ the nth next value
     */
    template <typename LimitedInt_, typename SummandType_>
    static LimitedInt_ nth_next(LimitedInt_ const &val, SummandType_ const &n, bool isReverse)
    {
        constexpr bool is_int_unsigned   = std::is_unsigned<LimitedInt_>();
        constexpr bool is_summand_signed = std::is_signed<SummandType_>();
        if constexpr (is_int_unsigned && is_summand_signed)
        { //
            n = -n;
            return isReverse ? (val.val() + n) : (val.val() - n);
        }
        return isReverse ? (val.val() - n) : (val.val() + n);
    }
};

/* Forward declared iterator type. */
template <typename LimitedIntTag>
class limited_int_iterator;

/**
 * @brief A class that is essentially an integer
 *
 * @tparam IntT_ integer type
 * @tparam min_ minimum value for this type, default to the standard minimum of the int type + 1, as we need a distinct
 * "invalid" value
 * @tparam max_ maximum value for this type, default to the standard maximum of the int type
 * @tparam Traits_ trais of the limited_int incluing the resolver and converter
 */
template <
    typename IntT_,
    IntT_ min_       = std::numeric_limits<IntT_>::min() + 1,
    IntT_ max_       = std::numeric_limits<IntT_>::max(),
    typename Traits_ = limited_int_traits<IntT_, min_, max_, resolve_modulo, convert_scale>>
struct limited_int
{
  private:
    explicit limited_int(bool dummy)
        : val_(Traits_::invalid_)
    {
    }

    IntT_ val_ = min_;

  public:
    using TraitsType = Traits_;
    using iterator   = limited_int_iterator<limited_int<IntT_, min_, max_, Traits_>>;

    /**
     * @brief Default construct a new limited int object.
     *
     * @param val the value of this
     */
    limited_int(IntT_ val = min_)
        : val_(val)
    {
        static_assert(
            true == std::is_integral<IntT_>::value,
            "limited_int<> needs integral type as template parameter"
        );
        static_assert(min_ < max_, "limited_int<> min needs to be smaller than max");
        static_assert(
            (min_ != std::numeric_limits<IntT_>::min()) || (max_ != std::numeric_limits<IntT_>::max()),
            "either min or max must be not equal numeric_limits min() and max()"
        );

        Traits_::apply(val_);
    }

    /**
     * @brief Construct a new limited int object from a different limited int
     *
     * @tparam IntT2_ the integer type of the rhs limited_int
     * @tparam min2_ the minimum of the rhs limited_int
     * @tparam max2_ the maximum of the rhs limited_int
     * @tparam Traits2_ the traits of the rhs limited_int
     * @param rhs the value to convert from
     */
    template <typename IntT2_, IntT2_ min2_, IntT2_ max2_, typename Traits2_>
    limited_int(limited_int<IntT2_, min2_, max2_, Traits2_> const &rhs)
    {
        val_ = Traits_::convertFrom(rhs);
    }

    /**
     * @brief Check, whether this limited_int is valid.
     *
     * @return true, if so, false otherwise
     */
    [[nodiscard]] bool isValid() const
    {
        return val_ != Traits_::invalid();
    }

    /**
     * @brief Get the minimum value this limited_int can assume
     *
     * @return constexpr limited_int the minimum
     */
    static constexpr limited_int min()
    {
        return min_;
    }

    /**
     * @brief Get the maximum value this limited_int can assume
     *
     * @return constexpr limited_int the maximum
     */
    static constexpr limited_int max()
    {
        return max_;
    }

    /**
     * @brief Get the invalid value of this limited_int.
     *
     * @return limited_int the distinct invalid value
     */
    static limited_int invalid()
    {
        static limited_int<IntT_, min_, max_, Traits_> invalidLimitedInt(false);

        return invalidLimitedInt;
    }

    /**
     * @brief Get the value.
     *
     * @return IntT_ this value
     */
    IntT_ val() const
    {
        return val_;
    }

    /**
     * @brief Get the value (via conversion operator).
     *
     * @return IntT_ this value
     */
    operator IntT_() const
    {
        return val();
    }

    /**
     * @brief Get a begin value iterator.
     *
     * @param start where to start iterating
     * @return iterator an iterator pointing to the given start
     */
    static iterator begin(limited_int start = limited_int::min())
    {
        return iterator(start, false);
    }

    /**
     * @brief Get an end value iterator.
     *
     * @param finish where to finish iterating
     * @return iterator an iterator pointing to the given finish
     */
    static iterator end(limited_int finish = limited_int::invalid())
    {
        return iterator(finish, false);
    }

    /**
     * @brief Get a reverse begin value iterator.
     *
     * @param rstart where to start iterating
     * @return iterator an iterator pointing to the given rstart
     */
    static iterator rbegin(limited_int rstart = limited_int::max())
    {
        return iterator(rstart, true);
    }

    /**
     * @brief Get a reverse end value iterator.
     *
     * @param rfinish where to finish iterating
     * @return iterator an iterator pointing to the given rfinish
     */
    static iterator rend(limited_int rfinish = limited_int::invalid())
    {
        return iterator(rfinish, true);
    }

    /**
     * @brief Output stream operator.
     *
     * @param os the output stream to modify
     * @param i the limited_int to stream
     * @return std::ostream& the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, limited_int const &i)
    {
        if (!i.isValid())
        {
            os << "<INV>";
        }
        else
        {
            os << i.val();
        }
        os << " "
           << "[" << static_cast<IntT_>(i.min()) << "," << static_cast<IntT_>(i.max()) << "]";

        return os;
    }
};

/**
 * @brief Templated random access Iterator on a limited_int type.
 * @param LimitedInt the limited int to iterate over.
 */
template <typename LimitedInt>
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

    static LimitedInt const getEnd(LimitedInt start, bool isReverse)
    {
        LimitedInt reval = LimitedInt::invalid();

        try
        {
            reval = start + (isReverse ? +1 : -1);
        }
        catch (...)
        {
            //
        }

        return reval;
    }

    void makeStep(IntType n_step, bool isReverse)
    {
        iterEl_ = LimitedInt::TraitsType::nth_next(iterEl_, n_step, isReverse);
    }

  public:
    /**
     * @brief Construct a new limited_int_iterator object.
     *
     * @param start the start-value
     * @param isReverse whether to iterate in positve (false) or negative (true) direction.
     */
    limited_int_iterator(LimitedInt start, bool isReverse)
        : iterEl_(start)
        , end_(getEnd(start, isReverse))
        , isReverse_(isReverse)
    {
    }

    limited_int_iterator(limited_int_iterator<LimitedInt> const &) = default;

    /**
     * @brief Postfix increment.
     *
     * @return limited_int_iterator& incremented iterator
     */
    limited_int_iterator &operator++()
    {
        makeStep(1, isReverse_);
        return *this;
    }

    /**
     * @brief Prefix increment.
     *
     * @return limited_int_iterator& incremented iterator
     */
    limited_int_iterator operator++(int)
    {
        limited_int_iterator<LimitedInt> tmp(*this);
        operator++();

        return tmp;
    }

    /**
     * @brief increment step times.
     *
     * @param step number of increments
     * @return limited_int_iterator&  incremented iterator
     */
    limited_int_iterator &operator+=(IntType step)
    {
        makeStep(step, isReverse_);

        return *this;
    }

    /**
     * @brief Postfix decrement.
     *
     * @return limited_int_iterator& decremented iterator
     */
    limited_int_iterator &operator--()
    {
        makeStep(-1, isReverse_);

        return *this;
    }

    /**
     * @brief Prefix decrement.
     *
     * @return limited_int_iterator& decremented iterator
     */
    limited_int_iterator operator--(int)
    {
        limited_int_iterator<LimitedInt> tmp(*this);
        operator--();

        return tmp;
    }

    /**
     * @brief decrement step times.
     *
     * @param step number of decrements
     * @return limited_int_iterator& decremented iterator
     */
    limited_int_iterator &operator-=(IntType step)
    {
        makeStep(-step, isReverse_);

        return *this;
    }

    /**
     * @brief Equality operator for limited_int_iterator.
     *
     * @param lhs left-hand-side iterator
     * @param rhs right-hand-side iterator
     * @return true, if lhs is equal to rhs, false otherwise
     */
    friend inline bool
        operator==(limited_int_iterator<LimitedInt> const &lhs, limited_int_iterator<LimitedInt> const &rhs)
    {
        return lhs.iterEl_ == rhs.iterEl_;
    }

    /**
     * @brief Inequality operator for limited_int_iterator.
     *
     * @param lhs left-hand-side iterator
     * @param rhs right-hand-side iterator
     * @return true, if lhs is not equal to rhs, false otherwise
     */
    friend inline bool
        operator!=(limited_int_iterator<LimitedInt> const &lhs, limited_int_iterator<LimitedInt> const &rhs)
    {
        return !(lhs.iterEl_ == rhs.iterEl_);
    }

    /**
     * @brief Dereference operator.
     *
     * @return LimitedInt& the value the iterator points to.
     */
    LimitedInt &operator*()
    {
        return iterEl_;
    }

    /**
     * @brief Dereference operator.
     *
     * @return LimitedInt& the value the iterator points to.
     */
    LimitedInt *operator->()
    {
        return &iterEl_;
    }

    /**
     * @brief Output stream operator.
     *
     * @param os the output stream to modify
     * @param it the iterator to stream
     * @return std::ostream& the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, limited_int_iterator<LimitedInt> const &it)
    {
        os << "&*" << it.iterEl_;

        return os;
    }

  private:
    LimitedInt       iterEl_;
    LimitedInt const end_;
    bool const       isReverse_;
};
}; // namespace util

#endif // LIMITED_INT_H_INCLUDED
