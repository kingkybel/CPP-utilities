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
#include <limits>
#include <initializer_list>

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
        return std::numeric_limits<T_>::lowest();
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

    enum class borderType
    {
        /** a non-infinite minimum specified */
        finiteMin = (unsigned char) 0x01,
        /** a non-infinite maximum specified */
        finiteMax = (unsigned char) 0x02,
        /** both minimum and maximum are finite */
        finite = finiteMin | finiteMax,
        /** include the left corner */
        leftClosed = (unsigned char) 0x04,
        /** include the right corner */
        rightClosed = (unsigned char) 0x08,
        /** include both corners */
        closed = leftClosed | rightClosed,

        // complements
        infiniteMin = ~finiteMin,
        infiniteMax = ~finiteMax,
        infinite = infiniteMin | infiniteMax,
        leftOpen = ~leftClosed,
        rightOpen = ~rightClosed,
        open = leftOpen | rightOpen
    };

    inline unsigned char operator&(const borderType& lhs, const borderType& rhs)
    {
        return (unsigned char) lhs & (unsigned char) lhs;
    }

    inline unsigned char operator|(const borderType& lhs, const borderType& rhs)
    {
        return (unsigned char) lhs | (unsigned char) lhs;
    }

    inline unsigned char operator<(const borderType& lhs, const borderType& rhs)
    {
        return (unsigned char) lhs < (unsigned char) lhs;
    }

    static const borderType finiteMin = borderType::finiteMin;
    static const borderType finiteMax = borderType::finiteMax;
    static const borderType finite = borderType::finite;
    static const borderType leftClosed = borderType::leftClosed;
    static const borderType rightClosed = borderType::rightClosed;
    static const borderType closed = borderType::closed;

    static const borderType infiniteMin = borderType::infiniteMin;
    static const borderType infiniteMax = borderType::infiniteMax;
    static const borderType infinite = borderType::infinite;
    static const borderType leftOpen = borderType::leftOpen;
    static const borderType rightOpen = borderType::rightOpen;
    static const borderType open = borderType::open;

    struct IntervalType
    {

        void setFlag(const borderType& bt)
        {
            if (bt == finiteMin ||
                bt == finiteMax ||
                bt == finite ||
                bt == leftClosed ||
                bt == rightClosed ||
                bt == closed)
                traits_ |= (unsigned char) bt;
            else
                traits_ &= (unsigned char) bt;
        }

        IntervalType(std::initializer_list<borderType> l =
                     std::initializer_list<borderType>({closed, finite}))
        : traits_(0)
        {
            for (auto it = l.begin(); it != l.end(); it++)
            {
                setFlag(*it);
            }
        }

        bool isLeftFinite() const
        {
            return (traits_ & (unsigned char) finiteMin) == (unsigned char) finiteMin;
        }

        bool isLeftInfinite() const
        {
            return !isLeftFinite();
        }

        bool isRightFinite() const
        {
            return (traits_ & (unsigned char) finiteMax) == (unsigned char) finiteMax;
        }

        bool isRightInfinite() const
        {
            return !isRightFinite();
        }

        bool isInfinite() const
        {
            return isLeftInfinite() && isRightInfinite();
        }

        bool isFinite() const
        {
            return !isInfinite();
        }

        bool isLeftClosed() const
        {
            return (traits_ & (unsigned char) leftClosed) == (unsigned char) leftClosed;
        }

        bool isRightClosed() const
        {
            return (traits_ & (unsigned char) rightClosed) == (unsigned char) rightClosed;
        }

        bool isClosed() const
        {
            return isLeftClosed() && isRightClosed();
        }

        bool isLeftOpen() const
        {
            return !isLeftClosed();
        }

        bool isRightOpen() const
        {
            return !isRightClosed();
        }

        bool isOpen() const
        {
            return !isClosed();
        }

        friend bool operator==(const IntervalType& lhs, const IntervalType& rhs)
        {
            return lhs.traits_ == rhs.traits_;
        }

        friend bool operator<(const IntervalType& lhs, const IntervalType& rhs)
        {
            return lhs.traits_ < rhs.traits_;
        }
    protected:
        unsigned char traits_;
    };

    /**
     *  Numeric/date intervals than can be half- or fully- open or closed.
     */
    template<typename T_>
    struct Interval : public IntervalType
    {

        /**
         * Default construct a left-or right- finite interval interval.
         * tags finiteMin and infiniteMax in this context mean the same: the
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
        Interval(const T_& v = minVal<T_>(),
                 std::initializer_list<borderType> l =
                 std::initializer_list<borderType>({finiteMin, leftClosed}))
        : IntervalType()
        {
            bool leftBoundaryIsMinimum = v == minVal<T_>();
            if (leftBoundaryIsMinimum) // use whole domain
            {
                low_ = minVal<T_>();
                high_ = maxVal<T_>();
            }
            else
            {
                bool foundInfiniteMin = false;
                bool foundInfiniteMax = false;
                bool foundLeftClosed = false;
                bool foundRightClosed = false;
                bool leftOpenClosedSpecified = false;
                bool rightOpenClosedSpecified = false;

                unsigned char newTraits_ = 0x00;
                for (auto it = l.begin(); it != l.end(); it++)
                {
                    switch (*it)
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
                            foundLeftClosed = true;
                            leftOpenClosedSpecified = true;
                            break;
                        case rightClosed:
                            foundRightClosed = true;
                            rightOpenClosedSpecified = true;
                        case closed:
                            foundLeftClosed = true;
                            foundRightClosed = true;
                            rightOpenClosedSpecified = true;
                            leftOpenClosedSpecified = true;
                            break;
                        case leftOpen:
                            foundLeftClosed = false;
                            leftOpenClosedSpecified = true;
                            break;
                        case rightOpen:
                            foundRightClosed = false;
                            rightOpenClosedSpecified = true;
                        case open:
                            foundLeftClosed = false;
                            foundRightClosed = false;
                            leftOpenClosedSpecified = true;
                            break;
                    }
                }

                traits_ = 0x00;
                if ((!foundInfiniteMax && !foundInfiniteMin) || foundInfiniteMax)
                {
                    setFlag(finiteMin);
                    setFlag(infiniteMax);
                }
                else
                {
                    setFlag(infiniteMin);
                    setFlag(finiteMax);
                }
                if (foundLeftClosed || !leftOpenClosedSpecified)
                    setFlag(leftClosed);
                if (foundRightClosed || !rightOpenClosedSpecified)
                    setFlag(rightClosed);

                low_ = (isLeftFinite() ? v : minVal<T_>());
                high_ = (isLeftFinite() ? maxVal<T_>() : v);
            }
        }

        /**
         * Construct a closed interval [v1,v2].
         * @param v1 one of the interval borders
         * @param v2 the other one of the interval borders
         * @param inclusivity can be used to include/exclude left or right borders
         */
        Interval(const T_& v1,
                 const T_& v2,
                 std::initializer_list<borderType> l =
                 std::initializer_list<borderType>({closed, finite}))
        : IntervalType(l)
        , low_(v1 < v2 ? v1 : v2)
        , high_(v1 < v2 ? v2 : v1)
        {
        }

        T_ left() const
        {

            return Interval<T_>::isLeftInfinite() ? minVal<T_>() : low_;
        }

        T_ right() const
        {

            return Interval<T_>::isRightInfinite() ? maxVal<T_>() : high_;
        }

        /**
         * Check whether the interval contains value v.
         * @param v value to check
         * @return true if v is contained in the interval, false otherwise
         */
        bool contains(const T_ & v) const
        {
            /* [-∞,-∞],  (-∞,-∞)                              */
            /* [-∞,high], [-∞,high), (-∞,high], (-∞,high)     */
            /* [low,+∞],  [low,+∞),  (low,+∞],  (low,+∞)      */
            /* [low,high], [low,high), (low,high], (low,high) */
            return (isLeftClosed() ? low_ <= v : low_ < v) &&
                (isRightClosed() ? v <= high_ : v < high_);

        }

        /**
         * Check whether the interval is wholly included in this interval.
         * @param rhs the interval to check
         * @return true if the given interval is fully covered in this interval,
         *         false otherwise
         */
        bool isSubIntervalOf(const Interval & rhs) const
        {
            //            std::cout << std::boolalpha
            //                << rhs
            //                << "rhs.contains("
            //                << low_
            //                << ")="
            //                << rhs.contains(low_)
            //                << " rhs.contains("
            //                << high_
            //                << ")="
            //                << rhs.contains(high_)
            //                << std::endl;

            return rhs.contains(low_) && rhs.contains(high_);
        }

        std::string verboseAsString() const
        {

            std::string reval = "";
            reval += (isLeftClosed()) ? "leftClosed[ " : "leftOpen (";
            reval += isLeftInfinite() ? "leftInf '" + asString(minVal<T_>()) + "' " : "leftMin '" + asString(low_) + "' ";
            reval += isRightInfinite() ? "rightInf '-" + asString(maxVal<T_>()) + "' " : "rightMax '" + asString(high_) + "' ";
            reval += (isRightClosed()) ? "] rightClosed " : ") rightOpen";
            return reval;
        }
        template<typename T1_, typename T2_>
        friend inline bool operator<(const Interval<T1_>& lhs, const Interval<T2_>& rhs);

        template<typename T1_, typename T2_>
        friend bool operator==(const Interval<T1_>& lhs, const Interval<T2_>& rhs);

        template<typename TT_>
        friend std::ostream& operator<<(std::ostream& os, const Interval<TT_>& itvl);
    private:
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

        enum StreamMode : int
        {
            reset = 0x0000, ///< reset the stream configuration to empty
            quoted_char = 0x0001, ///< enclose characters in single quotes
            hex_char = 0x0002, ///< display characters in hexadecimal representation
            quoted_string = 0x0004, ///< enclose strings in double quotes
            quoted_date = 0x0008, ///< enclose dates in double quotes
            alpha_bool = 0x0010, ///< display booleans as true and false
            short_float = 0x0020, ///< display floating point values in a short format
            long_float = 0x0040, ///< display floating point values in a longer format
            scientific_float = 0x0080, ///< display floating point values in scientific format
            round_open_brace = 0x0100, ///< indicate open intervals with round braces
            symbolic_infinity = 0x0200, ///< indicate full interval with symbolic infinity "oo"

            pure = alpha_bool | hex_char | scientific_float, ///< simple scannable format combination
            standard = alpha_bool | short_float | round_open_brace, ///< standard format combination
            safe = quoted_char | hex_char | quoted_string | quoted_date | alpha_bool | scientific_float ///< more complex combination
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
         * Check whether two vars have the same native type.
         */
        friend bool sameType(const Var& v1, const Var& v2)
        {

            return v1.type() == v2.type();
        }

        /**
         * Template equalT is needed to ensure that Var's can be put in
         * associative containers.
         * The other comparison functions only will be used to transcend their
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
         * Template lessT is needed to ensure that Var's can be put in
         * associative containers.
         * The other comparison functions only will be used to transcend their
         * native type's operators.
         * Helper template for definition of global operator &lt;.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is less than rhs, false otherwise
         */
        template <typename T_>
        friend bool lessT(const Var& lhs, const Var& rhs)
        {

            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() < rhs.get<T_>());
            return false;
        }

        /**
         * Helper template for definition of global operator &le;.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is less/equal than rhs, false otherwise
         */
        template <typename T_>
        friend bool lessEqualT(const Var& lhs, const Var& rhs)
        {

            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() <= rhs.get<T_>());
            return false;
        }

        /**
         * Helper template for definition of global operator &gt;.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is greater than rhs, false otherwise
         */
        template <typename T_>
        friend bool greaterT(const Var& lhs, const Var& rhs)
        {

            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() > rhs.get<T_>());
            return false;
        }

        /**
         * Helper template for definition of global operator &ge;.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is greater/equal than rhs, false otherwise
         */
        template <typename T_>
        friend bool greaterEqualT(const Var& lhs, const Var& rhs)
        {

            if (lhs.type() == typeid (T_) && rhs.type() == typeid (T_))
                return (lhs.get<T_>() >= rhs.get<T_>());
            return false;
        }

        /**
         * Helper template for definition of global containment check.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs contains rhs, false otherwise
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
         * Check whether an Interval - Variant contains a value.
         * Non-Interval variants are treated as Intervals containing only one value.
         * @param val value to check
         * @return if this contains val, false otherwise
         */
        bool contains(const Var& val) const;

        /**
         * Variant equality.
         * Transcends to native type operator.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is equal to rhs, false otherwise
         */
        friend bool operator==(const Var& lhs, const Var& rhs);

        /**
         * Variant less than.
         * Transcends to native type operator.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is less than rhs, false otherwise
         */
        friend bool operator<(const Var& lhs, const Var& rhs);

        /**
         * Variant less/equality.
         * Transcends to native type operator.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is less/equal to rhs, false otherwise
         */
        friend bool operator<=(const Var& lhs, const Var& rhs);

        /**
         * Variant greater.
         * Transcends to native type operator.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is greater than rhs, false otherwise
         */
        friend bool operator>(const Var& lhs, const Var& rhs);

        /**
         * Variant greater/equality.
         * Transcends to native type operator.
         * @param lhs left-hand-side of the comparison
         * @param rhs right-hand-side of the comparison
         * @return if lhs is greater/equal than rhs, false otherwise
         */
        friend bool operator>=(const Var& lhs, const Var& rhs);

        /**
         * Out-stream operator for variant stream modifiers.
         * @param os the ostream for output
         * @param sm the modifier
         * @return the modified stream
         */
        friend std::ostream& operator<<(std::ostream& os, Var::StreamMode sm);

        /**
         * Out-stream operator for variants.
         * @param os the ostream for output
         * @param v the variant to output
         * @return the modified stream
         */
        friend std::ostream& operator<<(std::ostream& os, const Var& v);

        /**
         * Out-stream operator for intervals.
         * @param os the ostream for output
         * @param itvl the interval to output
         * @return the modified stream
         */
        template<typename TT_>
        friend std::ostream& operator<<(std::ostream& os, const Interval<TT_>& itvl);

    private:
        boost::any value_;
        const static int xalloc_index;
    };

    /**
     * Abstract base class for (match-)operations.
     */
    struct Operation
    {
        /**
         * Check whether the left hand Var matches the right hand side.
         * This operation is not symmetric:
         * <ul>
         * <li>operator is taken from right-hand side</li>
         * <li>&lt; , &le;, &gt;, &ge; follow natural definition</li>
         * </ul>
         * @param lhs left-hand-side of comparison
         * @param rhs right-hand-side of comparison
         * @return true if left matches right, false otherwise
         */
        virtual bool leftMatchesRight(const Var& lhs, const Var& rhs) const = 0;

        /**
         * Provide a description of the operation for stream-operators.
         * @param v the variant to get a description for
         * @return a string description of v
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
     * Less than operator for intervals in order to enable Intervals as elements
     * of associative containers.
     *
     * @param lhs left-hand-side of comparison
     * @param rhs right-hand-side of comparison
     * @return true if lhs less than rhs, false otherwise
     */
    template<typename T1_, typename T2_>
    bool operator<(const Interval<T1_>& lhs, const Interval<T2_>& rhs)
    {
        if (typeid (lhs) == typeid (rhs))
        {

            auto lTp = lhs.traits_;
            auto rTp = rhs.traits_;
            return lTp < rTp ||
                (lTp == rTp && lhs.low_ < rhs.low_) ||
                (lTp == rTp && lhs.low_ == rhs.low_ && lhs.high_ < rhs.high_);
        }
        return std::string(typeid (lhs).name()) < std::string(typeid (rhs).name());
    }

    /**
     * Equality operator for intervals in order to enable Intervals as elements
     * of associative containers.
     *
     * @param lhs left-hand-side of comparison
     * @param rhs right-hand-side of comparison
     * @return true if lhs equal to rhs, false otherwise
     */
    template<typename T1_, typename T2_>
    bool operator==(const Interval<T1_>& lhs, const Interval<T2_>& rhs)
    {

        return typeid (lhs) == typeid (rhs) &&
            (IntervalType) lhs == (IntervalType) rhs &&
            lhs.low_ == rhs.low_ &&
            lhs.high_ == rhs.high_;
    }

    /**
     * Inequality operator for intervals in order to enable Intervals as elements
     * of associative containers.
     *
     * @param lhs left-hand-side of comparison
     * @param rhs right-hand-side of comparison
     * @return true if lhs not equal to rhs, false otherwise
     */
    template<typename T1_, typename T2_>
    bool operator!=(const Interval<T1_>& lhs, const Interval<T2_>& rhs)
    {

        return !(lhs == rhs);
    }

    /**
     * Generic Out-stream - &lt;&lt; operator for Interval's.
     * @param os the ostream for output
     * @param itvl the interval to output
     * @return the modified stream
     */
    template<typename T_>
    std::ostream& operator<<(std::ostream& os, const Interval<T_>& itvl)
    {
        Var::StreamMode sm = (Var::StreamMode)os.iword(Var::xalloc_index);
        if (sm == 0)
            sm = Var::standard;
        bool leftInf = itvl.isLeftInfinite();
        bool rightInf = itvl.isRightInfinite();
        bool leftOpen = itvl.isLeftOpen();
        bool rightOpen = itvl.isRightOpen();

        char left = (sm & Var::round_open_brace) == Var::round_open_brace && leftOpen ? '(' : '[';
        char right = (sm & Var::round_open_brace) == Var::round_open_brace && rightOpen ? ')' : ']';

        bool symbolic = (sm & Var::symbolic_infinity) == Var::symbolic_infinity;
        os << left;
        if (symbolic && leftInf)
            os << "-∞";
        else if (leftInf)
            os << minVal<T_> ();
        else
            os << itvl.low_;
        os << ", ";
        if (symbolic && rightInf)
            os << "+∞";
        else if (rightInf)
            os << maxVal<T_>();

        else
            os << itvl.high_;

        os << right;
        return os;
    }

    /**
     * Scan a string and convert to the template-type.
     *
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

