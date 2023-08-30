/*
 * File Name:   anyutil.h
 * Description: std::any utility functions
 *
 * Copyright (C) 2023 Dieter J Kybelksties
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

#ifndef NS_UTIL_ANYUTIL_H_INCLUDED
#define NS_UTIL_ANYUTIL_H_INCLUDED

#include "dateutil.h"
#include "stringutil.h"

#include <any>
#include <boost/date_time.hpp>
#include <cmath>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <sstream>

namespace util
{
class Var;

template<typename T_>
bool isA(const Var &v);

template<typename T_>
bool containsT(const Var &lhsInterval, const Var &rhs);

template<typename T_>
bool equalT(const Var &lhs, const Var &rhs);

template<typename T_>
bool lessT(const Var &lhs, const Var &rhs);

template<typename T_>
bool lessEqualT(const Var &lhs, const Var &rhs);

template<typename T_>
bool greaterT(const Var &lhs, const Var &rhs);

template<typename T_>
bool greaterEqualT(const Var &lhs, const Var &rhs);

/**
 *  Error handling for any-cast for variants.
 */
struct cast_error : public std::logic_error
{
    /**
     * Construct given the incompatible type-names,
     * @param from type to cast from
     * @param to type to cast to
     */
    cast_error(const std::string &from, const std::string &to)
    : std::logic_error("Cannot any_cast " + from + " to " + to)
    {
    }
};

/**
 *  Error handling for string-to-boolean conversion.
 */
struct boolstr_error : public std::logic_error
{
    /**
     * Construct with the string that cannot be parsed into a boolean value.
     * @param boolstr the failing string
     */
    boolstr_error(const std::string &boolstr) : std::logic_error("Cannot parse '" + boolstr + "' into valid bool")
    {
    }
};

/** The only boolean type allowed in Var-variants. */
using VAR_BOOL = bool;

/** The only character type allowed in Var-variants. */
using VAR_CHAR = char;

/** The only integer type allowed in Var-variants. */
using VAR_INT = long long;

/**The only unsigned integer type allowed in Var-variants. */
using VAR_UINT = unsigned long long;

/** The only floating point type allowed in Var-variants. */
using VAR_FLOAT = long double;
/** The only date type allowed in Var-variants. */
using VAR_DATE = boost::posix_time::ptime;

/**The only character-string type allowed in Var-variants.*/
using VAR_STRING = std::string;

inline bool operator<(const VAR_STRING &lhs, const VAR_STRING &rhs)
{
    return lhs.compare(rhs) < 0;
}

inline bool operator<=(const VAR_STRING &lhs, const VAR_STRING &rhs)
{
    return lhs.compare(rhs) <= 0;
}

inline bool operator>(const VAR_STRING &lhs, const VAR_STRING &rhs)
{
    return lhs.compare(rhs) > 0;
}

inline bool operator>=(const VAR_STRING &lhs, const VAR_STRING &rhs)
{
    return lhs.compare(rhs) >= 0;
}

/**
 * Helper to check that two floating point values are within tolerance of each other.
 * @param v1 value1
 * @param v2 value2
 * @param tolerance a permissible deviation between v1 and v2
 *
 */
inline bool withinTolerance(VAR_FLOAT v1, VAR_FLOAT v2, VAR_FLOAT tolerance = 1e-18L)
{
    return (std::abs(v1 - v2) < tolerance);
}

/**
 * Minimal (range) value for allowable Var-types.
 * @return the minimal value for the template type T_
 */
template<typename T_>
inline T_ minVal()
{
    return (std::numeric_limits<T_>::lowest());
}

/**
 * Maximal (range) value for allowable Var-types.
 * @return the maximal value for the template type T_
 */
template<typename T_>
inline T_ maxVal()
{
    return (std::numeric_limits<T_>::max());
}

/**
 * Minimal (range) value specialised for dates.
 * @return the minimal value for the date
 */
template<>
inline VAR_DATE minVal<VAR_DATE>()
{
    return (VAR_DATE(boost::posix_time::min_date_time));
}

/**
 * Maximal (range) value specialised for dates.
 * @return the maximal value for the date
 */
template<>
inline VAR_DATE maxVal<VAR_DATE>()
{
    return (VAR_DATE(boost::posix_time::max_date_time));
}

