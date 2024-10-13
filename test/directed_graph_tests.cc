/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/directed_graph_tests.cc
 * Description: Unit tests for string utilities
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

#include "dateutil.h"
#include "directed_graph.h"
#include "to_string.h"
// #define DO_TRACE_
#include "traceutil.h"
//#define DO_GRAPH_DEBUG_TRACE_
#include "directed_graph_tests_debug_functions.h"

#include <cmath>
#include <gtest/gtest.h>
#include <string>

using namespace std;
using namespace util;
using namespace util::datescan;
using namespace boost::posix_time;
using namespace boost::gregorian;

class DirectedGraphTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }

    string A{"A"};
    string B{"B"};
    string C{"C"};
    string D{"D"};
    string E{"E"};
    string F{"F"};
    string G{"G"};
    string H{"H"};

    string E01{"E01"};
    string E02{"E02"};
    string E03{"E03"};
    string E04{"E04"};
    string E05{"E05"};
    string E06{"E06"};
    string E07{"E07"};
    string E08{"E08"};
    string E09{"E09"};
    string E10{"E10"};
    string E11{"E11"};
};

TEST_F(DirectedGraphTest, directed_graph_base_test)
{
    directed_graph_base<string, string> g{};
    //    PRINT_TYPES_AND_BOOLS(g);

    ASSERT_FALSE(g.addEdge(A, B, E01)) << "adding an edge with none of the vertices existing should not work";
    ASSERT_TRUE(g.addVertex(A)) << "adding vertex 'A' to empty graph should work";
    ASSERT_TRUE(g.addVertex(A)) << "adding vertex 'A' to empty graph should work";
    ASSERT_TRUE(g.addEdge(A, A, E01));
    ASSERT_TRUE(g.addEdge(A, A, E01));
    ASSERT_EQ(g.findParallelEdges(A, A).size(), 2UL);
    ASSERT_FALSE(g.hasVertices(D, E, F, G, H));
    g.addVertices(D, E, F, G, H);
    ASSERT_TRUE(g.hasVertices(D, E, F, G, H));
}

