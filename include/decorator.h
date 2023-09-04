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
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <iostream>
#include <limits>
#include <map>
#include <numbers>
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

template<typename CharT_, typename Traits_, typename... T>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::tuple<T...>              &tuple_obj);
//////////

/**
 * @brief Convert a char to a different char type
 *
 * @tparam CharTto_ type to convert to
 * @tparam CharTfrom_ type to convert from
 * @param c the char to convert
 * @return constexpr CharTto_ the converted char
 */
template<typename CharTto_, typename CharTfrom_>
constexpr CharTto_ charToChar(CharTto_ c)
{
    return static_cast<CharTfrom_>(c);
}

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
template<typename CharT1_   = char16_t,
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
 * @brief enum to define how floats are displayed
 */
enum class FloatBase
{
    default_format,
    scientific,
    fixed,
    hexfloat
};

/**
 * @brief Struct to encapsulate format configuration for floating point values.
 *
 * @tparam CharT_ character type of the stream the format will be used on
 */
template<typename CharT_ = char>
struct floatFmt
{
    /**
     * @brief Construct a new float Fmt object
     *
     * @param base Basic format
     * @param width width used for floats
     * @param precision (post-comma-) precision of floats
     * @param fill a fill character, defaults to '0'
     */
    explicit floatFmt(FloatBase base      = FloatBase::default_format,
                      size_t    width     = 8,
                      size_t    precision = 5,
                      CharT_    fill      = charToChar<CharT_, char>('0'))
    : base_(base)
    , width_(width)
    , precision_(precision)
    , fill_(fill)
    {
    }

    /**
     * @brief Stringify the configuration for debug-purposes
     *
     * @return const std::string a string describing the format
     */
    const std::string toString() const
    {
        std::stringstream ss;
        ss << "floatFmt(";
        switch(base_)
        {
            case FloatBase::default_format:
                ss << "default_format";
                break;
            case FloatBase::scientific:
                ss << "scientific";
                break;
            case FloatBase::hexfloat:
                ss << "hexfloat";
                break;
            case FloatBase::fixed:
                ss << "fixed";
                break;
        }
        ss << ", width=" << width_ << ", precision=" << precision_ << ", fill='" << fill_ << "')";

        return ss.str();
    }

    FloatBase base_      = FloatBase::scientific;
    size_t    width_     = 0UL;
    size_t    precision_ = 0UL;
    CharT_    fill_      = charToChar<CharT_, char>('0');
};

/**
 * @brief enum to describe how integers are displayed
 */
enum class IntBase
{
    default_format,
    as_char,
    decimal,
    hexadecimal,
    octal
};

/**
 * @brief Struct to encapsulate format configuration for int values.
 *
 * @tparam CharT_ character type of the stream the format will be used on
 */
template<typename CharT_ = char>
struct intFmt
{
    explicit intFmt(IntBase                                                  base       = IntBase::decimal,
                    size_t                                                   width      = 0UL,
                    bool                                                     showBase   = false,
                    bool                                                     hexUpper   = false,
                    CharT_                                                   fill       = charToChar<CharT_, char>('0'),
                    std::basic_string_view<CharT_, std::char_traits<CharT_>> hexBaseStr = "0x",
                    std::basic_string_view<CharT_, std::char_traits<CharT_>> octBaseStr = "0o")
    : isValid_(base != IntBase::default_format)
    , base_(base)
    , width_(width)
    , showBase_(showBase)
    , hexUpper_(hexUpper)
    , fill_(fill)
    , hexBaseStr_(hexBaseStr)
    , octBaseStr_(octBaseStr)
    {
    }

    /**
     * @brief Stringify the configuration for debug-purposes
     *
     * @return const std::string a string describing the format
     */
    const std::string toString() const
    {
        std::stringstream ss;

        ss << "intFmt(";
        if(!isValid_)
            ss << "in-";
        ss << "valid, ";
        if(base_ == IntBase::as_char)
            ss << "IntBase::as_char";
        if(base_ == IntBase::decimal)
            ss << "IntBase::decimal";
        if(base_ == IntBase::hexadecimal)
            ss << "IntBase::hexdecimal";
        if(base_ == IntBase::octal)
            ss << "IntBase::octal";
        ss << ", width=" << width_ << ", fill='" << fill_ << "', ";
        if(!showBase_)
            ss << "don't ";
        ss << "show base, ";
        ss << "hexBaseStr='" << hexBaseStr_ << "', ";
        ss << "octBaseStr='" << octBaseStr_ << "', ";
        if(hexUpper_)
            ss << "hex upper)";
        else
            ss << "hex lower)";

        return ss.str();
    }