enum class borderType : unsigned char
{
    /** a non-infinite minimum specified */
    finiteMin = 0x01,
    /** a non-infinite maximum specified */
    finiteMax = 0x02,
    /** both minimum and maximum are finite */
    finite = finiteMin | finiteMax,
    /** include the left corner */
    leftClosed = 0x04,
    /** include the right corner */
    rightClosed = 0x08,
    /** include both corners */
    closed = leftClosed | rightClosed,

    // complements
    infiniteMin = static_cast<unsigned char>(~finiteMin),
    infiniteMax = static_cast<unsigned char>(~finiteMax),
    infinite    = infiniteMin | infiniteMax,
    leftOpen    = static_cast<unsigned char>(~leftClosed),
    rightOpen   = static_cast<unsigned char>(~rightClosed),
    open        = leftOpen | rightOpen
};

inline unsigned char operator&(const borderType &lhs, const borderType &rhs)
{
    return (lhs & lhs);
}

inline unsigned char operator|(const borderType &lhs, const borderType &rhs)
{
    return (lhs | lhs);
}

inline unsigned char operator<(const borderType &lhs, const borderType &rhs)
{
    return (static_cast<unsigned char>(lhs) < static_cast<unsigned char>(lhs));
}

constexpr borderType finiteMin   = borderType::finiteMin;
constexpr borderType finiteMax   = borderType::finiteMax;
constexpr borderType finite      = borderType::finite;
constexpr borderType leftClosed  = borderType::leftClosed;
constexpr borderType rightClosed = borderType::rightClosed;
constexpr borderType closed      = borderType::closed;

constexpr borderType infiniteMin = borderType::infiniteMin;
constexpr borderType infiniteMax = borderType::infiniteMax;
constexpr borderType infinite    = borderType::infinite;
constexpr borderType leftOpen    = borderType::leftOpen;
constexpr borderType rightOpen   = borderType::rightOpen;
constexpr borderType open        = borderType::open;

struct IntervalType
{
    void setFlag(const borderType &bt)
    {
        if(bt == finiteMin || bt == finiteMax || bt == finite || bt == leftClosed || bt == rightClosed || bt == closed)
            traits_ |= static_cast<unsigned char>(bt);
        else
            traits_ &= static_cast<unsigned char>(bt);
    }

    IntervalType(std::initializer_list<borderType> l = std::initializer_list<borderType>({closed, finite}))
    {
        for(auto it: l)
            setFlag(it);
    }

    void resetTraits()
    {
        traits_ = 0x0;
    }

    [[nodiscard]] bool isLeftFinite() const
    {
        return ((traits_ & static_cast<unsigned char>(finiteMin)) == static_cast<unsigned char>(finiteMin));
    }

    [[nodiscard]] bool isLeftInfinite() const
    {
        return (!isLeftFinite());
    }

    [[nodiscard]] bool isRightFinite() const
    {
        return ((traits_ & static_cast<unsigned char>(finiteMax)) == static_cast<unsigned char>(finiteMax));
    }

    [[nodiscard]] bool isRightInfinite() const
    {
        return (!isRightFinite());
    }

    [[nodiscard]] bool isInfinite() const
    {
        return (isLeftInfinite() && isRightInfinite());
    }

    [[nodiscard]] bool isFinite() const
    {
        return (!isInfinite());
    }

    [[nodiscard]] bool isLeftClosed() const
    {
        return ((traits_ & static_cast<unsigned char>(leftClosed)) == static_cast<unsigned char>(leftClosed));
    }

    [[nodiscard]] bool isRightClosed() const
    {
        return ((traits_ & static_cast<unsigned char>(rightClosed)) == static_cast<unsigned char>(rightClosed));
    }

    [[nodiscard]] bool isClosed() const
    {
        return (isLeftClosed() && isRightClosed());
    }

    [[nodiscard]] bool isLeftOpen() const
    {
        return (!isLeftClosed());
    }

    [[nodiscard]] bool isRightOpen() const
    {
        return (!isRightClosed());
    }

    [[nodiscard]] bool isOpen() const
    {
        return (!isClosed());
    }

    [[nodiscard]] unsigned char traits() const
    {
        return (traits_);
    }

    friend bool operator==(const IntervalType &lhs, const IntervalType &rhs)
    {
        return (lhs.traits_ == rhs.traits_);
    }

