/*
 * File:        performance_timer.h
 * Description: High-precision timer
 *
 * Copyright (C) 2023 Dieter J Kybelksties <github@kybelksties.com>
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

#include <chrono>
#include <exception>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>

#ifndef NS_UTIL_TIMER_H_INCLUDED
    #define NS_UTIL_TIMER_H_INCLUDED

namespace util
{
struct not_started : std::runtime_error
{
    not_started(const std::string& key = "<NO OPEN KEY>")
    : std::runtime_error(std::string("cannot find stats for for key '" + key + "'"))
    {
    }
};
class performance_timer
{
    public:
    using clock_t                                    = std::chrono::high_resolution_clock;
    using second_t                                   = std::chrono::duration<double, std::ratio<1>>;
    using nanosecond_t                               = std::chrono::duration<double, std::ratio<1>>;
    struct stats
    {
        int32_t                          start_line_     = -1;
        int32_t                          end_line_       = -1;
        std::chrono::time_point<clock_t> start_          = clock_t::now();
        std::chrono::time_point<clock_t> end_            = clock_t::now();
        size_t                           times_entered_  = 0UL;
        double                           aggegrate_time_ = 0.0;
    };

    private:
    performance_timer()                              = default;
    performance_timer(performance_timer&)            = delete;
    performance_timer& operator=(performance_timer&) = delete;

    std::unordered_map<std::string, stats> stat_map_{};
    std::stack<std::string>                marker_stack_{};

    public:
    /**
     * @brief Singleton instance.
     *
     * @return performance_timer& the one and only instance
     */
    static performance_timer& instance()
    {
        static auto the_instance = performance_timer{};
        return the_instance;
    }

    void reset()
    {
        stat_map_.clear();
        auto empty_stack = std::stack<std::string>{};
        marker_stack_.swap(empty_stack);
    }

    void start(const std::string& key, int32_t start_line)
    {
        auto found = stat_map_.find(key);
        if(found == stat_map_.end())
        {
            // insert an element
            auto emplaced = stat_map_.emplace(key, stats{});
            found         = emplaced.first;
        }

        found->second.start_line_ = start_line;
        found->second.start_      = clock_t::now();
        found->second.times_entered_++;
        marker_stack_.push(key);
    }

    void end(int32_t end_line)
    {
        if(marker_stack_.empty())
            throw util::not_started();

        auto key = marker_stack_.top();
        marker_stack_.pop();
        auto found = stat_map_.find(key);
        if(found == stat_map_.end())
            throw util::not_started(key);
        found->second.end_line_ = end_line;
        found->second.end_      = clock_t::now();
        found->second.aggegrate_time_ +=
         (std::chrono::duration_cast<nanosecond_t>(found->second.end_ - found->second.start_).count());
        stat_map_[key];
    }

    auto get_stats() const
    {
        return stat_map_;
    }

    auto get_stack() const
    {
        return marker_stack_;
    }

    friend std::ostream& operator<<(std::ostream& os, const util::performance_timer& tmr)
    {
        for(const auto& stat: tmr.get_stats())
        {
            os << stat.first << std::endl;
            os << "\tlines:          " << stat.second.start_line_ << "->" << stat.second.end_line_ << std::endl;
            os << "\tnum entered:    " << stat.second.times_entered_ << std::endl;
            os << "\taggregate time: " << stat.second.aggegrate_time_ << std::endl;
            os << "\taverage_time:   " << stat.second.aggegrate_time_ / static_cast<double>(stat.second.times_entered_)
               << std::endl;
        }
        return os;
    }
};
    #if defined DO_PERFORMANCE_
        #define RESET_PERF                                             \
            {                                                          \
                auto& the_timer = util::performance_timer::instance(); \
                the_timer.reset();                                     \
            }

        #define START_PERF                                                              \
            {                                                                           \
                auto&             the_timer = util::performance_timer::instance();      \
                std::stringstream ss;                                                   \
                ss << __FILE__ << ":" << __LINE__ << "(" << __PRETTY_FUNCTION__ << ")"; \
                the_timer.start(ss.str(), __LINE__);                                    \
            }

        #define END_PERF                                               \
            {                                                          \
                auto& the_timer = util::performance_timer::instance(); \
                the_timer.end(__LINE__);                               \
            }
    #else
        #define RESET_PERF //
        #define START_PERF //
        #define END_PERF //
    #endif  // defined DO_PERFORMANCE_
};          // namespace util

#endif  // NS_UTIL_TIMER_H_INCLUDED
