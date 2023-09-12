/*
 * File Name:   threadutil.h
 * Description: Utility for threads.
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

#ifndef NS_UTIL_THREADUTIL_H_INCLUDED
#define NS_UTIL_THREADUTIL_H_INCLUDED

#include <future>
#include <tuple>
#include <vector>
// #include <mutex>
// #include <thread>

namespace util
{
struct mutexed_object_base
{
    virtual void do_run() = 0;
    std::mutex mtx;
};

template <typename... MtxObjT_>
struct deferred_lock_barrier
{
    std::tuple<MtxObjT_...> objs;
    void synchronise()
    {
        std::vector<std::unique_lock<std::mutex>> locks;
        for(auto& i: std::index_sequence_for<MtxObjT_...>())
            locks.emplace_back(std::unique_lock<std::mutex>(std::get<i>(objs).mtx, std::defer_lock));

        
    }
};

/**
 * @brief future/prose implementation that is thread safe
 * 
 * @tparam Func function that may throw
 * @tparam Args variadic arument types
 * @param func function/function object
 * @param args function aruguments
 * @return std::future<decltype(func(std::forward<Args>(args)...))> a future that encapsulates the result, including any exception
 */
template<typename Func, typename... Args>
auto make_exception_safe_future(Func&& func, Args&&... args) -> std::future<decltype(func(std::forward<Args>(args)...))>
{
    using ResultType = decltype(func(std::forward<Args>(args)...));
    std::promise<ResultType> promise;
    auto                     future = promise.get_future();

    try
    {
        // Call the provided function and set the result in the promise
        promise.set_value(func(std::forward<Args>(args)...));
    }
    catch(const std::exception& ex)
    {
        // Handle exceptions and set the exception in the promise
        promise.set_exception(std::current_exception());
    }
    catch(...)
    {
        // Handle non-standard exceptions and set them in the promise
        promise.set_exception(std::current_exception());
    }

    return future;
}

};  // namespace util

#endif  // NS_UTIL_BIT_CONVERTER_H_INCLUDED
