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
 * @date: 2019-10-13
 * @author: Dieter J Kybelksties
 */

#include <iostream>
#include <complex>
#include <initializer_list>
#include "matrixTest.h"
#include <matrix.h>
#include <traceutil.h>
//#include <logvalue.h>
#define TEST_HEADER(tp,v0) { std::cout << __func__<< " " << #tp << "=" << tp <<  std::endl; }

using namespace std;
using namespace util;
//#define DO_TRACE_

CPPUNIT_TEST_SUITE_REGISTRATION(matrixTest);

matrixTest::matrixTest()
{
}

matrixTest::~matrixTest()
{
}

void matrixTest::setUp()
{
}

void matrixTest::tearDown()
{
}

template<typename T_, bool enableBoundsCheck = false >
void testMatrixConstructionT(initializer_list<T_> initList)
{
    TEST_HEADER(typeid (T_).name(), enableBoundsCheck);
    vector<T_> val;
    auto it = initList.begin();
    while (it != initList.end())
    {
        val.push_back(*it);
        it++;
    }
    CPPUNIT_ASSERT(val.size() >= 4);

    matrix<T_, enableBoundsCheck> m;
    CPPUNIT_ASSERT_EQUAL(size_t(1), m.sizeX());
    CPPUNIT_ASSERT_EQUAL(size_t(1), m.sizeY());
    CPPUNIT_ASSERT_EQUAL(T_(0), m(0, 0));
    CPPUNIT_ASSERT(m.isDiagonal());
    CPPUNIT_ASSERT(m.isHVector());
    CPPUNIT_ASSERT(m.isVVector());
    CPPUNIT_ASSERT(m.isLowerTriangular());
    CPPUNIT_ASSERT(m.isScalar());
    CPPUNIT_ASSERT(m.isSingular());
    CPPUNIT_ASSERT(m.isSquare());
    CPPUNIT_ASSERT(m.isSymmetric());
    CPPUNIT_ASSERT(m.isSkewSymmetric());
    CPPUNIT_ASSERT(!m.isUnit());
    CPPUNIT_ASSERT(m.isUpperTriangular());

    auto m2 = m;
    CPPUNIT_ASSERT_EQUAL(size_t(1), m2.sizeX());
    CPPUNIT_ASSERT_EQUAL(size_t(1), m2.sizeY());
    CPPUNIT_ASSERT_EQUAL(T_(0), m2(0, 0));
    CPPUNIT_ASSERT(m2.isDiagonal());
    CPPUNIT_ASSERT(m2.isHVector());
    CPPUNIT_ASSERT(m2.isVVector());
    CPPUNIT_ASSERT(m2.isLowerTriangular());
    CPPUNIT_ASSERT(m2.isScalar());
    CPPUNIT_ASSERT(m2.isSingular());
    CPPUNIT_ASSERT(m2.isSquare());
    CPPUNIT_ASSERT(m2.isSymmetric());
    CPPUNIT_ASSERT(m2.isSkewSymmetric());
    CPPUNIT_ASSERT(!m2.isUnit());
    CPPUNIT_ASSERT(m2.isUpperTriangular());

    size_t rootSize = sqrt(val.size());
    auto m3 = matrix<T_, enableBoundsCheck>(rootSize, rootSize);
    CPPUNIT_ASSERT_EQUAL(rootSize, m3.sizeX());
    CPPUNIT_ASSERT_EQUAL(rootSize, m3.sizeY());
    for (size_t y = 0; y < m3.sizeY(); y++)
        for (size_t x = 0; x < m3.sizeX(); x++)
            CPPUNIT_ASSERT_EQUAL(T_(0), m3(y, y));
    CPPUNIT_ASSERT(m3.isDiagonal());
    CPPUNIT_ASSERT(!m3.isHVector());
    CPPUNIT_ASSERT(!m3.isVVector());
    CPPUNIT_ASSERT(m3.isLowerTriangular());
    CPPUNIT_ASSERT(m3.isScalar());
    CPPUNIT_ASSERT(m3.isSingular());
    CPPUNIT_ASSERT(m3.isSquare());
    CPPUNIT_ASSERT(m3.isSymmetric());
    CPPUNIT_ASSERT(m3.isSkewSymmetric());
    CPPUNIT_ASSERT(!m3.isUnit());
    CPPUNIT_ASSERT(m3.isUpperTriangular());

    auto m4 = matrix<T_, enableBoundsCheck>(rootSize, rootSize, initList);
    CPPUNIT_ASSERT_EQUAL(rootSize, m4.sizeX());
    CPPUNIT_ASSERT_EQUAL(rootSize, m4.sizeY());
    size_t valInd = 0;
    for (size_t y = 0; y < m3.sizeY(); y++)
        for (size_t x = 0; x < m3.sizeX(); x++)
        {
            CPPUNIT_ASSERT_EQUAL(val[valInd], m4(x, y));
            valInd++;
        }

    CPPUNIT_ASSERT(!m4.isDiagonal());
    CPPUNIT_ASSERT(!m4.isHVector());
    CPPUNIT_ASSERT(!m4.isVVector());
    CPPUNIT_ASSERT(!m4.isLowerTriangular());
    CPPUNIT_ASSERT(!m4.isScalar());
    CPPUNIT_ASSERT(!m4.isSingular());
    CPPUNIT_ASSERT(m4.isSquare());
    CPPUNIT_ASSERT(!m4.isSymmetric());
    CPPUNIT_ASSERT(!m4.isSkewSymmetric());
    CPPUNIT_ASSERT(!m4.isUnit());
    CPPUNIT_ASSERT(!m4.isUpperTriangular());

    m3 = m4;
    CPPUNIT_ASSERT_EQUAL(rootSize, m4.sizeX());
    CPPUNIT_ASSERT_EQUAL(rootSize, m4.sizeY());
    valInd = 0;
    for (size_t y = 0; y < m3.sizeY(); y++)
        for (size_t x = 0; x < m3.sizeX(); x++)
        {
            CPPUNIT_ASSERT_EQUAL(val[valInd], m3(x, y));
            valInd++;
        }

    CPPUNIT_ASSERT(!m3.isDiagonal());
    CPPUNIT_ASSERT(!m3.isHVector());
    CPPUNIT_ASSERT(!m3.isVVector());
    CPPUNIT_ASSERT(!m3.isLowerTriangular());
    CPPUNIT_ASSERT(!m3.isScalar());
    CPPUNIT_ASSERT(!m3.isSingular());
    CPPUNIT_ASSERT(m3.isSquare());
    CPPUNIT_ASSERT(!m3.isSymmetric());
    CPPUNIT_ASSERT(!m3.isSkewSymmetric());
    CPPUNIT_ASSERT(!m3.isUnit());
    CPPUNIT_ASSERT(!m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::diag(initList);
    CPPUNIT_ASSERT_EQUAL(val.size(), m3.sizeX());
    CPPUNIT_ASSERT_EQUAL(val.size(), m3.sizeY());
    for (size_t y = 0; y < m3.sizeY(); y++)
        for (size_t x = 0; x < m3.sizeX(); x++)
            if (x == y)
                CPPUNIT_ASSERT_EQUAL(val[y], m3(y, y));
            else
                CPPUNIT_ASSERT_EQUAL(T_(0), m3(x, y));

    CPPUNIT_ASSERT(m3.isDiagonal());
    CPPUNIT_ASSERT(!m3.isHVector());
    CPPUNIT_ASSERT(!m3.isVVector());
    CPPUNIT_ASSERT(m3.isLowerTriangular());
    CPPUNIT_ASSERT(!m3.isScalar());
    CPPUNIT_ASSERT(!m3.isSingular());
    CPPUNIT_ASSERT(m3.isSquare());
    CPPUNIT_ASSERT(m3.isSymmetric());
    CPPUNIT_ASSERT(m3.isSkewSymmetric());
    CPPUNIT_ASSERT(!m3.isUnit());
    CPPUNIT_ASSERT(m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::scalar(val.size(), val[val.size() - 1]);
    CPPUNIT_ASSERT_EQUAL(val.size(), m3.sizeX());
    CPPUNIT_ASSERT_EQUAL(val.size(), m3.sizeY());
    for (size_t y = 0; y < m3.sizeY(); y++)
        for (size_t x = 0; x < m3.sizeX(); x++)
            CPPUNIT_ASSERT((x == y && m3(x, y) == val[val.size() - 1]) ||
                           m3(x, y) == (T_) (0));
    CPPUNIT_ASSERT(m3.isDiagonal());
    CPPUNIT_ASSERT(!m3.isHVector());
    CPPUNIT_ASSERT(!m3.isVVector());
    CPPUNIT_ASSERT(m3.isLowerTriangular());
    CPPUNIT_ASSERT(m3.isScalar());
    CPPUNIT_ASSERT(!m3.isSingular());
    CPPUNIT_ASSERT(m3.isSquare());
    CPPUNIT_ASSERT(m3.isSymmetric());
    CPPUNIT_ASSERT(m3.isSkewSymmetric());
    CPPUNIT_ASSERT(!m3.isUnit());
    CPPUNIT_ASSERT(m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::hvect(initList);
    CPPUNIT_ASSERT_EQUAL(val.size(), m3.sizeX());
    CPPUNIT_ASSERT_EQUAL(size_t(1), m3.sizeY());
    for (size_t x = 0; x < m3.sizeX(); x++)
        CPPUNIT_ASSERT_EQUAL(val[x], m3(x, 0));

    CPPUNIT_ASSERT(!m3.isDiagonal());
    CPPUNIT_ASSERT(m3.isHVector());
    CPPUNIT_ASSERT(!m3.isVVector());
    CPPUNIT_ASSERT(!m3.isLowerTriangular());
    CPPUNIT_ASSERT(!m3.isScalar());
    CPPUNIT_ASSERT(m3.isSingular());
    CPPUNIT_ASSERT(!m3.isSquare());
    CPPUNIT_ASSERT(!m3.isSymmetric());
    CPPUNIT_ASSERT(!m3.isSkewSymmetric());
    CPPUNIT_ASSERT(!m3.isUnit());
    CPPUNIT_ASSERT(!m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>::vvect(initList);
    CPPUNIT_ASSERT_EQUAL(size_t(1), m3.sizeX());
    CPPUNIT_ASSERT_EQUAL(val.size(), m3.sizeY());
    for (size_t y = 0; y < m3.sizeY(); y++)
        CPPUNIT_ASSERT_EQUAL(val[y], m3(0, y));

    CPPUNIT_ASSERT(!m3.isDiagonal());
    CPPUNIT_ASSERT(!m3.isHVector());
    CPPUNIT_ASSERT(m3.isVVector());
    CPPUNIT_ASSERT(!m3.isLowerTriangular());
    CPPUNIT_ASSERT(!m3.isScalar());
    CPPUNIT_ASSERT(!m3.isSquare());
    CPPUNIT_ASSERT(m3.isSingular());
    CPPUNIT_ASSERT(!m3.isSymmetric());
    CPPUNIT_ASSERT(!m3.isSkewSymmetric());
    CPPUNIT_ASSERT(!m3.isUnit());
    CPPUNIT_ASSERT(!m3.isUpperTriangular());

    m3 = matrix<T_, enableBoundsCheck>(rootSize, rootSize + 1, initList);
    CPPUNIT_ASSERT_EQUAL(rootSize, m3.sizeX());
    CPPUNIT_ASSERT_EQUAL(rootSize + 1, m3.sizeY());
    valInd = 0;
    for (size_t y = 0; y < m3.sizeY(); y++)
        for (size_t x = 0; x < m3.sizeX(); x++)
        {
            if (valInd < val.size())
                CPPUNIT_ASSERT_EQUAL(val[valInd], m3(x, y));
            else
                CPPUNIT_ASSERT_EQUAL(T_(0), m3(x, y));
            valInd++;
        }

    CPPUNIT_ASSERT(!m3.isDiagonal());
    CPPUNIT_ASSERT(!m3.isHVector());
    CPPUNIT_ASSERT(!m3.isVVector());
    CPPUNIT_ASSERT(!m3.isLowerTriangular());
    CPPUNIT_ASSERT(!m3.isScalar());
    CPPUNIT_ASSERT(!m3.isSquare());
    CPPUNIT_ASSERT(m3.isSingular());
    CPPUNIT_ASSERT(!m3.isSymmetric());
    CPPUNIT_ASSERT(!m3.isSkewSymmetric());
    CPPUNIT_ASSERT(!m3.isUnit());
    //    CPPUNIT_ASSERT(!m3.isUpperTriangular());
}

void matrixTest::testMatrixConstruction()
{
    testMatrixConstructionT<float>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<float, true>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<double>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<double, true>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<long double>({1.0, 2.0, 3.0, 4.0});
    testMatrixConstructionT<long double, true>({1.0, 2.0, 3.0, 4.0});

    testMatrixConstructionT<complex<float> >({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0}
    });
    testMatrixConstructionT<complex<float>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0}
    });

    testMatrixConstructionT<complex<double> >({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0}
    });
    testMatrixConstructionT<complex<double>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0}
    });

    testMatrixConstructionT<complex<long double> >({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0}
    });
    testMatrixConstructionT<complex<long double>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {4.0, 5.0}
    });

    testMatrixConstructionT<complex<long double>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0},
        {6.0, -10.0},
        {5.0, 4.0},
        {9.0, 4.0},
        {-5.0, 4.0},
        {3.0, 3.0},
        {-1.0, 4.0},
        {4.0, 5.0}
    });

}

