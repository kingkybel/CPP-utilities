/*
 * File Name:   grid.h
 * Description: a template structure to hold data in a rectangular scheme
 *
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
 * @date: 2019-05-06
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_GRID_H_INCLUDED
#define NS_UTIL_GRID_H_INCLUDED

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <utility>
#include <stdexcept>
#include "stringutil.h"

using namespace std;

namespace util
{

    class grid_error : public std::out_of_range
    {
    public:

        grid_error(const string& what_arg)
        : std::out_of_range(what_arg)
        {
        }
    };

    template <class EL_TYPE>
    class gridItf
    {
    public:

        enum class Mode
        {
            NoAutoGrow = 0x0,
            AutoGrowX = 0x1,
            AutoGrowY = 0x2,
            AutoGrow = Mode::AutoGrowX | Mode::AutoGrowY
        };

        enum class DisplayMode
        {
            Sparse = 0x01,
            Full = 0x02,
            Stats = 0x04
        };

        /**
         * Default constructor.
         */
        gridItf(EL_TYPE defaultValue = EL_TYPE())
        : defaultValue_(defaultValue)
        {
        }

        gridItf(const gridItf<EL_TYPE>& rhs) = default;
        virtual ~gridItf() = default;
        gridItf& operator=(const gridItf<EL_TYPE>& rhs) = default;

        /**
         *
         * @param mode
         */
        void setMode(Mode mode)
        {
            mode_ = mode;
        }

        /**
         *
         * @return
         */
        Mode getMode() const
        {
            return mode_;
        }

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                    pure virtual functions                                //
        //                                                                          //
        virtual void resize(const size_t newDimX, const size_t newDimY) = 0;
        virtual size_t sizeX() const = 0;
        virtual size_t sizeY() const = 0;
        virtual EL_TYPE& get(const size_t indexX, const size_t indexY) = 0;
        virtual EL_TYPE get(const size_t indexX, const size_t indexY) const = 0;
        virtual void set(const size_t indexX,
                         const size_t indexY,
                         const EL_TYPE& newValue) = 0;
        virtual void show(DisplayMode mode) = 0;
        virtual void setAll(const EL_TYPE& p_elValue) = 0;
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
         *
         * @return
         */
        EL_TYPE getDefaultValue() const
        {
            return defaultValue_;
        }

        /**
         *
         * @return
         */
        EL_TYPE& refDefaultValue()
        {
            return defaultValue_;
        }

        /**
         *
         * @param defaultValue
         */
        void setDefaultValue(const EL_TYPE& defaultValue)
        {
            defaultValue_ = defaultValue;
        }
    private:
        EL_TYPE defaultValue_;
        Mode mode_;
    };

    class index_pair : public std::pair<size_t, size_t>
    {
    public:

        /**
         * Default constructor.
         * @param x x-coordinate
         * @param y y-coordinate
         */
        index_pair(size_t x = 0, size_t y = 0)
        : std::pair<size_t, size_t>(x, y)
        {
        }
        index_pair(const index_pair& rhs) = default;
        virtual ~index_pair() = default;
        index_pair operator=(const index_pair& rhs) = default;

        /**
         * Get the x-component.
         * @return reference to the x-component
         */
        size_t& x()
        {
            return second;
        }

        /**
         * Get the y-component.
         * @return reference to the y-component
         */
        size_t& y()
        {
            return first;
        }

        /**
         * Get the x-component.
         * @return value of the x-component
         */
        size_t x() const
        {
            return second;
        }

        /**
         * Get the y-component.
         * @return value of the y-component
         */
        size_t y() const
        {
            return first;
        }

        /**
         * Lexical equality of two index_pair's.
         * @param lhs left-hand-side
         * @param rhs right-hand-side
         */
        friend bool operator==(const index_pair& lhs, const index_pair& rhs)
        {
            return (lhs.y() == rhs.y() && lhs.x() == rhs.x());
        }

        /**
         * Lexical inequality of two index_pair's.
         * @param lhs left-hand-side
         * @param rhs right-hand-side
         */
        friend bool operator!=(const index_pair& lhs, const index_pair& rhs)
        {
            return (lhs.y() != rhs.y() || lhs.x() != rhs.x());
        }

        /**
         * Lexical smaller than of two index_pair's.
         * @param lhs left-hand-side
         * @param rhs right-hand-side
         */
        friend bool operator<(const index_pair& lhs, const index_pair& rhs)
        {
            return (lhs.y() < rhs.y() ||
                    (lhs.y() == rhs.y() && lhs.x() < rhs.x()));
        }

        /**
         * Lexical smaller or equal than of two index_pair's.
         * @param lhs left-hand-side
         * @param rhs right-hand-side
         */
        friend bool operator<=(const index_pair& lhs, const index_pair& rhs)
        {
            return (y() < rhs.y() || (y() == rhs.y() && x() <= rhs.x()));
        }

        /**
         * Lexical bigger than of two index_pair's.
         * @param lhs left-hand-side
         * @param rhs right-hand-side
         */
        friend bool operator>(const index_pair& lhs, const index_pair& rhs)
        {
            return (lhs.y() > rhs.y() ||
                    (lhs.y() == rhs.y() && lhs.x() > rhs.x()));
        }

        /**
         * Lexical bigger or equal than of two index_pair's.
         * @param lhs left-hand-side
         * @param rhs right-hand-side
         */
        friend bool operator>=(const index_pair& lhs, const index_pair& rhs)
        {
            return (lhs.y() > rhs.y() ||
                    (lhs.y() == rhs.y() && lhs.x() >= rhs.x()));
        }

        /**
         * Out-stream operator.
         * @param os standard out-stream reference
         * @param ind the index_pair to stream
         */
        friend ostream& operator<<(ostream& os, const index_pair& ind)
        {
            os << "(" << ind.x() << "," << ind.y() << ")";
            return os;
        }

        /**
         * Increment the two-dimensional index according to the given boundaries.
         * Wraps around to the next row whan end of column is reached.
         * @param dims the boundaries
         */
        void increment(index_pair dims)
        {
            x()++;
            if (x() == dims.x())
            {
                x() = 0;
                y()++;
            }
        }

        /**
         * Check whether this is within the given boundaries.
         * @param dims  the boundaries
         * @return true is so, false otherwise
         */
        bool isWithinBounds(index_pair dims) const
        {
            return x() < dims.x() && y() < dims.y();
        }
    };

    template <typename EL_TYPE = long double>
    class sparse_grid : public gridItf<EL_TYPE>
    {
    private:
        typedef std::map<index_pair, EL_TYPE> DATA_CONTAINER;
        typedef std::set<size_t> INDEXSET;
        typedef std::map<size_t, INDEXSET> INDEXSETMAP;

        enum class IndexSetOper
        {
            Insert = 0x01,
            Remove = 0x02
        };

    public:
        typedef DATA_CONTAINER::iterator iterator;
        typedef INDEXSET::iterator iteratorX;
        typedef INDEXSET::iterator iteratorY;

        /**
         * Iterator into the raw underlying data container.
         * @return the begin() iterator of the underlying data
         */
        iterator begin()
        {
            return data_.begin();
        }

        /**
         * Just beyond the end of the raw underlying data container.
         * @return the end() iterator of the underlying data
         */
        iterator end()
        {
            return data_.end();
        }

        /**
         * The iterator to the first logical non-default element in X-direction
         * at row y.
         * @param y the row index
         * @return iterator to the first logical non-default element in X-direction
         */
        iteratorX beginX(size_t y)
        {
            auto indexIter = xIndices_.find(y);
            if (indexIter == xIndices_.end())
            {
                return NULL;
            }
            auto xIter = (indexIter->second).begin();
            return xIter;
        }

        /**
         * Just beyond the end of the  logical non-default element in X-direction.
         * @return the end() iterator in x-direction at y
         */
        iteratorX endX(size_t y)
        {
            auto indexIter = xIndices_.find(y);
            if (indexIter == xIndices_.end())
            {
                return NULL;
            }
            return (indexIter->second).end();
        }

        bool iteratorXValid(iteratorX iter)
        {
            if (iter == NULL)
                return false;
            else
                return true;
        }

        bool iteratorYValid(iteratorY iter)
        {
            if (iter == NULL)
                return false;
            else
                return true;
        }

        /**
         * The iterator to the first logical non-default element in Y-direction
         * at column x.
         * @param x the column index
         * @return iterator to the first logical non-default element in X-direction
         */
        iteratorY beginY(size_t x)
        {
            auto indexIter = yIndices_.find(x);
            if (indexIter == yIndices_.end())
            {
                return NULL;
            }
            auto yIter = (indexIter->second).begin();
            return yIter;
        }

        /**
         * Just beyond the end of the  logical non-default element in Y-direction.
         * @param x the column index
         * @return the end() iterator in y-direction at x
         */
        iteratorY endY(size_t x)
        {
            auto indexIter = yIndices_.find(x);
            if (indexIter == yIndices_.end())
            {
                return NULL;
            }
            return (indexIter->second).end();
        }

        /**
         * Default constructor.
         * @param dimX
         * @param dimY
         * @param defaultValue
         * @param mode
         */
        sparse_grid(size_t dimX = 0,
                    size_t dimY = 0,
                    EL_TYPE defaultValue = EL_TYPE(),
                    Mode mode = Mode::AutoGrow)
        : gridItf(defaultValue)
        , dims_(dimX, dimY)
        {
            setMode(mode);
        }

        sparse_grid(const sparse_grid<EL_TYPE>& rhs) = default;
        virtual ~sparse_grid() = default;
        sparse_grid<EL_TYPE> operator=(const sparse_grid<EL_TYPE>& rhs) = default;

        /**
         *
         * @param x
         * @param y
         * @param value
         */
        void set(const size_t x, const size_t y, const EL_TYPE& value)
        {
            Mode mode = getMode();

            if (
                (x < dims_.x() || (mode | Mode::AutoGrowX == Mode::AutoGrowX)) &&
                (y < dims_.y() || (mode | Mode::AutoGrowY == Mode::AutoGrowY))
                )
            {
                index_pair insertIndex;
                insertIndex.x() = x;
                insertIndex.y() = y;
                if (value != getDefaultValue())
                {
                    data_.insert(DATA_CONTAINER::value_type(insertIndex, value));
                    updateIndexSets(x, y, IndexSetOper::Insert);
                }
                else // if the new value equals the default value
                {
                    // if there is a value already there, we delete it
                    if (data_.find(insertIndex) != data_.end())
                    {
                        data_.erase(data_.find(insertIndex));
                        updateIndexSets(x, y, IndexSetOper::Remove);
                    }
                }
                dims_.x() = std::max(dims_.x(), x + 1);
                dims_.y() = std::max(dims_.y(), y + 1);
            }
            else
            {
                throw grid_error("Set a value at "
                                 + asString(index_pair)
                                 + " out of bounds"
                                 + asString(dims_)
                                 + ".");
            }
        }

        /**
         * Retrieve the element at coordinates (x,y) if possible.
         * @param x X-position
         * @param y Y-position
         * @return the value at coordinates (x,y)
         */
        EL_TYPE& get(const size_t x, const size_t y)

        {
            index_pair ind(x, y);
            auto iter = data_.find(ind);

            if (
                (x < dims_.x() || (getMode() | Mode::AutoGrowX) == Mode::AutoGrowX) &&
                (y < dims_.y() || (getMode() | Mode::AutoGrowY) == Mode::AutoGrowY))
            {

                if (iter == data_.end())
                {
                    set(x, y, getDefaultValue());
                    iter = data_.find(index_pair(x, y));
                }
            }
            else
            {
                throw grid_error("Get a value at "
                                 + asString(ind)
                                 + " out of bounds"
                                 + asString(dims_)
                                 + ".");
            }
            return iter->second;
        }

        /**
         * Retrieve the element at coordinates (x,y) if possible.
         * @param x X-position
         * @param y Y-position
         * @return the value at coordinates (x,y)
         */
        EL_TYPE get(const size_t x, const size_t y) const

        {
            index_pair findIndex(x, y);

            auto iter = data_.find(findIndex);

            if (iter == data_.end())
            {

                return getDefaultValue();
            }
            return iter->second;
        }

        /**
         * Resize the grid to new dimensions.
         * @param nNewSizeX new size in x-dimension
         * @param nNewSizeY new size in y-dimension
         */
        void resize(const size_t nNewSizeX, const size_t nNewSizeY)

        {
            // if we make the scheme larger in all dimensions, then we just need
            // to adjust the sizes
            if (nNewSizeX >= dims_.x() && nNewSizeY >= dims_.y())
            {
                dims_.x() = nNewSizeX;
                dims_.y() = nNewSizeY;

                return;
            }
            else
            {
                dims_.x() = nNewSizeX;
                dims_.y() = nNewSizeY;

                auto iter = data_.begin();
                while (iter != data_.end())
                {
                    if ((iter->first).x() >= dims_.x() || (iter->first).y() >= dims_.y())
                    {
                        // delete the map element
                        iter = data_.erase(iter);
                    }
                    else
                    {
                        iter++;
                    }
                }
            }
        }

        /**
         * Retrieve the size in x-dimension.
         * @return the size in x-dimension
         */
        size_t sizeX() const
        {
            return dims_.x();
        }

        /**
         * Retrieve the size in y-dimension.
         * @return the size in y-dimension
         */
        size_t sizeY() const
        {
            return dims_.y();
        }

        /**
         *
         * @param mode
         */
        void show(DisplayMode mode = DisplayMode::Stats)
        {
            cout << "rect Data " << dims_.x() << " x " << dims_.y() << endl;
            if ((mode | DisplayMode::Stats) == DisplayMode::Stats)
            {
                double totalValues = (double) sizeX() * (double) sizeY();
                double fillPercentage;
                if (totalValues != 0.0)
                    fillPercentage = ((double) (data_.size()) / (totalValues))*100.0;
                else
                    fillPercentage = 0.0;
                cout << "\telements different from default value:" << fillPercentage << "%" << endl;
            }
            auto iter = data_.begin();
            if ((mode | DisplayMode::Full) == DisplayMode::Full)
            {
                index_pair runner;
                index_pair start(0, 0);
                index_pair end;

                // display all elements and the gaps between them
                while (iter != data_.end())
                {
                    // end point (of the gap)
                    end = iter->first;

                    // if there is a non-empty gap
                    if (start < end)
                    {
                        // start from the start point and iterate through all
                        // indices that are not equal end and that are lexical
                        // before end
                        for (runner = start;
                             runner != end && runner.isWithinBounds(dims_);
                             runner.increment(dims_))
                        {
                            cout << getDefaultValue();
                            if (runner.x() == dims_.x() - 1)
                                cout << endl;
                            else
                                cout << ",";
                        }

                        start = end;
                        start.increment(dims_);
                    }
                    cout << iter->second;
                    if ((iter->first).x() == dims_.x() - 1)
                        cout << endl;
                    else
                        cout << ",";
                    iter++;
                }
                // now write the remaining fields
                for (runner = start;
                     runner.isWithinBounds(dims_);
                     runner.increment(dims_))
                {
                    cout << getDefaultValue();
                    if (runner.x() == dims_.x() - 1)
                        cout << endl;
                    else
                        cout << ",";
                }

            }
            else if ((mode | DisplayMode::Sparse) == DisplayMode::Sparse)
            {
                size_t line = 0;
                if (iter != data_.end())
                {
                    cout << "line [" << (iter->first).y() << "]\t";
                    line = (iter->first).y();
                }
                while (iter != data_.end())
                {
                    if (line != (iter->first).y())
                    {

                        cout << endl;
                        line = (iter->first).y();
                        cout << "line [" << line << "]\t";
                    }
                    cout << "[" << (iter->first).x() << "]" << iter->second << " ";
                    iter++;
                }
                cout << endl;
            }

            /*
           cout << "x - iter" << endl;
           for(size_t x = 0; x<getSizeX(); x++)
           {
               auto setIter = m_yIndices.find(x);

               // if we have any indices with (x,...)
               if(setIter != m_yIndices.end())
               {
                   cout << "[" << x << "]\t";
                   // find the first y-index for x
                   iteratorY yIter = beginY(x);
                   while(yIter != endY(x))
                   {
                       cout << *yIter << "|";
                       yIter++;
                   }
                   cout << endl;
               }
           }
           cout << "y - iter" << endl;
           for(size_t y = 0; y<getSizeY(); y++)
           {
               auto setIter = m_xIndices.find(y);

               // if we have any indices with (x,...)
               if(setIter != m_xIndices.end())
               {
                   cout << "[" << y << "]\t";
                   // find the first y-index for x
                   iteratorX xIter = beginX(y);
                   while(xIter != endX(y))
                   {
                       cout << *xIter << ",";
                       xIter++;
                   }
                   cout << endl;
               }
           }
             */
        }

        /**
         * Clear the grid and set all values to the same value.
         * @param value the new value
         */
        void setAll(const EL_TYPE& value)
        {
            setDefaultValue(value);
            data_.clear();
        }

        /**
         *
         * @param nIndexX
         * @param nIndexY
         * @return
         */
        EL_TYPE operator()(const size_t nIndexX, const size_t nIndexY) const
        {
            return get(nIndexX, nIndexY);
        }

        /**
         * @Name:       operator ()
         * @Purpose:
         * @Arguments:  @* s - dummy @nl
         *              @* p_nIndexX -  @nl
         *              @* p_nIndexY -  @nl
         * @Returns:    EL_TYPE&
         * @Description:
         */
        EL_TYPE& operator()(string s, const size_t nIndexX, const size_t nIndexY)
        {
            TRACE0();

            if (nIndexX >= sizeX() || nIndexY >= sizeY())
            {
                long mode = getMode();
                if ((mode | Mode::AutoGrow) != Mode::AutoGrow)
                {

                    throw grid_error("Get a value at "
                                     + asString(index_pair(nIndexX, nIndexY))
                                     + " out of bounds"
                                     + asString(dims_)
                                     + ".");
                }
            }
            createDataEntry(nIndexX, nIndexY);

            return get(nIndexX, nIndexY);
        }

        /**
         * Insert or remove indices.
         * @param x
         * @param y
         * @param mode
         */
        void updateIndexSets(size_t x, size_t y, IndexSetOper mode)
        {
            if (mode == IndexSetOper::Insert)
            {
                INDEXSET setToInsert;
                if (xIndices_.find(y) == xIndices_.end())
                {
                    xIndices_.insert(INDEXSETMAP::value_type(y, setToInsert));
                }
                ((xIndices_.find(y))->second).insert(x);

                if (yIndices_.find(x) == yIndices_.end())
                {
                    yIndices_.insert(INDEXSETMAP::value_type(x, setToInsert));
                }
                ((yIndices_.find(x))->second).insert(y);
                size_t t = ((yIndices_.find(x))->second).size();
            }
            else if (mode == IndexSetOper::Remove)
            {
                if (xIndices_.find(y) != xIndices_.end())
                {
                    xIndices_.erase(xIndices_.find(y));
                }
                if (xIndices_.find(y) != yIndices_.end())
                {

                    yIndices_.erase(yIndices_.find(x));
                }
            }
        }

        void indexSetInfo(size_t x, size_t y)
        {
            auto iterX = xIndices_.find(y);
            auto iterY = yIndices_.find(x);
            cout << "m_xIndices=" << xIndices_.size() << " m_yIndices=" << yIndices_.size() << endl;
            if (iterX == xIndices_.end())
                cout << "==== row " << y << " not found,";
            else
            {
                cout << (iterX->second).size() << " x-Indices attached to row " << y << endl;
                auto iy = (iterX->second).begin();
                while (iy != (iterX->second).end())
                {
                    cout << *iy << ",";
                    iy++;
                }
                cout << endl;
            }
            if (iterY == yIndices_.end())
                cout << " line " << x << " not found,";
            else
            {
                cout << (iterY->second).size() << " y-Indices attached to row " << x << endl;
                auto ix = (iterY->second).begin();
                while (ix != (iterY->second).end())
                {

                    cout << *ix << ",";
                    ix++;
                }
                cout << endl;
            }
            cout << endl;
        }

        /**
         * @Name:       createDataEntry
         * @Purpose:
         * @Arguments:  @* x -  @nl
         *              @* y -  @nl
         * @Returns:    void
         * @Description:
         */
        void createDataEntry(const size_t x, const size_t y)
        {
            long mode = getMode();
            if ((x < dims_.x() && y < dims_.y()) || ((mode | Mode::AutoGrow) == Mode::AutoGrow))
            {
                index_pair insertPoint;
                insertPoint.x() = x;
                insertPoint.y() = y;
                data_.insert(DATA_CONTAINER::value_type(insertPoint, getDefaultValue()));
                updateIndexSets(x, y, IndexSetOper::Insert);
                dims_.x() = std::max(dims_.x(), x + 1);
                dims_.y() = std::max(dims_.y(), y + 1);
            }
            else
            {
                throw grid_error("Set a value at "
                                 + asString(index_pair(x, y))
                                 + " out of bounds"
                                 + asString(dims_)
                                 + ".");
            }
        }
    private:
        index_pair dims_;
        DATA_CONTAINER data_;
        INDEXSETMAP xIndices_;
        INDEXSETMAP yIndices_;
    };

    template <typename EL_TYPE = long double>
    class grid : public gridItf<EL_TYPE>
    {
    private:

        typedef std::vector <EL_TYPE> EL_VECT;
        typedef std::vector <EL_VECT> EL_MATRIX;

        // variables
        EL_MATRIX values_;

    public:

        /**
         * Default constructor.
         * @param dimX X-dimension
         * @param dimY Y-dimension
         * @param pDefaultValue
         * @param p_nMode
         */
        grid(const size_t dimX = 0,
             const size_t dimY = 0,
             EL_TYPE* pDefaultValue = 0,
             Mode mode = Mode::AutoGrow)
        : gridItf<EL_TYPE>()
        {
            TRACE0();

            setMode(mode);
            if (pDefaultValue != NULL)
            {
                setDefaultValue(*pDefaultValue);
            }
            for (size_t iIndexX = 0; iIndexX < dimX; iIndexX++)
            {

                values_.push_back(vector<EL_TYPE>(dimY));
            }
        }

        grid(const grid<EL_TYPE>& rhs) = default;
        virtual ~grid() = default;
        grid<EL_TYPE>& operator=(const grid<EL_TYPE>& rhs) = default;

        /**
         *
         * @return
         */
        size_t sizeX() const
        {
            return values_.size();
        }

        /**
         *
         * @return
         */
        size_t sizeY() const
        {
            if (values_.size())
            {
                return values_[0].size();
            }
            else
            {
                return 0;
            }
        }

        /**
         *
         * @param newX
         * @param newY
         */
        void resize(const size_t newX, const size_t newY)

        {
            TRACE0();

            values_.resize(newX);

            for (size_t x = 0; x < newX; x++)
            {
                values_[x].resize(newY);
            }
        }

        /**
         *
         * @param p_nStartX
         * @param p_nNumberOfX
         * @param p_nStartY
         * @param p_nNumberOfY
         * @return
         */
        grid& subRange(size_t p_nStartX,
                       long p_nNumberOfX,
                       size_t p_nStartY,
                       long p_nNumberOfY)

        {

            TRACE0();

        }

        /**
         *
         * @param x
         * @param y
         * @return
         */
        EL_TYPE& get(const size_t x, const size_t y)

        {

            TRACE0();

            return ((values_[x])[y]);
        }

        /**
         *
         * @param value
         */
        void setAll(const EL_TYPE& value)
        {
            TRACE0();

            for (size_t iIndexY = 0; iIndexY < sizeY(); iIndexY++)
            {
                for (size_t iIndexX = 0; iIndexX < sizeX(); iIndexX++)
                {

                    set(iIndexX, iIndexY, value);
                }
            }
        }

        /**
         *
         * @param x
         * @param y
         * @param value
         */
        void set(const size_t x, const size_t y, const EL_TYPE& value)
        {
            TRACE0();

            if (x >= sizeX() || y >= sizeY())
            {
                long mode = getMode();
                if ((mode | Mode::AutoGrow) == Mode::AutoGrow)
                {

                    resize(std::max(x + 1, sizeX()),
                           std::max(y + 1, sizeY()));
                }
            }
            ((values_[x])[y]) = value;
        }

        /**
         *
         * @param x
         * @param y
         * @return
         */
        EL_TYPE& operator()(const size_t x, const size_t y = 0)
        {
            TRACE0();

            if (x >= sizeX() || y >= sizeY())
            {
                long mode = getMode();
                if ((mode | Mode::AutoGrow) == Mode::AutoGrow)
                {

                    resize(std::max(x + 1, sizeX()),
                           std::max(y + 1, sizeY()));
                }
            }
            return ((values_[x])[y]);
        }

        /**
         *
         * @param x
         * @param y
         * @return
         */
        EL_TYPE operator()(const size_t x, const size_t y = 0) const

        {
            TRACE0();

            if (x >= sizeX() || y >= sizeY())
            {

                cerr << "index is out of bounds";
                exit(-1);
            }
            return ((values_[x])[y]);
        }

        /**
         *
         * @param mode
         */
        void show(DisplayMode mode = DisplayMode::Full/*unused*/)
        {
            TRACE0();

            cout << "Rectangular Data (" << sizeX() << " x " << sizeY() << ")" << endl;
            for (size_t iIndexY = 0; iIndexY < sizeY(); iIndexY++)
            {
                for (size_t iIndexX = 0; iIndexX < sizeX(); iIndexX++)
                {
                    cout << get(iIndexX, iIndexY) << "\t";
                }
                cout << endl;
            }
            cout << "\n---\n" << endl;
        }
    };

}; // namepace util

#endif //NS_UTIL_GRID_H_INCLUDED

