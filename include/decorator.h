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

#ifndef DECORATOR_H_INCLUDED
#define DECORATOR_H_INCLUDED

#include "stream_decorate.h"
#include "stringutil.h"

#include <brackets.h>
#include <ios>
#include <map>
#include <string>
#include <typeinfo>

template<typename CharT_>
struct floatFmt
{
    explicit floatFmt(bool isScientific = false) : isScientific_(isScientific), isValid_(isScientific)
    {
    }

    floatFmt(size_t width, size_t precision = 5, CharT_ fill = '0', bool isFixed = false)
    : width_(width)
    , precision_(precision)
    , fill_(fill)
    , isFixed_(isFixed)
    , isScientific_(false)
    {
    }

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

template<typename CharT_, typename TraitsT_>
inline std::basic_ostream<CharT_, TraitsT_>& operator<<(std::basic_ostream<CharT_, TraitsT_>& os,
                                                        const floatFmt<CharT_>&               fmt)
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
 * This template class facilitates to configure how objects are streamed on output streams.
 */
template<typename CharT_, typename TraitsT_ = std::char_traits<CharT_>>
class decorator
{
    public:
    using StringT_     = std::basic_string<CharT_, TraitsT_>;
    using BracketsType = BracketsT<StringT_>;

    template<typename T_>
    StringT_ typeString()
    {
        return (convert<StringT_>(std::string(typeid(T_).name())));
    }

    const static StringT_ KEY_NONE;
    const static StringT_ KEY_VECTOR;
    const static StringT_ KEY_DEQUE;
    const static StringT_ KEY_UNORDERED_SET;
    const static StringT_ KEY_PAIR;
    const static StringT_ KEY_UNORDERED_MAP;
    const static StringT_ KEY_MAP;
    const static StringT_ KEY_SET;
    const static StringT_ KEY_STRING;
    const static StringT_ KEY_CHAR;
    const static StringT_ KEY_INT;
    const static StringT_ KEY_FLOAT;

    enum class intFmt
    {
        print_char,
        decimal,
        hexadecimal,
        octal,
        ignore
    };

    const static floatFmt<CharT_> invalidFmt;
    const static floatFmt<CharT_> scientificFmt;
    const static floatFmt<CharT_> shortFloatFmt;
    const static floatFmt<CharT_> longFloatFmt;

    private:
    static decorator theInstance;

    using BracketMapType     = std::map<StringT_, BracketsType>;
    using IntFormatMapType   = std::map<StringT_, intFmt>;
    using FloatFormatMapType = std::map<StringT_, floatFmt<CharT_>>;

    BracketMapType     type2brackets_;
    IntFormatMapType   intType2format_;
    FloatFormatMapType floatType2format_;

    bool             alphaBool_     = true;
    floatFmt<CharT_> floatFmt_      = scientificFmt;
    floatFmt<CharT_> doubleFmt_     = scientificFmt;
    floatFmt<CharT_> longDoubleFmt_ = scientificFmt;

    decorator()
    {
    }

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
     * Reset the contents.
     */
    void clear()
    {
        type2brackets_.clear();
        intType2format_.clear();
        floatType2format_.clear();
    }

    /**
     * Initialize the bracket configuration.
     */
    bool initialize()
    {
        clear();
        setBracket(KEY_NONE, BracketsType::Type::NONE);
        setBracket(KEY_VECTOR, BracketsType(BracketsType::Type::CHEFRON));
        setBracket(KEY_DEQUE, BracketsType(BracketsType::Type::QUEUE));
        setBracket(KEY_UNORDERED_SET, BracketsType(BracketsType::Type::UNORDERED));
        setBracket(KEY_PAIR, BracketsType(BracketsType::Type::PAIR));
        setBracket(KEY_UNORDERED_MAP, BracketsType(BracketsType::Type::UNORDERED));
        setBracket(KEY_MAP, BracketsType(BracketsType::Type::BRACKET));
        setBracket(KEY_SET, BracketsType(BracketsType::Type::BRACE));
        setBracket(KEY_STRING, BracketsType(BracketsType::Type::DOUBLEQUOTES));
        setBracket(KEY_CHAR, BracketsType(BracketsType::Type::SINGLEQUOTES));
        setBracket(KEY_INT, BracketsType(BracketsType::Type::NONE));
        setBracket(KEY_FLOAT, BracketsType(BracketsType::Type::NONE));

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

        setFloatFmt<float>(scientificFmt);
        setFloatFmt<double>(scientificFmt);
        setFloatFmt<long double>(scientificFmt);

        return (true);
    }

    /**
     * Retrieve the singleton instance.
     *
     * @return the instance
     */
    static decorator& instance()
    {
        static bool initialized = decorator::theInstance.initialize();

        return (decorator::theInstance);
    }