template <typename T_, bool enableBoundsCheck = false >
void testExceptionsT(initializer_list<T_> hVec,
                     initializer_list<T_> vVec,
                     initializer_list<T_> mat)
{
    TEST_HEADER(typeid (T_).name(), enableBoundsCheck);
    size_t dim1 = hVec.size();
    size_t dim2 = vVec.size();
    CPPUNIT_ASSERT_MESSAGE("matrices must have minimum size of 2 for these tests",
                           dim1 != dim2 && dim1 >= 2 && dim2 >= 2);

    auto hv = matrix<T_, enableBoundsCheck>::hvect(hVec);
    auto vv = matrix<T_, enableBoundsCheck>::vvect(vVec);
    auto m1_2 = matrix<T_, enableBoundsCheck>(dim1 - 1, dim2 + 1, mat);

    CPPUNIT_ASSERT_THROW(m1_2.det(), matrixMustBeSquare);
    CPPUNIT_ASSERT_THROW(m1_2.adj(), matrixMustBeSquare);
    CPPUNIT_ASSERT_THROW(m1_2.cofact(dim1 - 2, dim2 - 2), matrixMustBeSquare);

    auto singMat = ~hv*hv;
    CPPUNIT_ASSERT_THROW(singMat.inv(), matrixIsSingular);
    CPPUNIT_ASSERT_THROW(m1_2*vv, matrixSizesIncompatible);
    CPPUNIT_ASSERT_THROW(m1_2 / T_(0), matrixScalarMustNotBeZero);
    CPPUNIT_ASSERT_NO_THROW(m1_2 / T_(5.0));

    if (enableBoundsCheck)
    {
        for (size_t y = 0; y < m1_2.sizeY() + 5; y++)
            for (size_t x = 0; x < m1_2.sizeX() + 5; x++)
                if (x < m1_2.sizeX() && y < m1_2.sizeY())
                    CPPUNIT_ASSERT_NO_THROW(m1_2(x, y));
                else
                    CPPUNIT_ASSERT_THROW(m1_2(x, y), matrixIndexOutOfBounds);
    }

}

