/*
 * File:		performance_timer_tests.cc
 * Description: Unit tests for perfomance test utilities
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

#include "stringutil.h"
#define DO_PERFORMANCE_
#include "performance_timer.h"

#include <cmath>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <thread>

using namespace std;
using namespace util;

class TimerTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(TimerTest, correct_performance_measurement)
{
    #ifdef DO_PERFORMANCE_
    RESET_PERF
    auto& tmr = util::performance_timer::instance();
    ASSERT_EQ(tmr.get_stats().size(), 0UL);
    try
    {
        START_PERF
        for(int j = 0; j < 30; j++)
        {
            START_PERF
            for(int i = 0; i < 1000; i++)
            {
                int x = i * j;
                x     = x * x;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            END_PERF
        }
        END_PERF
    }
    catch(const std::exception& e)
    {
        FAIL() << "Well formed performance measurement should not have thrown";
    }
    ASSERT_EQ(tmr.get_stats().size(), 2UL);
    ASSERT_EQ(tmr.get_stack().size(), 0UL);
    #endif
}
