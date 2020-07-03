#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

#include <iostream>
#include <limits>
#include <exception>
#include <sstream>
#include <typeinfo>

using namespace std;

#define SHOW(expr, comment) {cout << "line("<< __LINE__ << "): " << boolalpha << (#expr)\
                                  << "=" << (expr) << "\t\t" << (comment) << endl; }

struct out_of_bounds_resolver {};

struct resolve_modulo : public out_of_bounds_resolver
{
   template<typename T_, T_ min, T_ max>
   T_ operator() (T_ const val)
   {
      T_ const dist = max - min + 1;
      val -= min;
      val %= dist;
      if (val < 0)
          val += dist;
      val += min;
      return val;
   }
};

struct resolve_throw : public out_of_bounds_resolver
{
   template<typename T_, T_ min, T_ max>
   void operator() (T_ & val)
   {
      val = (std::numeric_limits<T_>::min() != min ? 
               std::numeric_limits<T_>::min():
               std::numeric_limits<T_>::max());
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
};

struct resolve_invalid : public out_of_bounds_resolver
{
   template<typename T_, T_ min, T_ max>
   void operator() (T_ & val)
   {
      val = (std::numeric_limits<T_>::min() != min ? 
               std::numeric_limits<T_>::min():
               std::numeric_limits<T_>::max());
   }
};

struct limited_int_converter {};

struct convert_scale : limited_int_converter
{
    template<typename T_, T_ min_, T_ max_, typename LimitedInt2_>
    static T_ convertFrom(LimitedInt2_ const & rhs)
    {
      long double distLhs = ((long double)max_ - (long double)min_);
      long double distRhs = (rhs.max() - rhs.min());
      long double valRhsTo0 = ((long double)rhs.val() -rhs.min());
      long double scaleFactor = distLhs/distRhs;
      long double valLhsTo0 = valRhsTo0 * scaleFactor;
      T_ reval(valLhsTo0 + (long double)min_);
      return reval;
    }
   
};


template<typename INT_, INT_ min_, INT_ max_>
struct limited_int_policy
{
    constexpr static INT_ invalid_ = (min_ != std::numeric_limits<INT_>::min() ?
                                      std::numeric_limits<INT_>::min() :
                                      std::numeric_limits<INT_>::max());

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
        return val.val() + n; // rely on the constructor to make the value a limited int
    }
};

template<typename INT_, INT_ min_, INT_ max_, uint64_t features_ = 0UL >
struct PolicySetInvalid : public limited_int_policy<INT_, min_, max_>
{
};

template<typename INT_, INT_ min_, INT_ max_, uint64_t features_ = 0UL >
struct PolicyThrowException : public limited_int_policy<INT_, min_, max_>
{
    typedef limited_int_policy<INT_, min_, max_> Base;

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
struct PolicySetModulo : public limited_int_policy<INT_, min_, max_>
{
    typedef limited_int_policy<INT_, min_, max_> Base;

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


template <  typename T_,
            T_ min_ = std::numeric_limits<T_>::min() + 1,
            T_ max_ = std::numeric_limits<T_>::max()
         >
struct limited_int
{
private:
   T_ val_ = min_;

public:

   limited_int(T_ val = min_)
   {
      static_assert(true == std::is_integral<T_>::value,
                    "limited_int<> needs integral type as template parameter");
      static_assert(min_ < max_,
                    "limited_int<> min needs to be smaller than max");
      static_assert((min_ != std::numeric_limits<T_>::min()) || (max_ != std::numeric_limits<T_>::max()),
                    "either min or max must be not equal numeric_limits min() and max()");
 
      if ((val > max_) || (val < min_))
      {
         T_ const dist = max_ - min_ + 1;
         val -= min_;
         val %= dist;
         if (val < 0)
            val += dist;
         val += min_;
      }
      val_ = val;
   }

   template<typename T2_, T2_ min2_, T2_ max2_>
   limited_int(limited_int<T2_, min2_, max2_> const & rhs)
   {
      long double distLhs = (max() - min());
      long double distRhs = (rhs.max() - rhs.min());
      long double valRhsTo0 = ((long double)rhs.val() -rhs.min());
      long double scaleFactor = distLhs/distRhs;
      long double valLhsTo0 = valRhsTo0 * scaleFactor;
      val_ = T_(valLhsTo0 + (long double)min());
   }

   static constexpr limited_int min()
   {
      return min_;
   }

   static constexpr limited_int max()
   {
      return max_;
   }

   T_ val() const
   {
      return val_;
   }

   operator T_() const
   {
      return val();
   }

   // global operator defined as friend within the template body
   friend std::ostream & operator<<(std::ostream & os,
                                    limited_int<T_, min_, max_> const & i)
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

using namespace std;

constexpr int64_t const MICRO_RAD_PI  = 3'141'592LL;
constexpr int64_t const MICRO_RAD_2PI = MICRO_RAD_PI<<1; //NOTE 1 smaller than at KUKA

typedef limited_int<int64_t, -179, 180> Deg180;
typedef limited_int<int64_t, 0, 359> Deg360;
typedef limited_int<int64_t, 0, MICRO_RAD_2PI> Rad2Pi;

typedef limited_int<int64_t, -1'000'000, 1'000'000> MilliM;
typedef limited_int<int64_t, -1'000'000'000, 1'000'000'000> MicroM;
typedef limited_int<int64_t, 0, 2'000'000> MilliM2Million;

void execute()
{
   Deg360 deg360 = 270;         // ok
   SHOW(deg360, "valid");
   
   Deg180 deg180 = -90;         // ok
   SHOW(deg180, "valid");
   
   Rad2Pi rad2Pi = 1'234'567;   // ok
   SHOW(rad2Pi, "valid");
   
   deg360 = 510;  // we don't want values like that!
   SHOW(deg360, "now has a valid value");
   
   deg360 = (510 % 360); // [0, 359]
   SHOW(deg360, "valid after modulo REDUNDANT");
   
   deg360 = 359;
   
   deg180 = deg360; // > 180!
   SHOW("deg180 = deg360","");
   SHOW(deg180, "valid after assignment of 360 value to 180 value, but NOT intuitive in this context");

   
   MilliM milliM = -567'000;
   MicroM microM = milliM;
   SHOW(milliM, "");
   SHOW(microM,"natural scaling conversion");
   MilliM2Million mm2Mio = milliM;
   SHOW(mm2Mio,"natural scaling conversion");
   
   MilliM milliMStrange = 1'500'000;
   SHOW(milliMStrange,"Strange behavior for linear (milliMStrange = 1'500'000)");
   
#ifdef SHOW_COMPILE_ERROR
   limited_int<int, 5, -3> strange1;
   SHOW(strange1,"strange way to define a limited int min > max");
   limited_int<double, 1.6, 3.1415> strange2;
   SHOW(strange2,"strange way to define a limited int");
   limited_int<bool, false, true> strange3;
   SHOW(strange3,"strange way to define a limited int -- whole range");

   limited_int<bool, true, true> strange4;
   SHOW(strange4,"strange way to define a limited int -- min == max");
#endif
}


#endif // LIMITED_INT_H_INCLUDED

