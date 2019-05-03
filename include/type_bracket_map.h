/*
 * File:   type_bracket_map.h
 * Author: Dieter J Kybelksties
 *
 * Created on May 2, 2019, 2:31 PM
 */

#ifndef TYPE_BRACKET_MAP_H
#define	TYPE_BRACKET_MAP_H

#include <typeinfo>
#include <string>
#include <tr1/unordered_map>
#include <brackets.h>
namespace util
{

    class type_bracket_map
    {
    private:
        std::tr1::unordered_map <std::string, Brackets> type2brackets;
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
};
#endif	// TYPE_BRACKET_MAP_H

