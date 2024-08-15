/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   src/json_object.cc
 * Description: wrapper for json objects
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

#include "json_object.h"
namespace util
{
JsonObject::JsonObject() : json_(boost::json::object{})
{
}

JsonObject::JsonObject(const std::string& json_str)
{
    fromString(json_str);
}

JsonObject::JsonObject(const boost::json::value& json_val) : json_(json_val)
{
}

void JsonObject::fromString(const std::string& json_str)
{
    json_ = boost::json::parse(json_str);
}

std::string JsonObject::toString(int indent) const
{
    return boost::json::serialize(json_);
}

const boost::json::value& JsonObject::getJson() const
{
    return json_;
}

void JsonObject::set(const JsonKeyPath& path, const boost::json::value& value, bool force)
{
    // Handle the setting logic based on keys from path
    auto                keys    = path.keyList();
    boost::json::value* current = &json_;

    for(size_t i = 0; i < keys.size(); ++i)
    {
        auto key     = keys[i];
        bool is_last = (i == keys.size() - 1);

        if(auto index_key = std::dynamic_pointer_cast<JsonIndexKey>(key))
        {
            auto& arr   = current->as_array();
            int   index = std::stoi(index_key->toString());
            if(is_last)
            {
                arr[index] = value;
            }
            else
            {
                current = &arr[index];
            }
        }
        else if(auto string_key = std::dynamic_pointer_cast<JsonStringKey>(key))
        {
            auto& obj = current->as_object();
            if(is_last)
            {
                obj[string_key->toString()] = value;
            }
            else
            {
                current = &obj[string_key->toString()];
            }
        }
    }
}
void JsonObject::fromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if(!file.is_open())
    {
        throw std::runtime_error("File not found");
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    fromString(content);
}

void JsonObject::toFile(const std::string& filename, int indent)
{
    std::ofstream file(filename);
    if(!file.is_open())
    {
        throw std::runtime_error("Unable to open file");
    }
    file << toString(indent);
}

}  // namespace util