void matrixTest::testExceptions()
{
    testExceptionsT<float>({1.0, 2.0, 3.0},
    {
                           4.0, 2.0, 5.0, 6.0
    },
    {
                           10.0, 1.0, 3.0,
                           1.0, 5.0, 4.0,
                           6.0, 7.0, 10.0,
                           2.0, 3.0, 7.0
    });
    testExceptionsT<float, true>({1.0, 2.0, 3.0},
    {
                                 4.0, 2.0, 5.0, 6.0
    },
    {
                                 10.0, 1.0, 3.0,
                                 1.0, 5.0, 4.0,
                                 6.0, 7.0, 10.0,
                                 2.0, 3.0, 7.0
    });
    testExceptionsT<double>({1.0, 2.0, 3.0},
    {
                            4.0, 2.0, 5.0, 6.0
    },
    {
                            10.0, 1.0, 3.0,
                            1.0, 5.0, 4.0,
                            6.0, 7.0, 10.0,
                            2.0, 3.0, 7.0
    });
    testExceptionsT<double, true>({1.0, 2.0, 3.0},
    {
                                  4.0, 2.0, 5.0, 6.0
    },
    {
                                  10.0, 1.0, 3.0,
                                  1.0, 5.0, 4.0,
                                  6.0, 7.0, 10.0,
                                  2.0, 3.0, 7.0
    });
    testExceptionsT<long double>({1.0, 2.0, 3.0},
    {
                                 4.0, 2.0, 5.0, 6.0
    },
    {
                                 10.0, 1.0, 3.0,
                                 1.0, 5.0, 4.0,
                                 6.0, 7.0, 10.0,
                                 2.0, 3.0, 7.0
    });
    testExceptionsT<long double, true>({1.0, 2.0, 3.0},
    {
                                       4.0, 2.0, 5.0, 6.0
    },
    {
                                       10.0, 1.0, 3.0,
                                       1.0, 5.0, 4.0,
                                       6.0, 7.0, 10.0,
                                       2.0, 3.0, 7.0
    });

    testExceptionsT<complex<float>>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                    4.0, 6.0
        },
        {
                                    2.0, 5.0
        },
        {
                                    5.0, 7.0
        },
        {
                                    6.0, 8.0
        }
    },
    {
        {
                                    10.0, 1.0
        },
        {
                                    1.0, 2.0
        },
        {
                                    3.0, 3.0
        },
        {
                                    1.0, 2.0
        },
        {
                                    5.0, 1.0
        },
        {
                                    4.0, 5.0
        },
        {
                                    6.0, 2.0
        },
        {
                                    7.0, 3.0
        },
        {
                                    10.0, 9.0
        },
        {
                                    2.0, 7.0
        },
        {
                                    3.0, 2.0
        },
        {
                                    7.0, 4.
        }
    });
    testExceptionsT<complex<float>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                          4.0, 6.0
        },
        {
                                          2.0, 5.0
        },
        {
                                          5.0, 7.0
        },
        {
                                          6.0, 8.0
        }
    },
    {
        {
                                          10.0, 1.0
        },
        {
                                          1.0, 2.0
        },
        {
                                          3.0, 3.0
        },
        {
                                          1.0, 2.0
        },
        {
                                          5.0, 1.0
        },
        {
                                          4.0, 5.0
        },
        {
                                          6.0, 2.0
        },
        {
                                          7.0, 3.0
        },
        {
                                          10.0, 9.0
        },
        {
                                          2.0, 7.0
        },
        {
                                          3.0, 2.0
        },
        {
                                          7.0, 4.
        }
    });

    testExceptionsT<complex<double>>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                     4.0, 6.0
        },
        {
                                     2.0, 5.0
        },
        {
                                     5.0, 7.0
        },
        {
                                     6.0, 8.0
        }
    },
    {
        {
                                     10.0, 1.0
        },
        {
                                     1.0, 2.0
        },
        {
                                     3.0, 3.0
        },
        {
                                     1.0, 2.0
        },
        {
                                     5.0, 1.0
        },
        {
                                     4.0, 5.0
        },
        {
                                     6.0, 2.0
        },
        {
                                     7.0, 3.0
        },
        {
                                     10.0, 9.0
        },
        {
                                     2.0, 7.0
        },
        {
                                     3.0, 2.0
        },
        {
                                     7.0, 4.
        }
    });
    testExceptionsT<complex<double>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                           4.0, 6.0
        },
        {
                                           2.0, 5.0
        },
        {
                                           5.0, 7.0
        },
        {
                                           6.0, 8.0
        }
    },
    {
        {
                                           10.0, 1.0
        },
        {
                                           1.0, 2.0
        },
        {
                                           3.0, 3.0
        },
        {
                                           1.0, 2.0
        },
        {
                                           5.0, 1.0
        },
        {
                                           4.0, 5.0
        },
        {
                                           6.0, 2.0
        },
        {
                                           7.0, 3.0
        },
        {
                                           10.0, 9.0
        },
        {
                                           2.0, 7.0
        },
        {
                                           3.0, 2.0
        },
        {
                                           7.0, 4.
        }
    });

    testExceptionsT<complex<long double>>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                          4.0, 6.0
        },
        {
                                          2.0, 5.0
        },
        {
                                          5.0, 7.0
        },
        {
                                          6.0, 8.0
        }
    },
    {
        {
                                          10.0, 1.0
        },
        {
                                          1.0, 2.0
        },
        {
                                          3.0, 3.0
        },
        {
                                          1.0, 2.0
        },
        {
                                          5.0, 1.0
        },
        {
                                          4.0, 5.0
        },
        {
                                          6.0, 2.0
        },
        {
                                          7.0, 3.0
        },
        {
                                          10.0, 9.0
        },
        {
                                          2.0, 7.0
        },
        {
                                          3.0, 2.0
        },
        {
                                          7.0, 4.
        }
    });
    testExceptionsT<complex<long double>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                                4.0, 6.0
        },
        {
                                                2.0, 5.0
        },
        {
                                                5.0, 7.0
        },
        {
                                                6.0, 8.0
        }
    },
    {
        {
                                                10.0, 1.0
        },
        {
                                                1.0, 2.0
        },
        {
                                                3.0, 3.0
        },
        {
                                                1.0, 2.0
        },
        {
                                                5.0, 1.0
        },
        {
                                                4.0, 5.0
        },
        {
                                                6.0, 2.0
        },
        {
                                                7.0, 3.0
        },
        {
                                                10.0, 9.0
        },
        {
                                                2.0, 7.0
        },
        {
                                                3.0, 2.0
        },
        {
                                                7.0, 4.
        }
    });
}