    friend bool operator<(const IntervalType &lhs, const IntervalType &rhs)
    {
        return (lhs.traits_ < rhs.traits_);
    }

    private:
    unsigned char traits_{0};
};

/**
 *  Numeric/date intervals than can be half- or fully- open or closed.
 */
template<typename T_>
struct Interval : public IntervalType
{
    /**
     * Default construct a left- or right- finite interval interval.
     * The tags finiteMin and infiniteMax in this context mean the same: the
     * interval is infinite on the right side and finite on the left. Conversely
     * the tags infiniteMin and  finiteMax mean that the
     * interval is finite on the right side and infinite on the left.
     * right/left Open/Closed flags can be added. Any inconsistencies are made
     * consistent
     * @param v the value at the finite side, if it defaults to the minimal
     *          value for the parameter type T_, then the interval will be
     *          the entire domain (-oo..+oo)
     * @param l should contain at least one of the tags
     * <ul>
     * <li> finiteMin </li>
     * <li> infiniteMax </li>
     * <li> finiteMax </li>
     * <li> infiniteMin </li>
     * </ul> If none is provided "infiniteMax is assumed. if no open/close
     * tag is provided then the finite border will be closed, the infinite
     * will be open. The tag "finite" will be ignored as it doesn't make sense
     * in this context.
     */
    Interval(const T_                         &v = minVal<T_>(),
             std::initializer_list<borderType> l = std::initializer_list<borderType>({finiteMin, leftClosed}))
    : IntervalType()
    {
        bool leftBoundaryIsMinimum = v == minVal<T_>();

        if(leftBoundaryIsMinimum)  // use whole domain
        {
            low_  = minVal<T_>();
            high_ = maxVal<T_>();
        }
        else
        {
            bool foundInfiniteMin         = false;
            bool foundInfiniteMax         = false;
            bool foundLeftClosed          = false;
            bool foundRightClosed         = false;
            bool leftOpenClosedSpecified  = false;
            bool rightOpenClosedSpecified = false;

            // unsigned char newTraits_ = 0x00;

            for(auto it: l)
            {
                switch(it)
                {
                    case infiniteMin:
                    case finiteMax:
                        foundInfiniteMin = true;
                        break;
                    case infiniteMax:
                    case finiteMin:
                        foundInfiniteMax = true;
                        break;
                    case finite:
                        // ignore
                        break;
                    case leftClosed:
                        foundLeftClosed         = true;
                        leftOpenClosedSpecified = true;
                        break;
                    case rightClosed:
                        foundRightClosed         = true;
                        rightOpenClosedSpecified = true;
                        break;
                    case closed:
                        foundLeftClosed          = true;
                        foundRightClosed         = true;
                        rightOpenClosedSpecified = true;
                        leftOpenClosedSpecified  = true;
                        break;
                    case leftOpen:
                        foundLeftClosed         = false;
                        leftOpenClosedSpecified = true;
                        break;
                    case rightOpen:
                        foundRightClosed         = false;
                        rightOpenClosedSpecified = true;
                        break;
                    case open:
                        foundLeftClosed         = false;
                        foundRightClosed        = false;
                        leftOpenClosedSpecified = true;
                        break;
                    default:
                        break;
                }
            }

            resetTraits();

            if((!foundInfiniteMax && !foundInfiniteMin) || foundInfiniteMax)
            {
                setFlag(finiteMin);
                setFlag(infiniteMax);
            }
            else
            {
                setFlag(infiniteMin);
                setFlag(finiteMax);
            }

            if(foundLeftClosed || !leftOpenClosedSpecified)
                setFlag(leftClosed);

            if(foundRightClosed || !rightOpenClosedSpecified)
                setFlag(rightClosed);

            low_  = (isLeftFinite() ? v : minVal<T_>());
            high_ = (isLeftFinite() ? maxVal<T_>() : v);
        }
    }

    /**
     * Construct a closed interval [v1,v2].
     * @param v1 one of the interval borders
     * @param v2 the other one of the interval borders
     * @param inclusivity can be used to include/exclude left or right borders
     */
    Interval(const T_                         &v1,
             const T_                         &v2,
             std::initializer_list<borderType> inclusivity = std::initializer_list<borderType>({closed, finite}))
    : IntervalType(inclusivity)
    , low_(v1 < v2 ? v1 : v2)
    , high_(v1 < v2 ? v2 : v1)
    {
    }

