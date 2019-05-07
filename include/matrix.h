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
#include "stringutil.h"

namespace util
{

    class matrix_error : public std::logic_error
    {
    public:

        matrix_error(const std::string& what_arg)
        : logic_error(what_arg)
        {
        }
    };

    /**
     *  matrix template
     *
     * Note: This matrix template class defines majority of the matrix
     *  operations as overloaded operators or methods. It is assumed that
     * users of this class is familiar with matrix algebra. We have not
     * defined any specialization of this template here, so all the instances
     * of matrix will be created implicitly by the compiler. The data types
     * tested with this class are float, double, long double, complex<float>,
     * complex<double> and complex<long double>. Note that this class is not
     * optimized for performance.
     *
     *  Operator/Method                          Description
     *  ---------------                          -----------
     *   operator ()   :   This function operator can be used as a
     *                      two-dimensional subscript operator to get/set
     *                      individual matrix elements.
     *
     *    operator !    :   This operator has been used to calculate inversion
     *                      of matrix.
     *
     *    operator ~    :   This operator has been used to return transpose of
     *                      a matrix.
     *
     *    operator ^    :   It is used calculate power (by a scalar) of a matrix.
     *                      When using this operator in a matrix equation, care
     *                      must be taken by parenthesizing it because it has
     *                      lower precedence than addition, subtraction,
     *                      multiplication and division operators.
     *
     *    operator >>   :   It is used to read matrix from input stream as per
     *                      standard C++ stream operators.
     *
     *    operator <<   :   It is used to write matrix to output stream as per
     *                      standard C++ stream operators.
     */
    template <typename T = long double>
    class matrix
    {
    private:
        typedef std::vector<T> row_t;
        typedef std::vector<row_t> mat_t;
        mat_t m_;

    public:

        enum class InitType
        {
            NullMatrix,
            UnitMatrix,
            ScalarMatrix,
            DiagonalMatrix
        };

        mat_t&& initializeData(size_t& xDim, size_t& yDim)
        {
            mat_t reval;
            if (xDim == 0)
                xDim = 1;
            if (yDim == 0)
                yDim = 1;
            row_t newRow = row_t(xDim);
            for (size_t y = 0; y < yDim; y++)
                reval.push_back(newRow);
            return std::move(reval);
        }

        /**
         * Default constructor.
         * @param xDim x-dimension
         * @param yDim y-dimension
         */
        matrix(size_t xDim, size_t yDim)
        {
            std::swap(m_, initializeData(xDim, yDim));
        }

        /**
         * Default constructor.
         * @param xDim x-dimension
         * @param yDim y-dimension
         */
        explicit matrix(size_t xyDim, InitType type, T value = T(1.0))
        {
            std::swap(m_, initializeData(xyDim, xyDim));
            if (type != InitType::NullMatrix)
                for (size_t xy = 0; xy < xyDim; xy++)
                    m_[xy][xy] = (type == InitType::UnitMatrix ?
                                  1.0 :
                                  value);
        }

        matrix(const matrix& m) = default;
        ~matrix() = default;
        matrix& operator=(const matrix& m) = default;

        size_t sizeX() const
        {
            return sizeY() == 0 ? 0 : m_[0].size();
        }

        size_t sizeY() const
        {
            return m_.size();
        }

        void checkBounds(size_t x, size_t y, const std::string& location) const
        {
            if (x >= sizeX() || y >= sizeY())
                throw matrix_error(location +
                                   ": index (" +
                                   util::asString(x) +
                                   "," +
                                   util::asString(y) +
                                   ") is out of bounds (" +
                                   util::asString(sizeX()) +
                                   "," +
                                   util::asString(sizeY()) +
                                   ").");
        }

