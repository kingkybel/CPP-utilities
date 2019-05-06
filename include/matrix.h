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

namespace util
{

    class matrix_error : public std::logic_error
    {
    public:

        matrix_error(const string& what_arg)
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
    template <typename T>
    class matrix
    {
    private:
        typedef std::vector<T> row_t;
        typedef std::vector<row_t> mat_t;
        mat_t m_;

    public:

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
            return reval;
        }

        /**
         * Default constructor.
         * @param xDim x-dimension
         * @param yDim y-dimension
         */
        matrix(size_t xDim = 6, size_t yDim = 6)
        {
            std::swap(m_, initializeData(xDim, yDim));
        }

        /**
         * Default constructor.
         * @param xDim x-dimension
         * @param yDim y-dimension
         */
        matrix(size_t xyDim = 6, bool isUnit = false)
        {
            std::swap(m_, initializeData(xyDim, xyDim));
            if (isUnit)
                for (size_t xy = 0; xy < xyDim; xy++)
                    m_[xy][xy] = (T) 1.0;
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

        void checkBounds(size_t x, size_t y, const std::string& location)
        {
            if (x >= sizeX() || y >= sizeY())
                throw matrix_error(location +
                                   ": index (" +
                                   asString(x) +
                                   "," +
                                   asString(y) +
                                   ") is out of bounds (" +
                                   sizeX() +
                                   "," +
                                   sizeY() +
                                   ").");
        }

        void checkCompatibleSizes(const matrix<T>& lhs,
                                  const matrix<T>& rhs,
                                  const std::string& operation,
                                  const std::string& location)
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
        }

        void checkNotZero(T c, const std::string& location)
        {
            if (c == (T) 0)
                throw matrix_error(location +
                                   ": scalar " +
                                   asString(c) +
                                   "must not be 0(Zero).");
        }

        void checkSquare(const matrix<T>& lhs, const std::string& location)
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


        // Unary operators

        matrix operator+()
        {
            return *this;
        }
        matrix<T> operator-();