    bool                                                     isValid_  = true;
    IntBase                                                  base_     = IntBase::decimal;
    size_t                                                   width_    = 0UL;
    bool                                                     showBase_ = false;
    bool                                                     hexUpper_ = false;
    CharT_                                                   fill_     = charToChar<CharT_, char>('0');
    std::basic_string_view<CharT_, std::char_traits<CharT_>> hexBaseStr_;
    std::basic_string_view<CharT_, std::char_traits<CharT_>> octBaseStr_;
};

/**
 * @brief This template class facilitates to configure how objects are streamed on output streams.
 *        This is a singleton for each combination of the template parameters CharT_ and TraitsT_
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

    private:
    template<typename T_>
    std::string typeString() const
    {
        return (std::string(typeid(T_).name()));
    }

    decorator()                             = default;
    decorator(decorator &)                  = delete;
    decorator       &operator=(decorator &) = delete;
    static decorator theInstance;

    // keys of maps are strings, not the output string type StringT_
    using BracketMapType     = std::map<std::string, BracketsType>;
    using IntFormatMapType   = std::map<std::string, intFmt<CharT_>>;
    using FloatFormatMapType = std::map<std::string, floatFmt<CharT_>>;

    BracketMapType     type2brackets_;
    IntFormatMapType   intType2format_;
    FloatFormatMapType floatType2format_;
    bool               alphaBool_ = true;

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
        ss << "CharT_:" << typeid(CharT_).name() << " TraitsT_:" << typeid(CharT_).name() << std::endl;
        ss << "---- brackets ------" << std::endl;
        for(const auto &kv: type2brackets_)
            ss << kv.first << " -> ('" << convert<char>(kv.second.left()) << "', '" << convert<char>(kv.second.inner())
               << "', '" << convert<char>(kv.second.right()) << "')" << std::endl;
        ss << "------ int --------" << std::endl;
        for(const auto &kv: intType2format_)
            ss << kv.first << " -> " << (kv.second.toString()) << std::endl;
        ss << "------ float ------" << std::endl;
        for(const auto &kv: floatType2format_)
            ss << kv.first << " -> " << (kv.second.toString()) << std::endl;

        ss << "--------------------" << std::endl;
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

        setFormat<char>(intFmt<CharT_>{IntBase::as_char});
        setFormat<char16_t>(intFmt<CharT_>{IntBase::as_char});
        setFormat<char32_t>(intFmt<CharT_>{IntBase::as_char});
        setFormat<int8_t>(intFmt<CharT_>{IntBase::hexadecimal});
        setFormat<int16_t>(intFmt<CharT_>{IntBase::decimal});
        setFormat<int32_t>(intFmt<CharT_>{IntBase::decimal});
        setFormat<int64_t>(intFmt<CharT_>{IntBase::decimal});
        setFormat<uint8_t>(intFmt<CharT_>{IntBase::decimal});
        setFormat<uint16_t>(intFmt<CharT_>{IntBase::decimal});
        setFormat<uint32_t>(intFmt<CharT_>{IntBase::decimal});
        setFormat<uint64_t>(intFmt<CharT_>{IntBase::decimal});

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

        setFormat<float>(floatFmt{FloatBase::scientific});
        setFormat<double>(floatFmt{FloatBase::scientific});
        setFormat<long double>(floatFmt{FloatBase::scientific});

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
     * @brief Set the display for booleans to alpha ("true" and "false")
     */
    void setBoolAlpha()
    {
        alphaBool_ = true;
    }

    /**
     * @brief Set the display for booleans to no-alpha (1 and 0)
     */
    void setNoBoolAlpha()
    {
        alphaBool_ = false;
    }

    /**
     * @brief Set the format relating to an int-type.
     *
     * @tparam IntT_ int-type
     * @param fmt the format to set
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setFormat(intFmt<CharT_> fmt)
    {
        intType2format_[typeString<IntT_>()] = fmt;
    }

    /**
     * @brief Get the format relating to an int-type.
     *
     * @tparam IntT_ int-type
     * @return the format set for the int-type
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    intFmt<CharT_> getFormat() const
    {
        auto found = intType2format_.find(typeString<IntT_>());

        if(found == intType2format_.end())
            return (intFmt<CharT_>{IntBase::default_format});

        return (found->second);
    }

    /**
     * @brief Set the for display of integers. Only enabled if IntR_ is an integral type.
     *
     * @tparam IntT_ int-type
     * @param base IntBase value
     * @see util::IntBase
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setBase(IntBase base)
    {
        auto fmt  = getFormat<IntT_>();
        fmt.base_ = base;
        if(base == IntBase::default_format)
        {
            fmt.isValid_ = false;
        }
        setFormat<IntT_>(fmt);
    }

    /**
     * @brief Set the width for displaying integers
     *
     * @tparam IntT_
     * @param width width of display
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setWidth(size_t width)
    {
        auto fmt   = getFormat<IntT_>();
        fmt.width_ = width;
        setFormat<IntT_>(fmt);
    }

    /**
     * @brief Set the Show Base object. Only enabled if IntR_ is an integral type.
     *
     * @tparam IntT_ int-type
     * @param showBase if true, then display the base string, otherwise don't
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setShowBase(bool showBase)
    {
        auto fmt      = getFormat<IntT_>();
        fmt.showBase_ = showBase;
        setFormat<IntT_>(fmt);
    }

    /**
     * @brief Set the Hex-Base-String (default "0x"). Only enabled if IntR_ is an integral type.
     *
     * @tparam IntT_ int-type
     * @param hexBaseStr the new base indicator string
     * @param setBaseAsWell set the base to hex as well, if true
     * @param setShowBaseAsWell show the base string in displays as well, if true
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setHexBaseStr(std::basic_string_view<CharT_, TraitsT_> hexBaseStr,
                       bool                                     setBaseAsWell     = true,
                       bool                                     setShowBaseAsWell = true)
    {
        auto fmt = getFormat<IntT_>();
        if(setBaseAsWell)
            fmt.base_ = IntBase::hexadecimal;
        if(setShowBaseAsWell)
            fmt.showBase_ = true;
        fmt.hexBaseStr_ = hexBaseStr;
        setFormat<IntT_>(fmt);
    }

    /**
     * @brief Set the Hex-Base-String (default "0o"). Only enabled if IntR_ is an integral type.
     *
     * @tparam IntT_ int-type
     * @param hexBaseStr the new base indicator string
     * @param setBaseAsWell set the base to oct as well, if true
     * @param setShowBaseAsWell show the base string in displays as well, if true
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setOctBaseStr(std::basic_string_view<CharT_, TraitsT_> octBaseStr,
                       bool                                     setBaseAsWell     = true,
                       bool                                     setShowBaseAsWell = true)
    {
        auto fmt = getFormat<IntT_>();
        if(setBaseAsWell)
            fmt.base_ = IntBase::octal;
        if(setShowBaseAsWell)
            fmt.showBase_ = true;
        fmt.octBaseStr_ = octBaseStr;
        setFormat<IntT_>(fmt);
    }

    /**
     * @brief Set to display hex-alphas in uppercase. Only enabled if IntR_ is an integral type.
     *
     * @tparam IntT_ int-type
     * @param hexUpper display in upper if true, in lower otherwise
     * @param setBaseAsWell set the base to hex as well, if true
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setHexUpper(bool hexUpper, bool setBaseAsWell = true)
    {
        auto fmt = getFormat<IntT_>();
        if(setBaseAsWell)
            fmt.base_ = IntBase::hexadecimal;
        fmt.hexUpper_ = hexUpper;
        setFormat<IntT_>(fmt);
    }

    /**
     * @brief Set the Fill character. Only enabled if IntR_ is an integral type.
     *
     * @tparam IntT_ int-type
     * @param fill new fill  character
     */
    template<typename IntT_, typename std::enable_if<std::is_integral<IntT_>::value>::type * = nullptr>
    void setFill(CharT_ fill)
    {
        auto fmt  = getFormat<IntT_>();
        fmt.fill_ = fill;
        setFormat<IntT_>(fmt);
    }

    /**
     * @brief Set the format relating to an float-type. Only enabled if FloatT_ is an floating point type.
     *
     * @tparam FloatT_ float-type
     * @param fmt the format to set
     */
    template<typename FloatT_, typename std::enable_if<std::is_floating_point<FloatT_>::value>::type * = nullptr>
    void setFormat(floatFmt<CharT_> fmt)
    {
        floatType2format_[typeString<FloatT_>()] = fmt;
    }

    /**
     * @brief Get the format relating to an float-type. Only enabled if FloatT_ is an floating point type.
     *
     * @tparam FloatT_ float-type
     */
    template<typename FloatT_, typename std::enable_if<std::is_floating_point<FloatT_>::value>::type * = nullptr>
    void setFormat(size_t width, size_t precision = 5, char fill = '0', bool isFixed = false)
    {
        floatType2format_[typeString<FloatT_>()] = floatFmt<CharT_>(width, precision, fill, isFixed);
    }

    /**
     * @brief Get the for a floating point type. Only enabled if FloatT_ is an floating point type.
     *
     * @tparam FloatT_ float-type
     * @return floatFmt<CharT_> the format set for the float-type
     */
    template<typename FloatT_, typename std::enable_if<std::is_floating_point<FloatT_>::value>::type * = nullptr>
    floatFmt<CharT_> getFormat()
    {
        auto found = floatType2format_.find(typeString<FloatT_>());

        if(found == floatType2format_.end())
            return (floatFmt{FloatBase::default_format});

        return (found->second);
    }

    /**
     * @brief Set the Fill character for float display. Only enabled if FloatT_ is an floating point type.
     *
     * @tparam FloatT_ float-type
     * @param fill fill-character
     */
    template<typename FloatT_, typename std::enable_if<std::is_floating_point<FloatT_>::value>::type * = nullptr>
    void setFill(CharT_ fill)
    {
        auto fmt  = getFormat<FloatT_>();
        fmt.fill_ = fill;
        setFormat<FloatT_>(fmt);
    }

    /**
     * @brief Set the width for float display. Only enabled if FloatT_ is an floating point type.
     *
     * @tparam FloatT_ float-type
     * @tparam std::enable_if<std::is_floating_point<FloatT_>::value>::type
     * @param width new width
     */
    template<typename FloatT_, typename std::enable_if<std::is_floating_point<FloatT_>::value>::type * = nullptr>
    void setWidth(size_t width)
    {
        auto fmt   = getFormat<FloatT_>();
        fmt.width_ = width;
        setFormat<FloatT_>(fmt);
    }

    /**
     * @brief Set the precision for float display. Only enabled if FloatT_ is an floating point type.
     *
     * @tparam FloatT_ float-type
     * @param precision new precision
     */
    template<typename FloatT_, typename std::enable_if<std::is_floating_point<FloatT_>::value>::type * = nullptr>
    void setPrecision(size_t precision)
    {
        auto fmt       = getFormat<FloatT_>();
        fmt.precision_ = precision;
        setFormat<FloatT_>(fmt);
    }

    /**
     * @brief Set the base for float display. Only enabled if FloatT_ is an floating point type.
     *
     * @tparam FloatT_ float-type
     * @param base new float base display configuration
     */
    template<typename FloatT_, typename std::enable_if<std::is_floating_point<FloatT_>::value>::type * = nullptr>
    void setBase(FloatBase base)
    {
        auto fmt  = getFormat<FloatT_>();
        fmt.base_ = base;
        setFormat<FloatT_>(fmt);
    }
};

