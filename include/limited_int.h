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
#include <typeinfo>

namespace util
{

    template<typename INT_, INT_ min_, INT_ max_, uint64_t features_ = 0UL >
    struct limited_int_policy
    {
        constexpr static bool isPolicy_ = true;
        constexpr static INT_ invalid_ = (min_ != std::numeric_limits<INT_>::min() ?
                                          std::numeric_limits<INT_>::min() :
                                          std::numeric_limits<INT_>::max());

        enum Feature : uint64_t
        {
            INCREMENT = 1UL << 0,
            DECREMENT = 1UL << 1,
            UNARY_PLUS = 1UL << 2,
            BINARY_PLUS = 1UL << 3,
            UNARY_MINUS = 1UL << 4,
            BINARY_MINUS = 1UL << 5,
            MULTIPLY = 1UL << 6,
            DIVIDE = 1UL << 7,
        };

        constexpr static bool enabled(Feature const feature)
        {
            return (features_ & feature) == feature;
        }

        static bool withinBounds(INT_ const &val)
        {
            return ((val >= min_) && (val <= max_) && (min_ < max_));
        }

        static bool apply(INT_ & val)
        {
            if (!withinBounds(val))
            {
                val = invalid_;
                return false;
            }
            return true;
        }

        template<typename LimitedInt_>
        static LimitedInt_ nth_next(LimitedInt_ const & val, decltype(val.val()) const & n)
        {
            return val.val() + n;
        }
    };

    template<typename INT_, INT_ min_, INT_ max_, uint64_t features_ = 0UL >
    struct PolicySetInvalid : public limited_int_policy<INT_, min_, max_, features_>
    {
    };

    template<typename INT_, INT_ min_, INT_ max_, uint64_t features_ = 0UL >
    struct PolicyThrowException : public limited_int_policy<INT_, min_, max_, features_>
    {
        typedef limited_int_policy<INT_, min_, max_, features_> Base;

        static bool apply(INT_ & val)
        {
            if (!Base::withinBounds(val))
            {
                val = Base::invalid_;
                std::stringstream ss;
                ss << "limited_int<"
                        << typeid (INT_).name()
                        << ","
                        << min_
                        << ","
                        << max_
                        << ",PolicyThrowException>("
                        << val
                        << ") out of range.";
                throw std::out_of_range(ss.str());
            }
            return true;
        }
    };

    template<typename INT_, INT_ min_, INT_ max_, uint64_t features_ = 0UL >
    struct PolicySetModulo : public limited_int_policy<INT_, min_, max_, features_>
    {
        typedef limited_int_policy<INT_, min_, max_, features_> Base;

        static bool apply(INT_ & val)
        {
            if (!Base::withinBounds(val))
            {
                INT_ const dist = max_ - min_ + 1;
                val -= min_;
                val %= dist;
                if (val < 0)
                    val += dist;
                val += min_;
            }
            return true;
        }
    };


    template <typename LimitedIntTag>
    class limited_int_iterator;

    template < typename T_,
    T_ min_ = std::numeric_limits<T_>::min() + 1,
    T_ max_ = std::numeric_limits<T_>::max(),
    typename Policy_ = PolicySetModulo<T_, min_, max_, 1UL>
    >
    struct limited_int
    {
        typedef Policy_ PolicyType;
    private:

        explicit limited_int(bool dummy)
        : val_(Policy_::invalid_)
        {
        }

        // according to features flagged in Policy_ include features in this.
        constexpr static bool INCREMENT_ENABLED = PolicyType::enabled(PolicyType::Feature::INCREMENT);
        constexpr static bool DECREMENT_ENABLED = PolicyType::enabled(PolicyType::Feature::DECREMENT);
        constexpr static bool UNARY_PLUS_ENABLED = PolicyType::enabled(PolicyType::Feature::UNARY_PLUS);
        constexpr static bool BINARY_PLUS_ENABLED = PolicyType::enabled(PolicyType::Feature::BINARY_PLUS);
        constexpr static bool UNARY_MINUS_ENABLED = PolicyType::enabled(PolicyType::Feature::UNARY_MINUS);
        constexpr static bool BINARY_MINUS_ENABLED = PolicyType::enabled(PolicyType::Feature::BINARY_MINUS);
        constexpr static bool DIVIDE_ENABLED = PolicyType::enabled(PolicyType::Feature::DIVIDE);

        T_ val_ = min_;

    public:
        typedef limited_int_iterator<limited_int<T_, min_, max_, Policy_> > iterator;

        limited_int(T_ val = min_)
        : val_(val)
        {
            static_assert(true == std::is_integral<T_>::value,
                          "limited_int<> needs integral type as template parameter");
            static_assert(min_ < max_,
                          "limited_int<> min needs to be smaller than max");
            //      static_assert(true == is_policy<Policy_>::value,
            //                    "no valid Policy_");
            static_assert(std::numeric_limits<T_>::min() != min_
                          || std::numeric_limits<T_>::max() != max_,
                          "limited_int<> cannot extend from numeric limit min() to max().");

            Policy_::apply(val_);
        }

