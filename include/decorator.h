/*
 * File Name:   type_bracket_map.h
 * Description: Brackets mapped per type for streaming output
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

#ifndef NS_UTIL_DECORATOR_H_INCLUDED
#define NS_UTIL_DECORATOR_H_INCLUDED

#include "brackets.h"
#include "traits.h"
//#define DO_TRACE_
#include "traceutil.h"

#include <algorithm>
#include <deque>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace util
{
// forward declarations
template<typename T_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::vector<T_, Alloc_>       &vec);

template<typename T_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::deque<T_, Alloc_>        &dblEndQueue);

template<typename Key, typename Compare, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::set<Key, Compare, Alloc> &sortedSet);

template<typename Value, typename Hash, typename Pred, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_>                &os,
                                                       const std::unordered_set<Value, Hash, Pred, Alloc> &unordSet);

template<typename T1_, typename T2_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::pair<T1_, T2_>           &pair1st2nd);

template<typename Key, typename Value, typename Hash, typename Pred, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &
 operator<<(std::basic_ostream<CharT_, Traits_> &os, const std::unordered_map<Key, Value, Hash, Pred, Alloc> &unordMap);

template<typename Key, typename Value, typename Compare_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_>          &os,
                                                       const std::map<Key, Value, Compare_, Alloc_> &sortedMap);
//////////

/**
 * @brief Convert a string type into another.
 *
 * @tparam CharT1_ char type of the result string
 * @tparam TraitsT1_ traits of the result string
 * @tparam CharT2_ char type of the original string
 * @tparam TraitsT2_ traits of the original string
 * @param from the original string
 * @return std::basic_string<CharT1_, TraitsT1_> the converted string
 */
template<typename CharT1_   = wchar_t,
         typename TraitsT1_ = std::char_traits<CharT1_>,
         typename CharT2_   = char,
         typename TraitsT2_ = std::char_traits<CharT2_>>
std::basic_string<CharT1_, TraitsT1_> convert(const std::basic_string<CharT2_, TraitsT2_> &from)
{
    static_assert(std::is_integral_v<CharT1_>, "Type CharT1_ must be integral.");
    static_assert(std::is_integral_v<CharT2_>, "Type CharT2_ must be integral.");

    std::basic_string<CharT1_, TraitsT1_> to;

    std::transform(from.begin(),
                   from.end(),
                   std::back_inserter(to),
                   [](const auto &c) { return (static_cast<CharT2_>(c)); });

    return (to);
}

/**
 * @brief Struct to encapsulate format configuration for floating point values.
 *
 * @tparam CharT_ character type of the stream the format will be used on
 */
template<typename CharT_>
struct floatFmt
{
    /**
     * @brief Construct a new float format object
     *
     * @param isScientific set float display to scientific
     */
    explicit floatFmt(bool isScientific = false) : isScientific_(isScientific), isValid_(isScientific)
    {
    }

    /**
     * @brief Construct a new float format object with explicit configuration values
     *
     * @param width width used for floats
     * @param precision (post-comma-) precision of floats
     * @param fill a fill character, defaults to '0'
     * @param isFixed display as fixed float
     */
    floatFmt(size_t width, size_t precision = 5, char fill = '0', bool isFixed = false)
    : width_(width)
    , precision_(precision)
    , fill_(static_cast<CharT_>(char(fill)))
    , isFixed_(isFixed)
    , isScientific_(false)
    {
    }

    /**
     * @brief Check whether the forlat is valid.
     *
     * @return true, if so, false otherwise
     */
    bool isValid() const
    {
        return (isValid_);
    }

    bool   isValid_      = true;
    size_t width_        = 0UL;
    size_t precision_    = 0UL;
    CharT_ fill_         = static_cast<CharT_>(char('0'));
    bool   isFixed_      = false;
    bool   isScientific_ = true;
};

/**
 * @brief enumeration of int formats
 */