    [[nodiscard]] T_ left() const
    {
        return (Interval<T_>::isLeftInfinite() ? minVal<T_>() : low_);
    }

    [[nodiscard]] T_ right() const
    {
        return (Interval<T_>::isRightInfinite() ? maxVal<T_>() : high_);
    }

    /**
     * Check whether the interval contains value v.
     * @param v value to check
     * @return true if v is contained in the interval, false otherwise
     */
    [[nodiscard]] bool contains(const T_ &v) const
    {
        /* [-∞,+∞],  (-∞,+∞)                              */
        /* [-∞,high], [-∞,high), (-∞,high], (-∞,high)     */
        /* [low,+∞],  [low,+∞),  (low,+∞],  (low,+∞)      */
        /* [low,high], [low,high), (low,high], (low,high) */
        return ((isLeftClosed() ? low_ <= v : low_ < v) && (isRightClosed() ? v <= high_ : v < high_));
    }

    /**
     * Check whether the interval is wholly included in this interval.
     * @param rhs the interval to check
     *
     * @return true if the given interval is fully covered in this interval,
     *         false otherwise
     */
    [[nodiscard]] bool isSubIntervalOf(const Interval &rhs) const
    {
        return (rhs.contains(low_) && rhs.contains(high_));
    }

    [[nodiscard]] std::string verbosetoString() const
    {
        std::string reval = "";

        reval += (isLeftClosed()) ? "leftClosed[ " : "leftOpen (";
        reval += isLeftInfinite() ? "leftInf '" + toString(minVal<T_>()) + "' " : "leftMin '" + toString(low_) + "' ";
        reval +=
         isRightInfinite() ? "rightInf '-" + toString(maxVal<T_>()) + "' " : "rightMax '" + toString(high_) + "' ";
        reval += (isRightClosed()) ? "] rightClosed " : ") rightOpen";

        return (reval);
    }

    template<typename T1_, typename T2_>
    friend inline bool operator<(const Interval<T1_> &lhs, const Interval<T2_> &rhs);

    template<typename T1_, typename T2_>
    friend bool operator==(const Interval<T1_> &lhs, const Interval<T2_> &rhs);

    template<typename TT_>
    friend std::ostream &operator<<(std::ostream &os, const Interval<TT_> &itvl);

    private:
    T_ low_;   ///< Minimal value
    T_ high_;  ///< Maximal value
};

/** The only boolean-interval type allowed in Var-variants. */
using VAR_BOOL_INTERVAL = Interval<VAR_BOOL>;

/** The only character-interval type allowed in Var-variants. */
using VAR_CHAR_INTERVAL = Interval<VAR_CHAR>;

/** The only integer-interval type allowed in Var-variants. */
using VAR_INT_INTERVAL = Interval<VAR_INT>;

/** The only unsigned-integer-interval type allowed in Var-variants. */
using VAR_UINT_INTERVAL = Interval<VAR_UINT>;

/** The only floating-point-interval type allowed in Var-variants. */
using VAR_FLOAT_INTERVAL = Interval<VAR_FLOAT>;

/** The only date-interval type allowed in Var-variants. */
using VAR_DATE_INTERVAL = Interval<VAR_DATE>;

/**
 *  Restricted type variant.
 *  Only the longest integer and floating point types, dates and strings
 *  and intervals are allowed.
 */
class Var
{
    public:
    enum StreamMode : long
    {
        reset             = 0x0000,  ///< reset the stream configuration to empty
        quoted_char       = 0x0001,  ///< enclose characters in single quotes
        hex_char          = 0x0002,  ///< display characters in hexadecimal representation
        quoted_string     = 0x0004,  ///< enclose strings in double quotes
        quoted_date       = 0x0008,  ///< enclose dates in double quotes
        alpha_bool        = 0x0010,  ///< display booleans as true and false
        short_float       = 0x0020,  ///< display floating point values in a short format
        long_float        = 0x0040,  ///< display floating point values in a longer format
        scientific_float  = 0x0080,  ///< display floating point values in scientific format
        round_open_brace  = 0x0100,  ///< indicate open intervals with round braces
        symbolic_infinity = 0x0200,  ///< indicate full interval with symbolic infinity "oo"

