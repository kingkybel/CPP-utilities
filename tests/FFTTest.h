/*
 * File:		FFTTest.h
 * Description:         Unit tests for Fast Fourier Transform.
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

#ifndef FFT_TEST_H_INCLUDED
#define FFT_TEST_H_INCLUDED

#include <cppunit/extensions/HelperMacros.h>

class FFTTest : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(FFTTest);

    CPPUNIT_TEST(random_data_test);
    CPPUNIT_TEST(all_zeros_test);
    CPPUNIT_TEST(all_ones_test);
    CPPUNIT_TEST(alternate_1_and_minus_1_test);
    CPPUNIT_TEST(e_8_test);
    CPPUNIT_TEST(cos_8_test);
    CPPUNIT_TEST(e_43_7th_test);
    CPPUNIT_TEST(cos_43_7th_test);
    CPPUNIT_TEST(multi_test);
    CPPUNIT_TEST(linearity_test);
    CPPUNIT_TEST(impulse_test);
    CPPUNIT_TEST(time_shift_test);

    CPPUNIT_TEST_SUITE_END();

    public:
    FFTTest();
    virtual ~FFTTest();
    void setUp();
    void tearDown();

    private:
    void random_data_test();
    void all_zeros_test();
    void all_ones_test();
    void alternate_1_and_minus_1_test();
    void e_8_test();
    void cos_8_test();
    void e_43_7th_test();
    void cos_43_7th_test();
    void multi_test();
    void linearity_test();
    void impulse_test();
    void time_shift_test();
};

#endif  // FFT_TEST_H_INCLUDED
