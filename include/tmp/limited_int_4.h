#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

#include <iostream>
#include <limits>
#include <exception>
#include <sstream>
#include <typeinfo>
#include <set>

using namespace std;

#define SHOW(expr, comment) {cout << "line("<< __LINE__ << "): " << boolalpha << (#expr)\
                                  << "=" << (expr) << "\t\t" << (comment) << endl; }

struct resolve_modulo
{

    template<typename T_>
    static bool resolve(T_ min, T_ max, T_ & val, T_ invalid)
    {
        T_ const dist = max - min + 1;
        val -= min;
        val %= dist;
        if (val < 0)
            val += dist;
        val += min;
        return true;
    }
};

struct resolve_throw
{

    template<typename T_>
    static bool resolve(T_ min, T_ max, T_ & val, T_ invalid)
    {
        std::stringstream ss;
        ss  << "resolve_throw::resolve() limited_int<"
            << typeid (T_).name()
            << ","
            << min
            << ","
            << max
            << ">("
            << val
            << ") out of range.";
        val = (std::numeric_limits<T_>::min() != min ?
               std::numeric_limits<T_>::min() :
               std::numeric_limits<T_>::max());
        throw std::out_of_range(ss.str());
    }
};

struct resolve_invalid
{

    template<typename T_>
    static bool resolve(T_ min, T_ max, T_ & val, T_ invalid)
    {
        val = invalid;
        return false;
    }
};

struct convert_scale
{

    template<typename T_, typename LimitedInt2_>
    static T_ convertFrom(T_ min_, T_ max_, LimitedInt2_ const & rhs)
    {
        long double distLhs = ((long double) max_ - (long double) min_);
        long double distRhs = (rhs.max() - rhs.min());
        long double valRhsTo0 = ((long double) rhs.val() - rhs.min());
        long double scaleFactor = distLhs / distRhs;
        long double valLhsTo0 = valRhsTo0 * scaleFactor;
        T_ reval(valLhsTo0 + (long double) min_);
        return reval;
    }

};

struct convert_circular_scale
{

    template<typename T_, typename LimitedInt2_>
    static T_ convertFrom(T_ min_, T_ max_, LimitedInt2_ const & rhs)
    {
        if(    ((rhs.min()+rhs.max()>1) && rhs.min() != 0)
            || ((min_+max_>1) && min_ != 0))
        {
            std::stringstream ss;
            ss << "convert_circular_scale::convertFrom(" << min_ << "," << max_
                    << "," << rhs
                    << "):can only user circular scale conversion on symmetric around 0 or [0, pos] limited ints";
            throw std::out_of_range(ss.str());
        }

        T_ rhsDist = rhs.max() - rhs.min();
        T_ rhsValMapped = (rhs.min() < 0 && rhs.val() < 0) ?
                            rhs.val() + rhsDist :
                            rhs.val();
        T_ lhsDist = max_ - min_;
        T_ scale = lhsDist/rhsDist;
        T_ lhsValMapped = T_((long double) rhsValMapped * (long double) scale);
        if(min_ < 0)
            lhsValMapped = (lhsValMapped -lhsDist) % lhsDist;

        return lhsValMapped;
    }

};

template<typename INT_,
        INT_ min_,
        INT_ max_,
        typename Res_ = resolve_modulo,
        typename Conv_ = convert_scale>
struct limited_int_traits
{
    typedef Res_ Resolver;
    typedef Conv_ Converter;

    constexpr static INT_ invalid()
    {
        return (min_ != std::numeric_limits<INT_>::min() ?
                std::numeric_limits<INT_>::min() :
                std::numeric_limits<INT_>::max());
    }

    static bool withinBounds(INT_ const &val)
    {
        return ((val >= min_) && (val <= max_) && (min_ < max_));
    }

    static bool apply(INT_ & val)
    {
        if (!withinBounds(val))
        {
            return Resolver::resolve(min_, max_, val, invalid());
        }
        return true;
    }

    template<typename LimitedInt_>
    static INT_ convertFrom(LimitedInt_ const & rhs)
    {
        return Converter::convertFrom(min_, max_, rhs);
    }
};

template <  typename T_,
            T_ min_ = std::numeric_limits<T_>::min() + 1,
            T_ max_ = std::numeric_limits<T_>::max(),
            typename Traits_ = limited_int_traits< T_,
                                                    min_,
                                                    max_,
                                                    resolve_modulo,
                                                    convert_scale>
            >
struct limited_int
{
private:
    T_ val_ = min_;

public:

    limited_int(T_ val = min_)
    : val_(val)
    {
        static_assert(true == std::is_integral<T_>::value,
                      "limited_int<> needs integral type as template parameter");
        static_assert(min_ < max_,
                      "limited_int<> min needs to be smaller than max");
        static_assert((min_ != std::numeric_limits<T_>::min()) || (max_ != std::numeric_limits<T_>::max()),
                      "either min or max must be not equal numeric_limits min() and max()");

        Traits_::apply(val_);
    }