//////////////////////
// Static definitions

template<typename CharT_, typename TraitsT_>
decorator<CharT_, TraitsT_> decorator<CharT_, TraitsT_>::theInstance;

//////////////////////

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
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(value, util::BracketKey::BOOL);
    auto  alpha    = decoInst.getBoolAlpha();

    auto f(os.flags());  // store flags

    if(alpha)
        os << std::boolalpha;
    else
        os << std::noboolalpha;

    os << bracket.left() << value << bracket.right();

    os.flags(f);  // restore flags
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
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(value, key);
    auto  fmt      = decoInst.template getFormat<IntT_>();

    auto f(os.flags());  // store flags
    os << bracket.left();
    if(fmt.isValid_)
    {
        switch(fmt.base_)
        {
            case util::IntBase::decimal:
                os << std::dec;
                break;
            case util::IntBase::hexadecimal:
                if(fmt.showBase_)
                    os << fmt.hexBaseStr_;
                os << std::hex;
                break;
            case util::IntBase::octal:
                if(fmt.showBase_)
                    os << fmt.octBaseStr_;
                os << std::oct;
                break;
            default:
                break;
        }

        os.fill(fmt.fill_);
        os.width(fmt.width_);
        if(fmt.hexUpper_)
            os << std::uppercase;
        if(typeid(IntT_) == typeid(char) && fmt.base_ != IntBase::as_char)
            os << (int16_t)value;
        else if(typeid(IntT_) == typeid(char16_t) && fmt.base_ != IntBase::as_char)
            os << (int16_t)value;
        else if(typeid(IntT_) == typeid(char32_t) && fmt.base_ != IntBase::as_char)
            os << (int32_t)value;
        else if(typeid(IntT_) == typeid(int8_t) && fmt.base_ != IntBase::as_char)
            os << (int16_t)value;
        else
            os << value;
    }
    else
        os << value;

    if(fmt.isValid_)
        os << std::nouppercase;
    os << bracket.right();

    os.flags(f);  // restore flags
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
 * @brief Decorator specialized for 16-bit chars.
 *
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param value the value to decorate
 */
