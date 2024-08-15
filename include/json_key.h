/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/json_key.h
 * Description: abstract base class for json keys
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

#ifndef NS_UTIL_JSON_KEY_H_INCLUDED
#define NS_UTIL_JSON_KEY_H_INCLUDED
#include <string>

class JsonKey
{
    public:
    virtual ~JsonKey()                                 = default;
    [[nodiscard]] virtual std::string toString() const = 0;
};

#endif  // NS_UTIL_JSON_KEY_H_INCLUDED
