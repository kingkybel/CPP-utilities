/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/matrix_tests.cc
 * Description: Unit tests for matrix utilities
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

#include "logvalue.h"
#include "matrix.h"

#include <complex>
#include <gtest/gtest.h>
#include <initializer_list>
#include <ios>
#include <iostream>

using namespace std;
using namespace util;

class MatrixTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

template<typename T_, bool enableBoundsCheck = false>
void testMatrixConstructionT(initializer_list<T_> initList)
{
    vector<T_> val;
    auto       it = initList.begin();
    while(it != initList.end())
    {
        val.push_back(*it);
        it++;
    }
    ASSERT_TRUE(val.size() >= 4);

    matrix<T_, enableBoundsCheck> m;
    ASSERT_EQ(size_t(1), m.sizeX());
    ASSERT_EQ(size_t(1), m.sizeY());
    ASSERT_EQ(T_(0), m(0, 0));
    ASSERT_TRUE(m.isDiagonal());
    ASSERT_TRUE(m.isHVector());
    ASSERT_TRUE(m.isVVector());
    ASSERT_TRUE(m.isLowerTriangular());
    ASSERT_TRUE(m.isScalar());
    ASSERT_TRUE(m.isSingular());
    ASSERT_TRUE(m.isSquare());
    ASSERT_TRUE(m.isSymmetric());
    ASSERT_TRUE(m.isSkewSymmetric());
    ASSERT_TRUE(!m.isUnit());
    ASSERT_TRUE(m.isUpperTriangular());

    auto m2 = m;
    ASSERT_EQ(size_t(1), m2.sizeX());
    ASSERT_EQ(size_t(1), m2.sizeY());
    ASSERT_EQ(T_(0), m2(0, 0));
    ASSERT_TRUE(m2.isDiagonal());
    ASSERT_TRUE(m2.isHVector());
    ASSERT_TRUE(m2.isVVector());
    ASSERT_TRUE(m2.isLowerTriangular());
    ASSERT_TRUE(m2.isScalar());
    ASSERT_TRUE(m2.isSingular());
    ASSERT_TRUE(m2.isSquare());
    ASSERT_TRUE(m2.isSymmetric());
    ASSERT_TRUE(m2.isSkewSymmetric());
    ASSERT_TRUE(!m2.isUnit());
    ASSERT_TRUE(m2.isUpperTriangular());

    size_t rootSize = sqrt(val.size());
    auto   m3       = matrix<T_, enableBoundsCheck>(rootSize, rootSize);
    ASSERT_EQ(rootSize, m3.sizeX());
    ASSERT_EQ(rootSize, m3.sizeY());
    for(size_t y = 0; y < m3.sizeY(); y++)
        for(size_t x = 0; x < m3.sizeX(); x++)
            ASSERT_EQ(T_(0), m3(y, y));
    ASSERT_TRUE(m3.isDiagonal());
    ASSERT_TRUE(!m3.isHVector());
    ASSERT_TRUE(!m3.isVVector());
    ASSERT_TRUE(m3.isLowerTriangular());
    ASSERT_TRUE(m3.isScalar());
    ASSERT_TRUE(m3.isSingular());
    ASSERT_TRUE(m3.isSquare());
    ASSERT_TRUE(m3.isSymmetric());
    ASSERT_TRUE(m3.isSkewSymmetric());
    ASSERT_TRUE(!m3.isUnit());
    ASSERT_TRUE(m3.isUpperTriangular());

    auto m4 = matrix<T_, enableBoundsCheck>(rootSize, rootSize, initList);
    ASSERT_EQ(rootSize, m4.sizeX());
    ASSERT_EQ(rootSize, m4.sizeY());
    size_t valInd = 0;
    for(size_t y = 0; y < m3.sizeY(); y++)
        for(size_t x = 0; x < m3.sizeX(); x++)
        {
            ASSERT_EQ(val[valInd], m4(x, y));
            valInd++;
        }

    ASSERT_TRUE(!m4.isDiagonal());
    ASSERT_TRUE(!m4.isHVector());
    ASSERT_TRUE(!m4.isVVector());
    ASSERT_TRUE(!m4.isLowerTriangular());
    ASSERT_TRUE(!m4.isScalar());
    ASSERT_TRUE(!m4.isSingular());
    ASSERT_TRUE(m4.isSquare());
    ASSERT_TRUE(!m4.isSymmetric());
    ASSERT_TRUE(!m4.isSkewSymmetric());
    ASSERT_TRUE(!m4.isUnit());
    ASSERT_TRUE(!m4.isUpperTriangular());

    m3 = m4;
    ASSERT_EQ(rootSize, m4.sizeX());
    ASSERT_EQ(rootSize, m4.sizeY());
    valInd = 0;
    for(size_t y = 0; y < m3.sizeY(); y++)
        for(size_t x = 0; x < m3.sizeX(); x++)
        {
            ASSERT_EQ(val[valInd], m3(x, y));
            valInd++;
        }

    ASSERT_TRUE(!m3.isDiagonal());
    ASSERT_TRUE(!m3.isHVector());
    ASSERT_TRUE(!m3.isVVector());
    ASSERT_TRUE(!m3.isLowerTriangular());
    ASSERT_TRUE(!m3.isScalar());
    ASSERT_TRUE(!m3.isSingular());
    ASSERT_TRUE(m3.isSquare());
    ASSERT_TRUE(!m3.isSymmetric());
    ASSERT_TRUE(!m3.isSkewSymmetric());
    ASSERT_TRUE(!m3.isUnit());
    ASSERT_TRUE(!m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::diag(initList);
    ASSERT_EQ(val.size(), m3.sizeX());
    ASSERT_EQ(val.size(), m3.sizeY());
    for(size_t y = 0; y < m3.sizeY(); y++)
        for(size_t x = 0; x < m3.sizeX(); x++)
            if(x == y)
                ASSERT_EQ(val[y], m3(y, y));
            else
                ASSERT_EQ(T_(0), m3(x, y));

    ASSERT_TRUE(m3.isDiagonal());
    ASSERT_TRUE(!m3.isHVector());
    ASSERT_TRUE(!m3.isVVector());
    ASSERT_TRUE(m3.isLowerTriangular());
    ASSERT_TRUE(!m3.isScalar());
    ASSERT_TRUE(!m3.isSingular());
    ASSERT_TRUE(m3.isSquare());
    ASSERT_TRUE(m3.isSymmetric());
    ASSERT_TRUE(m3.isSkewSymmetric());
    ASSERT_TRUE(!m3.isUnit());
    ASSERT_TRUE(m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::scalar(val.size(), val[val.size() - 1]);
    ASSERT_EQ(val.size(), m3.sizeX());
    ASSERT_EQ(val.size(), m3.sizeY());
    for(size_t y = 0; y < m3.sizeY(); y++)
        for(size_t x = 0; x < m3.sizeX(); x++)
            ASSERT_TRUE((x == y && m3(x, y) == val[val.size() - 1]) || m3(x, y) == (T_)(0));
    ASSERT_TRUE(m3.isDiagonal());
    ASSERT_TRUE(!m3.isHVector());
    ASSERT_TRUE(!m3.isVVector());
    ASSERT_TRUE(m3.isLowerTriangular());
    ASSERT_TRUE(m3.isScalar());
    ASSERT_TRUE(!m3.isSingular());
    ASSERT_TRUE(m3.isSquare());
    ASSERT_TRUE(m3.isSymmetric());
    ASSERT_TRUE(m3.isSkewSymmetric());
    ASSERT_TRUE(!m3.isUnit());
    ASSERT_TRUE(m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::hvect(initList);
    ASSERT_EQ(val.size(), m3.sizeX());
    ASSERT_EQ(size_t(1), m3.sizeY());
    for(size_t x = 0; x < m3.sizeX(); x++)
        ASSERT_EQ(val[x], m3(x, 0));

    ASSERT_TRUE(!m3.isDiagonal());
    ASSERT_TRUE(m3.isHVector());
    ASSERT_TRUE(!m3.isVVector());
    ASSERT_TRUE(!m3.isLowerTriangular());
    ASSERT_TRUE(!m3.isScalar());
    ASSERT_TRUE(m3.isSingular());
    ASSERT_TRUE(!m3.isSquare());
    ASSERT_TRUE(!m3.isSymmetric());
    ASSERT_TRUE(!m3.isSkewSymmetric());
    ASSERT_TRUE(!m3.isUnit());
    ASSERT_TRUE(!m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::vvect(initList);
    ASSERT_EQ(size_t(1), m3.sizeX());
    ASSERT_EQ(val.size(), m3.sizeY());
    for(size_t y = 0; y < m3.sizeY(); y++)
        ASSERT_EQ(val[y], m3(0, y));

    ASSERT_TRUE(!m3.isDiagonal());
    ASSERT_TRUE(!m3.isHVector());
    ASSERT_TRUE(m3.isVVector());
    ASSERT_TRUE(!m3.isLowerTriangular());
    ASSERT_TRUE(!m3.isScalar());
    ASSERT_TRUE(!m3.isSquare());
    ASSERT_TRUE(m3.isSingular());
    ASSERT_TRUE(!m3.isSymmetric());
    ASSERT_TRUE(!m3.isSkewSymmetric());
    ASSERT_TRUE(!m3.isUnit());
    ASSERT_TRUE(!m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>(rootSize, rootSize + 1, initList);
    ASSERT_EQ(rootSize, m3.sizeX());
    ASSERT_EQ(rootSize + 1, m3.sizeY());
    valInd = 0;
    for(size_t y = 0; y < m3.sizeY(); y++)
        for(size_t x = 0; x < m3.sizeX(); x++)
        {
            if(valInd < val.size())
                ASSERT_EQ(val[valInd], m3(x, y));
            else
                ASSERT_EQ(T_(0), m3(x, y));
            valInd++;
        }

    ASSERT_TRUE(!m3.isDiagonal());
    ASSERT_TRUE(!m3.isHVector());
    ASSERT_TRUE(!m3.isVVector());
    ASSERT_TRUE(!m3.isLowerTriangular());
    ASSERT_TRUE(!m3.isScalar());
    ASSERT_TRUE(!m3.isSquare());
    ASSERT_TRUE(m3.isSingular());
    ASSERT_TRUE(!m3.isSymmetric());
    ASSERT_TRUE(!m3.isSkewSymmetric());
    ASSERT_TRUE(!m3.isUnit());
    //    ASSERT_TRUE(!m3.isUpperTriangular());
}

TEST_F(MatrixTest, testMatrixConstruction)
{
    testMatrixConstructionT<float>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<float, true>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<double>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<double, true>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<long double>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<long double, true>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<logVal>({1.0L, 2.0L, 3.0L, 4.0L});
    testMatrixConstructionT<logVal, true>({1.0L, 2.0L, 3.0L, 4.0L});

    testMatrixConstructionT<complex<float>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}});
    testMatrixConstructionT<complex<float>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}});

    testMatrixConstructionT<complex<double>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}});
    testMatrixConstructionT<complex<double>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}});

    testMatrixConstructionT<complex<long double>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}});
    testMatrixConstructionT<complex<long double>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}, {4.0, 5.0}});

    testMatrixConstructionT<complex<long double>, true>({{1.0, 2.0},
                                                         {2.0, 3.0},
                                                         {3.0, 4.0},
                                                         {6.0, -10.0},
                                                         {5.0, 4.0},
                                                         {9.0, 4.0},
                                                         {-5.0, 4.0},
                                                         {3.0, 3.0},
                                                         {-1.0, 4.0},
                                                         {4.0, 5.0}});
}

