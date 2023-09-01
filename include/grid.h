/*
 * File Name:   grid.h
 * Description: a template structure to hold data in a rectangular scheme
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

#ifndef NS_UTIL_GRID_H_INCLUDED
#define NS_UTIL_GRID_H_INCLUDED

#include "stringutil.h"

#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace util
{
/**
 * @brief General grid exception.
 */
class grid_error : public std::out_of_range
{
    public:
    grid_error(const std::string &what_arg) : std::out_of_range(what_arg)
    {
    }
};

/**
 * @brief Base class for grid-shaped containers (2-dimensional)
 *
 * @tparam EL_TYPE element-type
 */
template<class EL_TYPE>
class gridBase
{
    public:
    /**
     * @brief enumeration of modes for growing the grid
     */
    enum class Mode
    {
        NoAutoGrow = 0x0,
        AutoGrowX  = 0x1,
        AutoGrowY  = 0x2,
        AutoGrow   = Mode::AutoGrowX | Mode::AutoGrowY
    };

    /**
     * @brief display modes
     */
    enum class DisplayMode
    {
        Sparse = 0x01,
        Full   = 0x02,
        Stats  = 0x04
    };

    /**
     * @brief Default constructor.
     */
    gridBase(EL_TYPE defaultValue = EL_TYPE()) : defaultValue_(defaultValue), mode_(Mode::AutoGrow)
    {
    }

    gridBase(const gridBase<EL_TYPE> &rhs)            = default;
    virtual ~gridBase()                               = default;
    gridBase &operator=(const gridBase<EL_TYPE> &rhs) = default;

    /**
     * @brief Set the mode to a new one.
     *
     * @param mode the new mode
     */
    void setMode(Mode mode)
    {
        mode_ = mode;
    }

    /**
     * @brief Retrieve the currently set mode.
     *
     * @return the currently set mode
     */
    Mode getMode() const
    {
        return (mode_);
    }

    /**
     * @brief Check whether autogrow in X-dimention is set.
     *
     * @return true, if set, false otherwise
     */
    Mode isAutoGrowX() const
    {
        return ((mode_ | Mode::AutoGrowX) == Mode::AutoGrowX);
    }

    /**
     * @brief Check whether autogrow in Y-dimention is set.
     *
     * @return true, if set, false otherwise
     */
    Mode isAutoGrowY() const
    {
        return ((mode_ | Mode::AutoGrowY) == Mode::AutoGrowY);
    }

    /**
     * @brief Check whether autogrow in X- and Y-dimention is set.
     *
     * @return true, if set, false otherwise
     */
    Mode isAutoGrow() const
    {
        return ((mode_ | Mode::AutoGrow) == Mode::AutoGrow);
    }

    ////////////////////////////////////////////////////////////////////////
    //                                                                    //
    //                    pure virtual functions                          //
    //                                                                    //
    virtual void     resize(const size_t newDimX, const size_t newDimY)                     = 0;
    virtual size_t   sizeX() const                                                          = 0;
    virtual size_t   sizeY() const                                                          = 0;
    virtual EL_TYPE &get(const size_t indexX, const size_t indexY)                          = 0;
    virtual EL_TYPE  get(const size_t indexX, const size_t indexY) const                    = 0;
    virtual void     set(const size_t indexX, const size_t indexY, const EL_TYPE &newValue) = 0;
    virtual void     show(DisplayMode mode)                                                 = 0;
    virtual void     setAll(const EL_TYPE &p_elValue)                                       = 0;
    //                                                                    //
    ////////////////////////////////////////////////////////////////////////

    /**
     * @brief Retrieve the default value used if an element is not explicitly set.
     *
     * @return the default value.
     */
    EL_TYPE getDefaultValue() const
    {
        return (defaultValue_);
    }

    /**
     * @brief Retrieve a reference to the default value used if an element is not explicitly set.
     *
     * @return  reference to the default value.
     */
    EL_TYPE &refDefaultValue()
    {
        return (defaultValue_);
    }

