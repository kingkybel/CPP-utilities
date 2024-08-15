/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/json_exceptions.h
 * Description: exceptions thrown by json objects
 *
 * Copyright (C) 2024 Dieter J Kybelksties <github@kybelksties.com>
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
 * @date: 2024-08-15
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_JSON_EXCEPTIONS_H_INCLUDED
#define NS_UTIL_JSON_EXCEPTIONS_H_INCLUDED

#include <boost/json.hpp>
#include <exception>
#include <string>
#include <vector>

namespace util
{
class JsonError : public std::exception
{
    protected:
    std::string message;

    public:
    explicit JsonError(const std::string& msg = "") : message(msg)
    {
    }

    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class JsonGeneralError : public JsonError
{
    public:
    explicit JsonGeneralError(const std::string& msg = "") : JsonError(msg)
    {
    }
};

class JsonPartialKeyError : public JsonError
{
    public:
    explicit JsonPartialKeyError(const std::string& msg = "") : JsonError(msg)
    {
    }
};

class JsonIndexKeyError : public JsonPartialKeyError
{
    public:
    explicit JsonIndexKeyError(const std::string& index)
    : JsonPartialKeyError("Index '" + index + "' is not a valid index. Only 0, positive ints or '^'/'$' are allowed.")
    {
    }
};

class JsonStringKeyError : public JsonPartialKeyError
{
    public:
    explicit JsonStringKeyError(const std::string& key)
    : JsonPartialKeyError("Key '" + key + "' is not a valid key. Cannot be empty or contain any of [ \t\n[]\"].")
    {
    }
};

class JsonPathFormatError : public JsonError
{
    public:
    JsonPathFormatError(const std::string& path_string = "", const std::string& extra_info = "")
    : JsonError("Json path-string '" + path_string + "' does not conform to ([ <int> | '^' | '$' ] | <string-id>)+"
                + (!extra_info.empty() ? ". (" + extra_info + ")" : ""))
    {
    }
};

class JsonKeyError : public JsonError
{
    public:
    JsonKeyError(int key, const std::vector<std::string>& keys, const boost::json::value* json_obj = nullptr)
    : JsonError(buildMessage(key, keys, json_obj))
    {
    }

    private:
    static std::string buildMessage(int key, const std::vector<std::string>& keys, const boost::json::value* json_obj)
    {
        if(json_obj)
        {
            return "Key '" + keys[key] + "' at key-number " + std::to_string(key)
                   + " requires object type(dict) but found '" + typeid(*json_obj).name() + "'";
        }
        else
        {
            return "Key '" + keys[key] + "' at key-number " + std::to_string(key)
                   + " cannot be found in the given JSON object";
        }
    }
};

class JsonIndexError : public JsonError
{
    public:
    JsonIndexError(int key_number, const std::vector<std::string>& keys, const boost::json::value* json_obj = nullptr)
    : JsonError(buildMessage(key_number, keys, json_obj))
    {
    }

    private:
    static std::string
     buildMessage(int key_number, const std::vector<std::string>& keys, const boost::json::value* json_obj)
    {
        if(json_obj)
        {
            return "Index '" + keys[key_number] + "' at key-number " + std::to_string(key_number)
                   + " requires object type(list) but found '" + typeid(*json_obj).name() + "'";
        }
        else
        {
            return "Index '" + keys[key_number] + "' at key-number " + std::to_string(key_number)
                   + " cannot be found in the given JSON object";
        }
    }
};

class JsonValueMismatch : public JsonError
{
    public:
    template<typename T, typename U>
    JsonValueMismatch(const T& orig_value, const U& new_value)
    : JsonError("Cannot overwrite value of different type if not forced. "
                "Original value '"
                + std::to_string(orig_value) + "' type(" + typeid(orig_value).name() + "), New value '"
                + std::to_string(new_value) + "' type(" + typeid(new_value).name() + ")")
    {
    }
};
}  // namespace util
#endif  // NS_UTIL_JSON_EXCEPTIONS_H_INCLUDED
