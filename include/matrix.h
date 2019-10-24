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
 * @date: 2019-04-30
 * @author: Dieter J Kybelksties
 */

#if !defined(NS_UTIL_MATRIX_H_INCLUDED)
#define NS_UTIL_MATRIX_H_INCLUDED

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <utility>
#include <complex>
#include <initializer_list>
#include <limits>
#include "stringutil.h"
#define DO_TRACE_
#include "traceutil.h"

namespace util
{

    template<typename T>
    inline T normalMin(T val)
    {
        return std::numeric_limits<T>::min();
    }

    inline float normalMin(std::complex<float> val)
    {
        return std::numeric_limits<float>::min();
    }

    inline double normalMin(std::complex<double> val)
    {
        return std::numeric_limits<double>::min();
    }

    inline long double normalMin(std::complex<long double> val)
    {
        return std::numeric_limits<long double>::min();
    }

    struct matrix_interface
    {
        /**
         * Retrieve the horizontal extent of the matrix.
         * @return x-dimension
         */
        virtual size_t sizeX() const = 0;

        /**
         * Retrieve the vertical extent of the matrix.
         * @return y-dimension
         */
        virtual size_t sizeY() const = 0;

        /**
         * Check whether coordinates (x,y) are within the matrix.
         * @return true if so, false otherwise
         */
        bool withinBounds(size_t x, size_t y) const
        {
            return x < sizeX() && y < sizeY();
        }

        /**
         * Check whether this is a square matrix.
         * @return true if so, false otherwise
         */
        bool isSquare() const
        {
            return sizeX() == sizeY();
        }

        /**
         * Check whether matrix dimensions of the right-hand side are compatible
         * for adding to this matrix (x- and y- dimensions need to be same as in
         * this matrix)
         * @param rhs the right-hand-side matrix.
         * @return true if so, false otherwise
         */
        bool isAddCompatible(const matrix_interface& rhs) const
        {
            return sizeX() == rhs.sizeX() && sizeY() == rhs.sizeY();
        }

        /**
         * Check whether matrix dimensions of the right-hand side are compatible
         * for matrix multiplication with this matrix.
         * @param rhs the right-hand-side matrix.
         * @return true if so, false otherwise
         */
        bool isMultCompatible(const matrix_interface& rhs) const
        {
            return sizeX() == rhs.sizeY();
        }

        /**
         * Check whether this matrix is square and the rhs matrix has the same
         * number of rows for the solve algorithm to work.
         * @param rhs the right-hand-side matrix.
         * @return true if so, false otherwise
         */
        bool isSolveCompatible(const matrix_interface& rhs) const
        {
            return isSquare() && sizeY() == rhs.sizeY();
        }

        /**
         * Check whether this is a horizontal vector-shaped matrix.
         * @return true if so, false otherwise
         */
        bool isHVector() const
        {
            return (1 == sizeY());
        }

        /**
         * Check whether this is a vertical vector-shaped matrix.
         * @return true if so, false otherwise
         */
        bool isVVector() const
        {
            return (1 == sizeX());
        }

        /**
         * Check whether this is a diagonal matrix.
         * @return true if so, false otherwise
         */
        virtual bool isDiagonal() const = 0;

        /**
         * Check whether this is a scalar matrix.
         * @return true if so, false otherwise
         */
        virtual bool isScalar() const = 0;

        /**
         * Check whether this is a unit matrix.
         * @return true if so, false otherwise
         */
        virtual bool isUnit() const = 0;

        /**
         * Check whether this is a upper triangular matrix.
         * @return true if so, false otherwise
         */
        virtual bool isUpperTriangular() const = 0;

        /**
         * Check whether this is a lower triangular matrix.
         * @return true if so, false otherwise
         */
        virtual bool isLowerTriangular() const = 0;

        /**
         * Check whether this is a symmetric matrix.
         * @return true if so, false otherwise
         */
        virtual bool isSymmetric() const = 0;

        /**
         * Check whether this is a skew-symmetric matrix.
         * @return true if so, false otherwise
         */
        virtual bool isSkewSymmetric() const = 0;
    };

    enum class operType
    {
        MatrixAdd, MatrixSub, MatrixMult, MatrixSolve, MatrixDiv
    };

    class matrixSizesIncompatible : public std::logic_error
    {
    public:

        matrixSizesIncompatible(const std::string& what_arg)
        : logic_error(what_arg)
        {
        }
    };

    class matrixScalarMustNotBeZero : public std::logic_error
    {
    public:

        matrixScalarMustNotBeZero(const std::string& what_arg)
        : logic_error(what_arg)
        {
        }
    };

    class matrixMustBeSquare : public std::logic_error
    {
    public:

        matrixMustBeSquare(const std::string& what_arg)
        : logic_error(what_arg)
        {
        }
    };

    class matrixIndexOutOfBounds : public std::out_of_range
    {
    public:

        matrixIndexOutOfBounds(const std::string& what_arg)
        : out_of_range(what_arg)
        {
        }
    };

    class matrixIsSingular : public std::logic_error
    {
    public:

        matrixIsSingular(const std::string& what_arg)
        : logic_error(what_arg)
        {
        }
    };

    template<bool enable = false >
    void checkBounds(const matrix_interface& lhs,
                     size_t x,
                     size_t y,
                     const std::string& location);

    template<>
    void checkBounds<false> (const matrix_interface& lhs,
                             size_t x,
                             size_t y,
                             const std::string& location);

    /**
     * matrix template
     *
     * Note: This matrix template class defines majority of the matrix
     * operations as overloaded operators and methods. It is assumed that
     * users of this class are familiar with matrix algebra. We have not
     * defined any specialization of this template here, so all the instances
     * of matrix will be created implicitly by the compiler. . Note that this class is not
     * optimized for performance.
     * @param T                 value-type, tested with float, double, long double,
     *                          complex<float>, complex<double> and complex<long double>
     * @param enableBoundsCheck check the boundaries when accessing elements if set
     *                          to true
     */
    template <typename T = long double, bool enableBoundsCheck = false >
    class matrix : public matrix_interface
    {
    private:
        typedef std::vector<T> row_t;
        typedef std::vector<row_t> mat_t;
        /**
         * Data-container.
         */
        mat_t m_;

        /**
         * Initialize the data container by re-creating it with new dimensions.
         * @param xDim new x-dimension
         * @param yDim new y-dimension
         * @return the new initialized data-container
         */
        mat_t& initializeData(size_t xDim = 0, size_t yDim = 0)
        {
            m_.clear();
            if (xDim == 0)
                xDim = 1;
            if (yDim == 0)
                yDim = xDim;
            row_t newRow = row_t(xDim);
            for (size_t y = 0; y < yDim; y++)
                m_.push_back(newRow);
            return m_;
        }


    public:

        matrix(const matrix& rhs) = default;

        /**
         * Default constructor.
         * @param xDim x-dimension: if equal 0 than set to 1
         * @param yDim y-dimension: if equal 0 than make matrix square
         * @param l list of T-values to initialize the matrix
         */
        explicit matrix(size_t xDim = 0,
                        size_t yDim = 0,
                        std::initializer_list<T> l = std::initializer_list<T>())
        {
            initializeData(xDim, yDim);
            size_t x = 0;
            size_t y = 0;
            auto it = l.begin();
            size_t count = 0;
            size_t maxCount = sizeX() * sizeY();
            while (it != l.end() && count < maxCount)
            {
                (*this)(x, y) = *it;
                x++;
                if (x == sizeX())
                {
                    x = 0;
                    y++;
                }
                count++;
                it++;
            }

        }

        /**
         * Move copy constructor.
         * @param rhs right-hand-side matrix - will be reset
         */
        matrix(matrix&& rhs)
        : m_(rhs.m_)
        {
            rhs.initializeData();
        }

        ~matrix() = default;
        matrix& operator=(const matrix& rhs) = default;

        /**
         * Move assignment operator.
         * @param rhs right-hand-side matrix - will be reset
         * @return this matrix set to what rhs previously was.
         */
        matrix& operator=(matrix&& rhs)
        {
            if (this != &rhs)
            {
                m_ = rhs.m_;
                rhs.initializeData();
            }
            return *this;
        }

        /**
         * Create a diagonal matrix.
         * @param l the values of the diagonal. Length of the list determines the
         *          dimensions of the matrix.
         * @return a diagonal matrix with diagonal elements populated from l
         */
        static matrix<T, enableBoundsCheck> diag(std::initializer_list<T> l)
        {
            matrix<T, enableBoundsCheck> reval(l.size(), l.size());
            auto it = l.begin();
            for (size_t i = 0; i < l.size(); i++)
                reval(i, i) = *it++;
            return reval;
        }

        /**
         * Create a scalar matrix.
         * @param dim dimension (x and y are the same)
         * @param c scalar value on the diagonal
         * @return a scalar matrix with diagonal c-values
         */
        static matrix<T, enableBoundsCheck> scalar(size_t dim, const T& c = T(1.0L))
        {
            matrix<T, enableBoundsCheck> reval(dim);
            for (size_t i = 0; i < reval.sizeX(); i++)
                reval(i, i) = c;
            return reval;
        }

        /**
         * Create a horizontal vector-matrix (size in Y-dimension is 1)
         * @param l list of values, determines also the size of the matrix
         * @return l.size() x 1 - matrix with values from the list
         */
        static matrix<T, enableBoundsCheck> hvect(std::initializer_list<T> l)
        {
            matrix<T, enableBoundsCheck> reval(l.size(), 1);
            auto it = l.begin();
            for (size_t i = 0; i < l.size(); i++)
                reval(i, 0) = *it++;
            return reval;
        }