        pure     = alpha_bool | hex_char | scientific_float,     ///< simple scannable format combination
        standard = alpha_bool | short_float | round_open_brace,  ///< standard format combination
        safe     = quoted_char | hex_char | quoted_string | quoted_date | alpha_bool
               | scientific_float  ///< more complex combination
    };

    Var();                    ///< Construct empty variant.
    Var(const VAR_BOOL &v);   ///< Construct boolean variant.
    Var(const VAR_CHAR &v);   ///< Construct character variant.
    Var(const VAR_INT &v);    ///< Construct signed integer variant.
    Var(const VAR_UINT &v);   ///< Construct unsigned integer variant.
    Var(const VAR_FLOAT &v);  ///< Construct floating point variant.
    Var(const VAR_DATE &v);   ///< Construct date variant.

    /**
     *  Construct an T_-type interval variant.
     */
    template<typename T_>
    Var(const Interval<T_> &itvl) : value_(itvl)
    {
    }

    Var(const VAR_STRING &v);                  ///< Construct string variant.
    Var(const Var &rhs)            = default;  ///< Copy-construct a variant.
    Var &operator=(const Var &rhs) = default;  ///< Assign a variant.

    [[nodiscard]] const std::type_info &type() const;    ///< Get the typeid of the contained value.
    [[nodiscard]] bool                  empty() const;   ///< Check whether the variant is empty.
    Var                                &swap(Var &rhs);  ///< Swap this variant with the rhs- variant.
    [[nodiscard]] std::any              value() const;   ///< get the contained values as std::any.

    /**
     * @brief Check whether the value has the native type T_.
     * 
     * @tparam T_ template type
     * @param v value
     * @return true, if the typeid of the value matches the typeid of the T_, false otherwise 
     */
    template<typename T_>
    friend bool isA(const Var &v)
    {
        return (v.type() == typeid(T_));
    }

    /**
     *  Get the value as the native type.
     */
    template<typename T_>
    [[nodiscard]] T_ get() const
    {
        if(!isA<T_>(*this))
        {
            throw cast_error(type().name(), typeid(T_).name());
        }

        return (std::any_cast<T_>(value_));
    }

    /**
     * @brief Check whether two vars have the same native type.
     * 
     * @tparam T_ a native type
     * @param v1 first value
     * @param v2 second value
     * @return true, is v1 and v2 have the same type T_, false otherwise
    */
    template<typename T_>
    friend bool sameType(const Var &v1, const Var &v2)
    {
         return (v1.type() == v2.type() && v1.type() == typeid(T_));
    }

    /**
     * Check whether two vars have the same native type.
     */

    /**
     * @brief Check whether two vars have the same native type.
     * 
     * @param v1 first value
     * @param v2 second value
     * @return true, is v1 and v2 have the same type, false otherwise
     */
    friend bool sameType(const Var &v1, const Var &v2)
    {
        return (v1.type() == v2.type());
    }

    /**
     * Template equalT is needed to ensure that Var's can be put in
     * associative containers.
     * The other comparison functions only will be used to transcend their
     * native type's operators.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return true, if lhs is equal to rhs, false otherwise
     */
    template<typename T_>
    friend bool equalT(const Var &lhs, const Var &rhs)
    {
        try
        {
            return (lhs.get<T_>() == rhs.get<T_>());
        }
        catch(const util::cast_error& e)
        {
            return false;
        }
    }

    /**
     * Template lessT is needed to ensure that Var's can be put in
     * associative containers.
     * The other comparison functions only will be used to transcend their
     * native type's operators.
     * Helper template for definition of global operator &lt;.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return true, if lhs is less than rhs, false otherwise
     */
    template<typename T_>
    friend bool lessT(const Var &lhs, const Var &rhs)
    {
        try
        {
            return (lhs.get<T_>() < rhs.get<T_>());
        }
        catch(const util::cast_error& e)
        {
            return false;
        }
    }

    /**
     * Helper template for definition of global operator &le;.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return true, if lhs is less/equal than rhs, false otherwise
     */
    template<typename T_>
    friend bool lessEqualT(const Var &lhs, const Var &rhs)
    {
        try
        {
            return (lhs.get<T_>() <= rhs.get<T_>());
        }
        catch(const util::cast_error& e)
        {
            return false;
        }
    }
    