template<typename T_, bool enableBoundsCheck = false >
void testMatrixOperationsT(initializer_list<T_> hVec,
                           initializer_list<T_> vVec,
                           initializer_list<T_> mat
                           )
{
    TEST_HEADER(typeid (T_).name(), enableBoundsCheck);
    size_t dim1 = hVec.size();
    size_t dim2 = vVec.size();
    const matrix<T_, enableBoundsCheck> nullMatrix1(dim1);
    const matrix<T_, enableBoundsCheck> nullMatrix2(dim2);
    const matrix<T_, enableBoundsCheck> nullMatrix1_2(dim1, dim2);

    const matrix<T_, enableBoundsCheck> unit1 = matrix<T_, enableBoundsCheck>::scalar(dim1, T_(1));
    const matrix<T_, enableBoundsCheck> unit2 = matrix<T_, enableBoundsCheck>::scalar(dim2, T_(1));

    auto hv = matrix<T_, enableBoundsCheck>::hvect(hVec);
    auto vv = matrix<T_, enableBoundsCheck>::vvect(vVec);
    const auto m1_2 = matrix<T_, enableBoundsCheck>(dim1, dim2, mat);
    auto m2_1 = matrix<T_, enableBoundsCheck>(dim2, dim1, mat);

    auto result = m1_2 * unit1;
    for (size_t y = 0; y < min(m1_2.sizeY(), unit2.sizeY()); y++)
        for (size_t x = 0; x < min(m1_2.sizeX(), unit2.sizeX()); x++)
            CPPUNIT_ASSERT_EQUAL(m1_2(x, y), result(x, y));

    result = unit2 * m1_2;
    for (size_t y = 0; y < min(m1_2.sizeY(), unit1.sizeY()); y++)
        for (size_t x = 0; x < min(m1_2.sizeX(), unit1.sizeX()); x++)
        {
            CPPUNIT_ASSERT_EQUAL(m1_2(x, y), result(x, y));
        }

    result = -m1_2;
    for (size_t y = 0; y < min(m1_2.sizeY(), unit1.sizeY()); y++)
        for (size_t x = 0; x < min(m1_2.sizeX(), unit1.sizeX()); x++)
        {

            CPPUNIT_ASSERT_EQUAL(-m1_2(x, y), result(x, y));
        }

    result = ~m1_2;
    CPPUNIT_ASSERT_EQUAL(m1_2.sizeY(), result.sizeX());
    CPPUNIT_ASSERT_EQUAL(m1_2.sizeX(), result.sizeY());

    result = ~~m1_2;
    CPPUNIT_ASSERT_EQUAL(m1_2, result);

    result = ~hv*hv;
    CPPUNIT_ASSERT(result.isSquare());
    CPPUNIT_ASSERT(result.isSymmetric());
    CPPUNIT_ASSERT(result.sizeX() == 1 || result.isSingular());

    result = hv*~hv;
    CPPUNIT_ASSERT(result.isSquare());
    CPPUNIT_ASSERT(result.isSymmetric());
    CPPUNIT_ASSERT(result.sizeX() == 1 || result.isSingular());

    result = vv*~vv;
    CPPUNIT_ASSERT(result.isSquare());
    CPPUNIT_ASSERT(result.isSymmetric());
    CPPUNIT_ASSERT(result.sizeX() == 1 || result.isSingular());

    result = ~m1_2*m1_2;
    CPPUNIT_ASSERT(result.isSquare());
    CPPUNIT_ASSERT(result.isSymmetric());

    result = m1_2 - m1_2;
    CPPUNIT_ASSERT(result == nullMatrix1_2);

    result = m1_2 + m1_2;
    CPPUNIT_ASSERT(result == T_(2.0) * m1_2 && result == m1_2 * T_(2.0));
    CPPUNIT_ASSERT(result / T_(2.0) == m1_2);

    result = nullMatrix1_2;
    result -= m1_2;
    for (size_t y = 0; y < result.sizeY(); y++)
        for (size_t x = 0; x < result.sizeX(); x++)
        {

            CPPUNIT_ASSERT_EQUAL(-m1_2(x, y), result(x, y));
        }

    result = nullMatrix1_2;
    result += m1_2;
    for (size_t y = 0; y < result.sizeY(); y++)
        for (size_t x = 0; x < result.sizeX(); x++)
        {
            CPPUNIT_ASSERT_EQUAL(m1_2(x, y), result(x, y));
        }
}

