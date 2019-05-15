/*
 * File Name:   anyutil.h
 * Description: boost::any utility functions
 *
 * Copyright (C) 2019 Dieter J Kybelksties
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
 * @date: 2014-01-28
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_ANYUTIL_H_INCLUDED
#define NS_UTIL_ANYUTIL_H_INCLUDED

#include <exception>
#include <iostream>
#include <sstream>
#include <stringutil.h>
#include <boost/any.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time.hpp>
#include <dateutil.h> // gregorian dates/posix time/...
#include <cmath>

namespace util
{
    class Var;
    template<typename T_> bool isA(const Var& v);
    template <typename T_> bool containsT(const Var& lhsInterval, const Var& rhs);
    template<typename T_> bool equalT(const Var& lhs, const Var& rhs);
    template <typename T_> bool lessT(const Var& lhs, const Var& rhs);
    template <typename T_> bool lessEqualT(const Var& lhs, const Var& rhs);
    template <typename T_> bool greaterT(const Var& lhs, const Var& rhs);
    template <typename T_> bool greaterEqualT(const Var& lhs, const Var& rhs);

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
        cast_error(const std::string& from, const std::string& to)
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
         * Construct with the string that cannot be scanned into a boolean value.
         * @param boolstr the failing string
         */
        boolstr_error(const std::string& boolstr)
        : std::logic_error("Cannot scan '" + boolstr + "' into valid bool")
        {
        }
    };

    typedef bool VAR_BOOL; ///< The only boolean type allowed in Var-variants.
    typedef char VAR_CHAR; ///< The only character type allowed in Var-variants.
    typedef long long VAR_INT; ///< The only integer type allowed in Var-variants.
    typedef unsigned long long VAR_UINT; ///< The only unsigned integer type allowed in Var-variants.
    typedef long double VAR_FLOAT; ///< The only floating point type allowed in Var-variants.
    typedef boost::posix_time::ptime VAR_DATE; ///< The only date type allowed in Var-variants.
    typedef std::string VAR_STRING; ///< The only character-string type allowed in Var-variants.

    /**
     * Helper to check that two floating point values are within tolerance of each other.
     * @param v1 value1
     * @param v2 value2
     * @param tolerance a permissable deviation between v1 and v2
     *
     */
    inline bool withinTolerance(VAR_FLOAT v1, VAR_FLOAT v2, VAR_FLOAT tolerance = 1e-18L)
    {
        return std::abs(v1 - v2) < tolerance;
    }

    /**
     * Minimal (range) value for allowable Var-types.
     * @return the minimal value for the template type T_
     */
    template <typename T_>
    inline T_ minVal()
    {
        return std::numeric_limits<T_>::min();
    }

    /**
     * Maximal (range) value for allowable Var-types.
     * @return the maximal value for the template type T_
     */
    template <typename T_>
    inline T_ maxVal()
    {
        return std::numeric_limits<T_>::max();
    }

    /**
     * Minimal (range) value specialised for dates.
     * @return the minimal value for the date
     */
    template <>
    inline VAR_DATE minVal<VAR_DATE>()
    {
        return VAR_DATE(boost::posix_time::min_date_time);
    }

    /**
     * Maximal (range) value specialised for dates.
     * @return the maximal value for the date
     */
    template <>
    inline VAR_DATE maxVal<VAR_DATE>()
    {
        return VAR_DATE(boost::posix_time::max_date_time);
    }

    const static bool BELOW = true;
    const static bool ABOVE = false;

    /**
     *  Numeric/date intervals than can be half- or fully- open or closed.
     */
    template<typename T_>
    struct Interval
    {

        enum type : char
        {
            /**
             * minimum and maximum specified
             */
            closed = 0x00,
            /**
             * From the minimal possible value to a specified maximum
             */
            leftFull = 0x01,
            /**
             * minimum specified to the maximal possible maximal value
             */
            rightFull = 0x02,
            /**
             * the entire possible interval, e.g. [0..max(unsigned long)]
             */
            full = leftFull | rightFull,
            /**
             * include the left corner
             */
            leftInclusive = 0x04,
            /**
             * include the right corner
             */
            rightInclusive = 0x08,
            /**
             * include both corners
             */
            allInclusive = leftInclusive | rightInclusive
        };

        /**
         * Default construct.
         *  Full-range interval including left and right borders.
         */
        Interval()
        : low_()
        , high_()
        , rangeType_(full | allInclusive)
        {
        }

        /**
         * Construct a half-open interval.
         * @param v the value ant the NOT open side
         * @param below if true, then interval will be left-open, else right-open
         * @param inclusivity can be used to include/exclude left or right borders
         */
        Interval(const T_& v, bool below, type inclusivity = allInclusive)
        : low_(below ? T_() : v)
        , high_(below ? v : T_())
        , rangeType_(below ? leftFull : rightFull)
        {
            rangeType_ |= (inclusivity & allInclusive);
        }

        /**
         * Construct a closed interval [v1,v2].
         * @param v1 one of the interval borders
         * @param v2 the other one of the interval borders
         * @param inclusivity can be used to include/exclude left or right borders
         */
        Interval(const T_& v1, const T_& v2, type inclusivity = allInclusive)
        : low_(v1<v2 ? v1 : v2)
        , high_(v1<v2 ? v2 : v1)
        , rangeType_(closed)
        {
            rangeType_ |= (inclusivity & allInclusive);
        }

        /**
         * Check whether the interval contains value v.
         * @param v value to check
         * @return true if v is contained in the interval, false otherwise
         */
        bool contains(const T_& v) const
        {
            return rangeType_ ==
                    (full | allInclusive) ||
                    (isLeftFull() && (isLeftInclusive() ? v <= high_ : v < high_)) ||
                    (isRightFull() && (isRightInclusive() ? v >= low_ : v > low_)) ||
                    ((isLeftInclusive() ? v >= low_ : v > low_) &&
                     (isRightInclusive() ? v <= high_ : v < high_));

        }

        /**
         * Check whether the interval is left full.
         * @return true if so, false otherwise
         */
        bool isLeftFull() const
        {
            return (rangeType_ & leftFull) == leftFull;
        }

        /**
         * Check whether the interval is right full.
         * @return true if so, false otherwise
         */
        bool isRightFull() const
        {
            return (rangeType_ & rightFull) == rightFull;
        }

        /**
         * Check whether the interval includes the left border.
         * @return true if so, false otherwise
         */
        bool isLeftInclusive() const
        {
            return (rangeType_ & leftInclusive) == leftInclusive;
        }

        /**
         * Check whether the interval includes the right border.
         * @return true if so, false otherwise
         */
        bool isRightInclusive() const
        {
            return (rangeType_ & rightInclusive) == rightInclusive;
        }

        /**
         * Check whether the interval is wholly included in this interval.
         * @param rhs the interval to check
         * @return true if the given interval is fully covered in this interval,
         *         false otherwise
         */
        bool isSubIntervalOf(const Interval& rhs) const
        {
            //            return rhs.rangeType_ == full ||
            //                    (rhs.rangeType_ == leftFull && (rangeType_ == leftFull || rangeType_ == closed) && high_ <= rhs.high_) ||
            //                    (rhs.rangeType_ == rightFull && (rangeType_ == rightFull || rangeType_ == closed) && low_ >= rhs.low_) ||
            //                    (rhs.rangeType_ == closed && rangeType_ == closed && low_ >= rhs.low_ && high_ <= rhs.high_)
            //                    ;
            return (isLeftInclusive() && contains(rhs.low_) ||
                    !isLeftInclusive() && rhs.low_ >= low_) &&
                    (isRightInclusive() && contains(rhs.high_) ||
                     !isRightInclusive() && rhs.high_ <= high_);
        }

        template<typename T1_, typename T2_>
        friend inline bool operator<(const Interval<T1_>& lhs, const Interval<T2_>& rhs);

        template<typename T1_, typename T2_>
        friend bool operator==(const Interval<T1_>& lhs, const Interval<T2_>& rhs);

        template<typename TT_>
        friend std::ostream& operator<<(std::ostream& os, const Interval<TT_>& itvl);

        char rangeType_;
        T_ low_; ///< Minimal value
        T_ high_; ///< Maximal value
    };

    typedef Interval<VAR_BOOL> VAR_BOOL_INTERVAL; ///< The only boolean-interval type allowed in Var-variants.
    typedef Interval<VAR_CHAR> VAR_CHAR_INTERVAL; ///< The only character-interval type allowed in Var-variants.
    typedef Interval<VAR_INT> VAR_INT_INTERVAL; ///< The only integer-interval type allowed in Var-variants.
    typedef Interval<VAR_UINT> VAR_UINT_INTERVAL; ///< The only unsigned-integer-interval type allowed in Var-variants.
    typedef Interval<VAR_FLOAT> VAR_FLOAT_INTERVAL; ///< The only floating-point-interval type allowed in Var-variants.
    typedef Interval<VAR_DATE> VAR_DATE_INTERVAL; ///< The only date-interval type allowed in Var-variants.

    /**
     *  Restricted type variant.
     *  Only the longest integer and floating point types, dates and strings
     *  and intervals are allowed.
     */
    class Var
    {
    public:

        enum StreamMode
        {
            quoted_char = 0x0001, ///< enclose characters in single quotes
            hex_char = 0x0002, ///< display characters in hexadecimal representation
            quoted_string = 0x0004, ///< enclose strings in double quotes
            quoted_date = 0x0008, ///< enclose dates in double quotes
            alpha_bool = 0x0010, ///< display booleans as true and false
            short_float = 0x0020, ///< display floating point values in a short format
            long_float = 0x0040, ///< display floating point values in a longer format
            scientific_float = 0x0080, ///< display floating point values in scientific format
            round_open_itvl = 0x0100, ///< indicate open intervals with round braces
            symbolic_open_itvl = 0x0200, ///< indicate open interval with symbolic infinity "oo"

            pure = alpha_bool | hex_char | scientific_float, ///< simple scannable format combination
            standard = alpha_bool |
            short_float |
            round_open_itvl, ///< standard format combination
            safe = quoted_char |
            hex_char |
            quoted_string |
            quoted_date |
            alpha_bool |
            scientific_float ///< more complex combination
        };


        Var(); ///< Construct empty variant.
        Var(const VAR_BOOL& v); ///< Construct boolean variant.
        Var(const VAR_CHAR& v); ///< Construct character variant.
        Var(const VAR_INT& v); ///< Construct signed integer variant.
        Var(const VAR_UINT& v); ///< Construct unsigned integer variant.
        Var(const VAR_FLOAT& v); ///< Construct floating point variant.
        Var(const VAR_DATE& v); ///< Construct date variant.

        /**
         *  Construct an T_-type interval variant.
         */
        template<typename T_>
        Var(const Interval<T_>& itvl)
        : value_(itvl)
        {
        }

        Var(const VAR_STRING& v); ///< Construct string variant.
        Var(const Var& rhs); ///< Copy-construct a variant.
        Var& operator=(const Var& rhs); ///< Assign a variant.

        const std::type_info& type() const; ///< Get the typeid of the contained value.
        bool empty() const; ///< Check whether the variant is empty.
        Var& swap(Var& rhs); ///< Swap this variant with the rhs- variant.
        boost::any value() const; ///< get the contained values as boost::any.

        /**
         *  Check whether the value has the native type.
         */
        template<typename T_>
        friend bool isA(const Var& v)
        {
            return v.type() == typeid (T_);
        }

        /**
         *  Get the value as the native type.
         */
        template <typename T_>
        T_ get() const
        {
            if (!isA<T_ > (*this))
            {
                throw cast_error(type().name(), typeid (T_).name());
            }
            return boost::any_cast<T_>(value_);
        }

        /**
         *  Check whether two vars have the same native type.
         */
        friend bool sameType(const Var& v1, const Var& v2)
        {
            return v1.type() == v2.type();
        }

        /**
         *  Template equalT is needed to ensure that Var's can be put in
         * associative containers.
         *  The other comparison functions only will be used to transcend their
         * native type's operators.
         */
        template<typename T_>
        friend bool equalT(const Var& lhs, const Var& rhs)
        {
            return lhs.type() == rhs.type() &&
                    isA<T_>(lhs) &&
                    lhs.get<T_>() == rhs.get<T_>();
        }

        /**
         *  Template lessT is needed to ensure that Var's can be put in
         *  associative containers.
         *  The other comparison functions only will be used to transcend their
         * native type's operators.
         * Helper template for definition of global operator \<.
         */
        template <typename T_>
        friend bool lessT(const Var& lhs, const Var& rhs)
        {
            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() < rhs.get<T_>());
            return false;
        }

        /**
         *  Helper template for definition of global operator \<=.
         */
        template <typename T_>
        friend bool lessEqualT(const Var& lhs, const Var& rhs)
        {
            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() <= rhs.get<T_>());
            return false;
        }

        /**
         *  Helper template for definition of global operator \>.
         */
        template <typename T_>
        friend bool greaterT(const Var& lhs, const Var& rhs)
        {
            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() > rhs.get<T_>());
            return false;
        }

        /**
         *  Helper template for definition of global operator \<=.
         */
        template <typename T_>
        friend bool greaterEqualT(const Var& lhs, const Var& rhs)
        {
            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() >= rhs.get<T_>());
            return false;
        }

        /**
         *  Helper template for definition of global containment check.
         */
        template <typename T_>
        friend bool containsT(const Var& lhsInterval, const Var& rhs)
        {
            if (lhsInterval.type() != typeid (Interval<T_>) || rhs.type() != typeid (T_))
                return false;
            Interval<T_> itvl = boost::any_cast<Interval<T_> >(lhsInterval.value());
            return itvl.contains(boost::any_cast<T_>(rhs.value()));
        }

        /**
         *  Check whether an Interval - Variant contains a value.
         *  Non-Interval variants are treated as Intervals containing only one value.
         */
        bool contains(const Var& val) const;

        /**
         *  Transcend to native type operator.
         */
        friend bool operator==(const Var& lhs, const Var& rhs);
        /**
         *  Transcend to native type operator.
         */
        friend bool operator<(const Var& lhs, const Var& rhs);
        /**
         *  Transcend to native type operator.
         */
        friend bool operator<=(const Var& lhs, const Var& rhs);
        /**
         *  Transcend to native type operator.
         */
        friend bool operator>(const Var& lhs, const Var& rhs);
        /**
         *  Transcend to native type operator.
         */
        friend bool operator>=(const Var& lhs, const Var& rhs);
        /**
         *  Transcend to native type operator.
         */
        friend std::ostream& operator<<(std::ostream& os, Var::StreamMode sm);
        /**
         *  Transcend to native type operator.
         */
        friend std::ostream& operator<<(std::ostream& os, const Var& v);
        /**
         *  Transcend to native type operator.
         */
        template<typename TT_>
        friend std::ostream& operator<<(std::ostream& os, const Interval<TT_>& itvl);

    private:
        boost::any value_;
        const static int xalloc_index;
    };

    /**
     *  Abstract base class for (match-)operations.
     */
    struct Operation
    {
        /**
         *  Check whether the left hand Var matches the right hand side.
         * This operation is not symmetric:
         *  - operator is taken from right-hand side
         *  - < , <=, >, >= follow natural definition
         */
        virtual bool leftMatchesRight(const Var& lhs, const Var& rhs) const = 0;
        /**
         *  Provide a description of the operation for stream-operators.
         */
        virtual std::string desc(const Var& v) const = 0;
    };

    /**
     *  Operation to check whether two Variant are equal.
     */
    struct Equals : public Operation
    {
        bool leftMatchesRight(const Var& lhs, const Var& rhs) const;
        std::string desc(const Var& v) const;
    };

    /**
     *  Operation to check whether one Variant is less than another.
     */
    struct Less : public Operation
    {
        bool leftMatchesRight(const Var& lhs, const Var& rhs) const;
        std::string desc(const Var& v) const;
    };

    /**
     *  Operation to check whether one Variant is less-equal than another.
     */
    struct LessEqual : public Operation
    {
        bool leftMatchesRight(const Var& lhs, const Var& rhs) const;
        std::string desc(const Var& v) const;
    };

    /**
     *  Operation to check whether one Variant is greater than another.
     */
    struct Greater : public Operation
    {
        bool leftMatchesRight(const Var& lhs, const Var& rhs) const;
        std::string desc(const Var& v) const;
    };

    /**
     *  Operation to check whether one Variant is greater-equal than another.
     */
    struct GreaterEqual : public Operation
    {
        bool leftMatchesRight(const Var& lhs, const Var& rhs) const;
        std::string desc(const Var& v) const;
    };

    /**
     *  Operation to check whether one Variant contained in an interval-typed Variant.
     */
    struct IsElementOf : public Operation
    {
        bool leftMatchesRight(const Var& elem, const Var& itvl) const;
        std::string desc(const Var& itvl) const;
    };

    /**
     *  Empty Operation that always returns false (used as placeholder for plumbing).
     */
    struct PlaceHolderOp : public Operation
    {
        bool leftMatchesRight(const Var& elem, const Var& itvl) const;
        std::string desc(const Var& itvl) const;
    };

    /**
     *  Enable Intervals as elements of associative containers.
     */
    template<typename T1_, typename T2_>
    bool operator<(const Interval<T1_>& lhs, const Interval<T2_>& rhs)
    {
        if (typeid (lhs) == typeid (rhs))
        {
            auto lTp = lhs.rangeType_;
            auto rTp = rhs.rangeType_;
            return lTp < rTp ||
                    (lTp == rTp && lhs.low_ < rhs.low_) ||
                    (lTp == rTp && lhs.low_ == rhs.low_ && lhs.high_ < rhs.high_);
        }
        return std::string(typeid (lhs).name()) < std::string(typeid (rhs).name());
    }

    /**
     * Enable Intervals as elements of associative containers.
     */
    template<typename T1_, typename T2_>
    bool operator==(const Interval<T1_>& lhs, const Interval<T2_>& rhs)
    {
        return typeid (lhs) == typeid (rhs) &&
                lhs.rangeType_ == rhs.rangeType_ &&
                lhs.low_ == rhs.low_ &&
                lhs.high_ == rhs.high_;
    }

    /**
     * Enable Intervals as elements of associative containers.
     */
    template<typename T1_, typename T2_>
    bool operator!=(const Interval<T1_>& lhs, const Interval<T2_>& rhs)
    {
        return !(lhs == rhs);
    }

    /**
     * Generic ostream - &lt;&lt; operator for Interval's.
     */
    template<typename T_>
    std::ostream& operator<<(std::ostream& os, const Interval<T_>& itvl)
    {
        Var::StreamMode sm = (Var::StreamMode)os.iword(Var::xalloc_index);
        if (sm == 0)
            sm = Var::standard;
        bool leftFull = itvl.isLeftFull();
        bool rightFull = itvl.isRightFull();
        bool leftOpen = !itvl.isLeftInclusive();
        bool rightOpen = !itvl.isRightInclusive();

        char left = (sm & Var::round_open_itvl) == Var::round_open_itvl && leftOpen ? '(' : '[';
        char right = (sm & Var::round_open_itvl) == Var::round_open_itvl && rightOpen ? ')' : ']';

        bool symbolic = (sm & Var::symbolic_open_itvl) == Var::symbolic_open_itvl;
        os << left;
        if (symbolic && leftFull)
            os << "-oo";
        else if (leftFull)
            os << minVal<T_> ();
        else
            os << itvl.low_;
        os << ", ";
        if (symbolic && rightFull)
            os << "+oo";
        else if (rightFull)
            os << maxVal<T_>();
        else
            os << itvl.high_;

        os << right;
        return os;
    }

    /**
     *  Scan a string and convert to the template-type.
     */
    template <typename T_>
    inline T_ scanAs(const VAR_STRING& strVal)
    {
        T_ reval;
        std::stringstream ss;
        ss << strVal;
        ss >> reval;
        return reval;
    }

    /**
     * scan a string and convert to the template-type specialised for bool.
     */
    template <>
    inline VAR_BOOL scanAs<VAR_BOOL>(const VAR_STRING& strVal)
    {
        VAR_BOOL reval = false;
        if (!scanBoolString(strVal, reval))
        {
            throw boolstr_error(strVal);
        }
        return reval;
    }

    /**
     * scan a string and convert to the template-type specialised for string.
     */
    template <>
    inline VAR_STRING scanAs<VAR_STRING>(const VAR_STRING& strVal)
    {
        return strVal;
    }

    /**
     * Scan a string and convert to the template-type specialised for date.
     */
    template <>
    inline VAR_DATE scanAs<VAR_DATE>(const VAR_STRING& strVal)
    {
        return datescan::scanDate((std::string)strVal);
    }

    /**
     * Create a Var from any type using the most appropriate constructor.
     */
    template <typename T_>
    Var asVar(const T_ v)
    {
        return Var(v);
    }

    /**
     * Create a Var from a string using the template type constructor.
     */
    template<typename T_>
    inline Var scanAsVar(const VAR_STRING& strVal)
    {
        return asVar(scanAs<T_>(strVal));
    }

    /**
     * Convert a Var to its native (underlying) type if possible.
     */
    template<typename T_>
    T_ toNative(const Var& v)
    {
        if (!isA<T_>(v))
        {
            throw cast_error(v.type().name(), typeid (T_).name());
        }
        return v.get<T_>();
    }


}; // namespace util

#endif // NS_UTIL_ANYUTIL_H_INCLUDED