template<typename T_, bool enableBoundsCheck = false>
void testExceptionsT(initializer_list<T_> hVec, initializer_list<T_> vVec, initializer_list<T_> mat)
{
    size_t dim1 = hVec.size();
    size_t dim2 = vVec.size();
    ASSERT_TRUE(dim1 != dim2 && dim1 >= 2 && dim2 >= 2) << "matrices must have minimum size of 2 for these tests";
    auto hv   = matrix<T_, enableBoundsCheck>::hvect(hVec);
    auto vv   = matrix<T_, enableBoundsCheck>::vvect(vVec);
    auto m1_2 = matrix<T_, enableBoundsCheck>(dim1 - 1, dim2 + 1, mat);

    ASSERT_THROW(m1_2.det(), matrixMustBeSquare);
    ASSERT_THROW(m1_2.adj(), matrixMustBeSquare);
    ASSERT_THROW(m1_2.cofact(dim1 - 2, dim2 - 2), matrixMustBeSquare);

    auto singMat = ~hv * hv;
    ASSERT_THROW(singMat.inv(), matrixIsSingular);
    ASSERT_THROW(m1_2 * vv, matrixSizesIncompatible);
    ASSERT_THROW(m1_2 / T_(0), matrixScalarMustNotBeZero);
    ASSERT_NO_THROW(m1_2 / T_(5.0));

    if(enableBoundsCheck)
    {
        for(size_t y = 0; y < m1_2.sizeY() + 5; y++)
            for(size_t x = 0; x < m1_2.sizeX() + 5; x++)
                if(x < m1_2.sizeX() && y < m1_2.sizeY())
                    ASSERT_NO_THROW(m1_2(x, y));
                else
                    ASSERT_THROW(m1_2(x, y), matrixIndexOutOfBounds);
    }
}

