/*
 * Copyright (C) 2019 Dieter J Kybelksties
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
 * @date: 2020-11-08
 * @author: Dieter J Kybelksties
 */

#include "primesTest.h"

#include <complex>
#include <initializer_list>
#include <ios>
#include <iostream>
#include <primes.h>
#define IN_DEVELOPMENT
#ifdef IN_DEVELOPMENT
    #define TEST_HEADER(tp)                                                           \
        {                                                                             \
            std::cout << __PRETTY_FUNCTION__ << " " << #tp << "=" << tp << std::endl; \
        }
#else
    #define TEST_HEADER(tp)
#endif

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(primesTest);

primesTest::primesTest()
{
}

primesTest::~primesTest()
{
}

void primesTest::setUp()
{
}

void primesTest::tearDown()
{
}

void primesTest::testPrimesConstruction()
{
    TEST_HEADER("construct");
}
