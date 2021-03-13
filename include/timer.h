/*
 * File:        timer.h
 * Description: High-precision timer for
 *
 * Copyright (C) 2020 Dieter J Kybelksties <github@kybelksties.com>
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
 * @date: 2020-11-14
 * @author: Dieter J Kybelksties
 */

#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <chrono> // for std::chrono functions

namespace util
{

    class timer
    {
    private:
        using clock_t = std::chrono::high_resolution_clock;
        using second_t = std::chrono::duration<double, std::ratio<1> >;
        using nanosecond_t = std::chrono::duration<double, std::ratio<1> >;

        std::chrono::time_point<clock_t> start_;

    public:

        void start()
        {
            start_ = clock_t::now();
        }

        double elapsed() const
        {
            return std::chrono::duration_cast<nanosecond_t>(clock_t::now() - start_).count();
        }
    };
}; // namespace util

#endif // TIMER_H_INCLUDED

