/*
 * File Name:   type_bracket_map.h
 * Description: Brackets mapped per type for streaming output
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
 * @date: 2019-04-25
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_TYPE_BRACKET_MAP_H_INCLUDED
#define	NS_UTIL_TYPE_BRACKET_MAP_H_INCLUDED

#include <typeinfo>
#include <string>
#include <unordered_map>
#include <brackets.h>
namespace util
{

    class type_bracket_map
    {
    private:
        std::unordered_map <std::string, Brackets> type2brackets;
        static type_bracket_map theInstance;

        type_bracket_map()
        {

        }
    public:

        bool empty()
        {
            return type2brackets.empty();
        }

        void initialize()
        {
            type2brackets.erase(type2brackets.begin(), type2brackets.end());
            type2brackets["none"] = Brackets(Brackets::NONE);
            type2brackets["vector"] = Brackets(Brackets::CHEFRON);
            type2brackets["deque"] = Brackets("(*", " < ", "*)");
            type2brackets["unordered_set"] = Brackets("{~", " ", "~}");
            type2brackets["pair"] = Brackets("(", "->", ")");
            type2brackets["unordered_map"] = Brackets("{~", " ", "~}");
            type2brackets["map"] = Brackets(Brackets::BRACKET);
            type2brackets["set"] = Brackets(Brackets::BRACE);
        }

        static type_bracket_map& instance()
        {
            if (type_bracket_map::theInstance.empty())
            {
                type_bracket_map::theInstance.initialize();
            }
            return type_bracket_map::theInstance;
        }

        template<typename T>
        void add(Brackets bracket)
        {
            type2brackets[typeid (T).name()] = bracket;
        }

        template<typename T>
        const Brackets& get() const
        {
            if (type2brackets.find(typeid (T).name()) == type2brackets.end())
                return type2brackets["none"];
            return type2brackets[typeid (T).name()];
        }

        const Brackets& get(const std::string& name) const
        {
            if (type2brackets.find(name) == type2brackets.end())
                return type2brackets["none"];
            return type2brackets[name];
        }

    };
}; // namespace util

#endif	// NS_UTIL_TYPE_BRACKET_MAP_H_INCLUDED

