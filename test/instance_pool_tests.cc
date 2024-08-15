/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/instance_pool_tests.cc
 * Description: Unit tests for the instant pool
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

#include "instance_pool.h"
#include "stringutil.h"
// #define DO_TRACE_
#include "traceutil.h"

#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace util;

class InstancePoolTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

struct UnlimitedPool : public InstancePool<UnlimitedPool>
{
    private:
    int        i;
    static int counter;

    UnlimitedPool(int i_ = 0) : i(i_)
    {
    }

    public:
    static bool fill()
    {
        for(int i = 0; i < 5; i++)
        {
            try
            {
                ObjectPointer p = ObjectPointer(new UnlimitedPool(counter % 2 == 0 ? counter : -counter));
                counter++;
                addInstance(p);
            }
            catch(...)
            {
                return false;
            }
        }

        return true;
    }

    static void removeAnInstance()
    {
        if(!empty())
        {
            auto inst = getInstance();
            removeInstance(inst);
        }
    }

    static void clearPool()
    {
        clear();
    }

    int get()
    {
        return i;
    }
};
int UnlimitedPool::counter = 0;

TEST_F(InstancePoolTest, unlimited_pool_test)
{
    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(UnlimitedPool::getInstance()->get());
    }

    // adds more elements
    UnlimitedPool::fill();

    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(UnlimitedPool::getInstance()->get());
    }

    // remove one elements
    UnlimitedPool::removeAnInstance();

    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(UnlimitedPool::getInstance()->get());
    }

    // remove all elements
    UnlimitedPool::clearPool();

    for(size_t i = 0; i < 5; i++)
    {
        ASSERT_THROW(UnlimitedPool::getInstance()->get(), std::overflow_error);
    }
}

struct LimitedPool : public InstancePool<LimitedPool, 8, 3>
{
    private:
    int        i_;
    static int counter;

    LimitedPool(int i = 0) : i_(i)
    {
    }

    public:
    static bool fill()
    {
        for(int i = 0; i < 5; i++)
        {
            try
            {
                ObjectPointer p = ObjectPointer(new LimitedPool(counter % 2 == 0 ? counter : -counter));
                counter++;
                addInstance(p);
            }
            catch(...)
            {
                return false;
            }
        }

        return true;
    }

    static void removeAnInstance()
    {
        if(!empty())
        {
            removeFrontInstance();
        }
    }

    static void clearPool()
    {
        clear();
    }

    int get()
    {
        return i_;
    }
};

int LimitedPool::counter = 0;

TEST_F(InstancePoolTest, limited_pool_test)
{
    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(LimitedPool::getInstance()->get());
    }

    ASSERT_NO_THROW(LimitedPool::fill());

    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(LimitedPool::getInstance()->get());
    }

    // remove one elements
    LimitedPool::removeAnInstance();

    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(LimitedPool::getInstance()->get());
    }

    // remove all elements
    LimitedPool::clearPool();

    for(size_t i = 0; i < 5; i++)
    {
        ASSERT_THROW(LimitedPool::getInstance()->get(), std::overflow_error);
    }
}

struct SingletonPool : public Singleton<SingletonPool>
{
    private:
    std::string s_;

    SingletonPool(const std::string &s = "") : s_(s)
    {
    }

    public:
    static bool fill()
    {
        ObjectPointer p = ObjectPointer(new SingletonPool("Hello"));
        addInstance(p);

        return true;
    }

    static void clearInstance()
    {
        clear();
    }

    std::string get() const
    {
        return s_;
    }
};

TEST_F(InstancePoolTest, singleton_pool_test)
{
    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(SingletonPool::getInstance()->get());
    }

    ASSERT_THROW(SingletonPool::fill(), std::overflow_error);

    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_NO_THROW(SingletonPool::getInstance()->get());
    }

    // remove one elements
    SingletonPool::clearInstance();

    for(size_t i = 0; i < 20; i++)
    {
        ASSERT_THROW(SingletonPool::getInstance()->get(), std::overflow_error);
    }
}