    /**
     * Add a bracket identified by a unique string.
     *
     * @param key unique string
     * @param bracket the bracket to set
     */
    void setBracket(StringT_ key, BracketsType bracket)
    {
        type2brackets_[key] = bracket;
    }

    /**
     * Add a bracket identified by a unique string.
     *
     * @param object the typeid of this object is used as key
     * @param bracket the bracket to set
     */
    template<typename T>
    void setBracket(const T& object, BracketsType bracket)
    {
        type2brackets_[typeString<T>()] = bracket;
    }

    /**
     * Add a bracket identified by a unique string.
     *
     * @param object the typeid of this object is used as key
     * @param left left bracket
     * @param inner inner bracket
     * @param right right bracket
     */
    template<typename T>
    void setBracket(const T& object, const StringT_& left, const StringT_& inner, const StringT_& right)
    {
        BracketsType bracket{left, inner, right};

        type2brackets_[typeString<T>()] = bracket;
    }

    /**
     * Retrieve a bracket for the key identifier.
     *
     * @param object the typeid of this object is used as key
     * @param defaultKey use this default key, if no special configuration for this object type
     *
     * @return the bracket identified
     */
    template<typename T>
    BracketsType getBracket(const StringT_& defaultKey = decorator<CharT_, TraitsT_>::KEY_NONE)
    {
        auto found = type2brackets_.find(typeString<T>());

        if(found == type2brackets_.end())
        {
            found = type2brackets_.find(defaultKey);

            return ((found == type2brackets_.end()) ? BracketsType(BracketsType::Type::NONE) : found->second);
        }

        return (found->second);
    }

    bool getAlphaBool() const
    {
        return (alphaBool_);
    }

    template<typename IntT_>
    void setIntFmt(intFmt fmt)
    {
        intType2format_[typeString<IntT_>()] = fmt;
    }

    template<typename IntT_>
    intFmt getIntFmt()
    {
        auto found = intType2format_.find(typeString<IntT_>());

        if(found == intType2format_.end())
            return (intFmt::ignore);

        return (found->second);
    }

    template<typename FloatT_>
    void setFloatFmt(floatFmt<CharT_> fmt = scientificFmt)
    {
        floatType2format_[typeString<FloatT_>()] = fmt;
    }

    template<typename FloatT_>
    void setFloatFmt(size_t width, size_t precision = 5, char fill = '0', bool isFixed = false)
    {
        floatType2format_[typeString<FloatT_>()] = floatFmt<CharT_>(width, precision, fill, isFixed);
    }

    template<typename FloatT_>
    floatFmt<CharT_> getFloatFmt()
    {
        auto found = floatType2format_.find(typeString<FloatT_>());

        if(found == floatType2format_.end())
            return (invalidFmt);

        return (found->second);
    }
};

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> decorator<CharT_, TraitsT_>::invalidFmt{false};

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> decorator<CharT_, TraitsT_>::scientificFmt{true};

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> decorator<CharT_, TraitsT_>::shortFloatFmt{10, 2, '0', false};

template<typename CharT_, typename TraitsT_>
const floatFmt<CharT_> decorator<CharT_, TraitsT_>::longFloatFmt{20, 10, '0', false};

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_NONE = convert<std::basic_string<CharT_, TraitsT_>>(std::string("none"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_VECTOR = convert<std::basic_string<CharT_, TraitsT_>>(std::string("vector"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_DEQUE = convert<std::basic_string<CharT_, TraitsT_>>(std::string("deque"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_> decorator<CharT_, TraitsT_>::KEY_UNORDERED_SET =
 convert<std::basic_string<CharT_, TraitsT_>>(std::string("unordered_set"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_PAIR = convert<std::basic_string<CharT_, TraitsT_>>(std::string("pair"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_> decorator<CharT_, TraitsT_>::KEY_UNORDERED_MAP =
 convert<std::basic_string<CharT_, TraitsT_>>(std::string("unordered_map"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_MAP = convert<std::basic_string<CharT_, TraitsT_>>(std::string("map"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_SET = convert<std::basic_string<CharT_, TraitsT_>>(std::string("set"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_STRING = convert<std::basic_string<CharT_, TraitsT_>>(std::string("string"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_CHAR = convert<std::basic_string<CharT_, TraitsT_>>(std::string("char"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_INT = convert<std::basic_string<CharT_, TraitsT_>>(std::string("int"));

template<typename CharT_, typename TraitsT_>
const std::basic_string<CharT_, TraitsT_>
 decorator<CharT_, TraitsT_>::KEY_FLOAT = convert<std::basic_string<CharT_, TraitsT_>>(std::string("float"));

template<typename CharT_, typename TraitsT_>
decorator<CharT_, TraitsT_> decorator<CharT_, TraitsT_>::theInstance;

#endif  // DECORATOR_H_INCLUDED
