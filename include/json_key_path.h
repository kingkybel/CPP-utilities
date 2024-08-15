/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/json_key_path.h
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

#ifndef NS_UTIL_JSON_KEY_PATH_H_INCLUDED
#define NS_UTIL_JSON_KEY_PATH_H_INCLUDED

#include "json_index_key.h"
#include "json_string_key.h"

#include <memory>
#include <string>
#include <vector>

namespace util
{

class JsonKeyPath
{
    public:
    JsonKeyPath(const std::string& key_path);

    std::string                                  toString() const;
    const std::vector<std::shared_ptr<JsonKey>>& keyList() const;

    private:
    std::vector<std::shared_ptr<JsonKey>> list_of_keys_;
};

}  // namespace util
#endif  // NS_UTIL_JSON_KEY_PATH_H_INCLUDED
