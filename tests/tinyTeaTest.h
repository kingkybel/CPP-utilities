/*
 * File:		tinyTeaTest.h
 * Description:         Unit tests for tiny tea
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

#ifndef TINYTEATEST_H
#define TINYTEATEST_H

#include <cppunit/extensions/HelperMacros.h>

class tinyTeaTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(tinyTeaTest);

    CPPUNIT_TEST(encryption_test);

    CPPUNIT_TEST_SUITE_END();

    public:
    tinyTeaTest();
    virtual ~tinyTeaTest();
    void setUp();
    void tearDown();

    private:
    void encryption_test();
};

#endif /* TINYTEATEST_H */