enum class intFmt
{
    print_char,
    decimal,
    hexadecimal,
    octal,
    ignore
};

/**
 * @brief Apply the given float-format to the output-stream
 *
 * @tparam CharT_ char-type of the stream
 * @tparam TraitsT_ traits of the stream
 * @param os output stream
 * @param fmt float format to be applied
 * @return std::basic_ostream<CharT_, TraitsT_>& the modified stream
 */
template<typename CharT_, typename TraitsT_>
inline std::basic_ostream<CharT_, TraitsT_> &operator<<(std::basic_ostream<CharT_, TraitsT_> &os,
                                                        const floatFmt<CharT_>               &fmt)
{
    if(fmt.isScientific_)
    {
        os << std::scientific;
    }
    else
    {
        os.fill(fmt.fill_);
        os.width(fmt.width_);

        if(fmt.isFixed_)
            os << std::fixed;
    }

    return (os);
}

/**
 * @brief This template class facilitates to configure how objects are streamed on output streams.
 *
 * @tparam CharT_ char-type
 * @tparam TraitsT_ string-traits
 */
template<typename CharT_ = char, typename TraitsT_ = std::char_traits<CharT_>>
class decorator
{
    public:
    using StringT_     = std::basic_string<CharT_, TraitsT_>;
    using BracketsType = util::Brackets;

    template<typename T_>
    StringT_ typeString()
    {
        return (convert<CharT_, TraitsT_>(std::string(typeid(T_).name())));
    }

    const static floatFmt<CharT_> invalidFmt;
    const static floatFmt<CharT_> scientificFmt;
    const static floatFmt<CharT_> shortFloatFmt;
    const static floatFmt<CharT_> longFloatFmt;

    private:
    static decorator theInstance;

    // keys of maps are strings, not the output string type StringT_
    using BracketMapType     = std::map<std::string, BracketsType>;
    using IntFormatMapType   = std::map<std::string, intFmt>;
    using FloatFormatMapType = std::map<std::string, floatFmt<CharT_>>;

    BracketMapType     type2brackets_;
    IntFormatMapType   intType2format_;
    FloatFormatMapType floatType2format_;

    bool             alphaBool_     = true;
    floatFmt<CharT_> floatFmt_      = scientificFmt;
    floatFmt<CharT_> doubleFmt_     = scientificFmt;
    floatFmt<CharT_> longDoubleFmt_ = scientificFmt;

    decorator() = default;

    public:
    /**
     * Check whether the bracket configuration is empty.
     *
     * @return {@code true} if so, {@code false} otherwise
     */
    bool empty() const
    {
        return (type2brackets_.empty());
    }

    /**
     * @brief Debug function to display bracket information
     * 
     * @return std::string string describing the currently configured brackets
     */
    std::string showConfig() const
    {
        std::stringstream ss;
        ss << "--------------------" << std::endl;
        ss << "-- " << this << " --" << std::endl;
        for(const auto &kv: type2brackets_)
            ss << kv.first << " -> ('" << convert<char>(kv.second.left()) << "', '" << convert<char>(kv.second.inner())
               << "', '" << convert<char>(kv.second.right()) << "')" << std::endl;
        ss << "------------------" << std::endl;
        ss << std::endl;
        return ss.str();
    }

    /**
     * @brief Reset the bracket configuration.
     */
    void clearBrackets()
    {
        type2brackets_.clear();
    }

    /**
     * @brief Reset the int format configuration.
     */
    void clearIntFormat()
    {
        intType2format_.clear();
    }

    /**
     * @brief Reset the float format configuration.
     */
    void clearFloatFormat()
    {
        floatType2format_.clear();
    }

    /**
     * Reset the contents.
     */
    void clear()
    {
        clearBrackets();
        clearIntFormat();
        clearFloatFormat();
    }