template<typename CharT_, typename Traits_>
void decorate(std::basic_ostream<CharT_, Traits_> &os, const char16_t &value)
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
void decorate(std::basic_ostream<CharT_, Traits_> &os, const char32_t &value)
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
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(value, key);
    auto  fmt      = decoInst.template getFormat<FloatT_>();
    auto  f(os.flags());  // store flags

    os << bracket.left();
    switch(fmt.base_)
    {
        case FloatBase::default_format:
            os << std::defaultfloat;
            break;
        case FloatBase::scientific:
            os << std::scientific;
            break;
        case FloatBase::hexfloat:
            os << std::hexfloat;
            break;
        case FloatBase::fixed:
            os << std::fixed << std::setprecision(fmt.precision_);
            os.fill(fmt.fill_);
            os.width(fmt.width_);
            break;
        default:
            break;
    }
    os << value;
    os << bracket.right();

    os.flags(f);  // restore flags
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
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(value, util::BracketKey::STRING);

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
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(value, util::BracketKey::STRING);

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
 * @brief Decorator specialised for forward iterable containers. Iterate through a container and pushing each elements
 * on the given output stream enclosing in left and right brackets and separated by an inner separator.
 *
 * @tparam ContainerType_ type of container
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param container the forward iterable container
 * @param defaultBracketId  an ID to select a bracket if no special bracket is defined for the container
 */
