/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/brackets.h
 * Description: standard bracket types
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

#ifndef NS_UTIL_BRACKETS_H_INCLUDED
#define NS_UTIL_BRACKETS_H_INCLUDED

#include <string>
#include <unordered_map>
namespace util
{

namespace BracketKey
{
    using Type                        = std::string_view;
    constexpr Type NONE               = "";
    constexpr Type BOOL               = "bool";
    constexpr Type CHAR               = "char";
    constexpr Type INT                = "int";
    constexpr Type FLOAT              = "float";
    constexpr Type STRING             = "string";
    constexpr Type VECTOR             = "vector";
    constexpr Type DEQUE              = "deque";
    constexpr Type SET                = "set";
    constexpr Type UNORDERED_SET      = "unordered_set";
    constexpr Type MULTISET           = "multiset";
    constexpr Type UNORDERED_MULTISET = "unordered_multiset";
    constexpr Type MAP                = "map";
    constexpr Type UNORDERED_MAP      = "unordered_map";
    constexpr Type MULTIMAP           = "multimap";
    constexpr Type UNORDERED_MULTIMAP = "unordered_multimap";
    constexpr Type PAIR               = "pair";
    constexpr Type TUPLE              = "tuple";
    constexpr Type SLASH              = "slash";
    constexpr Type BACKSLASH          = "backslash";
    constexpr Type PIPE               = "pipe";
};
namespace
{
    struct LeftInnerRight
    {
        std::string left;
        std::string inner;
        std::string right;
    };

    const auto &DEFAULT_BRACKETS = std::unordered_map<util::BracketKey::Type, LeftInnerRight>{{
     {util::BracketKey::NONE, {"", " ", ""}},         {util::BracketKey::BOOL, {"", " ", ""}},
     {util::BracketKey::CHAR, {"'", " ", "'"}},       {util::BracketKey::INT, {"", " ", ""}},
     {util::BracketKey::FLOAT, {"", " ", ""}},        {util::BracketKey::STRING, {"\"", ",", "\""}},
     {util::BracketKey::VECTOR, {"<", ",", ">"}},     {util::BracketKey::DEQUE, {"^", "<-", "$"}},
     {util::BracketKey::SET, {"{", ",", "}"}},        {util::BracketKey::UNORDERED_SET, {"{~", ",", "~}"}},
     {util::BracketKey::MULTISET, {"{#", ",", "#}"}}, {util::BracketKey::UNORDERED_MULTISET, {"{#~", ",", "~#}"}},
     {util::BracketKey::MAP, {"[", ",", "]"}},        {util::BracketKey::UNORDERED_MAP, {"[~", ",", "~]"}},
     {util::BracketKey::MULTIMAP, {"[#", ",", "#]"}}, {util::BracketKey::UNORDERED_MULTIMAP, {"[#~", ",", "~#]"}},
     {util::BracketKey::PAIR, {"(", ",", ")"}},       {util::BracketKey::TUPLE, {"(", ",", ")"}},
     {util::BracketKey::SLASH, {"/", "/", ""}},       {util::BracketKey::BACKSLASH, {"\\", "\\", ""}},
     {util::BracketKey::PIPE, {"|", "|", "|"}},
    }};

};

class Brackets
{
    public:
    /**
     * @brief Default constructor.
     * @param type standard type of bracket. defaulted to NONE.
     */
    Brackets(util::BracketKey::Type type = util::BracketKey::NONE)
    {
        auto found = util::DEFAULT_BRACKETS.find(type);
        // only accept default bracket-keys in this constructor
        if(found != util::DEFAULT_BRACKETS.end())
        {
            type_  = std::string{type};
            left_  = std::string{found->second.left};
            inner_ = std::string{found->second.inner};
            right_ = std::string{found->second.right};
        }
    }

    /**
     * @brief Custom brackets constructor.
     *
     * @param left left bracket string
     * @param inner inner bracket string
     * @param right right bracket string
     */
    Brackets(util::BracketKey::Type type, const std::string &left, const std::string &inner, const std::string &right)
    : type_(std::string{type})
    , left_(left)
    , inner_(inner)
    , right_(right)
    {
    }

    Brackets(const Brackets &rhs) = default;

    /**
     * @brief Get the left (opening) bracket.
     * @param customLeft custom string to tack to the left of the separator
     * @param customRight custom string to tack to the right of the separator
     *
     * @return the left bracket as string.
     */
    const std::string left(const std::string &customLeft = "", const std::string &customRight = "") const
    {
        return (customLeft + left_ + customRight);
    }

    /**
     * @brief Get the inner separator.
     * @param customLeft custom string to tack to the left of the separator
     * @param customRight custom string to tack to the right of the separator
     *
     * @return the left bracket as string.
     */
    const std::string inner(const std::string &customLeft = "", const std::string &customRight = "") const
    {
        return (customLeft + inner_ + customRight);
    }

    /**
     * @brief Get the right (closing) bracket.
     * @param customLeft custom string to tack to the left of the separator
     * @param customRight custom string to tack to the right of the separator
     *
     * @return the right bracket as string.
     */
    const std::string right(const std::string &customLeft = "", const std::string &customRight = "") const
    {
        return (customLeft + right_ + customRight);
    }

    private:
    // save the values as std::string and convert to different string types when required
    std::string type_;
    std::string left_;
    std::string inner_;
    std::string right_;
};
};  // namespace util

#endif  // NS_UTIL_BRACKETS_H_INCLUDED