    /**
     * @brief Set the default value used if an element is not explicitly set.
     *
     * @param defaultValue new default value
     */
    void setDefaultValue(const EL_TYPE &defaultValue)
    {
        defaultValue_ = defaultValue;
    }

    private:
    EL_TYPE defaultValue_;
    Mode    mode_;
};

/**
 * @brief index-class for 2-dimensional container (grid)
 */
class index_pair : public std::pair<size_t, size_t>
{
    public:
    /**
     * @brief Default constructor.
     * @param x x-coordinate
     * @param y y-coordinate
     */
    index_pair(size_t x = 0, size_t y = 0) : std::pair<size_t, size_t>(x, y)
    {
    }

    index_pair(const index_pair &rhs)            = default;
    virtual ~index_pair()                        = default;
    index_pair &operator=(const index_pair &rhs) = default;

    /**
     * @brief Get the x-component.
     *
     * @return reference to the x-component
     */
    size_t &x()
    {
        return (second);
    }

    /**
     * @brief Get the y-component.
     *
     * @return reference to the y-component
     */
    size_t &y()
    {
        return (first);
    }

    /**
     * @brief Get the x-component.
     *
     * @return value of the x-component
     */
    size_t x() const
    {
        return (second);
    }

    /**
     * @brief Get the y-component.
     *
     * @return value of the y-component
     */
    size_t y() const
    {
        return (first);
    }

    /**
     * @brief Lexical equality of two index_pair's.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     */
    friend bool operator==(const index_pair &lhs, const index_pair &rhs)
    {
        return ((lhs.y() == rhs.y()) && (lhs.x() == rhs.x()));
    }

    /**
     * @brief Lexical inequality of two index_pair's.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     */
    friend bool operator!=(const index_pair &lhs, const index_pair &rhs)
    {
        return ((lhs.y() != rhs.y()) || (lhs.x() != rhs.x()));
    }

    /**
     * @brief Lexical smaller than of two index_pair's.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     */
    friend bool operator<(const index_pair &lhs, const index_pair &rhs)
    {
        return ((lhs.y() < rhs.y()) || ((lhs.y() == rhs.y()) && (lhs.x() < rhs.x())));
    }

    /**
     * @brief Lexical smaller or equal than of two index_pair's.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     */
    friend bool operator<=(const index_pair &lhs, const index_pair &rhs)
    {
        return ((lhs.y() < rhs.y()) || ((lhs.y() == rhs.y()) && (lhs.x() <= rhs.x())));
    }

    /**
     * @brief Lexical bigger than of two index_pair's.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     */
    friend bool operator>(const index_pair &lhs, const index_pair &rhs)
    {
        return ((lhs.y() > rhs.y()) || ((lhs.y() == rhs.y()) && (lhs.x() > rhs.x())));
    }

    /**
     * @brief Lexical bigger or equal than of two index_pair's.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     */
    friend bool operator>=(const index_pair &lhs, const index_pair &rhs)
    {
        return ((lhs.y() > rhs.y()) || ((lhs.y() == rhs.y()) && (lhs.x() >= rhs.x())));
    }

    /**
     * @brief Out-stream operator.
     *
     * @param os standard out-stream reference
     * @param ind the index_pair to stream
     *
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const index_pair &ind)
    {
        os << "(" << ind.x() << "," << ind.y() << ")";

        return (os);
    }

    /**
     * @brief Increment the two-dimensional index according to the given boundaries.
     * Wraps around to the next row when the end of the column is reached.
     *
     * @param dims the boundaries
     */
    void increment(index_pair dims)
    {
        x()++;
        if(x() == dims.x())
        {
            x() = 0;
            y()++;
        }
    }

    /**
     * @brief Check whether this is within the given boundaries.
     *
     * @param dims  the boundaries
     *
     * @return true is so, false otherwise
     */
    bool isWithinBounds(index_pair dims) const
    {
        return ((x() < dims.x()) && (y() < dims.y()));
    }
};

/**
 * @brief Specialisation of the gridBase class that is used for sparse population
 * 
 * @tparam EL_TYPE element-type 
 */