TEST_F(DirectedGraphTest, directed_acyclic_graph_tests)
{
    directed_acyclic_graph<string, string> g{};
    //    PRINT_TYPES_AND_BOOLS(g);

    ASSERT_FALSE(g.addEdge(A, B, E01)) << "adding an edge with none of the vertices existing should not work";
    ASSERT_TRUE(g.addVertex(A)) << "adding vertex 'A' to empty graph should work";
    ASSERT_FALSE(g.addVertex(A)) << "re-adding vertex 'A' to graph should not work";
    ASSERT_FALSE(g.addEdge(A, B, E01)) << "adding an edge with only one existing vertex should not work";
    ASSERT_TRUE(g.addVertex(B));
    ASSERT_TRUE(g.addVertex(C));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.hasVertices(D, E, F, G, H));
    g.addVertices(D, E, F, G, H);
    ASSERT_TRUE(g.hasVertices(D, E, F, G, H));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 8UL);

    ASSERT_TRUE(g.addEdge(A, B, E01)
    ) << "adding an edge with different existing vertices and unique edge property should work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_FALSE(g.addEdge(A, A, E02)) << "Adding an edge from a vertex to itself creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_FALSE(g.addEdge(B, A, E03)
    ) << "Adding an opposite edge to an existing edge creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_TRUE(g.addEdge(B, C, E02));
    ASSERT_TRUE(g.addEdge(C, D, E03));
    ASSERT_TRUE(g.addEdge(C, E, E04));
    ASSERT_TRUE(g.addEdge(B, E, E05));
    ASSERT_TRUE(g.addEdge(D, F, E06));
    ASSERT_TRUE(g.addEdge(D, E, E07));
    ASSERT_TRUE(g.addEdge(G, H, E08));
    ASSERT_TRUE(g.addEdge(E, F, E09));
    ASSERT_TRUE(g.addEdge(B, F, E10));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 2UL);
    ASSERT_FALSE(g.addEdge(E, A, E11)) << "Adding a cycle should not work";

    ASSERT_TRUE(g.hasEdge(G, H, E08));
    ASSERT_TRUE(g.removeEdge(G, H, E08)) << "removing an existing edge should work";
    ASSERT_FALSE(g.hasEdge(G, H, E08)) << "after removing an edge it should no longer be present";
    ASSERT_FALSE(g.hasEdge(G, H)) << "after removing an edge it should no longer be present";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.removeEdge(G, H, E08));
    ASSERT_FALSE(g.removeEdge(G, H));

    ASSERT_TRUE(g.hasEdge(C, E, E04));
    ASSERT_TRUE(g.removeEdge(C, E));
    ASSERT_FALSE(g.hasEdge(C, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));
    ASSERT_FALSE(g.removeEdge(B, E, E06))
        << "removing an edge between existing nodes but not-matching edge-property should not work";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_FALSE(g.hasEdge(B, E, E06));
    ASSERT_TRUE(g.addEdge(B, E, E06)) << "parallel edges are not allowed for this type of graph - but overwrite edge";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_EQ(g.findParallelEdges(B, E).size(), 1UL);
    ASSERT_FALSE(g.hasEdge(B, E, E06));

    ASSERT_TRUE(g.hasVertex(C));
    ASSERT_TRUE(g.hasEdge(B, C, E02));
    ASSERT_TRUE(g.hasEdge(C, D, E03));
    ASSERT_TRUE(g.removeVertex(C));
    ASSERT_FALSE(g.hasVertex(C));
    ASSERT_FALSE(g.hasEdge(B, C, E02)) << "removing the source-vertex should remove the edge";
    ASSERT_FALSE(g.hasEdge(C, D, E03)) << "removing the target-vertex should remove the edge";
    ASSERT_FALSE(g.removeVertex(C));
}

TEST_F(DirectedGraphTest, directed_acyclic_throwing_graph_tests)
{
    directed_acyclic_graph<string, string, throw_on_error> g{};
    //    PRINT_TYPES_AND_BOOLS(g);

    ASSERT_THROW(g.addEdge(A, B, E01), vertex_existence_error)
        << "adding an edge with none of the vertices existing should not work";
    ASSERT_TRUE(g.addVertex(A)) << "adding vertex 'A' to empty graph should work";
    ASSERT_THROW(g.addVertex(A), vertex_existence_error) << "re-adding vertex 'A' to graph should not work";
    ASSERT_THROW(g.addEdge(A, B, E01), vertex_existence_error)
        << "adding an edge with only one existing vertex should not work";
    ASSERT_TRUE(g.addVertex(B));
    ASSERT_TRUE(g.addVertex(C));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.hasVertices(D, E, F, G, H));
    g.addVertices(D, E, F, G, H);
    ASSERT_TRUE(g.hasVertices(D, E, F, G, H));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 8UL);

    ASSERT_TRUE(g.addEdge(A, B, E01)
    ) << "adding an edge with different existing vertices and unique edge property should work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_THROW(g.addEdge(A, A, E02), cycle_error)
        << "Adding an edge from a vertex to itself creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_THROW(g.addEdge(B, A, E03), cycle_error)
        << "Adding an opposite edge to an existing edge creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_TRUE(g.addEdge(B, C, E02));
    ASSERT_TRUE(g.addEdge(C, D, E03));
    ASSERT_TRUE(g.addEdge(C, E, E04));
    ASSERT_TRUE(g.addEdge(B, E, E05));
    ASSERT_TRUE(g.addEdge(D, F, E06));
    ASSERT_TRUE(g.addEdge(D, E, E07));
    ASSERT_TRUE(g.addEdge(G, H, E08));
    ASSERT_TRUE(g.addEdge(E, F, E09));
    ASSERT_TRUE(g.addEdge(B, F, E10));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 2UL);
    ASSERT_THROW(g.addEdge(E, A, E11), cycle_error) << "Adding a cycle should not work";

    ASSERT_TRUE(g.hasEdge(G, H, E08));
    ASSERT_TRUE(g.removeEdge(G, H, E08)) << "removing an existing edge should work";
    ASSERT_FALSE(g.hasEdge(G, H, E08)) << "after removing an edge it should no longer be present";
    ASSERT_FALSE(g.hasEdge(G, H)) << "after removing an edge it should no longer be present";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.removeEdge(G, H, E08));
    ASSERT_FALSE(g.removeEdge(G, H));

    ASSERT_TRUE(g.hasEdge(C, E, E04));
    ASSERT_TRUE(g.removeEdge(C, E));
    ASSERT_FALSE(g.hasEdge(C, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));
    ASSERT_FALSE(g.removeEdge(B, E, E06))
        << "removing an edge between existing nodes but not-matching edge-property should not work";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_FALSE(g.hasEdge(B, E, E06));
    ASSERT_TRUE(g.addEdge(B, E, E06)) << "parallel edges are not allowed for this type of graph";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_FALSE(g.hasEdge(B, E, E06));
    ASSERT_EQ(g.findParallelEdges(B, E).size(), 1UL);

    ASSERT_TRUE(g.hasVertex(C));
    ASSERT_TRUE(g.hasEdge(B, C, E02));
    ASSERT_TRUE(g.hasEdge(C, D, E03));
    ASSERT_TRUE(g.removeVertex(C));
    ASSERT_FALSE(g.hasVertex(C));
    ASSERT_FALSE(g.hasEdge(B, C, E02)) << "removing the source-vertex should remove the edge";
    ASSERT_FALSE(g.hasEdge(C, D, E03)) << "removing the target-vertex should remove the edge";
    ASSERT_FALSE(g.removeVertex(C));
}

