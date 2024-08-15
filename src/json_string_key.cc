/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   src/json_string_key.cc
 * Description: string key for json objects
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
#include "json_string_key.h"

#include "json_exceptions.h"

namespace util
{
JsonStringKey::JsonStringKey(const std::string& key_str)
{
    if(key_str.empty() || key_str.find_first_of(" \t\n[]\"") != std::string::npos)
    {
        throw JsonStringKeyError(key_str);
    }
    key_ = key_str;
}

std::string JsonStringKey::toString() const
{
    return key_;
}

}  // namespace util