void matrixTest::testMatrixOperations()
{
    testMatrixOperationsT<float>({1.0, 2.0, 3.0},
    {
                                 4.0, 2.0, 5.0, 6.0
    },
    {
                                 10.0, 1.0, 3.0,
                                 1.0, 5.0, 4.0,
                                 6.0, 7.0, 10.0,
                                 2.0, 3.0, 7.0
    });
    testMatrixOperationsT<float, true>({1.0, 2.0, 3.0},
    {
                                       4.0, 2.0, 5.0, 6.0
    },
    {
                                       10.0, 1.0, 3.0,
                                       1.0, 5.0, 4.0,
                                       6.0, 7.0, 10.0,
                                       2.0, 3.0, 7.0
    });
    testMatrixOperationsT<double>({1.0, 2.0, 3.0},
    {
                                  4.0, 2.0, 5.0, 6.0
    },
    {
                                  10.0, 1.0, 3.0,
                                  1.0, 5.0, 4.0,
                                  6.0, 7.0, 10.0,
                                  2.0, 3.0, 7.0
    });
    testMatrixOperationsT<double, true>({1.0, 2.0, 3.0},
    {
                                        4.0, 2.0, 5.0, 6.0
    },
    {
                                        10.0, 1.0, 3.0,
                                        1.0, 5.0, 4.0,
                                        6.0, 7.0, 10.0,
                                        2.0, 3.0, 7.0
    });
    testMatrixOperationsT<long double>({1.0, 2.0, 3.0},
    {
                                       4.0, 2.0, 5.0, 6.0
    },
    {
                                       10.0, 1.0, 3.0,
                                       1.0, 5.0, 4.0,
                                       6.0, 7.0, 10.0,
                                       2.0, 3.0, 7.0
    });
    testMatrixOperationsT<long double, true>({1.0, 2.0, 3.0},
    {
                                             4.0, 2.0, 5.0, 6.0
    },
    {
                                             10.0, 1.0, 3.0,
                                             1.0, 5.0, 4.0,
                                             6.0, 7.0, 10.0,
                                             2.0, 3.0, 7.0
    });

    testMatrixOperationsT<complex<float>>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                          4.0, 6.0
        },
        {
                                          2.0, 5.0
        },
        {
                                          5.0, 7.0
        },
        {
                                          6.0, 8.0
        }
    },
    {
        {
                                          10.0, 1.0
        },
        {
                                          1.0, 2.0
        },
        {
                                          3.0, 3.0
        },
        {
                                          1.0, 2.0
        },
        {
                                          5.0, 1.0
        },
        {
                                          4.0, 5.0
        },
        {
                                          6.0, 2.0
        },
        {
                                          7.0, 3.0
        },
        {
                                          10.0, 9.0
        },
        {
                                          2.0, 7.0
        },
        {
                                          3.0, 2.0
        },
        {
                                          7.0, 4.
        }
    });
    testMatrixOperationsT<complex<float>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                                4.0, 6.0
        },
        {
                                                2.0, 5.0
        },
        {
                                                5.0, 7.0
        },
        {
                                                6.0, 8.0
        }
    },
    {
        {
                                                10.0, 1.0
        },
        {
                                                1.0, 2.0
        },
        {
                                                3.0, 3.0
        },
        {
                                                1.0, 2.0
        },
        {
                                                5.0, 1.0
        },
        {
                                                4.0, 5.0
        },
        {
                                                6.0, 2.0
        },
        {
                                                7.0, 3.0
        },
        {
                                                10.0, 9.0
        },
        {
                                                2.0, 7.0
        },
        {
                                                3.0, 2.0
        },
        {
                                                7.0, 4.
        }
    });

    testMatrixOperationsT<complex<double>>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                           4.0, 6.0
        },
        {
                                           2.0, 5.0
        },
        {
                                           5.0, 7.0
        },
        {
                                           6.0, 8.0
        }
    },
    {
        {
                                           10.0, 1.0
        },
        {
                                           1.0, 2.0
        },
        {
                                           3.0, 3.0
        },
        {
                                           1.0, 2.0
        },
        {
                                           5.0, 1.0
        },
        {
                                           4.0, 5.0
        },
        {
                                           6.0, 2.0
        },
        {
                                           7.0, 3.0
        },
        {
                                           10.0, 9.0
        },
        {
                                           2.0, 7.0
        },
        {
                                           3.0, 2.0
        },
        {
                                           7.0, 4.
        }
    });
    testMatrixOperationsT<complex<double>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                                 4.0, 6.0
        },
        {
                                                 2.0, 5.0
        },
        {
                                                 5.0, 7.0
        },
        {
                                                 6.0, 8.0
        }
    },
    {
        {
                                                 10.0, 1.0
        },
        {
                                                 1.0, 2.0
        },
        {
                                                 3.0, 3.0
        },
        {
                                                 1.0, 2.0
        },
        {
                                                 5.0, 1.0
        },
        {
                                                 4.0, 5.0
        },
        {
                                                 6.0, 2.0
        },
        {
                                                 7.0, 3.0
        },
        {
                                                 10.0, 9.0
        },
        {
                                                 2.0, 7.0
        },
        {
                                                 3.0, 2.0
        },
        {
                                                 7.0, 4.
        }
    });

    testMatrixOperationsT<complex<long double>>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                                4.0, 6.0
        },
        {
                                                2.0, 5.0
        },
        {
                                                5.0, 7.0
        },
        {
                                                6.0, 8.0
        }
    },
    {
        {
                                                10.0, 1.0
        },
        {
                                                1.0, 2.0
        },
        {
                                                3.0, 3.0
        },
        {
                                                1.0, 2.0
        },
        {
                                                5.0, 1.0
        },
        {
                                                4.0, 5.0
        },
        {
                                                6.0, 2.0
        },
        {
                                                7.0, 3.0
        },
        {
                                                10.0, 9.0
        },
        {
                                                2.0, 7.0
        },
        {
                                                3.0, 2.0
        },
        {
                                                7.0, 4.
        }
    });
    testMatrixOperationsT<complex<long double>, true>({
        {1.0, 2.0},
        {2.0, 3.0},
        {3.0, 4.0}
    },
    {
        {
                                                      4.0, 6.0
        },
        {
                                                      2.0, 5.0
        },
        {
                                                      5.0, 7.0
        },
        {
                                                      6.0, 8.0
        }
    },
    {
        {
                                                      10.0, 1.0
        },
        {
                                                      1.0, 2.0
        },
        {
                                                      3.0, 3.0
        },
        {
                                                      1.0, 2.0
        },
        {
                                                      5.0, 1.0
        },
        {
                                                      4.0, 5.0
        },
        {
                                                      6.0, 2.0
        },
        {
                                                      7.0, 3.0
        },
        {
                                                      10.0, 9.0
        },
        {
                                                      2.0, 7.0
        },
        {
                                                      3.0, 2.0
        },
        {
                                                      7.0, 4.0
        }
    });
}