        /**
         * Create a vertical vector-matrix (size in X-dimension is 1)
         * @param l list of values, determines also the size of the matrix
         * @return 1 x l.size() - matrix with values from the list
         */
        static matrix<T, enableBoundsCheck> vvect(std::initializer_list<T> l)
        {
            matrix<T, enableBoundsCheck> reval(1, l.size());
            auto it = l.begin();
            for (size_t i = 0; i < l.size(); i++)
                reval(0, i) = *it++;
            return reval;
        }

        /**
         * Retrieve the horizontal extent of the matrix.
         * @return x-dimension
         */
        size_t sizeX() const
        {
            return sizeY() == 0 ? 0 : m_[0].size();
        }

        /**
         * Retrieve the vertical extent of the matrix.
         * @return y-dimension
         */
        size_t sizeY() const
        {
            return m_.size();
        }

        /**
         * Assert that dimensions of the given matrices are compatible for the
         * given operation.
         * @param lhs left-hand-side matrix
         * @param rhs left-hand-side matrix
         * @param operation the operation as string
         * @param location the location in the class
         * @throw matrixSizesIncompatible
         */
        static void assertCompatibleSizes(const matrix<T, enableBoundsCheck>& lhs,
                                          const matrix<T, enableBoundsCheck>& rhs,
                                          const operType& operation,
                                          const std::string& location)
        {
            if (operation == operType::MatrixAdd || operation == operType::MatrixSub)
            {
                if (!lhs.isAddCompatible(rhs))
                    throw matrixSizesIncompatible(location +
                                                  ": matrix-size lhs (" +
                                                  asString(lhs.sizeX()) +
                                                  "," +
                                                  asString(lhs.sizeY()) +
                                                  ") is not equal matrix-size rhs(" +
                                                  asString(rhs.sizeX()) +
                                                  "," +
                                                  asString(rhs.sizeY()) +
                                                  ").");
            }
            else if (operation == operType::MatrixMult)
            {
                if (lhs.sizeX() != rhs.sizeY())
                    throw matrixSizesIncompatible(location +
                                                  ": x-dimension of lhs-matrix (" +
                                                  asString(lhs.sizeX()) +
                                                  ") is not equal to y-dimension rhs (" +
                                                  asString(rhs.sizeY()) +
                                                  ").");
            }
            else if (operation == operType::MatrixSolve)
            {
                if (lhs.sizeY() != rhs.sizeY())
                    throw matrixSizesIncompatible(location +
                                                  ": matrix-y-dimension lhs " +
                                                  asString(lhs.sizeY()) +
                                                  " is not equal matrix-y-dimension rhs " +
                                                  asString(rhs.sizeY()) +
                                                  ".");
            }
        }

        /**
         * Assert that a given scalar is not zero.
         * @param c the offending scalar
         * @param location the location in the class
         * @throw matrixScalarMustNotBeZero
         */
        static void assertNotZero(T c, const std::string& location)
        {
            if (c == T(0))
                throw matrixScalarMustNotBeZero(location +
                                                ": scalar " +
                                                asString(c) +
                                                "must not be 0(Zero).");
        }

        /**
         * Assert that dimensions of the given matrices are compatible for the
         * given operation.
         * @param lhs left-hand-side matrix
         * @param location the location in the class
         * @throw matrixMustBeSquare
         */
        static void assertSquare(const matrix<T, enableBoundsCheck>& lhs,
                                 const std::string& location)
        {
            if (!lhs.isSquare())
                throw matrixMustBeSquare(location +
                                         ": operation only defined for square matrices.");
        }

        /**
         * Subscript operator to get/set individual elements.
         *
         * @param x x-coordinate
         * @param y y coordinate
         * @return reference to the value of the element at (x,y)
         */
        T& operator()(size_t x, size_t y)
        {
            //if enableBoundsCheck==false, this will be an empty function
            checkBounds<enableBoundsCheck>(*this,
                                           x,
                                           y,
                                           "T& matrix<T,enableBoundsCheck>::operator()");
            return m_[y][x];
        }

        /**
         * Subscript operator to get individual elements.
         *
         * @param x x-coordinate
         * @param y y coordinate
         * @return the value of the element at (x,y)
         */
        T operator()(size_t x, size_t y) const
        {
            //if enableBoundsCheck==false, this will be an empty function
            checkBounds<enableBoundsCheck>(*this,
                                           x,
                                           y,
                                           "T matrix<T,enableBoundsCheck>::operator() const");
            return m_[y][x];
        }

        /**
         * Retrieve the whole row(y) if in bounds.
         * @param y row index
         * @return reference to the data in row y
         */
        row_t& row(size_t y)
        {
            checkBounds<enableBoundsCheck>(*this,
                                           0,
                                           y,
                                           "row_t& matrix<T,enableBoundsCheck>::row(y)");
            return m_[y];
        }