        void checkCompatibleSizes(const matrix<T>& lhs,
                                  const matrix<T>& rhs,
                                  const std::string& operation,
                                  const std::string& location) const
        {
            if (operation == "+" || operation == "-")
            {
                if (lhs.sizeX() != rhs.sizeX() || lhs.sizeY() != rhs.sizeY())
                    throw matrix_error(location +
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
            else if (operation == "*")
            {
                if (lhs.sizeX() != rhs.sizeY())
                    throw matrix_error(location +
                                       ": matrix-x-dimension lhs " +
                                       asString(lhs.sizeX()) +
                                       " is not equal matrix-y-dimension rhs" +
                                       asString(rhs.sizeY()) +
                                       ".");
            }
            else if (operation == "solve")
            {
                if (lhs.sizeY() != rhs.sizeY())
                    throw matrix_error(location +
                                       ": matrix-y-dimension lhs " +
                                       asString(lhs.sizeY()) +
                                       " is not equal matrix-y-dimension rhs" +
                                       asString(rhs.sizeY()) +
                                       ".");
            }
        }

        void checkNotZero(T c, const std::string& location) const
        {
            if (c == (T) 0)
                throw matrix_error(location +
                                   ": scalar " +
                                   asString(c) +
                                   "must not be 0(Zero).");
        }

        void checkSquare(const matrix<T>& lhs, const std::string& location) const
        {
            if (!lhs.isSquare())
                throw matrix_error(location +
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
            checkBounds(x, y, "matrix<T>::operator()");
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
            checkBounds(x, y, "matrix<T>::operator()");
            return m_[y][x];
        }

        /**
         * Unary + operator.
         */
        matrix& operator+()
        {
            return *this;
        }

        /**
         * Unary negation operator.
         * @param rhs right-hand-side matrix
         */
        matrix<T>&& operator-(const matrix<T>& rhs)
        {
            matrix<T> temp(rhs);

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
        friend matrix<T>&& operator+(const matrix<T>& lhs, const matrix<T>& rhs)
        {
            checkCompatibleSizes(lhs, rhs, "operator+(lhs,rhs)", "+");
            matrix<T> reval = lhs;
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
        matrix<T>& operator+=(const matrix<T>& rhs)
        {
            checkCompatibleSizes(*this, rhs, "operator+=(rhs)", "+");
            *this = *this+rhs;
            return *this;
        }

        /**
         * Global matrix subtraction operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return the result of lhs-rhs
         */
        friend matrix<T>&& operator-(const matrix<T>& lhs, const matrix<T>& rhs)
        {
            checkCompatibleSizes(lhs, rhs, "operator-(lhs,rhs)", "+");
            matrix<T> reval = lhs;
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
        matrix<T>& operator-=(const matrix<T>& rhs)
        {
            checkCompatibleSizes(*this, rhs, "operator-=(rhs)", "-");
            *this = *this-rhs;
            return *this;
        }

        /**
         * Global scalar multiplication operator.
         * @param lhs left-hand-side matrix
         * @param c right-hand-side constant scalar value
         * @return the result of lhs*c
         */
        friend matrix<T>&& operator*(const matrix<T>& lhs, const T& c)
        {
            matrix<T> reval = lhs;
            for (size_t y = 0; y < reval.sizeY(); y++)
                for (size_t x = 0; x < reval.sizeX(); x++)
                    reval(x, y) *= c;
            return reval;
        }

        /**
         * Global scalar multiplication operator.
         * @param c left-hand-side constant scalar value
         * @param rhs left-hand-side matrix
         * @return the result of **rhs
         */
        friend matrix<T>&& operator*(const T& c, const matrix<T>& rhs)
        {
            matrix<T> reval = rhs;
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
        matrix<T>& operator*=(const T& c)
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
        friend matrix<T>&& operator*(const matrix<T>& lhs, const matrix<T>& rhs)
        {
            checkCompatibleSizes(lhs, rhs, "operator*(lhs,rhs)", "*");
            matrix<T> temp(rhs.sizeX(), lhs.sizeY());

            for (size_t y = 0; y < lhs.sizeY(); y++)
                for (size_t x = 0; x < rhs.sizeX(); x++)
                {
                    temp(y, x) = T(0);
                    for (size_t k = 0; k < lhs.sizeX(); k++)
                        temp(y, x) += lhs(y, k) * rhs(k, x);
                }

            return temp;
        }

        /**
         * Combined  multiplication and assignment operator.
         * @param rhs right-hand-side matrix
         * @return the result of *this * rhs
         */
        matrix<T>& operator*=(const matrix<T>& rhs)
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
        friend matrix<T>&& operator/(const matrix<T>& lhs, const T& c)
        {
            checkNotZero(c, "operator/(lhs,c)");
            matrix<T> reval = lhs;
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
        friend matrix<T>&& operator/(const T& c, const matrix<T>& rhs)
        {
            return (!rhs * c);
        }

        /**
         * Global matrix divided by matrix operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return the result of lhs/rhs
         */
        friend matrix<T>&& operator/(const matrix<T>& lhs, const matrix<T>& rhs)
        {
            return (lhs * !rhs);
        }

        /**
         * Combined scalar division and assignment operator.
         * @param c scalar
         * @return the result of *this * c
         */
        matrix<T>& operator/=(const T& c)
        {
            checkNotZero(c, "operator/=(lhs,c)");
            *this = *this / c;
            return *this;
        }

        /**
         * Power operator.
         * @param pow power
         * @return lhs ^ pow
         */
        friend matrix<T>& operator^(const matrix<T>& lhs, const size_t& pow)
        {
            checkSquare(lhs, "operator^(lhs,pow)");
            matrix<T> temp(lhs);
            for (size_t i = 2; i <= pow; i++)
                temp = lhs * temp;

            return temp;
        }

        /**
         * Combined power and assignment operator.
         * @param pow power
         * @return *this ^ pow
         */
        matrix<T>& operator^=(const size_t& pow)
        {
            checkSquare(*this, "operator^=(pow)");
            *this = *this ^ pow;
            return *this;
        }

        /**
         * Transpose operator.
         * @param rhs right-hand-side matrix
         * @return lhs.transposed
         */
        friend matrix<T>&& operator~(const matrix<T>& rhs)
        {
            matrix<T> temp(rhs.sizeY(), rhs.sizeX());

            for (size_t y = 0; y < rhs.sizeY(); y++)
                for (size_t x = 0; x < rhs.sizeX(); x++)
                {
                    temp(y, x) = rhs(x, y);
                }
            return temp;
        }

        // Miscellaneous -methods
        void Null(const size_t& row, const size_t& col);
        void Null();
        void Unit(const size_t& row);
        void Unit();

        void resize(size_t newXDim, size_t newYdim)
        {
            if (newXDim == 0)
                newXDim = sizeX();
            if (newYdim == 0)
                newYdim = sizeY();
            mat_t&& tmp = initializeData(newYdim, newXDim);
            for (size_t y = 0; y < std::min(sizeY(), newYdim); y++)
                std::copy(m_[y].begin(),
                          m_[y].begin()+(std::min(newXDim, sizeX())),
                          tmp.begin());
            std::swap(tmp, m_);
        }

        /**
         * Inversion operator.
         * @param rhs right-hand-side matrix
         * @return lhs.transposed
         */
        friend matrix<T>&& operator!(const matrix<T>& rhs)
        {
            matrix<T> temp = rhs;
            return temp.Inv();
        }

        /**
         * Inversion function.
         *
         * @return the inverted non-singular square matrix if possible.
         */
        matrix<T>&& inv()
        {
            checkSquare(*this, "matrix<T>::Inv()");
            size_t i, j, k;
            T a1, a2;

            matrix<T> temp = matrix<T>(sizeX(), true); //make unit

            for (k = 0; k < sizeX(); k++)
            {
                int indx = pivot(k);
                if (indx == -1)
                    throw matrix_error("matrix<T>::operator!: Inversion of a singular matrix");

                if (indx != 0)
                {
                    std::swap(m_[k], m_[indx]);
                }
                a1 = m_[k][k];
                for (j = 0; j < sizeX(); j++)
                {
                    m_[k][j] /= a1;
                    temp.m_[k][j] /= a1;
                }
                for (i = 0; i < sizeX(); i++)
                    if (i != k)
                    {
                        a2 = m_[i][k];
                        for (j = 0; j < sizeX(); j++)
                        {
                            m_[i][j] -= a2 * m_[k][j];
                            temp.m_[i][j] -= a2 * temp.m_[k][j];
                        }
                    }
            }
            return temp;
        }

        /**
         * Solve simultaneous equations.
         * @param v
         * @return
         */
        matrix<T> solve(const matrix<T>& v) const
        {
            checkSquare(*this, "matrix<T>::solve(v)");
            checkCompatibleSizes(*this, v, "solve", "matrix<T>::solve(v)");
            size_t x, y, k;
            T a1;

            matrix<T> temp(sizeX(), sizeY() + v.sizeY());
            for (x = 0; x < sizeX(); x++)
            {
                for (y = 0; y < sizeY(); y++)
                    temp.m_[x][y] = m_[x][y];
                for (k = 0; k < v.sizeY(); k++, y++)
                    temp.m_[x][y] = v.m_[x][k];
            }
            for (k = 0; k < sizeX(); k++)
            {
                long long indx = temp.pivot(k);
                if (indx == -1)
                    throw matrix_error("matrix<T>::solve(): Singular matrix!");

                a1 = temp.m_[k][k];
                for (y = k; y < temp.sizeY(); y++)
                    temp.m_[k][y] /= a1;

                for (x = k + 1; x < sizeX(); x++)
                {
                    a1 = temp.m_[x][k];
                    for (y = k; y < temp.sizeY(); y++)
                        temp.m_[x][y] -= a1 * temp.m_[k][y];
                }
            }
            matrix<T> s(v.sizeX(), v.sizeY());
            for (k = 0; k < v.sizeY(); k++)
                for (int m = int(sizeX()) - 1; m >= 0; m--)
                {
                    s.m_[m][k] = temp.m_[m][sizeY() + k];
                    for (y = m + 1; y < sizeY(); y++)
                        s.m_[m][k] -= temp.m_[m][y] * s.m_[y][k];
                }
            return s;
        }

        matrix<T> Adj();
        matrix<T> Inv();

        /**
         * Calculate the determinant of this matrix.
         * @return the determinant
         */
        T det() const
        {
            checkSquare(*this, "matrix<T>::det()");
            size_t x, y, k;
            T piv(0);
            T reval = T(1.0);

            matrix<T> temp(*this);

            for (k = 0; k < sizeX(); k++)
            {
                int indx = temp.pivot(k);
                if (indx == -1)
                    return 0;
                if (indx != 0)
                    reval = -reval;
                reval = reval * temp.m_[k][k];
                for (x = k + 1; x < sizeX(); x++)
                {
                    piv = temp.m_[x][k] / temp.m_[k][k];
                    for (y = k + 1; y < sizeY(); y++)
                        temp.m_[x][y] -= piv * temp.m_[k][y];
                }
            }
            return reval;
        }

        /**
         * Calculate the norm of a matrix
         * @return
         */
        T norm()
        {
            T reval = T(0);

            for (size_t x = 0; x < sizeX(); x++)
                for (size_t y = 0; y < sizeY(); y++)
                    reval += m_[x][y] * m_[x][y];
            reval = sqrt(reval);

            return reval;
        }

        /**
         * Calculate the condition number of a matrix.
         * @return
         */
        T cond()
        {
            matrix<T> inv = !(*this);
            return (norm() * inv.norm());
        }

        T Cofact(size_t row, size_t col);

        /**
         * Calculate the cofactor of a matrix for a given element.
         * @param x
         * @param y
         * @return
         */
        T cofact(size_t x, size_t y)
        {
            checkSquare(*this, "cofact(x,y)");
            checkBounds(x, y, "cofact(x,y)");
            size_t i, i1, j, j1;

            matrix<T> temp(sizeX() - 1, sizeY() - 1);

            for (i = i1 = 0; i < sizeX(); i++)
            {
                if (i == x)
                    continue;
                for (j = j1 = 0; j < sizeY(); j++)
                {
                    if (j == y)
                        continue;
                    temp.m_[i1][j1] = m_[i][j];
                    j1++;
                }
                i1++;
            }
            T cof = temp.det();
            if ((x + y) % 2 == 1)
                cof = -cof;

            return cof;
        }

        /**
         * Calculate adjoin of a matrix.
         * @return
         */
        matrix<T>&& adj()
        {
            checkSquare(*this, "matrix<T>::adj()");

            matrix<T> temp(sizeX(), sizeY());

            for (size_t i = 0; i < sizeX(); i++)
                for (size_t j = 0; j < sizeY(); j++)
                    temp.m_[j][i] = Cofact(i, j);
            return temp;
        }

        bool isSquare() const
        {
            return (sizeX() == sizeY());
        }

        /**
         * Check whether this is a singular matrix.
         * @return true if so, false otherwise
         */
        bool isSingular() const
        {
            return isSquare() && det() == T(0);
        }

        /**
         * Check whether this is a diagonal matrix.
         * @return true if so, false otherwise
         */
        bool isDiagonal() const
        {
            if (isSquare())
                return false;
            for (size_t i = 0; i < sizeX(); i++)
                for (size_t j = 0; j < sizeY(); j++)
                    if (i != j && m_[i][j] != T(0))
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
            T v = m_[0][0];
            for (size_t i = 1; i < sizeX(); i++)
                if (m_[i][i] != v)
                    return false;
            return true;
        }

        /**
         * Check whether this is a unit matrix.
         * @return true if so, false otherwise
         */
        bool isUnit() const
        {
            if (isScalar() && m_[0][0] == T(1))
                return true;
            return false;
        }

        /**
         * Check whether this is a null matrix.
         * @return true if so, false otherwise
         */
        bool isNull() const
        {
            for (size_t i = 0; i < sizeX(); i++)
                for (size_t j = 0; j < sizeY(); j++)
                    if (m_[i][j] != T(0))
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
            for (size_t i = 0; i < sizeX(); i++)
                for (size_t j = 0; j < sizeY(); j++)
                    if (m_[i][j] != m_[j][i])
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
            for (size_t i = 0; i < sizeX(); i++)
                for (size_t j = 0; j < sizeY(); j++)
                    if (m_[i][j] != -m_[j][i])
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
            for (size_t i = 1; i < sizeX(); i++)
                for (size_t j = 0; j < i - 1; j++)
                    if (m_[i][j] != T(0))
                        return false;
            return true;
        }

        /**
         * Check whether this is a lower triangular matrix.
         * @return true if so, false otherwise
         */
        bool isLowerTriangular() const
        {
            if (sizeX() != sizeY())
                return false;

            for (size_t j = 1; j < sizeY(); j++)
                for (size_t i = 0; i < j - 1; i++)
                    if (m_[i][j] != T(0))
                        return false;

            return true;
        }

        /**
         * Equality operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return true if equal, false, otherwise
         */
        friend bool operator==(const matrix<T>& lhs, const matrix<T>& rhs)
        {
            return lhs.m_ == rhs.m_;
        }

        /**
         * Inequality operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return true if *NOT* equal, false, otherwise
         */
        friend bool operator!=(const matrix<T>& lhs, const matrix<T>& rhs)
        {
            return !(lhs == rhs);
        }

        /**
         * Input stream function.
         * @param istrm the input stream to read from
         * @param m the matrix to read into
         * @return reference to the stream
         */
        friend std::istream& operator>>(std::istream& istrm, matrix<T>& m)
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
        friend std::ostream& operator<<(std::ostream& ostrm, const matrix<T>& m)
        {
            for (size_t i = 0; i < m.sizeY(); i++)
            {
                for (size_t j = 0; j < m.sizeX(); j++)
                {
                    T x = m(i, j);
                    ostrm << x << '\t';
                }
                ostrm << std::endl;
            }
            return ostrm;
        }

    private:

        /**
         * Partial pivoting method.
         * @param pivX
         * @return
         */
        long long pivot(size_t pivX)
        {
            long long k = pivX;
            long double amax = -1.0;
            long double temp = 0.0;

            for (size_t x = pivX; x < sizeX(); x++)
                if ((temp = abs(m_[x][pivX])) > amax && temp != 0.0)
                {
                    amax = temp;
                    k = x;
                }
            if (m_[k][pivX] == T(0))
                return -1;
            if (k != (long long) (pivX))
            {
                std::swap(m_[k], m_[pivX]);
                return k;
            }
            return 0;
        }
    };

}

#endif //NS_UTIL_MATRIX_H_INCLUDED
