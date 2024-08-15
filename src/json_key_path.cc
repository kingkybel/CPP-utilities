/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   src/json_key_path.cc
 * Description: slash separated path to a key in a json object
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

#include "json_key_path.h"

#include <sstream>

namespace util
{
JsonKeyPath::JsonKeyPath(const std::string& key_path)
{
    std::stringstream ss(key_path);
    std::string       token;
    while(std::getline(ss, token, '/'))
    {
        if(token.front() == '[' && token.back() == ']')
        {
            list_of_keys_.push_back(std::make_shared<JsonIndexKey>(token.substr(1, token.size() - 2)));
        }
        else
        {
            list_of_keys_.push_back(std::make_shared<JsonStringKey>(token));
        }
    }
}

std::string JsonKeyPath::toString() const
{
    std::string result;
    for(const auto& key: list_of_keys_)
    {
        result += key->toString() + "/";
    }
    if(!result.empty())
        result.pop_back();
    return result;
}

const std::vector<std::shared_ptr<JsonKey>>& JsonKeyPath::keyList() const
{
    return list_of_keys_;
}

}  // namespace util