        /**
         * Retrieve the whole row(y) if in bounds.
         * @param y row index
         * @return the data in row y by value
         */
        row_t row(size_t y) const
        {
            checkBounds<enableBoundsCheck>(*this,
                                           0,
                                           y,
                                           "row_t& matrix<T,enableBoundsCheck>::row(y)");
            return m_[y];
        }

        /**
         * Unary + operator.
         */
        friend matrix<T, enableBoundsCheck> operator+(const matrix<T, enableBoundsCheck>& rhs)
        {
            return rhs;
        }

        /**
         * Unary negation operator.
         * @param rhs right-hand-side matrix
         */
        friend matrix<T, enableBoundsCheck> operator-(const matrix<T, enableBoundsCheck>& rhs)
        {
            matrix<T, enableBoundsCheck> temp(rhs);

            for (size_t y = 0; y < temp.sizeY(); y++)
                for (size_t x = 0; x < temp.sizeX(); x++)
                    temp(x, y) = -temp(x, y);

            return temp;
        }

        /**
         * Global matrix addition operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return the sum of the two matrices
         */
        friend matrix<T, enableBoundsCheck> operator+(const matrix<T, enableBoundsCheck>& lhs, const matrix<T, enableBoundsCheck>& rhs)
        {
            assertCompatibleSizes(lhs, rhs, operType::MatrixAdd, "operator+(lhs,rhs)");
            matrix<T, enableBoundsCheck> reval = lhs;
            for (size_t y = 0; y < reval.sizeY(); y++)
                for (size_t x = 0; x < reval.sizeX(); x++)
                    reval(x, y) += rhs(x, y);
            return reval;
        }

        /**
         * Combined addition and assignment operator.
         * @param rhs right-hand-side matrix
         * @return the sum of this with the rhs
         */
        matrix<T, enableBoundsCheck>& operator+=(const matrix<T, enableBoundsCheck>& rhs)
        {
            assertCompatibleSizes(*this, rhs, operType::MatrixAdd, "operator+=(rhs)");
            *this = *this+rhs;
            return *this;
        }

        /**
         * Global matrix subtraction operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return the result of lhs-rhs
         */
        friend matrix<T, enableBoundsCheck> operator-(const matrix<T, enableBoundsCheck>& lhs, const matrix<T, enableBoundsCheck>& rhs)
        {
            assertCompatibleSizes(lhs, rhs, operType::MatrixAdd, "operator-(lhs,rhs)");
            matrix<T, enableBoundsCheck> reval = lhs;
            for (size_t y = 0; y < reval.sizeY(); y++)
                for (size_t x = 0; x < reval.sizeX(); x++)
                    reval(x, y) -= rhs(x, y);
            return reval;
        }

        /**
         * Combined subtraction and assignment operator.
         * @param rhs right-hand-side matrix
         * @return the result of *this - rhs
         */
        matrix<T, enableBoundsCheck>& operator-=(const matrix<T, enableBoundsCheck>& rhs)
        {
            assertCompatibleSizes(*this, rhs, operType::MatrixSub, "operator-=(rhs)");
            *this = *this-rhs;
            return *this;
        }

        /**
         * Global scalar multiplication operator.
         * @param lhs left-hand-side matrix
         * @param c right-hand-side constant scalar value
         * @return the result of lhs*c
         */
        friend matrix<T, enableBoundsCheck> operator*(const matrix<T, enableBoundsCheck>& lhs, const T& c)
        {
            matrix<T, enableBoundsCheck> reval = lhs;
            for (size_t y = 0; y < reval.sizeY(); y++)
                for (size_t x = 0; x < reval.sizeX(); x++)
                    reval(x, y) *= c;
            return reval;
        }

        /**
         * Global scalar multiplication operator.
         * @param c left-hand-side constant scalar value
         * @param rhs left-hand-side matrix
         * @return the result of c*rhs
         */
        friend matrix<T, enableBoundsCheck> operator*(const T& c, const matrix<T, enableBoundsCheck>& rhs)
        {
            matrix<T, enableBoundsCheck> reval = rhs;
            for (size_t y = 0; y < reval.sizeY(); y++)
                for (size_t x = 0; x < reval.sizeX(); x++)
                    reval(x, y) *= c;
            return reval;
        }

        /**
         * Combined scalar multiplication and assignment operator.
         * @param c scalar
         * @return the result of *this * c
         */
        matrix<T, enableBoundsCheck>& operator*=(const T& c)
        {
            *this = *this * c;
            return *this;
        }