template<typename ContainerType_,
         typename CharT_,
         typename Traits_,
         typename std::enable_if<!util::is_tuple<ContainerType_>::value>::type * = nullptr>
void decorate(std::basic_ostream<CharT_, Traits_>      &os,
              const ContainerType_                     &container,
              const std::basic_string<CharT_, Traits_> &defaultBracketId)
{
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(container, defaultBracketId);
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

/**
 * @brief Iterate through a tuple and pushing each elements on the given output stream enclosing in left and right
 *
 * @tparam TupleType_ type of tuple
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param tuple_obj tuple object
 * @param bracket brackets for this tuple object
 */
template<typename TupleType_,
         typename CharT_,
         typename Traits_,
         std::size_t Index                                                  = 0,
         typename std::enable_if<util::is_tuple<TupleType_>::value>::type * = nullptr>
void iterateTuple(std::basic_ostream<CharT_, Traits_> &os, const TupleType_ &tuple_obj, const util::Brackets &bracket)
{
    if constexpr(Index < std::tuple_size<TupleType_>::value)
    {
        decorate(os, std::get<Index>(tuple_obj));
        if constexpr(Index < std::tuple_size<TupleType_>::value - 1)
            os << bracket.inner();
        iterateTuple<TupleType_, CharT_, Traits_, Index + 1>(os, tuple_obj, bracket);
    }
}

/**
 * @brief Decorator specialised for forward tuples.
 *
 * @tparam TupleType_ type of tuple
 * @tparam CharT_ character type
 * @tparam Traits_ string traits
 * @param os output stream
 * @param tuple_obj tuple object
 * @param defaultBracketId  an ID to select a bracket if no special bracket is defined for the container
 * @return std::enable_if<std::is_same<TupleType_, std::tuple<typename TupleType_::value_type>>::value, void>::type
 */
template<typename TupleType_,
         typename CharT_,
         typename Traits_,
         typename std::enable_if<util::is_tuple<TupleType_>::value>::type * = nullptr>
void decorate(std::basic_ostream<CharT_, Traits_>      &os,
              const TupleType_                         &tuple_obj,
              const std::basic_string<CharT_, Traits_> &defaultBracketId)
{
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(tuple_obj, defaultBracketId);
    os << bracket.left();
    iterateTuple<TupleType_, CharT_, Traits_>(os, tuple_obj, bracket);
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
    auto &decoInst = util::decorator<CharT_, Traits_>::instance();
    auto  bracket  = decoInst.getBracket(pair1st2nd, util::BracketKey::PAIR);

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

template<typename CharT_, typename Traits_, typename... T>
inline std::basic_ostream<CharT_, Traits_> &operator<<(std::basic_ostream<CharT_, Traits_> &os,
                                                       const std::tuple<T...>              &tuple_obj)
{
    decorate(os, tuple_obj, std::basic_string<CharT_, Traits_>{util::BracketKey::TUPLE});

    return (os);
}

};  // namespace util

#endif  // NS_UTIL_DECORATOR_H_INCLUDED