        /**
         * unary negation operator
         */
        matrix<T>&& matrix<T>::operator-(const matrix<T>& rhs)
        {
            matrix<T> temp(rhs);

            for (size_t y = 0; i < temp.sizeY(); i++)
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
        matrix<T>&& operator*(const matrix<T>& lhs, const matrix<T>& rhs)
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
        matrix<T>&& operator/(const T& c, const matrix<T>& rhs)
        {
            return (!rhs * c);
        }

        /**
         * Global matrix divided by matrix operator.
         * @param lhs left-hand-side matrix
         * @param rhs right-hand-side matrix
         * @return the result of lhs/rhs
         */
        matrix<T>&& operator/(const matrix<T>& lhs, const matrix<T>& rhs)
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
        matrix<T>& operator^(const matrix<T>& lhs, const size_t& pow)
        {
            checkSquare(lhs, "operator^(lhs,pow)");
            matrix<T> temp(lhs);
            for (size_t i = 2; i <= pow; i++)
                temp = lhs * temp;

            return *this;
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
        matrix<T>&& operator~(const matrix<T>& rhs)
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
        matrix<T>&& operator!(const matrix<T>& rhs)
        {
            matrix<T> temp = rhs;
            return temp.Inv();
        }

        /**
         * Inversion function.
         *
         * @return the inverted non-singular square matrix if possible.
         */
        matrix<T>&& matrix<T>::Inv()
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

        // Utility methods
        matrix<T> Solve(const matrix<T>& v) const;
        matrix<T> Adj();
        matrix<T> Inv();
        T Det() const;
        T Norm();
        T Cofact(size_t row, size_t col);
        T Cond();

        bool isSquare() const
        {
            return (sizeX() == sizeY());
        }
        bool IsSingular();
        bool IsDiagonal();
        bool IsScalar();
        bool IsUnit();
        bool IsNull();
        bool IsSymmetric();
        bool IsSkewSymmetric();
        bool IsUpperTriangular();
        bool IsLowerTriangular();

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
        friend istream& operator>>(istream& istrm, matrix<T>& m)
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
        friend ostream& operator<<(ostream& ostrm, const matrix<T>& m)
        {
            for (size_t i = 0; i < m.sizeY(); i++)
            {
                for (size_t j = 0; j < m.sizeX(); j++)
                {
                    T x = m(i, j);
                    ostrm << x << '\t';
                }
                ostrm << endl;
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
            if (k != long long(pivX))
            {
                std::swap(m_[k], m_[pivX]);
                return k;
            }
            return 0;
        }
    };


    // solve simultaneous equation

    template<typename T> inline matrix<T>
    matrix<T>::Solve(const matrix<T>& v) const _THROW_MATRIX_ERROR
    {
        size_t i, j, k;
        T a1;

        if (!(m_->Row == m_->Col && m_->Col == v.m_->Row))
            REPORT_ERROR("matrix<T>::Solve():Inconsistent matrices!");

        matrix<T> temp(m_->Row, m_->Col + v.m_->Col);
        for (i = 0; i < m_->Row; i++)
        {
            for (j = 0; j < m_->Col; j++)
                temp.m_->Val[i][j] = m_->Val[i][j];
            for (k = 0; k < v.m_->Col; k++)
                temp.m_->Val[i][m_->Col + k] = v.m_->Val[i][k];
        }
        for (k = 0; k < m_->Row; k++)
        {
            int indx = temp.pivot(k);
            if (indx == -1)
                REPORT_ERROR("matrix<T>::Solve(): Singular matrix!");

            a1 = temp.m_->Val[k][k];
            for (j = k; j < temp.m_->Col; j++)
                temp.m_->Val[k][j] /= a1;

            for (i = k + 1; i < m_->Row; i++)
            {
                a1 = temp.m_->Val[i][k];
                for (j = k; j < temp.m_->Col; j++)
                    temp.m_->Val[i][j] -= a1 * temp.m_->Val[k][j];
            }
        }
        matrix<T> s(v.m_->Row, v.m_->Col);
        for (k = 0; k < v.m_->Col; k++)
            for (int m = int(m_->Row) - 1; m >= 0; m--)
            {
                s.m_->Val[m][k] = temp.m_->Val[m][m_->Col + k];
                for (j = m + 1; j < m_->Col; j++)
                    s.m_->Val[m][k] -= temp.m_->Val[m][j] * s.m_->Val[j][k];
            }
        return s;
    }


    // calculate the determinant of a matrix

    template<typename T> inline T
    matrix<T>::Det() const _THROW_MATRIX_ERROR
    {
        size_t i, j, k;
        T piv, detVal = T(1);

        if (m_->Row != m_->Col)
            REPORT_ERROR("matrix<T>::Det(): Determinant a non-square matrix!");

        matrix<T> temp(*this);
        if (temp.m_->Refcnt > 1) temp.clone();

        for (k = 0; k < m_->Row; k++)
        {
            int indx = temp.pivot(k);
            if (indx == -1)
                return 0;
            if (indx != 0)
                detVal = -detVal;
            detVal = detVal * temp.m_->Val[k][k];
            for (i = k + 1; i < m_->Row; i++)
            {
                piv = temp.m_->Val[i][k] / temp.m_->Val[k][k];
                for (j = k + 1; j < m_->Row; j++)
                    temp.m_->Val[i][j] -= piv * temp.m_->Val[k][j];
            }
        }
        return detVal;
    }

    // calculate the norm of a matrix

    template<typename T> inline T
    matrix<T>::Norm() _NO_THROW
    {
        T retVal = T(0);

        for (size_t i = 0; i < m_->Row; i++)
            for (size_t j = 0; j < m_->Col; j++)
                retVal += m_->Val[i][j] * m_->Val[i][j];
        retVal = sqrt(retVal);

        return retVal;
    }

    // calculate the condition number of a matrix

    template<typename T> inline T
    matrix<T>::Cond() _NO_THROW
    {
        matrix<T> inv = !(*this);
        return (Norm() * inv.Norm());
    }

    // calculate the cofactor of a matrix for a given element

    template<typename T> inline T
    matrix<T>::Cofact(size_t row, size_t col) _THROW_MATRIX_ERROR
    {
        size_t i, i1, j, j1;

        if (m_->Row != m_->Col)
            REPORT_ERROR("matrix<T>::Cofact(): Cofactor of a non-square matrix!");

        if (row > m_->Row || col > m_->Col)
            REPORT_ERROR("matrix<T>::Cofact(): Index out of range!");

        matrix<T> temp(m_->Row - 1, m_->Col - 1);

        for (i = i1 = 0; i < m_->Row; i++)
        {
            if (i == row)
                continue;
            for (j = j1 = 0; j < m_->Col; j++)
            {
                if (j == col)
                    continue;
                temp.m_->Val[i1][j1] = m_->Val[i][j];
                j1++;
            }
            i1++;
        }
        T cof = temp.Det();
        if ((row + col) % 2 == 1)
            cof = -cof;

        return cof;
    }


    // calculate adjoin of a matrix

    template<typename T> inline matrix<T>
    matrix<T>::Adj() _THROW_MATRIX_ERROR
    {
        if (m_->Row != m_->Col)
            REPORT_ERROR("matrix<T>::Adj(): Adjoin of a non-square matrix.");

        matrix<T> temp(m_->Row, m_->Col);

        for (size_t i = 0; i < m_->Row; i++)
            for (size_t j = 0; j < m_->Col; j++)
                temp.m_->Val[j][i] = Cofact(i, j);
        return temp;
    }

    // Determine if the matrix is singular

    template<typename T> inline bool
    matrix<T>::IsSingular() _NO_THROW
    {
        if (m_->Row != m_->Col)
            return false;
        return (Det() == T(0));
    }

    // Determine if the matrix is diagonal

    template<typename T> inline bool
    matrix<T>::IsDiagonal() _NO_THROW
    {
        if (m_->Row != m_->Col)
            return false;
        for (size_t i = 0; i < m_->Row; i++)
            for (size_t j = 0; j < m_->Col; j++)
                if (i != j && m_->Val[i][j] != T(0))
                    return false;
        return true;
    }

    // Determine if the matrix is scalar

    template<typename T> inline bool
    matrix<T>::IsScalar() _NO_THROW
    {
        if (!IsDiagonal())
            return false;
        T v = m_->Val[0][0];
        for (size_t i = 1; i < m_->Row; i++)
            if (m_->Val[i][i] != v)
                return false;
        return true;
    }

    // Determine if the matrix is a unit matrix

    template<typename T> inline bool
    matrix<T>::IsUnit() _NO_THROW
    {
        if (IsScalar() && m_->Val[0][0] == T(1))
            return true;
        return false;
    }

    // Determine if this is a null matrix

    template<typename T> inline bool
    matrix<T>::IsNull() _NO_THROW
    {
        for (size_t i = 0; i < m_->Row; i++)
            for (size_t j = 0; j < m_->Col; j++)
                if (m_->Val[i][j] != T(0))
                    return false;
        return true;
    }

    // Determine if the matrix is symmetric

    template<typename T> inline bool
    matrix<T>::IsSymmetric() _NO_THROW
    {
        if (m_->Row != m_->Col)
            return false;
        for (size_t i = 0; i < m_->Row; i++)
            for (size_t j = 0; j < m_->Col; j++)
                if (m_->Val[i][j] != m_->Val[j][i])
                    return false;
        return true;
    }

    // Determine if the matrix is skew-symmetric

    template<typename T> inline bool
    matrix<T>::IsSkewSymmetric() _NO_THROW
    {
        if (m_->Row != m_->Col)
            return false;
        for (size_t i = 0; i < m_->Row; i++)
            for (size_t j = 0; j < m_->Col; j++)
                if (m_->Val[i][j] != -m_->Val[j][i])
                    return false;
        return true;
    }

    // Determine if the matrix is upper triangular

    template<typename T> inline bool
    matrix<T>::IsUpperTriangular() _NO_THROW
    {
        if (m_->Row != m_->Col)
            return false;
        for (size_t i = 1; i < m_->Row; i++)
            for (size_t j = 0; j < i - 1; j++)
                if (m_->Val[i][j] != T(0))
                    return false;
        return true;
    }

    // Determine if the matrix is lower triangular

    template<typename T> inline bool
    matrix<T>::IsLowerTriangular() _NO_THROW
    {
        if (m_->Row != m_->Col)
            return false;

        for (size_t j = 1; j < m_->Col; j++)
            for (size_t i = 0; i < j - 1; i++)
                if (m_->Val[i][j] != T(0))
                    return false;

        return true;
    }

}

#endif //NS_UTIL_MATRIX_H_INCLUDED