template<typename T_, bool enableBoundsCheck = false >
void testSquareMatrixOperationsT(initializer_list<T_> m1_list,
                                 initializer_list<T_> expected_m1_inv,
                                 initializer_list<T_> vVec,
                                 T_ delta = numeric_limits<T_>::min()
                                 )
{
    TEST_HEADER(typeid (T_).name(), enableBoundsCheck);

    size_t dim1 = sqrt(m1_list.size());
    auto sq1 = matrix<T_, enableBoundsCheck>(dim1, dim1, m1_list);
    auto expectd = matrix<T_, enableBoundsCheck>(dim1, dim1, expected_m1_inv);
    auto vv = matrix<T_, enableBoundsCheck>::vvect(vVec);
    auto unit1 = matrix<T_>::scalar(dim1, T_(1.0));

    auto sq_inv = !sq1;
    auto result = sq1*sq_inv;
    for (size_t y = 0; y < expectd.sizeX(); y++)
        for (size_t x = 0; x < expectd.sizeX(); x++)
        {
            CPPUNIT_ASSERT_DOUBLES_EQUAL(expectd(x, y), sq_inv(x, y), delta);
            CPPUNIT_ASSERT_DOUBLES_EQUAL(unit1(x, y), result(x, y), T_(0.01));
        }
}

void matrixTest::testSquareMatrixOperations()
{
    testSquareMatrixOperationsT<long double>({
                                             5.0, 4.0, 7.0, 6.0, 1.0, 4.0, 2.0, 2.0, 5.0
    },
    {
                                             0.09090909090909090911, 0.18181818181818181817, -0.2727272727272727273,
                                             0.66666666666666666663, -0.33333333333333333332, -0.66666666666666666663,
                                             -0.3030303030303030303, 0.060606060606060606058, 0.575757575757
    },
    {
                                             2.0, 4.0, 5.0
    },
                                             1e-10L);
}