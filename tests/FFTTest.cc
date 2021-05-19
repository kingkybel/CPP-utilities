/*
 * File:		FFTTest.cc
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

#include "FFTTest.h"

#include <FFT.h>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <stringutil.h>

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(FFTTest);

static vector<FFT::FLOATTYPE> sampleVec;

FFTTest::FFTTest()
{
}

FFTTest::~FFTTest()
{
}

void FFTTest::setUp()
{
    std::default_random_engine                     generator;
    std::uniform_real_distribution<FFT::FLOATTYPE> distribution(0.0L, 255.0);
    sampleVec.clear();
    generate_n(sampleVec.begin(), 1024, [&]() { return distribution(generator); });
}

void FFTTest::tearDown()
{
}

// 1.Input random data
void FFTTest::random_data_test()
{
}

// 2.Inputs are all zeros
void FFTTest::all_zeros_test()
{
}

// 3.Inputs are all ones (or some other nonzero value)
void FFTTest::all_ones_test()
{
}

// 4.Inputs alternate between +1 and -1.
void FFTTest::alternate_1_and_minus_1_test()
{
}

// 5.Input is e^(8*j*2*pi*i/N) for i = 0,1,2, ...,N-1. (j = sqrt(-1))
void FFTTest::e_8_test()
{
}

// 6.Input is cos(8*2*pi*i/N) for i = 0,1,2, ...,N-1.
void FFTTest::cos_8_test()
{
}

// 7.Input is e^((43/7)*j*2*pi*i/N) for i = 0,1,2, ...,N-1. (j = sqrt(-1))
void FFTTest::e_43_7th_test()
{
}

// 8.Input is cos((43/7)*2*pi*i/N) for i = 0,1,2, ...,N-1.
void FFTTest::cos_43_7th_test()
{
}

//
// B.Multi FFT tests - run continuous sets of random data
// 1.Data sets start at times 0, N, 2N, 3N, 4N, ....
// 2.Data sets start at times 0, N+1, 2N+2, 3N+3, 4N+4, ....
void FFTTest::multi_test()
{
}

//
// Linearity: The DFT (along with its other cousin transforms in the Fourier
// family) is a linear operator, so for all values of a1,a2,x1[n],x2[n]
//
//    , the following equation must hold:
//
// FFT(a1x1[n]+a2x2[n])=a1FFT(x1[n])+a2FFT(x2[n])
//
void FFTTest::linearity_test()
{
}

// DFT of the unit impulse: A time-domain signal equal to the Kronecker delta
// function is applied to the input of the FFT algorithm and the output is checked
// against the known DFT of the unit impulse function (it transforms to a constant
// value in all output bins). If the FFT algorithm provides an IFFT, it can be
// tested in reverse to show that it yields the unit impulse function again.
void FFTTest::impulse_test()
{
}

//    Time shift: Two sets of data are applied to the input of the FFT algorithm;
// the only difference between the two in the time domain is a constant time shift.
// Based on the known properties of the DFT, this should effect a known linear
// phase shift between the two signals' frequency domain representations, where
// the slope of the phase shift is proportional to the time shift.
// A.Single FFT tests - N inputs and N outputs
void FFTTest::time_shift_test()
{
}
