/*
 * File:		instancePoolTest.h
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

#ifndef CSVUTILTEST_H
#define CSVUTILTEST_H

#include <cppunit/extensions/HelperMacros.h>

class instancePoolTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(instancePoolTest);

    CPPUNIT_TEST(unlimited_pool_test);
    CPPUNIT_TEST(limited_pool_test);
    CPPUNIT_TEST(singleton_pool_test);

    CPPUNIT_TEST_SUITE_END();

public:
    instancePoolTest();
    virtual ~instancePoolTest();
    void setUp();
    void tearDown();

private:
    void unlimited_pool_test();
    void limited_pool_test();
    void singleton_pool_test();
};

#endif /* CSVUTILTEST_H */

