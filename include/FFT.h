/*
 * File Name:   FFT.h
 * Description: Fast Fourier implementation
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
 * @date: 2019-04-30
 * @author: Dieter J Kybelksties
 */

#include <cmath>
#include <complex>
#include <vector>
#include <algorithm>
#include <assert.h>
#include <boost/math/constants/constants.hpp>
#include "contants.h"

#if !defined(NS_UTIL_FFT_H_INCLUDED)
#define NS_UTIL_FFT_H_INCLUDED

namespace util
{

    class FFT
    {
    public:
        typedef long long INTTYPE;
        typedef std::vector<INTTYPE> INTVECTOR;
        typedef long double FLOATTYPE;
        typedef std::vector<FLOATTYPE> FLOATVECTOR;
        typedef std::complex<FLOATTYPE> COMPLEXVALUE;
        typedef std::vector<COMPLEXVALUE> COMPLEXVECTOR;
        typedef std::vector<COMPLEXVECTOR> COMPLEXMATRIX;

    private:
        static FLOATTYPE PI;

    public:

        /**
         * Default constructor.
         * @param logOfNumOfPoints number of points must be a power of 2, so we
         *                         pass the log_2(numPoints) and calculate it
         * @param sampleRate the rate at which the input will be sampled
         * @param calibrate whether or not to calibrate on a 1kHz wave
         */
        FFT(INTTYPE logOfNumOfPoints = 10,
            INTTYPE sampleRate = 1024,
            bool calibrate = false)
        : logOfPoints_(logOfNumOfPoints)
        , numOfPoints_(powTwo[logOfNumOfPoints])
        , sampleRate_(sampleRate)
        {
            tapeOfDoubles_.resize(numOfPoints_);
            if (calibrate)
            {
                // 1 kHz calibration wave
                for (INTTYPE i = 0; i < numOfPoints_; i++)
                    tapeOfDoubles_[i] = 1600.0L * sin(2 * PI * 1000.0L * i / sampleRate_);
            }
            else
            {
                for (INTTYPE i = 0; i < numOfPoints_; i++)
                    tapeOfDoubles_[i] = FLOATTYPE(0);
            }
            sqrtOfPoints_ = sqrt((double) numOfPoints_);
            //            // calculate binary log
            //            logOfPoints_ = 0;
            //            numOfPoints--;
            //            while (numOfPoints != 0)
            //            {
            //                numOfPoints >>= 1;
            //                logOfPoints_++;
            //            }

            bitReverseVector_.resize(numOfPoints_);
            transformedComplexVector_.resize(numOfPoints_);
            complexExpontials_.resize(logOfPoints_ + 1);

            // Pre-compute complex exponentials
            for (INTTYPE l = 1, _2_l = 2; l <= logOfPoints_; l++)
            {
                (complexExpontials_[l]).resize(numOfPoints_);

                for (INTTYPE i = 0; i < numOfPoints_; i++)
                {
                    FLOATTYPE re = cos(2. * PI * i / _2_l);
                    FLOATTYPE im = -sin(2. * PI * i / _2_l);
                    complexExpontials_[l][i] = COMPLEXVALUE(re, im);
                }
                _2_l *= 2;
            }

            // set up bit reverse mapping
            INTTYPE rev = 0;
            INTTYPE halfPoints = numOfPoints_ / 2;

            for (INTTYPE i = 0; i < numOfPoints_ - 1; i++)
            {
                bitReverseVector_[i] = rev;
                INTTYPE mask = halfPoints;
                // add 1 backwards
                while (rev >= mask)
                {
                    rev -= mask; // turn off this bit
                    mask >>= 1;
                }
                rev += mask;
            }
            bitReverseVector_ [numOfPoints_ - 1] = numOfPoints_ - 1;
        }

        FFT(const FFT& lhs) = default;
        FFT& operator=(const FFT& lhs) = default;

        INTTYPE numberOfPoints() const
        {
            return numOfPoints_;
        }