    /**
     * @brief Initialize the bracket configuration.
     *
     * @return true always
     */
    bool initializeBrackets()
    {
        clearBrackets();

        clear();
        for(const auto &kv: util::DEFAULT_BRACKETS)
            setBracketForKey(kv.first, BracketsType(kv.first));

        return (true);
    }

    /**
     * @brief Initialize int format configuration.
     *
     * @return true always
     */
    bool initializeIntFormat()
    {
        clearIntFormat();

        setIntFmt<char>(intFmt::print_char);
        setIntFmt<wchar_t>(intFmt::print_char);
        setIntFmt<int8_t>(intFmt::decimal);
        setIntFmt<int16_t>(intFmt::decimal);
        setIntFmt<int32_t>(intFmt::decimal);
        setIntFmt<int64_t>(intFmt::decimal);
        setIntFmt<uint8_t>(intFmt::decimal);
        setIntFmt<uint16_t>(intFmt::decimal);
        setIntFmt<uint32_t>(intFmt::decimal);
        setIntFmt<uint64_t>(intFmt::decimal);

        return (true);
    }

    /**
     * @brief Initialize float format configuration.
     *
     * @return true always
     */
    bool initializeFloatFormat()
    {
        clearFloatFormat();

        setFloatFmt<float>(scientificFmt);
        setFloatFmt<double>(scientificFmt);
        setFloatFmt<long double>(scientificFmt);

        return (true);
    }

    /**
     * Initialize configuration.
     */
    bool initialize()
    {
        return (initializeBrackets() && initializeIntFormat() && initializeFloatFormat());
    }

    /**
     * @brief Retrieve the singleton instance.
     *
     * @return the instance
     */
    static decorator &instance()
    {
        static bool initialized = decorator::theInstance.initialize();
        decorator::theInstance.showConfig();
        return (decorator::theInstance);
    }

    /**
     * @brief Add a bracket identified by a unique string.
     *
     * @param key unique string
     * @param bracket the bracket to set
     */
    void setBracketForKey(const std::string &key, const BracketsType &bracket)
    {
        type2brackets_[key] = bracket;
    }

    /**
     * @brief Set the Bracket For a key 
     * 
     * @param key string key
     * @param bracket the bracket to set
     */
    void setBracketForKey(const std::string_view &key, const BracketsType &bracket)
    {
        type2brackets_[std::string{key}] = bracket;
    }

    /**
     * @brief Set the Bracket For Key object identified by a unique string.
     *
     * @param key string representing the key
     * @param left left bracket
     * @param inner inner bracket
     * @param right right bracket
     */
    void setBracketForKey(const std::string &key,
                          const std::string &left,
                          const std::string &inner,
                          const std::string &right)
    {
        BracketsType bracket{key, left, inner, right};

        setBracketForKey(key, bracket);
    }

    /**
     * @brief Set the Bracket For Key object identified by a unique string.
     *
     * @param key string representing the key
     * @param left left bracket
     * @param inner inner bracket
     * @param right right bracket
     */
    void setBracketForKey(const std::string_view &key,
                          const std::string      &left,
                          const std::string      &inner,
                          const std::string      &right)
    {
        BracketsType bracket{std::string{key}, left, inner, right};

        setBracketForKey(std::string{key}, bracket);
    }

    /**
     * @brief Set the Bracket object identified by a unique string.
     *
     * @tparam T type of the object
     * @param object the typeid of this object is used as key
     * @param left left bracket
     * @param inner inner bracket
     * @param right right bracket
     */
    template<typename T>
    void
     setBracketForObject(const T &object, const std::string &left, const std::string &inner, const std::string &right)
    {
        const std::string type = typeString<T>();
        BracketsType      bracket{type, left, inner, right};

        setBracketForKey(type, bracket);
    }

