/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/directed_graph_tests_debug_functions.cc
 * Description: Functions to show graph-settings
 *
 * Copyright (C) 2024 Dieter J Kybelksties <github@kybelksties.com>
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
 * @date: 2024-10-12
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_DIRECTED_GRAPH_TESTS_DEBUG_FUNCTIONS_H
#define NS_UTIL_DIRECTED_GRAPH_TESTS_DEBUG_FUNCTIONS_H

#undef TRACING_ADDED_
#ifndef DO_TRACE_
    #define DO_TRACE_
    #define TRACING_ADDED
#endif
#include "traceutil.h"

#include <typeinfo>

namespace util
{
#ifdef DO_GRAPH_DEBUG_TRACE_

    #define DECLARE_TEST_DEBUG_HELPERS                                                                                 \
        template <typename Type>                                                                                       \
        std::string scanForBoostSelector()                                                                             \
        {                                                                                                              \
            std::string              typeStr{typeid(Type).name()};                                                     \
            std::vector<std::string> selectors = {                                                                     \
                /*ORDER OF STRINGS MATTERS!*/                                                                          \
                "hash_setS",                                                                                           \
                "hash_mapS",                                                                                           \
                "hash_multisetS",                                                                                      \
                "hash_multimapS",                                                                                      \
                "multisetS",                                                                                           \
                "multimapS",                                                                                           \
                "vecS",                                                                                                \
                "listS",                                                                                               \
                "setS",                                                                                                \
                "mapS",                                                                                                \
            };                                                                                                         \
                                                                                                                       \
            for (auto const &selector: selectors)                                                                      \
            {                                                                                                          \
                if (typeStr.find(selector) != std::string::npos)                                                       \
                {                                                                                                      \
                    return std::string("boost::") + selector;                                                          \
                }                                                                                                      \
            }                                                                                                          \
                                                                                                                       \
            return ""; /* No match found */                                                                            \
        }                                                                                                              \
                                                                                                                       \
        void printTypesAndBools()                                                                                      \
        {                                                                                                              \
            TRACE1(AllowMultipleVertices);                                                                             \
            TRACE1(AllowParallelEdges);                                                                                \
            TRACE1(AllowCycles);                                                                                       \
            TRACE1(ThrowOnError);                                                                                      \
            TRACE1(OverWriteEdgeProperty);                                                                             \
            auto outEdgeStorage = scanForBoostSelector<OutEdgeListS>();                                                \
            TRACE1(outEdgeStorage);                                                                                    \
            auto vertexStorage = scanForBoostSelector<VertexStorage>();                                                \
            TRACE1(vertexStorage);                                                                                     \
            auto edgeStorage = scanForBoostSelector<EdgeStorage>();                                                    \
            TRACE1(edgeStorage);                                                                                       \
        }

    #define PRINT_TYPES_AND_BOOLS(graphP)                                                                              \
        {                                                                                                              \
            graphP.printTypesAndBools();                                                                               \
        }

#else
    #define DECLARE_TEST_DEBUG_HELPERS    ;
    #define PRINT_TYPES_AND_BOOLS(graphP) ;
#endif
} // namespace util

#ifdef TRACING_ADDED_
    #undef DO_TRACE_
    #undef TRACING_ADDED
#endif

#endif // NS_UTIL_DIRECTED_GRAPH_TESTS_DEBUG_FUNCTIONS_H