TEST_F(DirectedGraphTest, directed_acyclic_parallel_graph_tests)
{
    directed_acyclic_parallel_graph<string, string> g{};
    //    PRINT_TYPES_AND_BOOLS(g);

    ASSERT_FALSE(g.addEdge(A, B, E01)) << "adding an edge with none of the vertices existing should not work";
    ASSERT_TRUE(g.addVertex(A)) << "adding vertex 'A' to empty graph should work";
    ASSERT_FALSE(g.addVertex(A)) << "re-adding vertex 'A' to graph should not work";
    ASSERT_FALSE(g.addEdge(A, B, E01)) << "adding an edge with only one existing vertex should not work";
    ASSERT_TRUE(g.addVertex(B));
    ASSERT_TRUE(g.addVertex(C));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.hasVertices(D, E, F, G, H));
    g.addVertices(D, E, F, G, H);
    ASSERT_TRUE(g.hasVertices(D, E, F, G, H));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 8UL);

    ASSERT_TRUE(g.addEdge(A, B, E01)
    ) << "adding an edge with different existing vertices and unique edge property should work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_FALSE(g.addEdge(A, A, E02)) << "Adding an edge from a vertex to itself creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_FALSE(g.addEdge(B, A, E03)
    ) << "Adding an opposite edge to an existing edge creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_TRUE(g.addEdge(B, C, E02));
    ASSERT_TRUE(g.addEdge(C, D, E03));
    ASSERT_TRUE(g.addEdge(C, E, E04));
    ASSERT_TRUE(g.addEdge(B, E, E05));
    ASSERT_TRUE(g.addEdge(D, F, E06));
    ASSERT_TRUE(g.addEdge(D, E, E07));
    ASSERT_TRUE(g.addEdge(G, H, E08));
    ASSERT_TRUE(g.addEdge(E, F, E09));
    ASSERT_TRUE(g.addEdge(B, F, E10));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 2UL);
    ASSERT_FALSE(g.addEdge(E, A, E11)) << "Adding a cycle should not work";

    ASSERT_TRUE(g.hasEdge(G, H, E08));
    ASSERT_TRUE(g.removeEdge(G, H, E08)) << "removing an existing edge should work";
    ASSERT_FALSE(g.hasEdge(G, H, E08)) << "after removing an edge it should no longer be present";
    ASSERT_FALSE(g.hasEdge(G, H)) << "after removing an edge it should no longer be present";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.removeEdge(G, H, E08));
    ASSERT_FALSE(g.removeEdge(G, H));

    ASSERT_TRUE(g.hasEdge(C, E, E04));
    ASSERT_TRUE(g.removeEdge(C, E));
    ASSERT_FALSE(g.hasEdge(C, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));
    ASSERT_FALSE(g.removeEdge(B, E, E06))
        << "removing an edge between existing nodes but not-matching edge-property should not work";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_FALSE(g.hasEdge(B, E, E06));
    ASSERT_TRUE(g.addEdge(B, E, E06)) << "adding an edge with same source and target should create a parallel edge";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E, E06));

    ASSERT_TRUE(g.hasVertex(C));
    ASSERT_TRUE(g.hasEdge(B, C, E02));
    ASSERT_TRUE(g.hasEdge(C, D, E03));
    ASSERT_TRUE(g.removeVertex(C));
    ASSERT_FALSE(g.hasVertex(C));
    ASSERT_FALSE(g.hasEdge(B, C, E02)) << "removing the source-vertex should remove the edge";
    ASSERT_FALSE(g.hasEdge(C, D, E03)) << "removing the target-vertex should remove the edge";
    ASSERT_FALSE(g.removeVertex(C));
}