    /**
     * @brief Get the Bracket object
     *
     * @tparam T type of the object
     * @param object the typeid of this object is used as key, if it can be found
     * @param defaultKey otherwise the default-key
     * @return BracketsType the bracket for the object
     */
    template<typename T>
    BracketsType getBracket(const T &object, const std::string_view &defaultKey)
    {
        auto found = type2brackets_.find(typeString<T>());

        if(found == type2brackets_.end())
        {
            found = type2brackets_.find(std::string{defaultKey});

            return ((found == type2brackets_.end()) ? BracketsType(util::BracketKey::NONE) : found->second);
        }

        return (found->second);
    }

    /**
     * @brief Get the Bracket object identified by a key.
     *
     * @param key the key to look for
     * @return BracketsType  bracket for a key, if key is not defined then return the default bracket
     */
    BracketsType getBracket(const std::string_view &key)
    {
        auto found = type2brackets_.find(std::string{key});
        return ((found == type2brackets_.end()) ? BracketsType(util::BracketKey::NONE) : found->second);
    }

    /**
     * @brief Get the Alpha Bool configuration
     *
     * @return true, if booleans should be displayed as strings, false otherwise
     */
    bool getBoolAlpha() const
    {
        return (alphaBool_);
    }

    /**
     * @brief Set the format relating to an int-type.
     *
     * @tparam IntT_ int-type
     * @param fmt the format to set
     */
    template<typename IntT_>
    void setIntFmt(intFmt fmt)
    {
        static_assert(std::is_integral_v<IntT_>, "Type IntT_ must be integral.");
        intType2format_[typeString<IntT_>()] = fmt;
    }

    /**
     * @brief Get the format relating to an int-type.
     *
     * @tparam IntT_ int-type
     * @return the format set for the int-type
     */
    template<typename IntT_>
    intFmt getIntFmt()
    {
        static_assert(std::is_integral_v<IntT_>, "Type IntT_ must be integral.");
        auto found = intType2format_.find(typeString<IntT_>());

        if(found == intType2format_.end())
            return (intFmt::ignore);

        return (found->second);
    }

    /**
     * @brief Set the format relating to an float-type.
     *
     * @tparam FloatT_ float-type
     * @param fmt the format to set
     */
    template<typename FloatT_>
    void setFloatFmt(floatFmt<CharT_> fmt = scientificFmt)
    {
        static_assert(std::is_floating_point_v<FloatT_>, "Type FloatT_ must be floating point.");
        floatType2format_[typeString<FloatT_>()] = fmt;
    }

    /**
     * @brief Get the format relating to an float-type.
     *
     * @tparam FloatT_ float-type
     * @return the format set for the float-type
     */
    template<typename FloatT_>
    void setFloatFmt(size_t width, size_t precision = 5, char fill = '0', bool isFixed = false)
    {
        static_assert(std::is_floating_point_v<FloatT_>, "Type FloatT_ must be floating point.");
        floatType2format_[typeString<FloatT_>()] = floatFmt<CharT_>(width, precision, fill, isFixed);
    }

    template<typename FloatT_>
    floatFmt<CharT_> getFloatFmt()
    {
        static_assert(std::is_floating_point_v<FloatT_>, "Type FloatT_ must be floating point.");
        auto found = floatType2format_.find(typeString<FloatT_>());

        if(found == floatType2format_.end())
            return (invalidFmt);

        return (found->second);
    }
};

// Static definitions

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> decorator<CharT_, TraitsT_>::invalidFmt{false};

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> decorator<CharT_, TraitsT_>::scientificFmt{true};

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> decorator<CharT_, TraitsT_>::shortFloatFmt{10, 2, '0', false};

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> longFloatFmt{20, 10, '0', false};

template<typename CharT_, typename TraitsT_>
decorator<CharT_, TraitsT_> decorator<CharT_, TraitsT_>::theInstance;