    /**
     * Helper template for definition of global operator &gt;.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return true if lhs is greater than rhs, false otherwise
     */
    template<typename T_>
    friend bool greaterT(const Var &lhs, const Var &rhs)
    {
        try
        {
            return (lhs.get<T_>() > rhs.get<T_>());
        }
        catch(const util::cast_error& e)
        {
            return false;
        }
   }

    /**
     * Helper template for definition of global operator &ge;.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return true, if lhs is greater/equal than rhs, false otherwise
     */
    template<typename T_>
    friend bool greaterEqualT(const Var &lhs, const Var &rhs)
    {
        try
        {
            return (lhs.get<T_>() >= rhs.get<T_>());
        }
        catch(const util::cast_error& e)
        {
            return false;
        }
    }

    /**
     * Helper template for definition of global containment check.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return if lhs contains rhs, false otherwise
     */
    template<typename T_>
    friend bool containsT(const Var &lhsInterval, const Var &rhs)
    {
        if(lhsInterval.type() != typeid(Interval<T_>) || rhs.type() != typeid(T_))
            return (false);

        auto itvl = std::any_cast<Interval<T_>>(lhsInterval.value());

        return (itvl.contains(std::any_cast<T_>(rhs.value())));
    }

    /**
     * Check whether an Interval - Variant contains a value.
     * Non-Interval variants are treated as Intervals containing only one value.
     *
     * @param val value to check
     *
     * @return if this contains val, false otherwise
     */
    [[nodiscard]] bool contains(const Var &val) const;

    /**
     * Variant equality.
     * Transcends to native type operator.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return if lhs is equal to rhs, false otherwise
     */
    friend bool operator==(const Var &lhs, const Var &rhs);

    /**
     * Variant less than.
     * Transcends to native type operator.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return if lhs is less than rhs, false otherwise
     */
    friend bool operator<(const Var &lhs, const Var &rhs);

    /**
     * Variant less/equality.
     * Transcends to native type operator.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return if lhs is less/equal to rhs, false otherwise
     */
    friend bool operator<=(const Var &lhs, const Var &rhs);

    /**
     * Variant greater.
     * Transcends to native type operator.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     *
     * @return if lhs is greater than rhs, false otherwise
     */
    friend bool operator>(const Var &lhs, const Var &rhs);

    /**
     * Variant greater/equality.
     * Transcends to native type operator.
     *
     * @param lhs left-hand-side of the comparison
     * @param rhs right-hand-side of the comparison
     * @return if lhs is greater/equal than rhs, false otherwise
     */
    friend bool operator>=(const Var &lhs, const Var &rhs);

    /**
     * Out-stream operator for variant stream modifiers.
     *
     * @param os the ostream for output
     * @param sm the modifier
     *
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, Var::StreamMode sm);

    /**
     * Out-stream operator for variants.
     *
     * @param os the ostream for output
     * @param v the variant to output
     *
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const Var &v)
    {
        if(isA<VAR_BOOL>(v))
            os << v.get<VAR_BOOL>();
        else if(isA<VAR_CHAR>(v))
            os << v.get<VAR_CHAR>();
        else if(isA<VAR_INT>(v))
            os << v.get<VAR_INT>();
        else if(isA<VAR_UINT>(v))
            os << v.get<VAR_UINT>();
        else if(isA<VAR_FLOAT>(v))
            os << v.get<VAR_FLOAT>();
        else if(isA<VAR_DATE>(v))
            os << v.get<VAR_DATE>();
        else if(isA<VAR_STRING>(v))
            os << v.get<VAR_STRING>();

        return (os);
    }

    /**
     * Out-stream operator for intervals.
     *
     * @param os the ostream for output
     * @param itvl the interval to output
     *
     * @return the modified stream
     */
    template<typename TT_>
    friend std::ostream &operator<<(std::ostream &os, const Interval<TT_> &itvl);

    private:
    std::any         value_;
    const static int xalloc_index;
};

/**
 * Abstract base class for (match-)operations.
 */
struct Operation
{
    virtual ~Operation() = default;

