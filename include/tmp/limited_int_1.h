#ifndef LIMITED_INT_H_INCLUDED
#define LIMITED_INT_H_INCLUDED

#include <iostream>
#include <limits>
#include <exception>
#include <sstream>

using namespace std;

#define SHOW(expr, comment) {cout << "line("<< __LINE__ << "): " << boolalpha << (#expr)\
                                  << "=" << (expr) << "\t\t" << (comment) << endl; }

template < typename T_,
T_ min_ = std::numeric_limits<T_>::min() + 1,
T_ max_ = std::numeric_limits<T_>::max()
>
struct limited_int
{
private:
    T_ val_ = min_;

public:

    limited_int(T_ val = min_) // NOT explicit!
    {
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

    static constexpr limited_int min()
    {
        return min_;
    }

    static constexpr limited_int max()
    {
        return max_;
    }

    T_ val() const // NOT explicit!
    {
        return val_;
    }

    operator T_() const
    {
        return val();
    }


};

constexpr int64_t const MICRO_RAD_PI = 3'141'592LL;
constexpr int64_t const MICRO_RAD_2PI = MICRO_RAD_PI << 1 + 1;

typedef limited_int<long, -179, 180 > Deg180;
typedef limited_int<long, 0, 359> Deg360;
typedef limited_int<long, 0, MICRO_RAD_2PI> Rad2Pi;

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
#ifdef SHOW_COMPILE_ERROR
    deg180 = deg360; // > 180!
    SHOW(deg180, "after assignment of 360 value to 180 value");
#endif

    deg180 = (deg360 > 179 ? ((deg360 % 360) - 360) : (deg360 % 360));
    SHOW(deg180, "after conversion NOTE: uses implicit conversion to POD");
    //...
}


#endif // LIMITED_INT_H_INCLUDED