template<typename EL_TYPE = long double>
class sparse_grid : public gridBase<EL_TYPE>
{
    private:
    using DATA_CONTAINER = std::map<index_pair, EL_TYPE>;
    using INDEXSET       = std::set<size_t>;
    using INDEXSETMAP    = std::map<size_t, INDEXSET>;

    /**
     * @brief enumeration of modes how to set an index
     */
    enum class IndexSetOper
    {
        Insert = 0x01,
        Remove = 0x02
    };

    public:
    using iterator  = typename DATA_CONTAINER::iterator;
    using iteratorX = INDEXSET::iterator;
    using iteratorY = INDEXSET::iterator;

    /**
     * @brief Iterator into the raw underlying data container.
     *
     * @return the begin() iterator of the underlying data
     */
    iterator begin()
    {
        return (data_.begin());
    }

    /**
     * @brief Just beyond the end of the raw underlying data container.
     *
     * @return the end() iterator of the underlying data
     */
    iterator end()
    {
        return (data_.end());
    }

    /**
     * @brief The iterator to the first logical non-default element in X-direction
     * at row y.
     *
     * @param y the row index
     *
     * @return iterator to the first logical non-default element in X-direction
     */
    iteratorX beginX(size_t y)
    {
        auto indexIter = xIndices_.find(y);

        if(indexIter == xIndices_.end())
        {
            return (iteratorX{nullptr});
        }

        auto xIter = (indexIter->second).begin();

        return (xIter);
    }

    /**
     * @brief Just beyond the end of the  logical non-default element in X-direction.
     * @return the end() iterator in x-direction at y
     */
    iteratorX endX(size_t y)
    {
        auto indexIter = xIndices_.find(y);

        if(indexIter == xIndices_.end())
        {
            return (iteratorX{nullptr});
        }

        return ((indexIter->second).end());
    }

    /**
     * @brief Check whether the given X-dimension-iterator is valid or not
     * 
     * @param iter the X-dimension iterator
     * @return true, if valid, false otherwise
     */
    bool iteratorXValid(iteratorX iter)
    {
        return ((iter == iteratorX{nullptr}) ? false : true);
    }

    /**
     * @brief Check whether the given Y-dimension-iterator is valid or not
     * 
     * @param iter the Y-dimension iterator
     * @return true, if valid, false otherwise
     */
    bool iteratorYValid(iteratorY iter)
    {
        return ((iter == iteratorY{nullptr}) ? false : true);
    }

    /**
     * @brief The iterator to the first logical non-default element in Y-direction
     * at column x.
     * @param x the column index
     * @return iterator to the first logical non-default element in X-direction
     */
    iteratorY beginY(size_t x)
    {
        auto indexIter = yIndices_.find(x);

        if(indexIter == yIndices_.end())
        {
            return (iteratorY{nullptr});
        }

        auto yIter = (indexIter->second).begin();

        return (yIter);
    }

    /**
     * @brief Just beyond the end of the  logical non-default element in Y-direction.
     * @param x the column index
     * @return the end() iterator in y-direction at x
     */
    iteratorY endY(size_t x)
    {
        auto indexIter = yIndices_.find(x);

        if(indexIter == yIndices_.end())
        {
            return (iteratorY{nullptr});
        }

        return ((indexIter->second).end());
    }

    /**
     * @brief Default constructor.
     *
     * @param dimX size of X-dimension
     * @param dimY size of Y-dimension
     * @param defaultValue default value to return if a cell has not been set
     * @param mode auto-grow behaviour
     */
    sparse_grid(size_t                                 dimX         = 0,
                size_t                                 dimY         = 0,
                EL_TYPE                                defaultValue = EL_TYPE(),
                typename util::gridBase<EL_TYPE>::Mode mode         = util::gridBase<EL_TYPE>::Mode::AutoGrow)
    : util::gridBase<EL_TYPE>(defaultValue)
    , dims_(dimX, dimY)
    {
        setMode(mode);
    }

    sparse_grid(const sparse_grid<EL_TYPE> &rhs)                    = default;
    virtual ~sparse_grid()                                          = default;
    sparse_grid<EL_TYPE> operator=(const sparse_grid<EL_TYPE> &rhs) = default;

