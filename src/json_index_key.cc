/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   src/json_index_key.cc
 * Description: index key for json collections
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

#include "json_index_key.h"

#include "json_exceptions.h"

namespace util
{
JsonIndexKey::JsonIndexKey(const std::string& index_str) : isStart_(false), isEnd_(false)
{
    if(index_str == "^")
    {
        isStart_ = true;
    }
    else if(index_str == "$")
    {
        isEnd_ = true;
    }
    else
    {
        try
        {
            index_ = std::stoi(index_str);
            if(index_ < 0)
                throw JsonIndexKeyError(index_str);
        }
        catch(...)
        {
            throw JsonIndexKeyError(index_str);
        }
    }
}

bool JsonIndexKey::isStart() const
{
    return isStart_;
}

bool JsonIndexKey::isEnd() const
{
    return isEnd_;
}

std::string JsonIndexKey::toString() const
{
    if(isStart())
        return "[^]";
    if(isEnd())
        return "[$]";
    return "[" + std::to_string(index_) + "]";
}

}  // util