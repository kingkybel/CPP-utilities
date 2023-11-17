/*
 * File Name:   container_convert.h
 * Description: Utilities for converting one container into another.
 *
 * Copyright (C) 2023 Dieter J Kybelksties
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
 * @date: 2023-08-28
 * @author: Dieter J Kybelksties
 */

#ifndef CONTAINER_CONVERT_H_INCLUDED
#define CONTAINER_CONVERT_H_INCLUDED

#include "traits.h"

#include <algorithm>
#include <deque>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace util
{
/**
 * @brief a predicate that is always true.
 *
 * @tparam Value_ @tparam Value_  type of the value
 * @return true always, false never
 */
template<typename Value_>
struct true_pred
{
    bool operator()(Value_)
    {
        return true;
    }
};

/**
 * @brief Erase remove from forward-iterable containers
 * 
 * @tparam Container type of container
 * @tparam Predicate predicate
 * @param container the container to remove from
 * @param pred predicate to decide which elements to remove
 */
template<typename Container, typename Predicate>
void eraseRemove(Container &container, Predicate pred)
{
    container.erase(std::remove_if(container.begin(), container.end(), pred), container.end());
}

/**
 * @brief Remove key-value-pairs from a map, if the key matches the given predicate.
 *
 * @tparam Key_ key-type
 * @tparam Value_ value-type
 * @tparam Compare_ comparison function
 * @tparam Alloc_ allocator for map-elements
 * @tparam KeyPred_ predicate function for keys
 * @param map2filter map-bject
 * @param pred key-predicate on which to filter
 */
template<typename Key_, typename Value_, typename Compare_, typename Alloc_, typename KeyPred_>
void eraseByKey(std::map<Key_, Value_, Compare_, Alloc_> &map2filter, KeyPred_ pred = true_pred<Key_>{})
{
    auto it = map2filter.begin();
    while(it != map2filter.end())
    {
        if(pred(it->first))
        {
            it = map2filter.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // since C++-11 this does no longer work because of explicitly removed assignment for pairs:
    // map2filter.erase(std::remove_if(map2filter.begin(), map2filter.end(), removeCondition), map2filter.end());
}

/**
 * @brief Remove key-value-pairs from a map, if the value matches the given predicate.
 *
 * @tparam Key_ key-type
 * @tparam Value_ value-type
 * @tparam Compare_ comparison function
 * @tparam Alloc_ allocator for map-elements
 * @tparam KeyPred_ predicate function for keys
 * @param map2filter map-bject
 * @param pred value-predicate on which to filter
 */
template<typename Key_, typename Value_, typename Compare_, typename Alloc_, typename ValuePred_>
void eraseByValue(std::map<Key_, Value_, Compare_, Alloc_> &map2filter, ValuePred_ pred = true_pred<Value_>{})
{
    auto it = map2filter.begin();
    while(it != map2filter.end())
    {
        if(pred(it->second))
        {
            it = map2filter.erase(it);
        }
        else
        {
            ++it;
        }
    }
    // since C++-11 this does no longer work because of explicitly removed assignmentfor pairs:
    // map2filter.erase(std::remove_if(map2filter.begin(), map2filter.end(), removeCondition), map2filter.end());
}

/**
 * @brief  Function to move elements of one vector meeting the predicate to the end of another vector
 *
 * @tparam Value_  type of the container elements
 * @tparam Alloc_  allocator function
 * @tparam Pred_ predicate function
 * @param source source vectoe
 * @param destination destination vector
 * @param pred precicate object
 */
template<typename Value_, typename Alloc_, typename Pred_ = true_pred<Value_>>
void moveElementsTo(std::vector<Value_, Alloc_> &source, std::vector<Value_, Alloc_> &destination, Pred_ pred = Pred_{})
{
    for(auto it = source.begin(); it != source.end();)
    {
        if(pred(*it))
        {
            destination.push_back(*it);
            it = source.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

/**
 * @brief Create a vector from a given set.
 *
 * @tparam Value_ the value type
 * @tparam Compare_ comparison function
 * @tparam Alloc_  allocator function
 * @param orderedSet  the ordered set to convert from
 * @return std::vector<Value_, Alloc_>  a vector with all the elements of the ordered set in the same order
 */
template<typename Value_, typename Compare_, typename Alloc_>
inline std::vector<Value_, Alloc_> toVector(const std::set<Value_, Compare_, Alloc_> &orderedSet)
{
    std::vector<Value_, Alloc_> reval;

    if(!orderedSet.empty())
        std::copy(orderedSet.begin(), orderedSet.end(), std::back_inserter(reval));

    return (reval);
}

/**
 * @brief Create a vector from a given double ended queue.
 *
 * @tparam Value_  type of the container elements
 * @tparam Alloc_  allocator function
 * @param que the deque to convert from
 * @return std::vector<Value_, Alloc_> a vector with all the elements of the deque in the same order
 */
template<typename Value_, typename Alloc_>
inline std::vector<Value_, Alloc_> toVector(const std::deque<Value_, Alloc_> &que)
{
    std::vector<Value_, Alloc_> reval;

    if(!que.empty())
        std::copy(que.begin(), que.end(), std::back_inserter(reval));

    return (reval);
}

/**
 * @brief Create a double ended queue from a given vector.
 *
 * @tparam Value_  type of the container elements
 * @tparam Alloc_  allocator function
 * @param vec the vector to convert from
 * @return std::deque<Value_, Alloc_> a vector with all the elements of the deque in the same order
 */
template<typename Value_, typename Alloc_>
inline std::deque<Value_, Alloc_> toDeque(const std::vector<Value_, Alloc_> &vec)
{
    std::deque<Value_, Alloc_> reval;

    if(!vec.empty())
        std::copy(vec.begin(), vec.end(), std::back_inserter(reval));

    return (reval);
}

/**
 * @brief Create a set from a given vector (will remove duplicates).
 *
 * @tparam Value_  type of the container elements
 * @tparam Alloc_  allocator function
 * @param vec the vector with comparable elements
 * @return std::set<Value_, std::less<Value_>, Alloc_>
 */
template<typename Value_, typename Alloc_>
inline std::set<Value_, std::less<Value_>, Alloc_> toSet(const std::vector<Value_, Alloc_> &vec)
{
    static_assert(util::has_operator_less<Value_>::value, "Ordered set elements must be less-than comparable");
    static_assert(util::has_operator_equal<Value_>::value, "Ordered set elements must be equal comparable");

    std::set<Value_, std::less<Value_>, Alloc_> reval;

    if(!vec.empty())
        std::copy(vec.begin(), vec.end(), std::inserter(reval, reval.begin()));

    return (reval);
}

/**
 * @brief Create a standard (ordered) set from a given unordered set.
 *
 * @tparam Value_  type of the container elements
 * @tparam Hash_ hash function
 * @tparam Pred_ predicate function
 * @tparam Alloc_  allocator function
 * @param uSet the unordered set of comparable elements to convert from
 * @return std::set<Value_, std::less<Value_>, Alloc_> a set of all unique elements of the vector
 */
template<typename Value_, typename Hash_, typename Pred_, typename Alloc_>
inline std::set<Value_, std::less<Value_>, Alloc_> toSet(const std::unordered_set<Value_, Hash_, Pred_, Alloc_> &uSet)
{
    static_assert(util::has_operator_less<Value_>::value, "Ordered set elements must be less-than comparable");
    static_assert(util::has_operator_equal<Value_>::value, "Ordered set elements must be equal comparable");

    std::set<Value_, std::less<Value_>, Alloc_> reval;

    if(!uSet.empty())
        std::copy(uSet.begin(), uSet.end(), std::inserter(reval, reval.begin()));

    return (reval);
}

/**
 * @brief Create a standard (ordered by key) map from a given unordered (hash-) map.
 *
 * @tparam Key_ type of the map-keys
 * @tparam Value_ type of the map-values
 * @tparam Hash_ hash function
 * @tparam Pred_ predicate function
 * @tparam Alloc_ allocator function
 * @param uMap the unordered set of comparable elements to convert from
 * @return std::map<Key_, Value_, std::less<Key_>, Alloc_> an ordered key/value map
 */
template<typename Key_, typename Value_, typename Hash_, typename Pred_, typename Alloc_>
inline std::map<Key_, Value_, std::less<Key_>, Alloc_>
 toMap(const std::unordered_map<Key_, Value_, Hash_, Pred_, Alloc_> &uMap)
{
    static_assert(util::has_operator_less<Key_>::value, "Ordered map keys must be less-than comparable");
    static_assert(util::has_operator_equal<Key_>::value, "Ordered map keys must be equal comparable");

    std::map<Key_, Value_, std::less<Key_>, Alloc_> reval;

    if(!uMap.empty())
        for(const auto &kv: uMap)
            reval[kv.first] = kv.second;

    return (reval);
}

/**
 * @brief Create a standard (ordered by key) set of keys from a given unordered (hash-) map. Can only be instantiated if
 * the key is comparable.
 *
 * @tparam Key_ type of the map-keys
 * @tparam Value_ type of the map-values
 * @tparam Hash_ hash function
 * @tparam Pred_ predicate function
 * @tparam Alloc_ allocator function
 * @tparam AllocRet_ allocator function for return set
 * @param uMap the unordered set of comparable elements to convert from
 * @return std::set<Key_, std::less<Key_>, Alloc_> an ordered set of all keys of the unordered map
 */
template<typename Key_,
         typename Value_,
         typename Hash_,
         typename Pred_,
         typename Alloc_,
         typename AllocRet_ = std::allocator<Key_>>
inline std::set<Key_, std::less<Key_>, AllocRet_>
 toOrderedKeySet(const std::unordered_map<Key_, Value_, Hash_, Pred_, Alloc_> &uMap)
{
    static_assert(util::has_operator_less<Key_>::value, "Ordered set elements need to be less-than comparable");
    static_assert(util::has_operator_equal<Key_>::value, "Ordered set elements keys need to be equal comparable");

    std::set<Key_, std::less<Key_>, AllocRet_> reval;

    if(!uMap.empty())
        for(const auto &kv: uMap)
            reval.insert(kv.first);

    return (reval);
}
};  // namepace util

#endif  // CONTAINER_CONVERT_H_INCLUDED
