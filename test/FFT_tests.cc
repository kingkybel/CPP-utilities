/*
 * File:		FFTTest.cc
 * Description:         Unit tests for Fast Fourier Transform.
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

#include "FFT.h"
#include "stringutil.h"

#include <algorithm>
#include <cstdlib>
#include <gtest/gtest.h>
#include <iostream>
#include <random>
#include <string>

using namespace std;
using namespace util;

class FFTTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
        std::default_random_engine                     generator;
        std::uniform_real_distribution<FFT::FLOATTYPE> distribution(0.0L, 255.0);
        sampleVec.clear();
        for(int i = 0; i < 1024; i++)
        {
            FFT::FLOATTYPE val = distribution(generator);
            sampleVec.emplace_back(val);
        }
    }

    void TearDown() override
    {
    }

    private:
    vector<FFT::FLOATTYPE> sampleVec;
};

// 1.Input random data
TEST_F(FFTTest, random_data_test)
{
}

// 2.Inputs are all zeros
TEST_F(FFTTest, all_zeros_test)
{
}

// 3.Inputs are all ones (or some other nonzero value)
TEST_F(FFTTest, all_ones_test)
{
}

// 4.Inputs alternate between +1 and -1.
TEST_F(FFTTest, alternate_1_and_minus_1_test)
{
}

// 5.Input is e^(8*j*2*pi*i/N) for i = 0,1,2, ...,N-1. (j = sqrt(-1))
TEST_F(FFTTest, e_8_test)
{
}

// 6.Input is cos(8*2*pi*i/N) for i = 0,1,2, ...,N-1.
TEST_F(FFTTest, cos_8_test)
{
}

// 7.Input is e^((43/7)*j*2*pi*i/N) for i = 0,1,2, ...,N-1. (j = sqrt(-1))
TEST_F(FFTTest, e_43_7th_test)
{
}

// 8.Input is cos((43/7)*2*pi*i/N) for i = 0,1,2, ...,N-1.
TEST_F(FFTTest, cos_43_7th_test)
{
}

//
// B.Multi FFT tests - run continuous sets of random data
// 1.Data sets start at times 0, N, 2N, 3N, 4N, ....
// 2.Data sets start at times 0, N+1, 2N+2, 3N+3, 4N+4, ....
TEST_F(FFTTest, multi_test)
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
TEST_F(FFTTest, linearity_test)
{
}

// DFT of the unit impulse: A time-domain signal equal to the Kronecker delta
// function is applied to the input of the FFT algorithm and the output is checked
// against the known DFT of the unit impulse function (it transforms to a constant
// value in all output bins). If the FFT algorithm provides an IFFT, it can be
// tested in reverse to show that it yields the unit impulse function again.
TEST_F(FFTTest, impulse_test)
{
}

//    Time shift: Two sets of data are applied to the input of the FFT algorithm;
// the only difference between the two in the time domain is a constant time shift.
// Based on the known properties of the DFT, this should effect a known linear
// phase shift between the two signals' frequency domain representations, where
// the slope of the phase shift is proportional to the time shift.
// A.Single FFT tests - N inputs and N outputs
TEST_F(FFTTest, time_shift_test)
{
}