    template<typename T2_, T2_ min2_, T2_ max2_, typename Traits2_>
    limited_int(limited_int<T2_, min2_, max2_, Traits2_> const & rhs)
    {
        val_ = Traits_::convertFrom(rhs);
    }

    bool isValid() const
    {
        return val_ != Traits_::invalid();
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
                                     limited_int<T_, min_, max_, Traits_> const & i)
    {
        if (!i.isValid())
            os << "<INV>";
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

constexpr int64_t const MICRO_RAD_PI = 3'141'592LL;
constexpr int64_t const MICRO_RAD_2PI = MICRO_RAD_PI << 1 + 1LL;

typedef limited_int<int64_t, -179, 180,
limited_int_traits<int64_t, -179, 180, resolve_modulo, convert_circular_scale>> Deg180;
typedef limited_int<int64_t, 0, 359,
limited_int_traits<int64_t, 0, 359, resolve_modulo, convert_circular_scale>> Deg360;
typedef limited_int<int64_t, 0, MICRO_RAD_2PI,
limited_int_traits<int64_t, 0, MICRO_RAD_2PI, resolve_modulo, convert_circular_scale>> Rad2Pi;

typedef limited_int<int64_t, -1'000'000, 1'000'000,
limited_int_traits<int64_t, -1'000'000, 1'000'000, resolve_invalid, convert_scale>> MilliM;
typedef limited_int<int64_t, -1'000'000'000, 1'000'000'000,
limited_int_traits<int64_t, -1'000'000'000, 1'000'000'000, resolve_invalid, convert_scale>> MicroM;
typedef limited_int<int64_t, 0, 2'000'000,
limited_int_traits<int64_t, 0, 2'000'000, resolve_invalid, convert_scale> > MilliM2Million;

void execute()
{
    Deg360 deg360 = 270; // ok
    SHOW(deg360, "valid");

    Deg180 deg180 = -90; // ok
    SHOW(deg180, "valid");

    Rad2Pi rad2Pi = 1'234'567; // ok
    SHOW(rad2Pi, "valid");

    deg360 = 510; // we don't want values like that!
    SHOW(deg360, "now has a valid value");

    deg360 = (510 % 360); // [0, 359]
    SHOW(deg360, "valid after modulo REDUNDANT");

    deg360 = 359;

    deg180 = deg360; // > 180!
    SHOW("deg180 = deg360(359)", "");
    SHOW(deg180, "valid after assignment of 360 value to 180 value, but now intuitive");


    MilliM milliM = -567'000;
    MicroM microM = milliM;
    SHOW(milliM, "");
    SHOW(microM, "natural scaling conversion");
    MilliM2Million mm2Mio = milliM;
    SHOW(mm2Mio, "natural scaling conversion");

    MilliM milliMStrange = 1'500'000;
    SHOW(milliMStrange, "No longer strange behavior for linear (milliMStrange = 1'500'000)");

#ifdef SHOW_COMPILE_ERROR
    limited_int<int, 5, -3 > strange1;
    SHOW(strange1, "strange way to define a limited int min > max");
    limited_int<double, 1.6, 3.1415> strange2;
    SHOW(strange2, "strange way to define a limited int");
    limited_int<bool, false, true > strange3;
    SHOW(strange3, "strange way to define a limited int -- whole range");

    limited_int<bool, true, true > strange4;
    SHOW(strange4, "strange way to define a limited int -- min == max");
#endif

    SHOW(std::is_integral<MilliM>::value,"");
    SHOW(std::is_integral<Deg360>::value,"");
    std::set<MilliM> mmSet;
    for(int64_t v = -3; v < 3; v++)
    {
        mmSet.insert(v);
    }
    for(auto v : mmSet)
    {
        SHOW(MicroM(v),"conversion from MilliM");
        SHOW(MicroM(v+10),"conversion from int64_t ######## STRANGE");
        SHOW(MicroM(MilliM(v+10)),"conversion from MilliM");
    }

    typedef limited_int<short, -10, 10> ShortCircuit;
    for(ShortCircuit i = 5; i != 2; i = ShortCircuit(i+1))
        SHOW(i, "ShortCircuit");

    typedef limited_int<short, -10, 10, limited_int_traits<short, -10, 10, resolve_invalid>> ShortCut;
    for(ShortCut i = 5; i.isValid(); i = ShortCut(i+1))
        SHOW(i, "ShortCut");

    SHOW(sizeof(ShortCut),"");
    SHOW(sizeof(short),"");

    SHOW(std::max(ShortCircuit(43),ShortCircuit(31415)),"");
    SHOW(ShortCircuit(43),"");
    SHOW(ShortCircuit(31415),"");

    int64_t x = ShortCircuit(31415);
    SHOW(x,"");

#ifdef SHOW_COMPILE_ERROR
    typedef limited_int<long, -10, 10, limited_int_traits<long, -10, 10, long>> LongShot;
    LongShot longShot = 5;
#endif

    typedef limited_int<long, -10, 10, limited_int_traits<long, -10, 10, resolve_throw>> LongJump;
    LongJump longJump = 13;

}


#endif // LIMITED_INT_H_INCLUDED