    /**
     * @brief Set the element value at coorinates (x, y).
     *
     * @param x X-position
     * @param y Y-position
     * @param value the new value
     */
    void set(const size_t x, const size_t y, const EL_TYPE &value)
    {
        auto autoGrowX = this->util::gridBase<EL_TYPE>::isAutoGrowX();
        auto autoGrowY = this->util::gridBase<EL_TYPE>::isAutoGrowY();

        if((x < dims_.x() || autoGrowX) && (y < dims_.y() || autoGrowY))
        {
            index_pair insertIndex;

            insertIndex.x() = x;
            insertIndex.y() = y;

            if(value != this->util::gridBase<EL_TYPE>::getDefaultValue())
            {
                data_.insert(DATA_CONTAINER::value_type(insertIndex, value));
                updateIndexSets(x, y, IndexSetOper::Insert);
            }
            else  // if the new value equals the default value
            {
                // if there is a value already there, we delete it
                if(data_.find(insertIndex) != data_.end())
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
            throw grid_error("Set a value at " + toString(index_pair(x, y)) + " out of bounds" + toString(dims_) + ".");
        }
    }

    /**
     * @brief Retrieve the element at coordinates (x,y) if possible.
     *
     * @param x X-position
     * @param y Y-position
     *
     * @return the value at coordinates (x,y)
     */
    EL_TYPE &get(const size_t x, const size_t y)

    {
        index_pair ind(x, y);
        auto       iter      = data_.find(ind);
        auto       autoGrowX = this->util::gridBase<EL_TYPE>::isAutoGrowX();
        auto       autoGrowY = this->util::gridBase<EL_TYPE>::isAutoGrowY();

        if((x < dims_.x() || autoGrowX) && (y < dims_.y() || autoGrowY))
        {
            if(iter == data_.end())
            {
                set(x, y, this->util::gridBase<EL_TYPE>::getDefaultValue());
                iter = data_.find(index_pair(x, y));
            }
        }
        else
        {
            throw grid_error("Get a value at " + toString(ind) + " out of bounds" + toString(dims_) + ".");
        }

        return (iter->second);
    }

    /**
     * @brief Retrieve the element at coordinates (x,y) if possible.
     *
     * @param x X-position
     * @param y Y-position
     *
     * @return the value at coordinates (x,y)
     */
    EL_TYPE get(const size_t x, const size_t y) const

    {
        index_pair findIndex(x, y);
        auto       iter = data_.find(findIndex);

        if(iter == data_.end())
        {
            return (this->util::gridBase<EL_TYPE>::getDefaultValue());
        }

        return (iter->second);
    }

    /**
     * @brief Resize the grid to new dimensions.
     *
     * @param nNewSizeX new size in x-dimension
     * @param nNewSizeY new size in y-dimension
     */
    void resize(const size_t nNewSizeX, const size_t nNewSizeY)

    {
        // if we make the scheme larger in all dimensions, then we just need
        // to adjust the sizes
        if(nNewSizeX >= dims_.x() && nNewSizeY >= dims_.y())
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

            while(iter != data_.end())
            {
                if((iter->first).x() >= dims_.x() || (iter->first).y() >= dims_.y())
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
     * @brief Retrieve the size in x-dimension.
     * @return the size in x-dimension
     */
    size_t sizeX() const
    {
        return (dims_.x());
    }

    /**
     * @brief Retrieve the size in y-dimension.
     * @return the size in y-dimension
     */
    size_t sizeY() const
    {
        return (dims_.y());
    }


    /**
     * @brief Show the grid on cout stream
     * 
     * @param mode display mode
     */
    void show(typename util::gridBase<EL_TYPE>::DisplayMode mode = util::gridBase<EL_TYPE>::DisplayMode::Stats)
    {
        std::cout << "grid sizeX=" << sizeX() << "grid sizeY=" << sizeY() << std::endl;
        if((mode | util::gridBase<EL_TYPE>::DisplayMode::Stats) == util::gridBase<EL_TYPE>::DisplayMode::Stats)
        {
            double totalValues = static_cast<double>(sizeX()) * static_cast<double>(sizeY());
            double fillPercentage;

            if(totalValues > 0.0)
                fillPercentage = (tatic_cast<double>((data_.size())) / (totalValues)) * 100.0;
            else
                fillPercentage = 0.0;
            std::cout << "\telements different from default value:" << fillPercentage << "%" << std::endl;
        }

        auto iter = data_.begin();

        if((mode | util::gridBase<EL_TYPE>::DisplayMode::Full) == util::gridBase<EL_TYPE>::DisplayMode::Full)
        {
            index_pair runner;
            index_pair start(0, 0);
            index_pair end;

            // display all elements and the gaps between them
            while(iter != data_.end())
            {
                // end point (of the gap)
                end = iter->first;

                // if there is a non-empty gap
                if(start < end)
                {
                    // start from the start point and iterate through all
                    // indices that are not equal end and that are lexical
                    // before end
                    for(runner = start; runner != end && runner.isWithinBounds(dims_); runner.increment(dims_))
                    {
                        std::cout << this->util::gridBase<EL_TYPE>::getDefaultValue();
                        if(runner.x() == dims_.x() - 1)
                            std::cout << std::endl;
                        else
                            std::cout << ",";
                    }

                    start = end;
                    start.increment(dims_);
                }

                std::cout << iter->second;

                if((iter->first).x() == dims_.x() - 1)
                    std::cout << std::endl;
                else
                    std::cout << ",";

                iter++;
            }

            // now write the remaining fields
            for(runner = start; runner.isWithinBounds(dims_); runner.increment(dims_))
            {
                std::cout << this->util::gridBase<EL_TYPE>::getDefaultValue();
                if(runner.x() == dims_.x() - 1)
                    std::cout << std::endl;
                else
                    std::cout << ",";
            }
        }
        else if((mode | util::gridBase<EL_TYPE>::DisplayMode::Sparse) == util::gridBase<EL_TYPE>::DisplayMode::Sparse)
        {
            size_t line = 0;

            if(iter != data_.end())
            {
                std::cout << "line [" << (iter->first).y() << "]\t";
                line = (iter->first).y();
            }

            while(iter != data_.end())
            {
                if(line != (iter->first).y())
                {
                    std::cout << std::endl;
                    line = (iter->first).y();
                    std::cout << "line [" << line << "]\t";
                }

                std::cout << "[" << (iter->first).x() << "]" << iter->second << " ";
                iter++;
            }

            std::cout << std::endl;
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
     * @brief Clear the grid and set all values to the same value.
     * @param value the new value
     */
    void setAll(const EL_TYPE &value)
    {
        setDefaultValue(value);
        data_.clear();
    }

    /**
     * @brief Retrieve the element at position (x, y).
     *
     * @param x X-position
     * @param y Y-position
     *
     * @return the element at position (x, y)
     */
    EL_TYPE operator()(const size_t x, const size_t y) const
    {
        return (get(x, y));
    }

    /**
     * @brief Retrieve the element at position (x, y) as non constant reference.
     *
     * @param x X-position
     * @param y Y-position
     *
     * @return the element at position (x, y)
     */
    EL_TYPE &operator()(const size_t x, const size_t y)
    {
        if(x >= sizeX() || y >= sizeY())
        {
            if(!this->util::gridBase<EL_TYPE>::isAutoGrow())
            {
                throw grid_error("Get a value at " + toString(index_pair(x, y)) + " out of bounds" + toString(dims_)
                                 + ".");
            }
        }

        createDataEntry(x, y);

        return (get(x, y));
    }

    /**
     * @brief Insert or remove indices.
     *
     * @param x X-position
     * @param y Y-position
     * @param operationMode Insert or Remove
     */
    void updateIndexSets(size_t x, size_t y, IndexSetOper operationMode)
    {
        if(operationMode == IndexSetOper::Insert)
        {
            INDEXSET setToInsert;

            if(xIndices_.find(y) == xIndices_.end())
            {
                xIndices_.insert(INDEXSETMAP::value_type(y, setToInsert));
            }

            ((xIndices_.find(y))->second).insert(x);

            if(yIndices_.find(x) == yIndices_.end())
            {
                yIndices_.insert(INDEXSETMAP::value_type(x, setToInsert));
            }

            ((yIndices_.find(x))->second).insert(y);
        }
        else if(operationMode == IndexSetOper::Remove)
        {
            if(xIndices_.find(y) != xIndices_.end())
            {
                xIndices_.erase(xIndices_.find(y));
            }
            if(xIndices_.find(y) != yIndices_.end())
            {
                yIndices_.erase(yIndices_.find(x));
            }
        }
    }

    /**
     * @brief Output information about index-set (x, y).
     * 
     * @param x X-coordinate
     * @param y Y-coordinate
     */
    void indexSetInfo(size_t x, size_t y)
    {
        auto iterX = xIndices_.find(y);
        auto iterY = yIndices_.find(x);

        std::cout << "m_xIndices=" << xIndices_.size() << " m_yIndices=" << yIndices_.size() << std::endl;

        if(iterX == xIndices_.end())
            std::cout << "==== row " << y << " not found,";
        else
        {
            std::cout << (iterX->second).size() << " x-Indices attached to row " << y << std::endl;

            auto iy = (iterX->second).begin();

            while(iy != (iterX->second).end())
            {
                std::cout << *iy << ",";
                iy++;
            }
            std::cout << std::endl;
        }

        if(iterY == yIndices_.end())
            std::cout << " line " << x << " not found,";
        else
        {
            std::cout << (iterY->second).size() << " y-Indices attached to row " << x << std::endl;

            auto ix = (iterY->second).begin();

            while(ix != (iterY->second).end())
            {
                std::cout << *ix << ",";
                ix++;
            }

            std::cout << std::endl;
        }

        std::cout << std::endl;
    }

    /**
     * @brief Create a data-entry at (x, y).
     *
     * @param x X-position
     * @param y Y-position
     */
    void createDataEntry(const size_t x, const size_t y)
    {
        if((x < dims_.x()) && (y < dims_.y()) || this->util::gridBase<EL_TYPE>::isAutoGrow())
        {
            index_pair insertPoint;

            insertPoint.x() = x;
            insertPoint.y() = y;
            data_.insert(DATA_CONTAINER::value_type(insertPoint, this->util::gridBase<EL_TYPE>::getDefaultValue()));
            updateIndexSets(x, y, IndexSetOper::Insert);
            dims_.x() = std::max(dims_.x(), x + 1);
            dims_.y() = std::max(dims_.y(), y + 1);
        }
        else
        {
            throw grid_error("Set a value at " + toString(index_pair(x, y)) + " out of bounds" + toString(dims_) + ".");
        }
    }

    private:
    index_pair     dims_;
    DATA_CONTAINER data_;
    INDEXSETMAP    xIndices_;
    INDEXSETMAP    yIndices_;
};

/**
 * @brief Specialisation of the gridBase class that expexts to be non-sparsely populated
 * 
 * @tparam EL_TYPE element-type 
 */
template<typename EL_TYPE = long double>
class grid : public gridBase<EL_TYPE>
{
    private:
    using EL_VECT   = std::vector<EL_TYPE>;
    using EL_MATRIX = std::vector<EL_VECT>;

    // variables
    EL_MATRIX values_;

    public:
    /**
     * @brief Default constructor.
     *
     * @param dimX X-dimension
     * @param dimY Y-dimension
     * @param pDefaultValue default value to use when a data value has not been explicitly set
     * @param mode mode to use for this grid
     */
    grid(const size_t                           dimX          = 0,
         const size_t                           dimY          = 0,
         EL_TYPE                               *pDefaultValue = 0,
         typename util::gridBase<EL_TYPE>::Mode mode          = util::gridBase<EL_TYPE>::Mode::AutoGrow)
    : gridBase<EL_TYPE>()
    {
        setMode(mode);

        if(pDefaultValue != NULL)
        {
            setDefaultValue(*pDefaultValue);
        }

        for(size_t iIndexX = 0; iIndexX < dimX; iIndexX++)
        {
            values_.push_back(vector<EL_TYPE>(dimY));
        }
    }

    grid(const grid<EL_TYPE> &rhs)                     = default;
    virtual ~grid()                                    = default;
    grid<EL_TYPE> &operator=(const grid<EL_TYPE> &rhs) = default;

    /**
     * @brief Retrieve the (current) size of the grid in X-dimensiuon.
     *
     * @return the size
     */
    size_t sizeX() const
    {
        return (values_.size());
    }

    /**
     * @brief Retrieve the (current) size of the grid in Y-dimensiuon.
     *
     * @return the size; if size in X dimension is 0, then size in Y-dimension is automatically also 0.
     */
    size_t sizeY() const
    {
        if(values_.size())
        {
            return (values_[0].size());
        }
        else
        {
            return (0);
        }
    }

    /**
     * @brief Resize the grid no new dimensions.
     *
     * @param newX new X-dimension
     * @param newY new Y-dimension
     */
    void resize(const size_t newX, const size_t newY)

    {
        values_.resize(newX);

        for(size_t x = 0; x < newX; x++)
        {
            values_[x].resize(newY);
        }
    }

    /**
     * @brief Retrieve the element at posion (x, y).
     *
     * @param x X-position
     * @param y Y-position
     *
     * @return the element
     */
    EL_TYPE &get(const size_t x, const size_t y)
    {
        return ((values_[x])[y]);
    }

    /**
     * @brief Set all elements to the same value.
     *
     * @param value the new value
     */
    void setAll(const EL_TYPE &value)
    {
        for(size_t x = 0; x < sizeY(); x++)
        {
            for(size_t y = 0; y < sizeX(); y++)
            {
                set(x, y, value);
            }
        }
    }

    /**
     * @brief Set a new value for element (x, y).
     *
     * @param x X-position
     * @param y Y-position
     * @param value the value to set
     */
    void set(const size_t x, const size_t y, const EL_TYPE &value)
    {
        if(x >= sizeX() || y >= sizeY())
        {
            if(this->util::gridBase<EL_TYPE>::isAutoGrow())
            {
                resize(std::max(x + 1, sizeX()), std::max(y + 1, sizeY()));
            }
        }

        ((values_[x])[y]) = value;
    }

    /**
     * @brief Retrieve a reference to the element at position (x, y).
     *
     * @param x X-position
     * @param y Y-position
     *
     * @return the element
     */
    EL_TYPE &operator()(const size_t x, const size_t y = 0)
    {
        if(x >= sizeX() || y >= sizeY())
        {
            if(this->util::gridBase<EL_TYPE>::isAutoGrow())
            {
                resize(std::max(x + 1, sizeX()), std::max(y + 1, sizeY()));
            }
        }

        return ((values_[x])[y]);
    }

    /**
     * @brief Retrieve the element at position (x, y).
     *
     * @param x X-position
     * @param y Y-position
     *
     * @return the element
     */
    EL_TYPE operator()(const size_t x, const size_t y = 0) const
    {
        if(x >= sizeX() || y >= sizeY())
        {
            throw grid_error("Get a value at " + toString(index_pair(x, y)) + " out of bounds [" + sizeX() + ","
                             + sizeY() + "].");
        }

        return ((values_[x])[y]);
    }

    /**
     * @brief Output a grid.
     *
     * @param mode
     */
    void
     show(typename util::gridBase<EL_TYPE>::DisplayMode mode = util::gridBase<EL_TYPE>::DisplayMode::Full /*unused*/)
    {
        std::cout << "Rectangular Data (" << sizeX() << " x " << sizeY() << ")" << std::endl;

        for(size_t y = 0; y < sizeY(); y++)
        {
            for(size_t x = 0; x < sizeX(); x++)
            {
                std::cout << get(x, y) << "\t";
            }

            std::cout << std::endl;
        }

        std::cout << "\n---\n" << std::endl;
    }
};

};  // namepace util

#endif  // NS_UTIL_GRID_H_INCLUDED