/**
 * @brief Default decoration: just stream as is with no further adjustment.
 *
 * @tparam Value_ value type
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename Value_, typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const Value_ &value)
{
    os << value;
}

/**
 * @brief Decorator for boolean values. Apart from configured brackets also can be as 0/1 or true/false.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const bool &value)
{
    auto decoInst = util::decorator<CharT_, Traits_>::instance();
    auto bracket  = decoInst.getBracket(value, util::BracketKey::BOOL);
    auto alpha    = decoInst.getBoolAlpha();

    std::ios_base::fmtflags f(os.flags());

    if(alpha)
        os << std::boolalpha;
    else
        os << std::noboolalpha;

    os << bracket.left() << value << bracket.right();
    os.flags(f);
}

/**
 * @brief Decorator for integer typed values. Apart from configured brackets also can be streamed as hex/oct/dec.
 *
 * @tparam IntT_ integer type
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param key key to get the appropriate brackets
 * @param value the value to decorate
 */
template<typename IntT_, typename CharT_, typename Traits_>
void decorateInt(std::basic_ostream<CharT_, Traits_> &os, const std::string_view &key, IntT_ &value)
{
    auto                    decoInst = util::decorator<CharT_, Traits_>::instance();
    auto                    bracket  = decoInst.getBracket(value, key);
    auto                    fmt      = decoInst.template getIntFmt<IntT_>();
    std::ios_base::fmtflags f(os.flags());

    if(fmt != util::intFmt::ignore)
    {
        if(fmt == util::intFmt::hexadecimal)
            os << std::hex;
        else if(fmt == util::intFmt::decimal)
            os << std::dec;
        else if(fmt == util::intFmt::octal)
            os << std::oct;
    }

    os << bracket.left() << value << bracket.right();
    os.flags(f);
}

/**
 * @brief Decorator specialized for 8-bit chars.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const char &value)
{
    decorateInt(os, util::BracketKey::CHAR, value);
}

/**
 * @brief Decorator specialized for 32-bit chars.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const wchar_t &value)
{
    decorateInt(os, util::BracketKey::CHAR, value);
}

/**
 * @brief Decorator specialized for 8-bit ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int8_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator specialized for 16-bit ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int16_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator specialized for 32-bit ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int32_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator specialized for 64-bit ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const int64_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator specialized for 8-bit unsigned ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint8_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator specialized for 16-bit unsigned ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint16_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator specialized for 32-bit unsigned ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint32_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator specialized for 64-bit unsigned ints.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const uint64_t &value)
{
    decorateInt(os, util::BracketKey::INT, value);
}

/**
 * @brief Decorator for float typed values. Apart from configured brackets also floating point format can be configured.
 *
 * @tparam FloatT_ floating point type
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param key key to get the appropriate brackets
 * @param value the value to decorate
 */
template<typename FloatT_, typename CharT_, typename Traits_>
void decorateFloat(std::basic_ostream<CharT_, Traits_> &os, const std::string_view &key, FloatT_ &value)
{
    auto                    decoInst = util::decorator<CharT_, Traits_>::instance();
    auto                    bracket  = decoInst.getBracket(value, key);
    auto                    fmt      = decoInst.template getFloatFmt<FloatT_>();
    std::ios_base::fmtflags f(os.flags());

    if(fmt.isValid())
        os << fmt;

    os << bracket.left() << value << bracket.right();
    os.flags(f);
}

