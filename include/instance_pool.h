/*
 * File:		instance_pool.h
 * Description:
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

#ifndef INSTANCE_POOL_H_INCLUDED
#define INSTANCE_POOL_H_INCLUDED

#include "traits.h"

#include <exception>
#include <memory>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_set>

namespace util
{
// create a trait with which we can check, that a derived class has
// a "static bool fill()" method
DEFINE_HAS_STATIC_MEMBER(has_static_bool_fill, T::fill, bool (*)(void));

/**
 * A class to contain a number of instances.
 * exactly one instance: singleton
 * limited with a minimum and a maximum of instances
 * unlimited number of instances, at least one
 *
 * Intended to be derived from. Derived classes are responsible to manage
 * the pool of instances, so they should make all constructors private and
 * use the protected static members to add/remove/ clear instances and not
 * expose those functions indirectly.
 */
template<typename T_, size_t maxInstances = 0, size_t minInstances = 1>
struct InstancePool
{
    using ObjectPointer           = std::shared_ptr<T_>;
    using ObjectContainerType     = std::unordered_set<ObjectPointer>;
    using ObjectContainerIterator = typename ObjectContainerType::iterator;

    private:
    static ObjectContainerType     avail_;
    static ObjectContainerIterator current_;

    protected:
    static void addInstance(ObjectPointer pObject)
    {
        if(avail_.find(pObject) == avail_.end())
        {
            if(maxInstances > 0 && avail_.size() == maxInstances)
            {
                std::stringstream ss;
                ss << "Instance pool has reached the maximum number " << maxInstances << " of instances in class '"
                   << typeid(T_).name() << "'";
                throw std::overflow_error(ss.str());
            }

            avail_.insert(pObject);
            current_ = avail_.begin();
        }
    }

    static void removeInstance(ObjectPointer pObject)
    {
        auto found = avail_.find(pObject);

        if(found != avail_.end())
        {
            avail_.erase(found);
        }

        current_ = avail_.begin();
    }

    static void removeFrontInstance()
    {
        if(avail_.size() > 0)
        {
            avail_.erase(*avail_.begin());
        }

        current_ = avail_.begin();
    }

    static void clear()
    {
        avail_.clear();
    }

    public:
    /**
     * Retrieve an instance from the pool in round- robin-fashion, in case
     * of multiple instance.
     *
     * @return an instance from the pool
     */
    static ObjectPointer getInstance()
    {
        static_assert((maxInstances == 0 || minInstances <= maxInstances),
                      "Minimal instance number must be less or equal to maximal instance number");

        // make sure derived classes implement "static bool fill(){...}"
        // where the pool of instances is filled
        // with at least minInstances elements and at most
        // maxInstances elements

        static_assert(has_static_bool_fill<T_>::value,
                      "InstancePool<T_> derived class needs static member 'bool fill()");

        [[maybe_unused]]static bool   isFilled = T_::fill();
        //static size_t index    = avail_.size() - 1;
        if(avail_.size() < minInstances)
        {
            std::stringstream ss;
            ss << "Instance pool of '" << std::string(typeid(T_).name()) << "' [" << minInstances << "..";

            if(maxInstances == 0)
                ss << "∞";
            else
                ss << maxInstances;

            ss << "] instances, but found " << avail_.size() << ".";
            throw std::overflow_error(ss.str());
        }

        current_++;

        if(current_ == avail_.end())
            current_ = avail_.begin();

        return (*current_);
    }

    /**
     * Retrieves the number of the currently available instances.
     *
     * @return the size of the pool
     */
    static size_t size()
    {
        return (avail_.size());
    }

    /**
     * Sanity check whether the number of instances in the pool is within the
     * required limits.
     *
     * @return true, if so, false otherwise
     */
    static bool hasRequiredInstances()
    {
        return ((avail_.size() >= minInstances) && (avail_.size() <= maxInstances));
    }

    /**
     * Check whether the pool is empty.
     *
     * @return  true if so, false otherwise
     */
    static bool empty()
    {
        return (avail_.size() == 0UL);
    }
};

// define static members
template<typename T_, size_t maxInstances, size_t minInstances>
typename InstancePool<T_, maxInstances, minInstances>::ObjectContainerType
 InstancePool<T_, maxInstances, minInstances>::avail_;

template<typename T_, size_t maxInstances, size_t minInstances>
typename InstancePool<T_, maxInstances, minInstances>::ObjectContainerIterator
 InstancePool<T_, maxInstances, minInstances>::current_;

/**
 * A singleton is an instance-pool with exactly one contained instance.
 */
template<typename T_>
struct Singleton : public InstancePool<T_, 1, 1>
{
};

};
// namespace util

#endif  // INSTANCE_POOL_H_INCLUDED