TEST_F(DirectedGraphTest, directed_acyclic_parallel_throwing_graph_tests)
{
    directed_acyclic_parallel_graph<string, string, throw_on_error> g{};
    PRINT_TYPES_AND_BOOLS(g);

    ASSERT_THROW(g.addEdge(A, B, E01), vertex_existence_error)
        << "adding an edge with none of the vertices existing should not work";
    ASSERT_TRUE(g.addVertex(A)) << "adding vertex 'A' to empty graph should work";
    ASSERT_THROW(g.addVertex(A), vertex_existence_error) << "re-adding vertex 'A' to graph should not work";
    ASSERT_THROW(g.addEdge(A, B, E01), vertex_existence_error)
        << "adding an edge with only one existing vertex should not work";
    ASSERT_TRUE(g.addVertex(B));
    ASSERT_TRUE(g.addVertex(C));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.hasVertices(D, E, F, G, H));
    g.addVertices(D, E, F, G, H);
    ASSERT_TRUE(g.hasVertices(D, E, F, G, H));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 8UL);

    ASSERT_TRUE(g.addEdge(A, B, E01)
    ) << "adding an edge with different existing vertices and unique edge property should work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_THROW(g.addEdge(A, A, E02), cycle_error)
        << "Adding an edge from a vertex to itself creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_THROW(g.addEdge(B, A, E03), cycle_error)
        << "Adding an opposite edge to an existing edge creates a cycle and should not work";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 7UL);
    ASSERT_TRUE(g.addEdge(B, C, E02));
    ASSERT_TRUE(g.addEdge(C, D, E03));
    ASSERT_TRUE(g.addEdge(C, E, E04));
    ASSERT_TRUE(g.addEdge(B, E, E05));
    ASSERT_TRUE(g.addEdge(D, F, E06));
    ASSERT_TRUE(g.addEdge(D, E, E07));
    ASSERT_TRUE(g.addEdge(G, H, E08));
    ASSERT_TRUE(g.addEdge(E, F, E09));
    ASSERT_TRUE(g.addEdge(B, F, E10));
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 2UL);
    ASSERT_THROW(g.addEdge(E, A, E11), cycle_error) << "Adding a cycle should not work";

    ASSERT_TRUE(g.hasEdge(G, H, E08));
    ASSERT_TRUE(g.removeEdge(G, H, E08)) << "removing an existing edge should work";
    ASSERT_FALSE(g.hasEdge(G, H, E08)) << "after removing an edge it should no longer be present";
    ASSERT_FALSE(g.hasEdge(G, H)) << "after removing an edge it should no longer be present";
    ASSERT_EQ(g.getDisconnectedSubGraphs().size(), 3UL);
    ASSERT_FALSE(g.removeEdge(G, H, E08));
    ASSERT_FALSE(g.removeEdge(G, H));

    ASSERT_TRUE(g.hasEdge(C, E, E04));
    ASSERT_TRUE(g.removeEdge(C, E));
    ASSERT_FALSE(g.hasEdge(C, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));
    ASSERT_FALSE(g.removeEdge(B, E, E06))
        << "removing an edge between existing nodes but not-matching edge-property should not work";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E));

    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_FALSE(g.hasEdge(B, E, E06));
    ASSERT_TRUE(g.addEdge(B, E, E06)) << "adding an edge with same source and target should create a parallel edge";
    ASSERT_TRUE(g.hasEdge(B, E, E05));
    ASSERT_TRUE(g.hasEdge(B, E, E06));

    ASSERT_TRUE(g.hasVertex(C));
    ASSERT_TRUE(g.hasEdge(B, C, E02));
    ASSERT_TRUE(g.hasEdge(C, D, E03));
    ASSERT_TRUE(g.removeVertex(C));
    ASSERT_FALSE(g.hasVertex(C));
    ASSERT_FALSE(g.hasEdge(B, C, E02)) << "removing the source-vertex should remove the edge";
    ASSERT_FALSE(g.hasEdge(C, D, E03)) << "removing the target-vertex should remove the edge";
    ASSERT_FALSE(g.removeVertex(C));

    // extra parallel edges
    ASSERT_TRUE(g.addVertex(C));
    ASSERT_TRUE(g.addEdge(B, C, "Extra1"));
    ASSERT_TRUE(g.addEdge(B, C, "Extra2"));
    ASSERT_TRUE(g.hasEdge(B, C, "Extra1"));
    ASSERT_TRUE(g.addEdge(B, C, "Extra3"));
    ASSERT_TRUE(g.addEdge(B, C, "Extra4"));
    ASSERT_TRUE(g.hasEdge(B, C, "Extra2"));
    ASSERT_TRUE(g.hasEdge(B, C, "Extra4"));
    ASSERT_TRUE(g.hasEdge(B, C, "Extra4"));
}

TEST_F(DirectedGraphTest, directed_acyclic_parallel_throwing_graph_parallel_tests)
{
    directed_graph<string, string> defaultDirectedGraph{};
    //    PRINT_TYPES_AND_BOOLS(defaultDirectedGraph);
    directed_acyclic_parallel_graph<string, string, util::throw_on_error> acyclicParallelGraph{};
    //    PRINT_TYPES_AND_BOOLS(acyclicParallelGraph);
    defaultDirectedGraph.addVertices(A, B);
    acyclicParallelGraph.addVertices(A, B);

    ASSERT_TRUE(defaultDirectedGraph.addEdge(A, B, "Extra1"));
    ASSERT_TRUE(acyclicParallelGraph.addEdge(A, B, "Extra1"));
    TRACE1(defaultDirectedGraph.findParallelEdges(A, B));
    TRACE1(acyclicParallelGraph.findParallelEdges(A, B));

    ASSERT_TRUE(defaultDirectedGraph.addEdge(A, B, "Extra2"));
    TRACE1(defaultDirectedGraph.findParallelEdges(A, B));
    ASSERT_TRUE(acyclicParallelGraph.addEdge(A, B, "Extra2"));
    TRACE1(acyclicParallelGraph.findParallelEdges(A, B));
}
