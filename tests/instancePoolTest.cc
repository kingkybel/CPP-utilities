/*
 * File:		instancePoolTest.cc
 * Description:         Unit tests for the instant pool
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
 * @date: 2020-06-17
 * @author: Dieter J Kybelksties
 */

#include "instancePoolTest.h"

#include <instance_pool.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stringutil.h>

//#define DO_TRACE_
#include <traceutil.h>

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(instancePoolTest);

instancePoolTest::instancePoolTest()
{
}

instancePoolTest::~instancePoolTest()
{
}

void instancePoolTest::setUp()
{
}

void instancePoolTest::tearDown()
{
}

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

void instancePoolTest::unlimited_pool_test()
{
    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "Initial UnlimitedPool::getInstance()->get()" << UnlimitedPool::getInstance()->get() << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    // adds more elements
    UnlimitedPool::fill();

    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "After re-fill UnlimitedPool::getInstance()->get()" << UnlimitedPool::getInstance()->get() << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    // remove one elements
    UnlimitedPool::removeAnInstance();

    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "Removed one instance UnlimitedPool::getInstance()->get()" << UnlimitedPool::getInstance()->get()
                 << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    // remove all elements
    UnlimitedPool::clearPool();

    for(size_t i = 0; i < 5; i++)
    {
        try
        {
            cout << "Removed one instance UnlimitedPool::getInstance()->get()" << UnlimitedPool::getInstance()->get()
                 << endl;
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Should have thrown", true);
        }
        catch(exception &e)
        {
            cout << "Attempt to get instance from empty pool threw expectedly" << endl;
        }
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

void instancePoolTest::limited_pool_test()
{
    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "Initial LimitedPool::getInstance()->get()" << LimitedPool::getInstance()->get() << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    cout << "before refill" << endl;
    // try to add more elements
    try
    {
        LimitedPool::fill();
        CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Re-filling more than allowed number of instances should have thrown",
                                              false);
    }
    catch(...)
    {
        cout << "Expectedly thrown error when re-filling more than allowed number of instances:" << endl;
    }
    cout << "after refill" << endl;

    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "After re-fill LimitedPool::getInstance()->get(): " << LimitedPool::getInstance()->get() << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    // remove one elements
    LimitedPool::removeAnInstance();

    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << i << "/" << 20
                 << ": Removed one instance LimitedPool::getInstance()->get(): " << LimitedPool::getInstance()->get()
                 << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    cout << "before clean LimitedPool" << endl;
    // remove all elements
    LimitedPool::clearPool();

    cout << "after clean LimitedPool" << endl;

    for(size_t i = 0; i < 5; i++)
    {
        try
        {
            cout << "Removed one instance LimitedPool::getInstance()->get(): " << LimitedPool::getInstance()->get()
                 << endl;
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Should have thrown", true);
        }
        catch(...)
        {
            cout << "Attempt to get instance from empty pool threw expectedly" << endl;
        }
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

void instancePoolTest::singleton_pool_test()
{
    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "Initial SingletonPool::getInstance()->get(): " << SingletonPool::getInstance()->get() << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    try
    {
        // adds more elements
        SingletonPool::fill();
    }
    catch(const exception &e)
    {
        cout << "Attempt add another instance to singleton threw expectedly" << endl;
    }

    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "After re-fill SingletonPool::getInstance()->get(): " << SingletonPool::getInstance()->get()
                 << endl;
        }
        catch(...)
        {
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance should not have thrown", false);
        }
    }

    // remove one elements
    SingletonPool::clearInstance();

    for(size_t i = 0; i < 20; i++)
    {
        try
        {
            cout << "Removed one instance SingletonPool::getInstance()->get(): " << SingletonPool::getInstance()->get()
                 << endl;
            CPPUNIT_ASSERT_ASSERTION_FAIL_MESSAGE("Getting instance from empty singleton pool should have thrown",
                                                  true);
        }
        catch(...)
        {
            cout << "Attempt add an instance from empty singleton pool threw expectedly" << endl;
        }
    }
}