/**
 * @brief Decorator specialized for floats.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const float &value)
{
    decorateFloat(os, util::BracketKey::FLOAT, value);
}

/**
 * @brief Decorator specialized for doubles.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const double &value)
{
    decorateFloat(os, util::BracketKey::FLOAT, value);
}

/**
 * @brief Decorator specialized for long doubles.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const long double &value)
{
    decorateFloat(os, util::BracketKey::FLOAT, value);
}

/**
 * @brief Decorator specialized for std::strings.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const std::basic_string<CharT_, Traits_> &value)
{
    auto decoInst = util::decorator<CharT_, Traits_>::instance();
    auto bracket  = decoInst.getBracket(value, util::BracketKey::STRING);

    os << bracket.left() << value << bracket.right();
}

/**
 * @brief Decorator specialized for char*.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const CharT_ *value)
{
    auto decoInst = util::decorator<CharT_, Traits_>::instance();
    auto bracket  = decoInst.getBracket(value, util::BracketKey::STRING);

    os << bracket.left() << value << bracket.right();
}

/**
 * @brief Decorator specialized for std::strings.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const std::basic_string_view<CharT_, Traits_> &value)
{
    decorate(os, std::basic_string<CharT_, Traits_>{value});
}

/**
 * @brief Iterate through a container and pushing each elements on the given output stream enclosing in left and right
 * brackets and separated by an inner separator.
 *
 * @tparam ContainerType_ type of container
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param container the forward iterable container
 * @param defaultBracketId  an ID to select a bracket if no special bracket is defined for the container
 */
template<typename ContainerType_, typename CharT_, typename Traits_>
typename std::enable_if<!std::is_same<ContainerType_, std::tuple<typename ContainerType_::value_type>>::value,
                        void>::type
 decorate(std::basic_ostream<CharT_, Traits_>      &os,
          const ContainerType_                     &container,
          const std::basic_string<CharT_, Traits_> &defaultBracketId)
{
    auto decoInst = util::decorator<CharT_, Traits_>::instance();
    auto bracket  = decoInst.getBracket(container, defaultBracketId);
    os << bracket.left();

    if(!container.empty())
    {
        auto iter = container.begin();

        while(iter != container.end())
        {
            decorate(os, *iter);
            iter++;

            if(iter != container.end())
                os << bracket.inner();
        }
    }

    os << bracket.right();
}

template<typename TupleType_,
         typename CharT_,
         typename Traits_,
         std::size_t Index = 0,
         typename          = std::enable_if<util::is_tuple<TupleType_>::value>>

void iterateTuple(std::basic_ostream<CharT_, Traits_> &os, const TupleType_ &tuple, const util::Brackets &bracket)
{
    if constexpr(Index < std::tuple_size<TupleType_>::value)
    {
        decorate(os, std::get<Index>(tuple));
        os << bracket.inner();
        iterateTuple<TupleType_, Index + 1>(tuple);
    }
}

template<typename TupleType_, typename CharT_, typename Traits_, typename = std::enable_if<util::is_tuple<TupleType_>::value>>
typename std::enable_if<std::is_same<TupleType_, std::tuple<typename TupleType_::value_type>>::value, void>::type
 decorate(std::basic_ostream<CharT_, Traits_>      &os,
          const TupleType_                         &tuple_obj,
          const std::basic_string<CharT_, Traits_> &defaultBracketId)
{
    auto decoInst = util::decorator<CharT_, Traits_>::instance();
    auto bracket  = decoInst.getBracket(tuple_obj, defaultBracketId);
    os << bracket.left();
    iterateTuple(os, tuple_obj, bracket);
    os << bracket.right();
}

/**
 * @brief Generic ostream - &lt;&lt; operator for vectors.
 *
 * @tparam T_ type of vector elements
 * @tparam Alloc_ allocator for vector
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os the output-stream
 * @param vec the vector to stream
 * @return std::basic_ostream<CharT_, Traits_>& the modified stream
 */
template<typename T_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::vector<T_, Alloc_>       &vec)
{
    decorate(os, vec, std::basic_string<CharT_, Traits_>{util::BracketKey::VECTOR});

    return (os);
}

/**
 * @brief Generic ostream - &lt;&lt; operator for double ended queues.
 *
 * @tparam T_ type of deque elements
 * @tparam Alloc_ allocator for deque
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os the output-stream
 * @param dblEndQueue the deque to stream
 * @return std::basic_ostream<CharT_, Traits_>& the modified stream
 */