        template<typename T2_, T2_ min2_, T2_ max2_, typename Policy2_>
        limited_int(limited_int<T2_, min2_, max2_, Policy2_> const & rhs)
        : val_(T_((long double) rhs.val() / (long double) (max2_ - min2_)*(long double) (max_ - min_)))
        {
        }

        bool isValid() const
        {
            return val_ != Policy_::invalid_;
        }

        template<typename = typename std::enable_if<INCREMENT_ENABLED, bool> >
        limited_int& operator++()
        {
            limited_int tmp = (val_ + 1);
            val_ = tmp.val_;
            return *this;
        }

        template<typename = typename std::enable_if<INCREMENT_ENABLED, bool> >
        limited_int& operator++(int)
        {
            limited_int tmp = (val_ + 1);
            val_ = tmp.val_;
            return *this;
        }

        template<typename = typename std::enable_if<DECREMENT_ENABLED, bool> >
        limited_int& operator--()
        {
            limited_int tmp = (val_ - 1);
            val_ = tmp.val_;
            return *this;
        }

        template<typename = typename std::enable_if<DECREMENT_ENABLED, bool> >
        limited_int& operator--(int)
        {
            limited_int tmp = (val_ - 1);
            val_ = tmp.val_;
            return *this;
        }

        static iterator begin(limited_int start = limited_int::min())
        {
            return iterator(start, false);
        }

        static iterator end(limited_int finish = limited_int::invalid())
        {
            return iterator(finish, false);
        }

        static iterator rbegin(limited_int rstart = limited_int::max())
        {
            return iterator(rstart, true);
        }

        static iterator rend(limited_int rfinish = limited_int::invalid())
        {
            return iterator(rfinish, true);
        }

        static limited_int min()
        {
            return min_;
        }

        static limited_int max()
        {
            return max_;
        }

        operator T_() const
        {
            return val();
        }

        T_ val() const
        {
            return val_;
        }

        static limited_int invalid()
        {
            static limited_int<T_, min_, max_, Policy_> invalidLimitedInt(false);
            return invalidLimitedInt;
        }

        friend std::ostream & operator<<(std::ostream & os,
                                         limited_int<T_, min_, max_, Policy_> const & i)
        {
            os << i.val()
                    << " "
                    << "["
                    << (T_) i.min()
                    << ","
                    << (T_) i.max()
                    << "]";
            return os;
        }

    };

    template <typename LimitedInt>
    class limited_int_iterator
    : public std::iterator<std::random_access_iterator_tag, LimitedInt>
    {
        typedef decltype(LimitedInt::invalid().val()) IntType;

        static const LimitedInt getEnd(LimitedInt start, bool isReverse)
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

        void makeStep(IntType step, bool isReverse)
        {
            try
            {
                IntType step = isReverse_ ? IntType(-1) : IntType(1);
                iterEl_ = LimitedInt::PolicyType::nth_next(iterEl_, step);
            }
            catch (std::out_of_range &e)
            {
                iterEl_ = LimitedInt::invalid();
            }
        }
    public:

        limited_int_iterator(LimitedInt start, bool isReverse)
        : iterEl_(start)
        , end_(getEnd(start, isReverse))
        , isReverse_(isReverse)
        {
        }

        limited_int_iterator(const limited_int_iterator<LimitedInt> &) = default;

        limited_int_iterator& operator++()
        {
            makeStep(1, isReverse_);
            return *this;
        }

        limited_int_iterator operator++(int)
        {
            limited_int_iterator<LimitedInt> tmp(*this);
            operator++();

            return tmp;
        }

        limited_int_iterator& operator+=(IntType step)
        {
            makeStep(step, isReverse_);
            return *this;
        }

        limited_int_iterator& operator--()
        {
            makeStep(-1, isReverse_);
            return *this;
        }

        limited_int_iterator operator--(int)
        {
            limited_int_iterator<LimitedInt> tmp(*this);
            operator--();

            return tmp;
        }

        limited_int_iterator& operator-=(IntType step)
        {
            makeStep(-step, isReverse_);
            return *this;
        }

        friend inline bool operator==(const limited_int_iterator<LimitedInt>& lhs,
                                      const limited_int_iterator<LimitedInt>& rhs)
        {
            return lhs.iterEl_ == rhs.iterEl_;
        }

        friend inline bool operator!=(const limited_int_iterator<LimitedInt>& lhs,
                                      const limited_int_iterator<LimitedInt>& rhs)
        {
            return !(lhs.iterEl_ == rhs.iterEl_);
        }

        LimitedInt& operator*()
        {
            return iterEl_;
        }

        LimitedInt* operator->()
        {
            return &iterEl_;
        }

        friend std::ostream & operator<<(std::ostream & os,
                                         limited_int_iterator<LimitedInt> const & i)
        {
            os << "&*" << i.iterEl_;
            return os;
        }

    private:
        LimitedInt iterEl_;
        LimitedInt const end_;
        bool const isReverse_;
    };


}; // namespace util

#endif // LIMITED_INT_H_INCLUDED