    /**
     * Check whether the left hand Var matches the right hand side.
     * This operation is not symmetric:
     * <ul>
     * <li>operator is taken from right-hand side</li>
     * <li>&lt; , &le;, &gt;, &ge; follow natural definition</li>
     * </ul>
     *
     * @param lhs left-hand-side of comparison
     * @param rhs right-hand-side of comparison
     *
     * @return true if left matches right, false otherwise
     */
    [[nodiscard]] virtual bool leftMatchesRight(const Var &lhs, const Var &rhs) const = 0;

    /**
     * Provide a description of the operation for stream-operators.
     *
     * @param v the variant to get a description for
     *
     * @return a string description of v
     */
    [[nodiscard]] virtual std::string desc(const Var &v) const = 0;
};

/**
 *  Operation to check whether two Variant are equal.
 */
struct Equals : public Operation
{
    ~Equals() override = default;

    [[nodiscard]] bool        leftMatchesRight(const Var &lhs, const Var &rhs) const override;
    [[nodiscard]] std::string desc(const Var &v) const override;
};

/**
 *  Operation to check whether one Variant is less than another.
 */
struct Less : public Operation
{
    ~Less() override = default;

    [[nodiscard]] bool        leftMatchesRight(const Var &lhs, const Var &rhs) const override;
    [[nodiscard]] std::string desc(const Var &v) const override;
};

/**
 *  Operation to check whether one Variant is less-equal than another.
 */
struct LessEqual : public Operation
{
    ~LessEqual() override = default;

    [[nodiscard]] bool        leftMatchesRight(const Var &lhs, const Var &rhs) const override;
    [[nodiscard]] std::string desc(const Var &v) const override;
};

/**
 *  Operation to check whether one Variant is greater than another.
 */
struct Greater : public Operation
{
    ~Greater() override = default;

    [[nodiscard]] bool        leftMatchesRight(const Var &lhs, const Var &rhs) const override;
    [[nodiscard]] std::string desc(const Var &v) const override;
};

/**
 *  Operation to check whether one Variant is greater-equal than another.
 */
struct GreaterEqual : public Operation
{
    ~GreaterEqual() override = default;

    [[nodiscard]] bool        leftMatchesRight(const Var &lhs, const Var &rhs) const override;
    [[nodiscard]] std::string desc(const Var &v) const override;
};

/**
 *  Operation to check whether one Variant contained in an interval-typed Variant.
 */
struct IsElementOf : public Operation
{
    ~IsElementOf() override = default;

    [[nodiscard]] bool        leftMatchesRight(const Var &elem, const Var &itvl) const override;
    [[nodiscard]] std::string desc(const Var &itvl) const override;
};

/**
 *  Empty Operation that always returns false (used as placeholder for plumbing).
 */
struct PlaceHolderOp : public Operation
{
    ~PlaceHolderOp() override = default;

    [[nodiscard]] bool        leftMatchesRight(const Var &elem, const Var &itvl) const override;
    [[nodiscard]] std::string desc(const Var &itvl) const override;
};

/**
 * Less than operator for intervals in order to enable Intervals as elements
 * of associative containers.
 *
 * @param lhs left-hand-side of comparison
 * @param rhs right-hand-side of comparison
 *
 * @return true if lhs less than rhs, false otherwise
 */
template<typename T1_, typename T2_>
inline bool operator<(const Interval<T1_> &lhs, const Interval<T2_> &rhs)
{
    if(typeid(lhs) == typeid(rhs))
    {
        auto lTp = lhs.traits();
        auto rTp = rhs.traits();

        return ((lTp < rTp) || (lTp == rTp && lhs.low_ < rhs.low_)
                || ((lTp == rTp) && (lhs.low_ == rhs.low_) && (lhs.high_ < rhs.high_)));
    }
    return (std::string(typeid(lhs).name()) < std::string(typeid(rhs).name()));
}

/**
 * Equality operator for intervals in order to enable Intervals as elements
 * of associative containers.
 *
 * @param lhs left-hand-side of comparison
 * @param rhs right-hand-side of comparison
 *
 * @return true if lhs equal to rhs, false otherwise
 */
template<typename T1_, typename T2_>
bool operator==(const Interval<T1_> &lhs, const Interval<T2_> &rhs)
{
    return ((typeid(lhs) == typeid(rhs)) && (static_cast<IntervalType>(lhs) == static_cast<IntervalType>(rhs))
            && (lhs.low_ == rhs.low_) && (lhs.high_ == rhs.high_));
}