        /**
         * Matrix multiplication operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return the product lhs*rhs
         */
        friend matrix<T, enableBoundsCheck> operator*(const matrix<T, enableBoundsCheck>& lhs,
                                                      const matrix<T, enableBoundsCheck>& rhs)
        {
            assertCompatibleSizes(lhs, rhs, operType::MatrixMult, "operator*(lhs,rhs)");
            matrix<T, enableBoundsCheck> reval(rhs.sizeX(), lhs.sizeY());

            for (size_t y = 0; y < lhs.sizeY(); y++)
                for (size_t x = 0; x < rhs.sizeX(); x++)
                {
                    //                    reval(y, x) = T(0);
                    //                    for (size_t k = 0; k < lhs.sizeX(); k++)
                    //                        reval(y, x) += lhs(k, y) * rhs(x, k);
                    reval(x, y) = T(0);
                    for (size_t k = 0; k < lhs.sizeX(); k++)
                        reval(x, y) += lhs(k, y) * rhs(x, k);
                }

            return reval;
        }

        /**
         * Combined  multiplication and assignment operator.
         * @param rhs right-hand-side matrix
         * @return the result of *this * rhs
         */
        matrix<T, enableBoundsCheck>& operator*=(const matrix<T, enableBoundsCheck>& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        /**
         * Global scalar division operator.
         * @param lhs left-hand-side matrix
         * @param c right-hand-side constant scalar value
         * @return the result of lhs/c
         */
        friend matrix<T, enableBoundsCheck> operator/(const matrix<T, enableBoundsCheck>& lhs,
                                                      const T& c)
        {
            assertNotZero(c, "operator/(lhs,c)");
            matrix<T, enableBoundsCheck> reval = lhs;
            for (size_t y = 0; y < reval.sizeY(); y++)
                for (size_t x = 0; x < reval.sizeX(); x++)
                    reval(x, y) /= c;
            return reval;
        }

        /**
         * Global scalar divided by matrix operator.
         * @param rhs right-hand-side matrix
         * @param c left-hand-side constant scalar value
         * @return the result of c/rhs
         */
        friend matrix<T, enableBoundsCheck> operator/(const T& c,
                                                      const matrix<T, enableBoundsCheck>& rhs)
        {
            return (!rhs * c);
        }

        /**
         * Global matrix divided by matrix operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return the result of lhs/rhs
         */
        friend matrix<T, enableBoundsCheck> operator/(const matrix<T, enableBoundsCheck>& lhs,
                                                      const matrix<T, enableBoundsCheck>& rhs)
        {
            return (lhs * !rhs);
        }

        /**
         * Combined scalar division and assignment operator.
         * @param c scalar
         * @return the result of *this * c
         */
        matrix<T, enableBoundsCheck>& operator/=(const T& c)
        {
            assertNotZero(c, "operator/=(lhs,c)");
            *this = *this / c;
            return *this;
        }

        /**
         * Power operator.
         * @param pow power
         * @return lhs ^ pow
         */
        friend matrix<T, enableBoundsCheck> operator^(const matrix<T, enableBoundsCheck>& lhs,
                                                      const size_t& pow)
        {
            assertSquare(lhs, "operator^(lhs,pow)");
            matrix<T, enableBoundsCheck> reval(lhs);
            for (size_t i = 2; i <= pow; i++)
                reval = lhs * reval;

            return reval;
        }

        /**
         * Combined power and assignment operator.
         * @param pow power
         * @return *this ^ pow
         */
        matrix<T, enableBoundsCheck>& operator^=(const size_t& pow)
        {
            assertSquare(*this, "operator^=(pow)");
            *this = *this ^ pow;
            return *this;
        }

        /**
         * This operator is used to return the transposition of the matrix.
         * @param rhs right-hand-side matrix
         * @return rhs.transposed
         */
        friend matrix<T, enableBoundsCheck> operator~(const matrix<T, enableBoundsCheck>& rhs)
        {
            matrix<T, enableBoundsCheck> reval(rhs.sizeY(), rhs.sizeX());

            for (size_t y = 0; y < rhs.sizeY(); y++)
                for (size_t x = 0; x < rhs.sizeX(); x++)
                {
                    reval(y, x) = rhs(x, y);
                }
            return reval;
        }

        /**
         * Resize the matrix to new dimensions whilst preserving the values where
         * possible.
         * @param newXDim new x-dimension
         * @param newYdimnew y-dimension
         */
        void resize(size_t newXDim, size_t newYdim)
        {
            if (newXDim == 0)
                newXDim = sizeX();
            if (newYdim == 0)
                newYdim = sizeY();
            mat_t tmp = initializeData(newYdim, newXDim);
            for (size_t y = 0; y < std::min(sizeY(), newYdim); y++)
                std::copy(row(y).begin(),
                          row(y).begin()+(std::min(newXDim, sizeX())),
                          tmp[y].begin());
            std::swap(tmp, m_);
        }

        /**
         * This operator has been used to calculate inversion of matrix.
         * @param rhs right-hand-side matrix
         * @return lhs^(-1)
         */
        friend matrix<T, enableBoundsCheck> operator!(const matrix<T, enableBoundsCheck>& rhs)
        {
            matrix<T, enableBoundsCheck> reval = rhs;
            return reval.inv();
        }

        /**
         * Inversion function.
         *
         * @return the inverted non-singular square matrix if possible.
         */
        matrix<T, enableBoundsCheck> inv()
        {
            assertSquare(*this, "matrix<T,enableBoundsCheck>::inv()");
            size_t y, x, k;
            T pivotVal, a2;

            // initialize the return matrix as the unit-matrix of sizeX
            matrix<T, enableBoundsCheck> reval =
                matrix<T, enableBoundsCheck>::scalar(sizeX(), T(1.0L));

            for (k = 0; k < sizeX(); k++)
            {
                // if we can not find a new pivot-element then the matrix is
                // singular and cannot be inverted
                int pivIdx = pivot(k);
                if (pivIdx == -1)
                    throw matrixIsSingular("matrix<T,enableBoundsCheck>::operator!: Inversion of a singular matrix");

                // swap rows so that the pivot element is on the diagonal at k
                if (pivIdx != 0)
                {
                    std::swap(row(k), row(pivIdx));
                }
                // divide all values at the row by the pivot value and thus
                // ensuring that the value at (k,k) == 1
                pivotVal = (*this)(k, k);
                for (x = 0; x < sizeX(); x++)
                {
                    (*this)(x, k) /= pivotVal;
                    reval(x, k) /= pivotVal;
                }
                for (y = 0; y < sizeY(); y++)
                    if (y != k)
                    {
                        a2 = (*this)(k, y);
                        for (x = 0; x < sizeX(); x++)
                        {
                            (*this)(x, y) -= a2 * (*this)(x, k);
                            reval(x, y) -= a2 * reval(x, k);
                        }
                    }
            }
            return reval;
        }

        /**
         * Solve simultaneous equations.
         * @param v matrix of values. Can be seen as set of vertical vectors
         * @return matrix representing the solutions
         */
        matrix<T, enableBoundsCheck> solve(const matrix<T, enableBoundsCheck>& v) const
        {
            assertSquare(*this, "matrix<T,enableBoundsCheck>::solve(v)");
            assertCompatibleSizes(*this, v, operType::MatrixSolve, "matrix<T,enableBoundsCheck>::solve(v)");

            matrix<T, enableBoundsCheck> temp = *this;
            matrix<T, enableBoundsCheck> solutions = v;

            for (size_t pivIdx = 0; pivIdx < sizeX(); pivIdx++)
            {
                if (temp.pivot(pivIdx, &solutions) == -1)
                    throw matrixIsSingular("matrix<T,enableBoundsCheck>::solve(): Singular matrix!");

                size_t x, y;
                ////////////////////////////////////////////////////////////////
                // Divide the pivot-row of the temporary matrix and the
                // solution matrix by the pivot value leaving a "1" at the
                // diagonal.
                T pivotValue = temp(pivIdx, pivIdx);
                for (x = pivIdx; x < temp.sizeX(); x++)
                    temp(x, pivIdx) /= pivotValue;
                for (x = 0; x < solutions.sizeX(); x++)
                    solutions(x, pivIdx) /= pivotValue;
                ////////////////////////////////////////////////////////////////

                // empty the column except the diagonal of the temporary
                for (y = 0; y < sizeY(); y++)
                {
                    if (y != pivIdx)
                    {
                        T rowFactor = temp(pivIdx, y);
                        if (rowFactor != T(0))
                        {
                            for (x = pivIdx; x < temp.sizeX(); x++)
                                temp(x, y) -= rowFactor * temp(x, pivIdx);
                            for (x = 0; x < solutions.sizeX(); x++)
                                solutions(x, y) -= rowFactor * solutions(x, pivIdx);
                        }
                    }
                }
            }
            return solutions;
        }

#if defined HAVE_LEGACY_DETERMINANT_METHOD

        /**
         *
         * Calculate the determinant of this matrix.
         * @return the determinant
         */
        T determinant() const
        {
            assertSquare(*this, "matrix<T,enableBoundsCheck>::determinant()");
            T piv(0);
            T reval = T(1.0L);

            matrix<T, enableBoundsCheck> temp(*this);

            for (size_t k = 0; k < sizeX(); k++)
            {
                int indx = temp.pivot(k);
                if (indx == -1)
                    return 0;
                if (indx != 0)
                    reval = -reval;
                reval *= temp(k, k);
                for (size_t x = k + 1; x < sizeX(); x++)
                {
                    piv = temp(x, k) / temp(k, k);

                    for (size_t y = k + 1; y < sizeY(); y++)
                        temp(x, y) -= piv * temp(k, y);
                }
            }
            if (std::abs(reval) < normalMin(reval))
                reval = T(0);
            return reval;
        }
#endif // defined HAVE_LEGACY_DETERMINANT_METHOD

    private: // make the recursive method private

        /**
         * Calculate the sub-determinant of this matrix at level n. This is a
         * recursive method.
         * @param n the level with first call to be level n == sizeX().
         * @return the sub-determinant
         */
        T determinantR(size_t n) const
        {
            assertSquare(*this, "matrix<T,enableBoundsCheck>::det()");
            T reval = T(0);
            matrix<T, enableBoundsCheck> submatrix(sizeX() - 1, sizeY() - 1);
            if (n == 2)
                return ((*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1));
            else
            {
                T pm = T(1.0L);
                const T minus = T(-1.0L);
                for (size_t x = 0; x < n; x++)
                {
                    size_t subi = 0;
                    for (size_t i = 1; i < n; i++)
                    {
                        size_t subj = 0;
                        for (size_t j = 0; j < n; j++)
                        {
                            if (j == x)
                                continue;
                            submatrix(subi, subj) = (*this)(i, j);
                            subj++;
                        }
                        subi++;
                    }
                    reval = reval + ((pm) * (*this)(0, x) * determinantR(n - 1));
                    pm *= minus;
                }
            }
            return reval;
        }

    public:

        /**
         * Calculate the determinant of this matrix.
         * @return the determinant
         */
        T det() const
        {
            return determinantR(sizeX());
        }

        /**
         * Calculate the norm of a matrix.
         * @return the norm
         */
        T norm() const
        {
            T reval = T(0);

            for (size_t x = 0; x < sizeX(); x++)
                for (size_t y = 0; y < sizeY(); y++)
                    reval += (*this)(x, y)*(*this)(x, y);
            reval = sqrt(reval);

            return reval;
        }

        /**
         * Calculate the condition number of a matrix.
         * @return the condition number
         */
        T cond() const
        {
            matrix<T, enableBoundsCheck> inv = !(*this);
            return (norm() * inv.norm());
        }

        /**
         * Calculate the cofactor of a matrix for a given element.
         * @param x x-coordinate
         * @param y y-coordinate
         * @return the cofactor of matrix value at (x,y)
         */
        T cofact(size_t x, size_t y) const
        {
            assertSquare(*this, "cofact(x,y)");
            checkBounds(*this, x, y, "cofact(x,y)");

            matrix<T, enableBoundsCheck> subMat(sizeX() - 1, sizeY() - 1);

            for (size_t x1 = 0, x2 = 0; x1 < sizeX(); x1++)
            {
                if (x1 == x)
                    continue;
                for (size_t y1 = 0, y2 = 0; y1 < sizeY(); y1++)
                {
                    if (y1 == y)
                        continue;
                    subMat(x2, y2) = (*this)(x1, y1);
                    y2++;
                }
                x2++;
            }
            T cof = subMat.det();

            if ((x + y) % 2 == 1)
                cof = -cof;

            return cof;
        }

        /**
         * Calculate adjoin of a matrix.
         * @return the adjoin
         */
        matrix<T, enableBoundsCheck> adj() const
        {
            assertSquare(*this, "matrix<T,enableBoundsCheck>::adj()");
            matrix<T, enableBoundsCheck> reval(sizeX(), sizeY());

            for (size_t x = 0; x < sizeX(); x++)
                for (size_t y = 0; y < sizeY(); y++)
                    reval(x, y) = cofact(x, y);
            return reval;
        }

        /**
         * Check whether this is a singular matrix.
         * @return true if so, false otherwise
         */
        bool isSingular() const
        {
            return !isSquare() || det() == T(0);
        }

        /**
         * Check whether this is a diagonal matrix.
         * @return true if so, false otherwise
         */
        bool isDiagonal() const
        {
            if (!isSquare())
                return false;
            for (size_t y = 0; y < sizeX(); y++)
                for (size_t x = 0; x < sizeY(); x++)
                    if (x != y && (*this)(x, y) != T(0))
                        return false;
            return true;
        }

        /**
         * Check whether this is a scalar matrix.
         * @return true if so, false otherwise
         */
        bool isScalar() const
        {
            if (!isDiagonal())
                return false;
            T v = (*this)(0, 0);
            for (size_t xy = 1; xy < sizeX(); xy++)
                if ((*this)(xy, xy) != v)
                    return false;
            return true;
        }

        /**
         * Check whether this is a unit matrix.
         * @return true if so, false otherwise
         */
        bool isUnit() const
        {
            if (isScalar() && (*this)(0, 0) == T(1.0L))
                return true;
            return false;
        }

        /**
         * Check whether this is a null matrix.
         * @return true if so, false otherwise
         */
        bool isNull() const
        {
            for (size_t y = 0; y < sizeX(); y++)
                for (size_t x = 0; x < sizeY(); x++)
                    if ((*this)(x, y) != T(0))
                        return false;
            return true;
        }

        /**
         * Check whether this is a symmetric matrix.
         * @return true if so, false otherwise
         */
        bool isSymmetric() const
        {
            if (!isSquare())
                return false;
            for (size_t y = 1; y < sizeY(); y++)
                for (size_t x = 0; x < y; x++)
                    if ((*this)(x, y) != (*this)(y, x))
                        return false;
            return true;
        }

        /**
         * Check whether this is a skew-symmetric matrix.
         * @return true if so, false otherwise
         */
        bool isSkewSymmetric() const
        {
            if (!isSquare())
                return false;
            for (size_t y = 1; y < sizeY(); y++)
                for (size_t x = 0; x < y; x++)
                    if ((*this)(x, y) != -(*this)(y, x))
                        return false;
            return true;
        }

        /**
         * Check whether this is a upper triangular matrix.
         * @return true if so, false otherwise
         */
        bool isUpperTriangular() const
        {
            if (!isSquare())
                return false;
            for (size_t y = 1; y < sizeY(); y++)
                for (size_t x = 0; x < y; x++)
                    if ((*this)(x, y) != T(0))
                        return false;
            return true;
        }

        /**
         * Check whether this is a lower triangular matrix.
         * @return true if so, false otherwise
         */
        bool isLowerTriangular() const
        {
            if (!isSquare())
                return false;
            for (size_t x = 1; x < sizeX(); x++)
                for (size_t y = 0; y < x; y++)
                    if ((*this)(x, y) != T(0))
                        return false;

            return true;
        }

        /**
         * Equality operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return true if equal, false, otherwise
         */
        friend bool operator==(const matrix<T, enableBoundsCheck>& lhs,
                               const matrix<T, enableBoundsCheck>& rhs)
        {

            return lhs.m_ == rhs.m_;
        }

        /**
         * Inequality operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return true if *NOT* equal, false, otherwise
         */
        friend bool operator!=(const matrix<T, enableBoundsCheck>& lhs,
                               const matrix<T, enableBoundsCheck>& rhs)
        {

            return !(lhs == rhs);
        }

        /**
         * Input stream function.
         * @param istrm the input stream to read from
         * @param m the matrix to read into
         * @return reference to the stream
         */
        friend std::istream& operator>>(std::istream& istrm,
                                        matrix<T, enableBoundsCheck>& m)
        {
            for (size_t y = 0; y < m.sizeY(); y++)
                for (size_t x = 0; x < m.sizeX(); x++)
                {
                    T val;
                    istrm >> val;
                    m(y, x) = val;
                }
            return istrm;
        }

        /**
         * Output stream function.
         * @param ostrm the output stream to write to
         * @param m the matrix to write
         * @return reference to the stream
         */
        friend std::ostream& operator<<(std::ostream& ostrm,
                                        const matrix<T, enableBoundsCheck>& m)
        {
            for (size_t y = 0; y < m.sizeY(); y++)
            {
                for (size_t x = 0; x < m.sizeX(); x++)
                {

                    const T& v = m(x, y);
                    ostrm << v << '\t';
                }
                ostrm << std::endl;
            }
            return ostrm;
        }

    private:

        /**
         * Partial pivoting method.
         * @param pivX the pivot index
         * @param solutions a pointer to a possible solutions matrix
         * @return the pivot index.
         */
        long long pivot(size_t pivX, matrix<T, enableBoundsCheck>* solutions = 0)
        {
            long long k = pivX;
            long double amax = -1.0;
            long double temp = 0.0;

            for (size_t x = pivX; x < sizeX(); x++)
                if ((temp = abs((*this)(pivX, x))) > amax && temp != 0.0)
                {
                    amax = temp;
                    k = x;
                }
            if ((*this)(pivX, k) == T(0))
                return -1;
            if (k != (long long) (pivX))
            {
                std::swap(row(k), row(pivX));
                if (solutions != 0)
                    std::swap(solutions->row(k), solutions->row(pivX));

                return k;
            }
            return 0;
        }
    };

    template<bool enable = false >
    inline void checkBounds(const matrix_interface& lhs,
                            size_t x,
                            size_t y,
                            const std::string& location)
    {
        if (!lhs.withinBounds(x, y))
        {

            std::stringstream ss;
            ss << location
                << ": index ("
                << x
                << ","
                << y
                << ") is out of bounds ("
                << lhs.sizeX()
                << ","
                << lhs.sizeY()
                << ").";
            throw matrixIndexOutOfBounds(ss.str());
        }
    }

    template<>
    inline void checkBounds<false>(const matrix_interface& lhs,
                                   size_t x,
                                   size_t y,
                                   const std::string& location)
    {
    }

}; // namespace util

#endif // NS_UTIL_MATRIX_H_INCLUDED
