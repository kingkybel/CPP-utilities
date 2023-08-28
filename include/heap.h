/*
 * File:		heap.h
 * Description: implementation of a heap-container
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

#ifndef HEAP_H_INCLUDED
#define HEAP_H_INCLUDED

#include <algorithm>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace util
{
/**
 * @class heap
 */
template<typename T_, typename Compare = std::greater<T_>>
class heap
{
    std::vector<T_> arr;
    size_t          last = 0UL;

    static size_t parentOf(const size_t idx)
    {
        return (idx > 0UL ? (((idx - 1)) / 2UL) : 0UL);
    }

    void bubbleUp(size_t nodeIdx)
    {
        if(nodeIdx != 0UL)
        {
            size_t parentIdx = parentOf(nodeIdx);

            if(arr[parentIdx] > arr[nodeIdx])
            {
                std::swap(arr[parentIdx], arr[nodeIdx]);
                bubbleUp(parentIdx);
            }
        }
    }

    void bubbleDown(size_t nodeIndex)
    {
        size_t minIdx;
        size_t leftIdx  = nodeIndex * 2 + 1;
        size_t rightIdx = nodeIndex * 2 + 2;

        if(rightIdx >= last)
        {
            if(leftIdx >= last)
                return;
            else
                minIdx = leftIdx;
        }
        else
        {
            if(arr[leftIdx] <= arr[rightIdx])
                minIdx = leftIdx;
            else
                minIdx = rightIdx;
        }

        if(arr[nodeIndex] > arr[minIdx])
        {
            std::swap(arr[minIdx], arr[nodeIndex]);
            bubbleDown(minIdx);
        }
    }

    public:
    heap()                = default;
    heap(const heap &rhs) = default;
    virtual ~heap()       = default;

    /**
     *  Function to delete the root from Heap
     */
    void pop()
    {
        if(!empty())
        {
            // Replace root with last element
            arr[0] = arr[last - 1];

            // Decrement size of heap
            last--;

            bubbleDown(0L);
        }
    }

    /**
     *  Function to insert a new node to the Heap
     */
    void insert(T_ Key)
    {
        // Increase the size of Heap by 1
        last++;
        if(last >= arr.size())
            arr.resize(last);

        // Insert the element at end of Heap
        arr[last - 1] = Key;
        bubbleUp(last - 1);
    }

    size_t size() const
    {
        return (last);
    }

    T_ operator[](size_t index) const
    {
        if(index > last)
            throw std::out_of_range("");

        return (arr[index]);
    }

    bool empty() const
    {
        return (size() == 0UL);
    }

    friend std::ostream &operator<<(std::ostream &os, const heap &h)
    {
        size_t layerSize  = 1;
        size_t layerCount = 1;
        for(size_t i = 0; i < h.size(); i++)
        {
            os << h[i];
            if(layerCount == layerSize)
            {
                os << "| ";
                layerSize *= 2;
                layerCount = 1;
            }
            else
            {
                os << " ";
                layerCount++;
            }
        }

        return os;
    }
};

template<typename T_, typename Compare = std::greater<T_>>
class std_heap
{
    std::vector<T_> arr;
    size_t          last = 0UL;

    public:
    std_heap()                    = default;
    std_heap(const std_heap &rhs) = default;
    virtual ~std_heap()           = default;

    T_ *const top()
    {
        if(empty())
            return nullptr;
        return &arr[0];
    }

    /**
     *  Function to delete the root from Heap
     */
    void pop()
    {
        std::pop_heap(arr.begin(), arr.begin() + last, Compare());
        last--;

        if(last < arr.size() / 2)
            arr.resize(last);
    }

    size_t size() const
    {
        return (last);
    }

    T_ operator[](size_t index) const
    {
        if(index >= last)
            throw std::out_of_range("");

        return (arr[index]);
    }

    bool empty() const
    {
        return (size() == 0UL);
    }

    friend std::ostream &operator<<(std::ostream &os, const std_heap &h)
    {
        size_t layerSize  = 1;
        size_t layerCount = 1;

        for(size_t i = 0; i < h.size(); i++)
        {
            os << h[i];
            if(layerCount == layerSize)
            {
                os << "| ";
                layerSize *= 2;
                layerCount = 1;
            }
            else
            {
                os << " ";
                layerCount++;
            }
        }

        return (os);
    }

    /**
     *  Function to insert a new node to the Heap
     */
    void insert(T_ Key)
    {
        // Increase the size of Heap by 1
        last++;
        if(arr.size() < size())
            arr.resize(size() + 1);

        arr[size() - 1] = Key;
        // Insert the element at end of Heap
        std::push_heap(arr.begin(), arr.begin() + last, Compare());
    }
};
};  // namepace util

#endif  // HEAP_H_INCLUDED