TEST_F(MatrixTest, testExceptions)
{
    testExceptionsT<float>({1.0, 2.0, 3.0},
                           {4.0, 2.0, 5.0, 6.0},
                           {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testExceptionsT<float, true>({1.0, 2.0, 3.0},
                                 {4.0, 2.0, 5.0, 6.0},
                                 {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testExceptionsT<double>({1.0, 2.0, 3.0},
                            {4.0, 2.0, 5.0, 6.0},
                            {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testExceptionsT<double, true>({1.0, 2.0, 3.0},
                                  {4.0, 2.0, 5.0, 6.0},
                                  {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testExceptionsT<long double>({1.0, 2.0, 3.0},
                                 {4.0, 2.0, 5.0, 6.0},
                                 {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testExceptionsT<long double, true>({1.0, 2.0, 3.0},
                                       {4.0, 2.0, 5.0, 6.0},
                                       {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});

    testExceptionsT<logVal>({1.0, 2.0, 3.0},
                            {4.0, 2.0, 5.0, 6.0},
                            {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testExceptionsT<logVal, true>({1.0, 2.0, 3.0},
                                  {4.0, 2.0, 5.0, 6.0},
                                  {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});

    testExceptionsT<complex<float>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                    {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                    {{10.0, 1.0},
                                     {1.0, 2.0},
                                     {3.0, 3.0},
                                     {1.0, 2.0},
                                     {5.0, 1.0},
                                     {4.0, 5.0},
                                     {6.0, 2.0},
                                     {7.0, 3.0},
                                     {10.0, 9.0},
                                     {2.0, 7.0},
                                     {3.0, 2.0},
                                     {7.0, 4.}});
    testExceptionsT<complex<float>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                          {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                          {{10.0, 1.0},
                                           {1.0, 2.0},
                                           {3.0, 3.0},
                                           {1.0, 2.0},
                                           {5.0, 1.0},
                                           {4.0, 5.0},
                                           {6.0, 2.0},
                                           {7.0, 3.0},
                                           {10.0, 9.0},
                                           {2.0, 7.0},
                                           {3.0, 2.0},
                                           {7.0, 4.}});

    testExceptionsT<complex<double>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                     {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                     {{10.0, 1.0},
                                      {1.0, 2.0},
                                      {3.0, 3.0},
                                      {1.0, 2.0},
                                      {5.0, 1.0},
                                      {4.0, 5.0},
                                      {6.0, 2.0},
                                      {7.0, 3.0},
                                      {10.0, 9.0},
                                      {2.0, 7.0},
                                      {3.0, 2.0},
                                      {7.0, 4.}});
    testExceptionsT<complex<double>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                           {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                           {{10.0, 1.0},
                                            {1.0, 2.0},
                                            {3.0, 3.0},
                                            {1.0, 2.0},
                                            {5.0, 1.0},
                                            {4.0, 5.0},
                                            {6.0, 2.0},
                                            {7.0, 3.0},
                                            {10.0, 9.0},
                                            {2.0, 7.0},
                                            {3.0, 2.0},
                                            {7.0, 4.}});

    testExceptionsT<complex<long double>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                          {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                          {{10.0, 1.0},
                                           {1.0, 2.0},
                                           {3.0, 3.0},
                                           {1.0, 2.0},
                                           {5.0, 1.0},
                                           {4.0, 5.0},
                                           {6.0, 2.0},
                                           {7.0, 3.0},
                                           {10.0, 9.0},
                                           {2.0, 7.0},
                                           {3.0, 2.0},
                                           {7.0, 4.}});
    testExceptionsT<complex<long double>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                                {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                                {{10.0, 1.0},
                                                 {1.0, 2.0},
                                                 {3.0, 3.0},
                                                 {1.0, 2.0},
                                                 {5.0, 1.0},
                                                 {4.0, 5.0},
                                                 {6.0, 2.0},
                                                 {7.0, 3.0},
                                                 {10.0, 9.0},
                                                 {2.0, 7.0},
                                                 {3.0, 2.0},
                                                 {7.0, 4.}});
}

template<typename T_, bool enableBoundsCheck = false>
void testMatrixOperationsT(initializer_list<T_> hVec, initializer_list<T_> vVec, initializer_list<T_> mat)
{
    size_t                              dim1 = hVec.size();
    size_t                              dim2 = vVec.size();
    const matrix<T_, enableBoundsCheck> nullMatrix1(dim1);
    const matrix<T_, enableBoundsCheck> nullMatrix2(dim2);
    const matrix<T_, enableBoundsCheck> nullMatrix1_2(dim1, dim2);

    const matrix<T_, enableBoundsCheck> unit1 = matrix<T_, enableBoundsCheck>::scalar(dim1, T_(1));
    const matrix<T_, enableBoundsCheck> unit2 = matrix<T_, enableBoundsCheck>::scalar(dim2, T_(1));

    auto       hv   = matrix<T_, enableBoundsCheck>::hvect(hVec);
    auto       vv   = matrix<T_, enableBoundsCheck>::vvect(vVec);
    const auto m1_2 = matrix<T_, enableBoundsCheck>(dim1, dim2, mat);
    auto       m2_1 = matrix<T_, enableBoundsCheck>(dim2, dim1, mat);

    auto result = m1_2 * unit1;
    for(size_t y = 0; y < min(m1_2.sizeY(), unit2.sizeY()); y++)
        for(size_t x = 0; x < min(m1_2.sizeX(), unit2.sizeX()); x++)
            ASSERT_EQ(m1_2(x, y), result(x, y));

    result = unit2 * m1_2;
    for(size_t y = 0; y < min(m1_2.sizeY(), unit1.sizeY()); y++)
        for(size_t x = 0; x < min(m1_2.sizeX(), unit1.sizeX()); x++)
        {
            ASSERT_EQ(m1_2(x, y), result(x, y));
        }

    result = -m1_2;
    for(size_t y = 0; y < min(m1_2.sizeY(), unit1.sizeY()); y++)
        for(size_t x = 0; x < min(m1_2.sizeX(), unit1.sizeX()); x++)
        {
            ASSERT_EQ(-m1_2(x, y), result(x, y));
        }

    result = ~m1_2;
    ASSERT_EQ(m1_2.sizeY(), result.sizeX());
    ASSERT_EQ(m1_2.sizeX(), result.sizeY());

    result = ~~m1_2;
    ASSERT_EQ(m1_2, result);

    result = ~hv * hv;
    ASSERT_TRUE(result.isSquare());
    ASSERT_TRUE(result.isSymmetric());
    ASSERT_TRUE(result.sizeX() == 1 || result.isSingular());

    result = hv * ~hv;
    ASSERT_TRUE(result.isSquare());
    ASSERT_TRUE(result.isSymmetric());
    ASSERT_TRUE(result.sizeX() == 1 || result.isSingular());

    result = vv * ~vv;
    ASSERT_TRUE(result.isSquare());
    ASSERT_TRUE(result.isSymmetric());
    ASSERT_TRUE(result.sizeX() == 1 || result.isSingular());

    result = ~m1_2 * m1_2;
    ASSERT_TRUE(result.isSquare());
    ASSERT_TRUE(result.isSymmetric());

    result = m1_2 - m1_2;
    ASSERT_TRUE(result == nullMatrix1_2);

    result  = m1_2 + m1_2;
    auto t1 = m1_2 * T_(2.0);
    auto t2 = T_(2.0) * m1_2;
    for(size_t y = 0; y < t1.sizeY(); y++)
        for(size_t x = 0; x < t1.sizeX(); x++)
        {
            stringstream msg;
            msg.precision(40);
            msg << "[" << x << "," << y << "]\n\tt1=" << t1(x, y) << "\n\tt2=" << t2(x, y) << endl;
            ASSERT_EQ(t1(x, y), t2(x, y)) << msg.str();
        }
    ASSERT_EQ(T_(2.0) * m1_2, m1_2 * T_(2.0));

    t1 = result / T_(2.0);
    t2 = m1_2;
    for(size_t y = 0; y < t1.sizeY(); y++)
        for(size_t x = 0; x < t1.sizeX(); x++)
        {
            stringstream msg;
            msg.precision(128);
            msg << "Error in [" << x << "," << y << "]\n\tt1=" << t1(x, y) << "\n\tt2=" << t2(x, y) << endl;
            ASSERT_EQ(t1(x, y), t2(x, y)) << msg.str();
        }

    ASSERT_EQ(result / T_(2.0), m1_2);

    result = nullMatrix1_2;
    result -= m1_2;
    for(size_t y = 0; y < result.sizeY(); y++)
        for(size_t x = 0; x < result.sizeX(); x++)
        {
            ASSERT_EQ(-m1_2(x, y), result(x, y));
        }

    result = nullMatrix1_2;
    result += m1_2;
    for(size_t y = 0; y < result.sizeY(); y++)
        for(size_t x = 0; x < result.sizeX(); x++)
        {
            ASSERT_EQ(m1_2(x, y), result(x, y));
        }
}

TEST_F(MatrixTest, testMatrixOperations)
{
    testMatrixOperationsT<float>({1.0, 2.0, 3.0},
                                 {4.0, 2.0, 5.0, 6.0},
                                 {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testMatrixOperationsT<float, true>({1.0, 2.0, 3.0},
                                       {4.0, 2.0, 5.0, 6.0},
                                       {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testMatrixOperationsT<double>({1.0, 2.0, 3.0},
                                  {4.0, 2.0, 5.0, 6.0},
                                  {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testMatrixOperationsT<double, true>({1.0, 2.0, 3.0},
                                        {4.0, 2.0, 5.0, 6.0},
                                        {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testMatrixOperationsT<long double>({1.0, 2.0, 3.0},
                                       {4.0, 2.0, 5.0, 6.0},
                                       {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});
    testMatrixOperationsT<long double, true>({1.0, 2.0, 3.0},
                                             {4.0, 2.0, 5.0, 6.0},
                                             {10.0, 1.0, 3.0, 1.0, 5.0, 4.0, 6.0, 7.0, 10.0, 2.0, 3.0, 7.0});

    //    testMatrixOperationsT<logVal>({1.0, 2.0, 3.0},
    //    {
    //                                  4.0, 2.0, 5.0, 6.0
    //    },
    //    {
    //                                  10.0, 1.0, 3.0,
    //                                  1.0, 5.0, 4.0,
    //                                  6.0, 7.0, 10.0,
    //                                  2.0, 3.0, 7.0
    //    });
    //    testMatrixOperationsT<logVal, true>({1.0, 2.0, 3.0},
    //    {
    //                                        4.0, 2.0, 5.0, 6.0
    //    },
    //    {
    //                                        10.0, 1.0, 3.0,
    //                                        1.0, 5.0, 4.0,
    //                                        6.0, 7.0, 10.0,
    //                                        2.0, 3.0, 7.0
    //    });

    testMatrixOperationsT<complex<float>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                          {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                          {{10.0, 1.0},
                                           {1.0, 2.0},
                                           {3.0, 3.0},
                                           {1.0, 2.0},
                                           {5.0, 1.0},
                                           {4.0, 5.0},
                                           {6.0, 2.0},
                                           {7.0, 3.0},
                                           {10.0, 9.0},
                                           {2.0, 7.0},
                                           {3.0, 2.0},
                                           {7.0, 4.}});
    testMatrixOperationsT<complex<float>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                                {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                                {{10.0, 1.0},
                                                 {1.0, 2.0},
                                                 {3.0, 3.0},
                                                 {1.0, 2.0},
                                                 {5.0, 1.0},
                                                 {4.0, 5.0},
                                                 {6.0, 2.0},
                                                 {7.0, 3.0},
                                                 {10.0, 9.0},
                                                 {2.0, 7.0},
                                                 {3.0, 2.0},
                                                 {7.0, 4.}});

    testMatrixOperationsT<complex<double>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                           {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                           {{10.0, 1.0},
                                            {1.0, 2.0},
                                            {3.0, 3.0},
                                            {1.0, 2.0},
                                            {5.0, 1.0},
                                            {4.0, 5.0},
                                            {6.0, 2.0},
                                            {7.0, 3.0},
                                            {10.0, 9.0},
                                            {2.0, 7.0},
                                            {3.0, 2.0},
                                            {7.0, 4.}});
    testMatrixOperationsT<complex<double>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                                 {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                                 {{10.0, 1.0},
                                                  {1.0, 2.0},
                                                  {3.0, 3.0},
                                                  {1.0, 2.0},
                                                  {5.0, 1.0},
                                                  {4.0, 5.0},
                                                  {6.0, 2.0},
                                                  {7.0, 3.0},
                                                  {10.0, 9.0},
                                                  {2.0, 7.0},
                                                  {3.0, 2.0},
                                                  {7.0, 4.}});

    testMatrixOperationsT<complex<long double>>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                                {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                                {{10.0, 1.0},
                                                 {1.0, 2.0},
                                                 {3.0, 3.0},
                                                 {1.0, 2.0},
                                                 {5.0, 1.0},
                                                 {4.0, 5.0},
                                                 {6.0, 2.0},
                                                 {7.0, 3.0},
                                                 {10.0, 9.0},
                                                 {2.0, 7.0},
                                                 {3.0, 2.0},
                                                 {7.0, 4.}});
    testMatrixOperationsT<complex<long double>, true>({{1.0, 2.0}, {2.0, 3.0}, {3.0, 4.0}},
                                                      {{4.0, 6.0}, {2.0, 5.0}, {5.0, 7.0}, {6.0, 8.0}},
                                                      {{10.0, 1.0},
                                                       {1.0, 2.0},
                                                       {3.0, 3.0},
                                                       {1.0, 2.0},
                                                       {5.0, 1.0},
                                                       {4.0, 5.0},
                                                       {6.0, 2.0},
                                                       {7.0, 3.0},
                                                       {10.0, 9.0},
                                                       {2.0, 7.0},
                                                       {3.0, 2.0},
                                                       {7.0, 4.0}});
}

template<typename T_, bool enableBoundsCheck = false>
void testSquareMatrixOperationsT(initializer_list<T_> m1_list,
                                 initializer_list<T_> expected_m1_inv,
                                 initializer_list<T_> vVec,
                                 T_                   delta = numeric_limits<T_>::min())
{
    size_t dim1    = sqrt(m1_list.size());
    auto   sq1     = matrix<T_, enableBoundsCheck>(dim1, dim1, m1_list);
    auto   expectd = matrix<T_, enableBoundsCheck>(dim1, dim1, expected_m1_inv);
    auto   vv      = matrix<T_, enableBoundsCheck>::vvect(vVec);
    auto   unit1   = matrix<T_>::scalar(dim1, T_(1.0));

    auto sq_inv = !sq1;
    auto result = sq1 * sq_inv;
    for(size_t y = 0; y < expectd.sizeX(); y++)
        for(size_t x = 0; x < expectd.sizeX(); x++)
        {
            ASSERT_TRUE(abs(expectd(x, y) - sq_inv(x, y)) < delta);
            ASSERT_TRUE(abs(unit1(x, y) - result(x, y)) < T_(0.01));
        }
}

TEST_F(MatrixTest, testSquareMatrixOperations)
{
    testSquareMatrixOperationsT<long double>({5.0, 4.0, 7.0, 6.0, 1.0, 4.0, 2.0, 2.0, 5.0},
                                             {0.09090909090909090911,
                                              0.18181818181818181817,
                                              -0.2727272727272727273,
                                              0.66666666666666666663,
                                              -0.33333333333333333332,
                                              -0.66666666666666666663,
                                              -0.3030303030303030303,
                                              0.060606060606060606058,
                                              0.575757575757},
                                             {2.0, 4.0, 5.0},
                                             1e-10L);
    testSquareMatrixOperationsT<long double, true>({5.0, 4.0, 7.0, 6.0, 1.0, 4.0, 2.0, 2.0, 5.0},
                                                   {0.09090909090909090911,
                                                    0.18181818181818181817,
                                                    -0.2727272727272727273,
                                                    0.66666666666666666663,
                                                    -0.33333333333333333332,
                                                    -0.66666666666666666663,
                                                    -0.3030303030303030303,
                                                    0.060606060606060606058,
                                                    0.575757575757},
                                                   {2.0, 4.0, 5.0},
                                                   1e-10L);
    //    testSquareMatrixOperationsT<logVal>({
    //                                        5.0, 4.0, 7.0, 6.0, 1.0, 4.0, 2.0, 2.0, 5.0
    //    },
    //    {
    //                                        0.09090909090909090911, 0.18181818181818181817, -0.2727272727272727273,
    //                                        0.66666666666666666663, -0.33333333333333333332, -0.66666666666666666663,
    //                                        -0.3030303030303030303, 0.060606060606060606058, 0.575757575757
    //    },
    //    {
    //                                        2.0, 4.0, 5.0
    //    },
    //                                        1e-10L);
    //    testSquareMatrixOperationsT<logVal, true>({
    //                                              5.0, 4.0, 7.0, 6.0, 1.0, 4.0, 2.0, 2.0, 5.0
    //    },
    //    {
    //                                              0.09090909090909090911, 0.18181818181818181817,
    //                                              -0.2727272727272727273, 0.66666666666666666663,
    //                                              -0.33333333333333333332, -0.66666666666666666663,
    //                                              -0.3030303030303030303, 0.060606060606060606058, 0.575757575757
    //    },
    //    {
    //                                              2.0, 4.0, 5.0
    //    },
    //                                              1e-10L);
}
