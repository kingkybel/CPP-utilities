/*
 * File Name:   brackets.h
 * Description: standard bracket types
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

#ifndef NS_UTIL_BRACKETS_H_INCLUDED
#define NS_UTIL_BRACKETS_H_INCLUDED

#include <string>
namespace util
{
class Brackets
{
    public:
    /**
     * Enumeration of common bracket types.
     */
    enum Type
    {
        /**
         * No brackets.
         */
        NONE,
        /**
         * Curly brackets: "{", "}".
         */
        BRACE,
        /**
         * Square brackets: "[", "]".
         */
        BRACKET,
        /**
         * Chevron brackets: "<", ">".
         */
        CHEFRON,
        /**
         * Round brackets: "(", ")".
         */
        ROUND,
        /**
         * Pipe brackets: "|", "|".
         */
        PIPE,
        /**
         * Slash brackets: "/","/".
         */
        SLASH,
        /**
         * Backslash brackets: "\","\".
         */
        BACKSLASH,
        /**
         * UNIX-path brackets.
         */
        UNIXPATH,
        /**
         * Windows-path brackets.
         */
        WINDOWSPATH,
        /**
         * Singles quotes "\'".
         */
        SINGLEQUOTES,
        /**
         * Double quotes "\"".
         */
        DOUBLEQUOTES,
        /**
         * user defined brackets.
         */
        USER
    };

    /**
     * Default constructor.
     * @param type standard type of bracket. defaulted to NONE.
     */
    Brackets(Type type = NONE)
    : type_(type)
    , left_(type == NONE         ? "" :
            type == BRACE        ? "{" :
            type == BRACKET      ? "[" :
            type == CHEFRON      ? "<" :
            type == ROUND        ? "(" :
            type == PIPE         ? "|" :
            type == SLASH        ? "/" :
            type == BACKSLASH    ? "\\" :
            type == UNIXPATH     ? "/" :
            type == WINDOWSPATH  ? ":\\" :
            type == SINGLEQUOTES ? "\'" :
            type == DOUBLEQUOTES ? "\"" :
                                   "")
    , inner_(type == NONE         ? " " :
             type == BRACE        ? "," :
             type == BRACKET      ? "," :
             type == CHEFRON      ? "," :
             type == ROUND        ? "," :
             type == PIPE         ? "|" :
             type == SLASH        ? "/" :
             type == BACKSLASH    ? "\\" :
             type == UNIXPATH     ? "/" :
             type == WINDOWSPATH  ? "\\" :
             type == SINGLEQUOTES ? "" :
             type == DOUBLEQUOTES ? "" :
                                    "")
    , right_(type == NONE         ? "" :
             type == BRACE        ? "}" :
             type == BRACKET      ? "]" :
             type == CHEFRON      ? ">" :
             type == ROUND        ? ")" :
             type == PIPE         ? "|" :
             type == SLASH        ? "/" :
             type == BACKSLASH    ? "\\" :
             type == UNIXPATH     ? "" :
             type == WINDOWSPATH  ? "" :
             type == SINGLEQUOTES ? "\'" :
             type == DOUBLEQUOTES ? "\"" :
                                    "")
    {
    }

    /**
     * Custom brackets constructor.
     *
     * @param left left bracket string
     * @param inner inner bracket string
     * @param right right bracket string
     */
    Brackets(const std::string &left, const std::string &inner, const std::string &right)
    : type_(USER)
    , left_(left)
    , inner_(inner)
    , right_(right)
    {
    }

    Brackets(const Brackets &rhs)           = default;
    Brackets operator=(const Brackets &rhs) = default;

    /**
     * Get the left (opening) bracket.
     * @param customLeft custom string to tack to the left of the separator
     * @param customRight custom string to tack to the right of the separator
     *
     * @return the left bracket as string.
     */
    const std::string &left(const std::string &customLeft = "", const std::string &customRight = "") const
    {
        return (customLeft + left_ + customRight);
    }

    /**
     * Get the inner separator.
     * @param customLeft custom string to tack to the left of the separator
     * @param customRight custom string to tack to the right of the separator
     *
     * @return the left bracket as string.
     */
    const std::string &inner(const std::string &customLeft = "", const std::string &customRight = "") const
    {
        return (customLeft + inner_ + customRight);
    }

    /**
     * Get the right (closing) bracket.
     * @param customLeft custom string to tack to the left of the separator
     * @param customRight custom string to tack to the right of the separator
     *
     * @return the right bracket as string.
     */
    const std::string &right(const std::string &customLeft = "", const std::string &customRight = "") const
    {
        return (customLeft + right_ + customRight);
    }

    private:
    const Type         type_;
    const std::string &left_;
    const std::string &inner_;
    const std::string &right_;
};
};
// namespace util

#endif  // NS_UTIL_BRACKETS_H_INCLUDED
