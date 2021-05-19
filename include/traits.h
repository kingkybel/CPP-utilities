/*
 * File:            traits.h
 * Description:     Macros to define traits that can static_assert whether
 *                  static or non static member-functions are present in
 *                  a type.
 *
 * Copyright (C) 2021 Dieter J Kybelksties <github@kybelksties.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @date: 2021-03-13
 * @author: Dieter J Kybelksties
 */

#ifndef TRAITS_H_INCLUDED
#define TRAITS_H_INCLUDED

#define DEFINE_HAS_STATIC_MEMBER(traitsName, funcName, signature)                  \
    template<typename U>                                                           \
    class traitsName                                                               \
    {                                                                              \
        private:                                                                   \
        template<typename T, T>                                                    \
        struct helper;                                                             \
        template<typename T>                                                       \
        static std::uint8_t check(helper<signature, &funcName>*);                  \
        template<typename T>                                                       \
        static std::uint16_t check(...);                                           \
                                                                                   \
        public:                                                                    \
        static constexpr bool value = sizeof(check<U>(0)) == sizeof(std::uint8_t); \
    }

// usage:
// add this macro to your project with the first parameter the name
// you want to give the trait and the second the name of the member function
// whose presence you wish to assert and the third the signature of the
// static member
//
// DEFINE_HAS_STATIC_MEMBER(has_static_bool_fill, T::fill, bool (*)(void));
//
// struct test1
// {
//     static bool fill() { return false; }
// };
//
// struct test2
// {
//     static double fill() { return 0; } // wrong signature!
// };
//
// cout << has_static_bool_fill<test1>::value << endl; // --> 1
// cout << has_static_bool_fill<test2>::value << endl; // --> 0

// Primary template with a static assertion
// for a meaningful error message
// if it ever gets instantiated.
// We could leave it undefined if we didn't care.

#define DEFINE_HAS_MEMBER(TraitName, FunctionName)                                                    \
    template<typename, typename T>                                                                    \
    struct TraitName                                                                                  \
    {                                                                                                 \
        static_assert(std::integral_constant<T, false>::value,                                        \
                      "Second template parameter needs to be of function type.");                     \
    };                                                                                                \
                                                                                                      \
    /* specialization that does the checking */                                                       \
    template<typename C, typename Ret, typename... Args>                                              \
    struct TraitName<C, Ret(Args...)>                                                                 \
    {                                                                                                 \
        private:                                                                                      \
        template<typename T>                                                                          \
        static constexpr auto check(T*) ->                                                            \
         typename std::is_same<decltype(std::declval<T>().FunctionName(std::declval<Args>()...)),     \
                               Ret      /* ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/   \
                               >::type; /* attempt to call it and see if the return type is correct*/ \
                                                                                                      \
        template<typename>                                                                            \
        static constexpr std::false_type check(...);                                                  \
                                                                                                      \
        using type = decltype(check<C>(0));                                                           \
                                                                                                      \
        public:                                                                                       \
        static constexpr bool value = type::value;                                                    \
    };

// usage:
// add this macro to your project with the first parameter the name
// you want to give the trait and the second the name of the member function
// whose presence you wish to assert
//
// DEFINE_HAS_MEMBER(has_some_func, some_func)
//
// struct test1
// {
//     int some_func(std::string s) { return 0; }
// };
//
// struct test2
// {
//     double some_func(std::string s) { return 0; } // wrong signature!
// };
//
// cout << has_some_func<test1, int(std::string)>::value << endl; // --> 1
// cout << has_some_func<test2, int(std::string)>::value << endl; // --> 0

#endif  // TRAITS_H_INCLUDED