template<typename T_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::deque<T_, Alloc_>        &dblEndQueue)
{
    decorate(os, dblEndQueue, std::basic_string<CharT_, Traits_>{util::BracketKey::DEQUE});

    return (os);
}

/**
 * @brief Generic ostream - &lt;&lt; operator for sets.
 *
 * @tparam Key key-type
 * @tparam Compare comparison function
 * @tparam Alloc allocator for set
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os the output-stream
 * @param sortedSet the set to stream
 * @return std::basic_ostream<CharT_, Traits_>& the modified stream
 */
template<typename Key, typename Compare, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::set<Key, Compare, Alloc> &sortedSet)
{
    decorate(os, sortedSet, std::basic_string<CharT_, Traits_>{util::BracketKey::SET});

    return (os);
}

/**
 * @brief Generic ostream - &lt;&lt; operator for unordered sets.
 *
 * @tparam Value value-type
 * @tparam Hash hash-function for set-elements
 * @tparam Pred predicate
 * @tparam Alloc allocator for unordered_set
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os the output-stream
 * @param unordSet the unordered_set to stream
 * @return std::basic_ostream<CharT_, Traits_>& the modified stream
 */
template<typename Value, typename Hash, typename Pred, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_>                &os,
                                                       const std::unordered_set<Value, Hash, Pred, Alloc> &unordSet)
{
    decorate(os, unordSet, std::basic_string<CharT_, Traits_>{util::BracketKey::UNORDERED_SET});

    return (os);
}

/**
 * @brief Generic ostream - &lt;&lt; operator for pairs.
 *
 * @tparam T1_ type of first element
 * @tparam T2_ type of second element
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os the output-stream
 * @param pair1st2nd the pair to stream
 * @return std::basic_ostream<CharT_, Traits_>& the modified stream
 */
template<typename T1_, typename T2_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::pair<T1_, T2_>           &pair1st2nd)
{
    auto decoInst = util::decorator<CharT_, Traits_>::instance();
    auto bracket  = decoInst.getBracket(pair1st2nd, util::BracketKey::PAIR);

    os << bracket.left();
    decorate(os, pair1st2nd.first);
    os << bracket.inner();
    decorate(os, pair1st2nd.second);
    os << bracket.right();

    return (os);
}

/**
 * @brief Generic ostream - &lt;&lt; operator for unordered maps.
 *
 * @tparam Key key-type
 * @tparam Value value-type
 * @tparam Hash hash-function for key
 * @tparam Pred predicate
 * @tparam Alloc allocator function for unordered_map
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os the output-stream
 * @param unordMap the unordered_map to stream
 * @return std::basic_ostream<CharT_, Traits_>& the modified stream
 */
template<typename Key, typename Value, typename Hash, typename Pred, typename Alloc, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &
 operator<<(std::basic_ostream<CharT_, Traits_> &os, const std::unordered_map<Key, Value, Hash, Pred, Alloc> &unordMap)
{
    decorate(os, unordMap, std::basic_string<CharT_, Traits_>{util::BracketKey::UNORDERED_MAP});

    return (os);
}

/**
 * @brief Generic ostream - &lt;&lt; operator for standard (ordered) maps.
 *
 * @tparam Key key-type
 * @tparam Value value-type
 * @tparam Compare_ comparison function
 * @tparam Alloc_ allocator function for map
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os the output-stream
 * @param sortedMap the map to stream
 * @return std::basic_ostream<CharT_, Traits_>& the modified stream
 */
template<typename Key, typename Value, typename Compare_, typename Alloc_, typename CharT_, typename Traits_>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_>          &os,
                                                       const std::map<Key, Value, Compare_, Alloc_> &sortedMap)
{
    decorate(os, sortedMap, std::basic_string<CharT_, Traits_>{util::BracketKey::MAP});

    return (os);
}

};  // namespace util

#endif  // NS_UTIL_DECORATOR_H_INCLUDED
