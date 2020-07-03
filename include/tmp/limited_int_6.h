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


struct PolicySetInvalid
{

    template<typename T_>
    static bool apply(T_ const & min, T_ const & max, T_ & val)
    {
        return ((val >= min) && (val <= max));
    }

    template<typename T_>
    static T_ nth_next(T_ const & val, decltype(val.val()) const & n)
    {
        T_ reval;
        return val.val() + n;
    }
};

struct PolicyThrowException
{

    template<typename T_>
    static bool apply(T_ const & min, T_ const & max, T_ & val)
    {
        if ((val > max) || (val < min))
        {
            std::stringstream ss;
            ss << "limited_int<"
                    << typeid (T_).name()
                    << ","
                    << min
                    << ","
                    << max
                    << ",PolicyThrowException>("
                    << val
                    << ") out of range.";
            throw std::out_of_range(ss.str());
        }
        return true;
    }

    template<typename T_>
    static T_ nth_next(T_ const & val, decltype(val.val()) const & n)
    {
        T_ reval;
        return val.val() + n;
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

    template<typename T_>
    static T_ nth_next(T_ const & val, decltype(val.val()) const & n)
    {
       return val.val() + n;
    }
};

template<typename T_>
struct is_policy
{
    static const bool value = false;
};

template<>
struct is_policy<PolicySetInvalid>
{
    static const bool value = true;
};

template<>
struct is_policy<PolicyThrowException>
{
    static const bool value = true;
};

template<>
struct is_policy<PolicySetModulo>
{
    static const bool value = true;
};

template <typename LimitedIntTag>
class limited_int_iterator;

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
   explicit limited_int(bool dummy)
   : val_(invalid_)
   {
   }

public:
   typedef Policy_ PolicyType;
   typedef limited_int_iterator<limited_int<T_, min_, max_, Policy_> > iterator;

   limited_int(T_ val = min_)
   {
      static_assert(true == std::is_integral<T_>::value,
                    "limited_int<> needs integral type as template parameter");
      static_assert(min_ < max_,
                    "limited_int<> min needs to be smaller than max");
      static_assert(true == is_policy<Policy_>::value,
                    "no valid Policy_");
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
            reval = start + (isReverse ? + 1 : -1);
        }
        catch(...)
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

using namespace std;

typedef limited_int<long, -10, 15 > Neg10Pos15;
typedef limited_int<long, -10, 15, PolicySetInvalid> Neg10Pos15Inv;
typedef limited_int<long, -10, 15, PolicyThrowException> Neg10Pos15Throw;

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
   limited_int<char, std::numeric_limits<char>::min(), std::numeric_limits<char>::max()> cc;
#endif
#if SHOW_COMPILE_ERROR == 4
   limited_int<char, 0, 10, long> cc;
#endif

   Neg10Pos15Inv neg10Pos15Inv = 20;
   cout << "initializedValue=" << neg10Pos15Inv << endl;
   
   try
   {
      Neg10Pos15Throw neg10Pos15Throw = 20;
   }
   catch(std::exception const & e)
   {
      cout << "Exception caught: " << e.what() << endl;
   }

   for(auto it = Neg10Pos15::begin(); it != Neg10Pos15::end(); it++)
      cout << *it << endl;
}


#endif // LIMITED_INT_H_INCLUDED

