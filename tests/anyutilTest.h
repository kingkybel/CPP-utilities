/*
 * File:		anyutilTest.h
 * Description:         Unit tests for date utilities
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

#ifndef ANYUTILTEST_H
#define ANYUTILTEST_H

#include <cppunit/extensions/HelperMacros.h>

class anyutilTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(anyutilTest);

    CPPUNIT_TEST(util_any_test);
    CPPUNIT_TEST(util_any_interval_test);

    CPPUNIT_TEST_SUITE_END();

    public:
    anyutilTest();
    virtual ~anyutilTest();
    void setUp();
    void tearDown();

    private:
    void util_any_test();
    void util_any_interval_test();
};

#endif /* ANYUTILTEST_H */
