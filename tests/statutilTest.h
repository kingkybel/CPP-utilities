/*
 * File:		statutilTest.h
 * Description:         Unit tests for statistics utilities
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

#ifndef GRAPHUTILTEST_H
#define GRAPHUTILTEST_H

#include <cppunit/extensions/HelperMacros.h>

class statutilTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(statutilTest);

    CPPUNIT_TEST(util_stat_test);
    CPPUNIT_TEST(util_event_operation_test);
    CPPUNIT_TEST(util_event_test);
    CPPUNIT_TEST(util_continuous_stat_test);

    CPPUNIT_TEST_SUITE_END();

    public:
    statutilTest();
    virtual ~statutilTest();
    void setUp();
    void tearDown();

    private:
    void util_stat_test();
    void util_event_operation_test();
    void util_event_test();
    void util_continuous_stat_test();
};

#endif /* GRAPHUTILTEST_H */
