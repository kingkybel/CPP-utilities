/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/json_object.h
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

#ifndef NS_UTIL_JSON_OBJECT_H_INCLUDED
#define NS_UTIL_JSON_OBJECT_H_INCLUDED

#include "json_index_key.h"
#include "json_key_path.h"
#include "json_string_key.h"

#include <boost/json.hpp>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

namespace util
{

class JsonObject
{
    public:
    JsonObject();
    JsonObject(const std::string& json_str);
    JsonObject(const boost::json::value& json_val);

    void                      fromString(const std::string& json_str);
    std::string               toString(int indent = 2) const;
    const boost::json::value& getJson() const;
    void                      set(const JsonKeyPath& path, const boost::json::value& value, bool force = false);
    void                      fromFile(const std::string& filename);
    void                      toFile(const std::string& filename, int indent = 4);

    private:
    boost::json::value json_;
};
}  // namespace util
#endif  // NS_UTIL_JSON_OBJECT_H_INCLUDED