/**
 * Inequality operator for intervals in order to enable Intervals as elements
 * of associative containers.
 *
 * @param lhs left-hand-side of comparison
 * @param rhs right-hand-side of comparison
 *
 * @return true if lhs not equal to rhs, false otherwise
 */
template<typename T1_, typename T2_>
bool operator!=(const Interval<T1_> &lhs, const Interval<T2_> &rhs)
{
    return (!(lhs == rhs));
}

/**
 * Generic Out-stream - &lt;&lt; operator for Interval's.
 *
 * @param os the ostream for output
 * @param itvl the interval to output
 *
 * @return the modified stream
 */
template<typename T_>
std::ostream &operator<<(std::ostream &os, const Interval<T_> &itvl)
{
    auto sm = static_cast<Var::StreamMode>(os.iword(Var::xalloc_index));

    if(sm == 0)
        sm = Var::standard;

    bool isLeftInf   = itvl.isLeftInfinite();
    bool isRightInf  = itvl.isRightInfinite();
    bool isLeftOpen  = itvl.isLeftOpen();
    bool isRightOpen = itvl.isRightOpen();
    char left        = (sm & Var::round_open_brace) == Var::round_open_brace && isLeftOpen ? '(' : '[';
    char right       = (sm & Var::round_open_brace) == Var::round_open_brace && isRightOpen ? ')' : ']';
    bool symbolic    = (sm & Var::symbolic_infinity) == Var::symbolic_infinity;

    os << left;

    if(symbolic && isLeftInf)
        os << "-∞";
    else if(isLeftInf)
        os << minVal<T_>();
    else
        os << itvl.low_;

    os << ", ";

    if(symbolic && isRightInf)
        os << "+∞";
    else if(isRightInf)
        os << maxVal<T_>();

    else
        os << itvl.high_;

    os << right;

    return (os);
}

/**
 * Scan a string and convert to the template-type.
 *
 * @param strVal the string-representation
 *
 * @return the scanned value
 */
template<typename T_>
inline T_ scanAs(const VAR_STRING &strVal)
{
    T_                reval;
    std::stringstream ss;

    ss << strVal;
    ss >> reval;

    return (reval);
}

/**
 * Scan a string and convert to the template-type specialised for VAR_BOOL.
 *
 * @param strVal the string-representation
 *
 * @return the scanned boolean value
 * @throws boolstr_error if string cannot be interpreted as boolean
 */
template<>
inline VAR_BOOL scanAs<VAR_BOOL>(const VAR_STRING &strVal)
{
    VAR_BOOL reval = false;

    if(!scanBoolString(strVal, reval))
        throw boolstr_error(strVal);

    return (reval);
}

/**
 * Scan a string and convert to the template-type specialised for string.
 *
 * @param strVal the string-representation
 *
 * @return the string itself
 */
template<>
inline VAR_STRING scanAs<VAR_STRING>(const VAR_STRING &strVal)
{
    return (strVal);
}

/**
 * Scan a string and convert to the template-type specialised for date.
 *
 * @param strVal the string-representation
 *
 * @return the parsed VAR_DATE
 */
template<>
inline VAR_DATE scanAs<VAR_DATE>(const VAR_STRING &strVal)
{
    return (datescan::scanDate(static_cast<std::string>(strVal)));
}

/**
 * Create a Var from any type using the most appropriate constructor.
 *
 * @param val the POD value
 *
 * @return the constructed Var
 */
template<typename T_>
Var asVar(const T_ val)
{
    return (Var(val));
}

/**
 * Create a Var from a string using the template type constructor.
 * @param strVal the string-representation
 *
 * @return the constructed Var
 */
template<typename T_>
inline Var scanAsVar(const VAR_STRING &strVal)
{
    return (asVar(scanAs<T_>(strVal)));
}

/**
 * Convert a Var to its native (underlying) type if possible.
 *
 * @param val the Var value
 *
 * @return the POD value
 */
template<typename T_>
T_ toNative(const Var &val)
{
    if(!isA<T_>(val))
        throw cast_error(val.type().name(), typeid(T_).name());

    return (val.get<T_>());
}

};
// namespace util

#endif  // NS_UTIL_ANYUTIL_H_INCLUDED