        /**
         *
         *                0   1   2   3   4   5   6   7
         *   level   1
         *   step    1                                     0
         *   increm  2                                   W
         *   j = 0        <--->   <--->   <--->   <--->   1
         *   level   2
         *   step    2
         *   increm  4                                     0
         *   j = 0        <------->       <------->      W      1
         *   j = 1            <------->       <------->   2   W
         *   level   3                                         2
         *   step    4
         *   increm  8                                     0
         *   j = 0        <--------------->              W      1
         *   j = 1            <--------------->           3   W      2
         *   j = 2                <--------------->            3   W      3
         *   j = 3                    <--------------->             3   W
         *                                                               3
         */
        void transform()
        {
            // step = 2 ^ (level-1)
            // increm = 2 ^ level;
            INTTYPE step = 1;
            for (INTTYPE level = 1; level <= logOfPoints_; level++)
            {
                INTTYPE increm = step * 2;
                for (INTTYPE j = 0; j < step; j++)
                {
                    // U = exp ( - 2 PI j / 2 ^ level )
                    COMPLEXVALUE U = complexExpontials_ [level][j];
                    for (INTTYPE i = j; i < numOfPoints_; i += increm)
                    {
                        // butterfly
                        COMPLEXVALUE T = U;
                        T *= transformedComplexVector_ [i + step];
                        transformedComplexVector_ [i + step] = transformedComplexVector_[i];
                        transformedComplexVector_ [i + step] -= T;
                        transformedComplexVector_ [i] += T;
                    }
                }
                step *= 2;
            }
        }

        void loadFloatVector(FLOATVECTOR sampleVector)
        {
            INTTYPE cSample = sampleVector.size();
            if (cSample > numOfPoints_)
                return;

            auto start = tapeOfDoubles_.begin();
            auto middle = start + cSample;
            auto end = tapeOfDoubles_.end();

            // make space for samples at the end of the tape
            // shifting previous samples towards the beginning
            std::rotate(start, middle, end);

            // copy samples from iterator to tail end of tape
            INTTYPE iTail = numOfPoints_ - cSample;
            for (INTTYPE i = 0; i < cSample; i++)
            {
                tapeOfDoubles_ [i + iTail] = sampleVector[i];
            }
            // Initialize the FFT buffer
            for (INTTYPE i = 0; i < numOfPoints_; i++)
                set(i, tapeOfDoubles_[i]);
        }

        FLOATTYPE getIntensityAt(INTTYPE index) const
        {
            assert(index < numOfPoints_);
            return std::abs(transformedComplexVector_[index]) / sqrtOfPoints_;
        }

        FLOATTYPE realAt(INTTYPE index) const
        {
            assert(index < numOfPoints_);
            return transformedComplexVector_[index].real();
        }

        FLOATTYPE imagAt(INTTYPE index) const
        {
            assert(index < numOfPoints_);
            return transformedComplexVector_[index].imag();
        }

        INTTYPE getFrequencyOfSampleAt(INTTYPE point) const
        {
            // return frequency in Hz of a given point
            assert(point < numOfPoints_);
            INTTYPE x = sampleRate_ * point;
            return x / numOfPoints_;
        }

        INTTYPE HzToPoint(INTTYPE freq) const
        {
            return (INTTYPE) numOfPoints_ * freq / sampleRate_;
        }

        INTTYPE MaxFreq() const
        {
            return sampleRate_;
        }

        INTTYPE Tape(INTTYPE i) const
        {
            assert(i < numOfPoints_);
            return (INTTYPE) tapeOfDoubles_[i];
        }


    private:

        void set(INTTYPE i, FLOATTYPE val)
        {
            transformedComplexVector_ [bitReverseVector_[i]] = COMPLEXVALUE(val);
        }

        INTTYPE numOfPoints_;
        INTTYPE sampleRate_;
        INTTYPE logOfPoints_;
        FLOATTYPE sqrtOfPoints_;
        INTVECTOR bitReverseVector_; // bit reverse vector
        COMPLEXVECTOR transformedComplexVector_; // in-place fft array
        COMPLEXMATRIX complexExpontials_; // exponentials
        FLOATVECTOR tapeOfDoubles_; // recording tape
    };
}; // namespace util

#endif // !defined(NS_UTIL_FFT_H_INCLUDED)
