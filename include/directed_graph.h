/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/graphutil.h
 * Description: graph utility functions
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
 * @date: 2024-10-08
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_DIRECTED_GRAPH_H_INCLUDED
#define NS_UTIL_DIRECTED_GRAPH_H_INCLUDED

#include "directed_graph_traits.h"
#include "traits.h"
// #define DO_TRACE_
#include "traceutil.h"
// #define DO_GRAPH_DEBUG_TRACE_
#include "directed_graph_tests_debug_functions.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/visitors.hpp>
#include <deque>
#include <functional>
#include <gtest/gtest_prod.h>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

template <
    typename PropTypeT,
    typename AllowParallelEdgesT,
    typename AllowMultipleVerticesT,
    typename ExpectedSelectorEdgeT,
    typename ExpectedSelectorVertexT>
void testCorrectTypes();

namespace util
{

/**
 * @brief Exception to be thrown when a new edge would create a cycle.
 * This is only thrown when exceptions are enabled by trait DirectedGraphTraits::throw_on_error.
 */
struct cycle_error : public std::invalid_argument
{
    explicit cycle_error(std::string const& msg)
        : std::invalid_argument(msg)
    {
    }
};

/**
 * @brief Exception to be thrown when a new edge would create a parallel.
 * This is only thrown when exceptions are enabled by trait DirectedGraphTraits::throw_on_error.
 */
struct parallel_edge_error : public std::invalid_argument
{
    explicit parallel_edge_error(std::string const& msg)
        : std::invalid_argument(msg)
    {
    }
};

/**
 * @brief Exception to be thrown when a vertex is required to exist and does not or when it must not exist and does.
 * This is only thrown when exceptions are enabled by trait DirectedGraphTraits::throw_on_error.
 */
struct vertex_existence_error : public std::invalid_argument
{
    explicit vertex_existence_error(std::string const& msg)
        : std::invalid_argument(msg)
    {
    }
};

/**
 * @brief Exception to be thrown when an edge is required to exist and does not or when it must not exist and does.
 * This is only thrown when exceptions are enabled by trait DirectedGraphTraits::throw_on_error.
 */
struct edge_existence_error : public std::invalid_argument
{
    explicit edge_existence_error(std::string const& msg)
        : std::invalid_argument(msg)
    {
    }
};

template <typename VertexProperty, typename EdgeProperty>
class VertexEdgePath
{
  public:
    void add(VertexProperty const& source, VertexProperty const& target, EdgeProperty const& edge)
    {
        if (vertices_.empty())
        {
            vertices_.push_back(source);
        }
        vertices_.push_back(target);
        edges_.push_back(edge);
    }

    [[nodiscard]] std::deque<std::tuple<VertexProperty, EdgeProperty, VertexProperty>> getPath() const
    {
        std::deque<std::tuple<VertexProperty, EdgeProperty, VertexProperty>> reval{};
        for (size_t i = 0UL; i < edges_.size(); ++i)
        {
            // there always is 1 more vertices in the path than edges
            reval.push_back(std::make_tuple(vertices_[i], edges_[i], vertices_[i + 1]));
        }
        return reval;
    }

  private:
    std::deque<VertexProperty> vertices_{};
    std::deque<VertexProperty> edges_{};
};

/**
 * @brief A class for directed graphs.
 * @tparam VertexProperty vertex (property) type
 * @tparam Options variadic list of options
 *  <ul>
 *  <li>allow_multiple_vertices_t, disallow_multiple_vertices_t</li>
 *  </ul>
 */
template <typename VertexProperty, typename EdgeProperty, typename... Options>
class directed_graph_base
{
    // friend-declarations for unit-tests
    friend class DirectedGraphTraitsTest;
    FRIEND_TEST(DirectedGraphTest, directed_graph_base_test);
    FRIEND_TEST(DirectedGraphTest, directed_acyclic_graph_tests);
    template <
        typename PropTypeT,
        typename AllowParallelEdgesT,
        typename AllowMultipleVerticesT,
        typename ExpectedSelectorEdgeT,
        typename ExpectedSelectorVertexT>
    friend void ::testCorrectTypes();
    //

    // Detect which options are enabled
    static constexpr bool AllowMultipleVertices =
        set_or_default_option<allow_multiple_vertices, disallow_multiple_vertices, true, Options...>::value;
    static constexpr bool AllowParallelEdges =
        set_or_default_option<allow_parallel_edges, disallow_parallel_edges, true, Options...>::value;
    static constexpr bool AllowCycles = set_or_default_option<allow_cycles, disallow_cycles, true, Options...>::value;
    static constexpr bool ThrowOnError =
        set_or_default_option<throw_on_error, no_throw_on_error, false, Options...>::value;
    static constexpr bool OverWriteEdgeProperty =
        set_or_default_option<overwrite_edge_property, overwrite_edge_property, true, Options...>::value;

    // Determine vertex and edge storage types
    using OutEdgeListS =
        typename storage_selector<appropriate_storage_selector_t<EdgeProperty, AllowParallelEdges>, Options...>::type;
    using VertexStorage =
        typename storage_selector<appropriate_storage_selector_t<VertexProperty, AllowMultipleVertices>, Options...>::
            type;
    using EdgeStorage = appropriate_storage_selector_t<EdgeProperty, AllowParallelEdges>;

  public:
    DECLARE_TEST_DEBUG_HELPERS

  private:
    using Graph = boost::adjacency_list<
        OutEdgeListS,
        VertexStorage,
        boost::directedS,
        VertexProperty,
        EdgeProperty,
        boost::no_property,
        EdgeStorage>;
    Graph graph_;
    //    adjacency_list<OutEdgeListS,
    //                   VertexListS,
    //                   DirectedS,
    //                   VertexProperty,
    //                   EdgeProperty,
    //                   GraphProperty,
    //                   EdgeListS> ;

  public:
    using VertexDescriptor = typename boost::graph_traits<Graph>::vertex_descriptor;
    using EdgeDescriptor   = typename boost::graph_traits<Graph>::edge_descriptor;

    /**
     * @brief Check if a vertex exists.
     * @param vertex vertex to look for
     * @return true if the vertex exists in the graph false otherwise
     */
    bool hasVertex(VertexProperty const& vertex) const
    {
        auto vertices = boost::vertices(graph_);
        for (auto it = vertices.first; it != vertices.second; ++it)
        {
            if (graph_[*it] == vertex)
            {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Variadic function to check if all provided vertices exist.
     * @tparam Vertices type of vertices
     * @param vertices the vertices to check
     * @return true, is all vertices exist, false if at least one does not exist
     */
    template <typename... Vertices>
    bool hasVertices(Vertices const&... vertices) const
    {
        return (hasVertex(vertices) && ...);
    }

    /**
     * @brief Check whether an edge exist between source and target. If edge is given, then it has to match also.
     * @param vertexSource source-vertex of the edge
     * @param vertexTarget target-vertex of the edge
     * @param edge optional edge-property
     * @return true if the edge exists, false otherwise
     */
    bool hasEdge(
        VertexProperty const&       vertexSource,
        VertexProperty const&       vertexTarget,
        std::optional<EdgeProperty> edge = std::nullopt
    ) const
    {
        VertexDescriptor src, tgt;
        if (!getVertexDescriptor(vertexSource, src) || !getVertexDescriptor(vertexTarget, tgt))
        {
            return false;
        }
        auto [edgeIter, found] = boost::edge(src, tgt, graph_);
        if (!found)
        {
            return false;
        }
        if (edge)
        {
            if constexpr (!AllowParallelEdges)
            {
                return graph_[edgeIter] == *edge;
            }
            else
            {
                // Check if there is at least one edge between src and tgt
                auto [edgeIter, edgeEnd] = boost::edges(graph_);

                bool foundEdge = false;
                for (; edgeIter != edgeEnd; ++edgeIter)
                {
                    if (boost::source(*edgeIter, graph_) == src && boost::target(*edgeIter, graph_) == tgt)
                    {
                        // If an edge property is specified, check if it matches the edge property in the graph
                        if (edge)
                        {
                            if (get(boost::edge_bundle, graph_)[*edgeIter] == edge.value())
                            {
                                return true; // Found the specific edge
                            }
                        }
                        else
                        {
                            foundEdge = true; // Found an edge between the vertices
                        }
                    }
                }

                // If we're looking for any edge between the vertices (not a specific one)
                return foundEdge;
            }
        }
        return true;
    }

    /**
     * @brief Add a vertex, if possible.
     * @param vertex the vertex to add
     * @return true/false depending on success
     * @throw vertex_existence_error if graph is configured to throw
     */
    bool addVertex(VertexProperty const& vertex)
    {
        if constexpr (!AllowMultipleVertices)
        {
            if (hasVertex(vertex))
            {
                if constexpr (ThrowOnError)
                {
                    throw vertex_existence_error("VertexProperty already exists in the graph");
                }
                else
                {
                    return false;
                }
            }
        }
        boost::add_vertex(vertex, graph_);
        return true;
    }

    /**
     * @brief Add multiple vertices to the graph.
     * @tparam Vertices vertices type
     * @param vertices the vertices to add
     * @throw vertex_existence_error on the first failure to add a vertex, if graph is configured to throw
     */
    template <typename... Vertices>
    void addVertices(Vertices&&... vertices)
    {
        (addVertex(std::forward<Vertices>(vertices)), ...);
    }

    std::vector<VertexProperty> getVertices() const
    {
        std::vector<VertexProperty> vertices;

        // Iterate over all vertices in the graph
        auto [vertexIter, vertexEnd] = boost::vertices(graph_);
        for (; vertexIter != vertexEnd; ++vertexIter)
        {
            // Push the vertex property into the vector
            vertices.push_back(graph_[*vertexIter]); // Assuming vertex properties are stored in the graph
        }

        return vertices;
    }

    /**
     * @brief Add an edge, if possible.
     * @param vertexSource source-vertex of the edge
     * @param vertexTarget target-vertex of the edge
     * @param edge edge-property
     * @return true/false depending on success
     * @throw vertex_existence_error if source or target vertices do not exist
     * @throw edge_existence_error if the edge between source and target already exists
     * @throw cycle_error if adding the edge would create a cycle
     */
    bool addEdge(VertexProperty const& vertexSource, VertexProperty const& vertexTarget, EdgeProperty const& edge)
    {
        if (!hasVertices(vertexSource, vertexTarget))
        {
            if constexpr (ThrowOnError)
            {
                throw vertex_existence_error("Both vertices must exist in the graph when adding an edge");
            }
            else
            {
                return false;
            }
        }

        if constexpr (!AllowParallelEdges)
        {
            if (hasEdge(vertexSource, vertexTarget))
            {
                if constexpr (!OverWriteEdgeProperty)
                {
                    if constexpr (ThrowOnError)
                    {
                        throw parallel_edge_error("Parallel edges are not allowed");
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        if constexpr (!AllowCycles)
        {
            if (vertexSource == vertexTarget || createsCycle(vertexSource, vertexTarget, edge))
            {
                if constexpr (ThrowOnError)
                {
                    throw cycle_error("Adding this edge would create a cycle");
                }
                else
                {
                    return false;
                }
            }
        }

        VertexDescriptor src = getVertexDescriptorUnsafe(vertexSource);
        VertexDescriptor tgt = getVertexDescriptorUnsafe(vertexTarget);
        boost::add_edge(src, tgt, edge, graph_);
        return true;
    }

    /**
     * @brief Retrieve a collection of all parallel edges between sourceVertex and targetVertex.
     * @param sourceVertex property of the source-vertex
     * @param targetVertex property of the target-vertex
     * @return a collection of all parallel edges between sourceVertex and targetVertex
     */
    std::vector<EdgeProperty>
        findParallelEdges(VertexProperty const& sourceVertex, VertexProperty const& targetVertex) const
    {
        std::vector<EdgeProperty> parallelEdges;

        if (!hasVertex(sourceVertex) || !hasVertex(targetVertex))
        {
            return parallelEdges; // Return empty if vertices are not in the graph
        }

        auto [edgeIter_begin, edgeIter_end] = boost::edges(graph_);

        for (auto it = edgeIter_begin; it != edgeIter_end; ++it)
        {
            VertexDescriptor src = boost::source(*it, graph_);
            VertexDescriptor tgt = boost::target(*it, graph_);

            // Check if both sourceVertex and targetVertex match
            if (graph_[src] == sourceVertex && graph_[tgt] == targetVertex)
            {
                parallelEdges.push_back(graph_[*it]);
            }
        }

        return parallelEdges;
    }

    /**
     * @brief Remove a vertex if it exists, don't fail if the vertex does not exist.
     * @param vertex property of the vertex to remove
     * @return true, if the vertex existed and was removed, false if it did not exist
     */
    bool removeVertex(VertexProperty const& vertex)
    {
        VertexDescriptor src;
        if (!getVertexDescriptor(vertex, src))
        {
            // non-critical so no throw
            return false;
        }
        boost::remove_vertex(src, graph_);
        return true;
    }

    /**
     * @brief Remove an edge if it exists.
     * @param vertexSource source vertex
     * @param vertexTarget target vertex
     * @param optional edge
     * @return true if an edge actually has been removed, false otherwise
     */
    bool removeEdge(
        VertexProperty const&       vertexSource,
        VertexProperty const&       vertexTarget,
        std::optional<EdgeProperty> edge = std::nullopt
    )
    {
        VertexDescriptor src, tgt;
        if (!getVertexDescriptor(vertexSource, src) || !getVertexDescriptor(vertexTarget, tgt))
        {
            // non-critical so no throw
            return false;
        }

        auto [edgeIter, found] = boost::edge(src, tgt, graph_);
        if (!found)
        {
            // non-critical so no throw
            return false;
        }
        auto has_edge = hasEdge(vertexSource, vertexTarget, edge);
        if (hasEdge(vertexSource, vertexTarget, edge))
        {
            boost::remove_edge(edgeIter, graph_);
            return true;
        }
        return false;
    }

    /**
     * @brief Apply a visitor to all vertices, filtered by a given filter function.
     * @tparam Visitor visitor function type
     * @tparam FilterFunc filter function type
     * @param vis visitor object
     * @param filter vertex filter
     */
    template <typename Visitor, typename FilterFunc = std::function<bool(VertexProperty const&)>>
    void applyVertices(Visitor& vis, FilterFunc filter = allVerticesFilter)
    {
        auto vertices = boost::vertices(graph_);
        for (auto it = vertices.first; it != vertices.second; ++it)
        {
            if (filter(graph_[*it])) // Apply the filter
            {
                vis(graph_[*it]); // Apply the visitor
            }
        }
    }

    /**
     * @brief Apply a visitor to all edges, filtered by a given filter function.
     * @tparam Visitor visitor function type
     * @tparam FilterFunc filter function type
     * @param vis visitor object
     * @param filter edge filter
     */
    template <typename Visitor, typename FilterFunc = std::function<bool(EdgeProperty const&)>>
    void applyEdges(Visitor& vis, FilterFunc filter = allEdgesFilter)
    {
        auto edges = boost::edges(graph_);
        for (auto it = edges.first; it != edges.second; ++it)
        {
            if (filter(graph_[*it])) // Apply the filter
            {
                vis(graph_[*it]); // Apply the visitor
            }
        }
    }

    /**
     * @brief Default vertex filter function.
     * @return true for all vertices
     */
    static bool allVerticesFilter(VertexProperty const&)
    {
        return true;
    }

    /**
     * @brief Default edge filter function.
     * @return true for all edges
     */
    static bool allEdgesFilter(EdgeProperty const&)
    {
        return true;
    }

    /**
     * @brief Function to get the children (outgoing neighbors) of a vertex.
     * @param vertex the common parent of the children
     * @return all children of the vertex
     */
    std::vector<VertexProperty> getChildren(VertexProperty const& vertex) const
    {
        VertexDescriptor vtx;
        if (!getVertexDescriptor(vertex, vtx))
        {
            if constexpr (ThrowOnError)
            {
                throw vertex_existence_error("The vertex does not exist in the graph");
            }
            else
            {
                return {}; // Return an empty vector if the vertex does not exist
            }
        }

        std::vector<VertexProperty> children;
        auto                        outEdges = boost::out_edges(vtx, graph_);
        for (auto it = outEdges.first; it != outEdges.second; ++it)
        {
            VertexDescriptor target = boost::target(*it, graph_);
            children.push_back(graph_[target]);
        }

        return children;
    }

    /**
     * @brief Function to get the parents (incoming neighbors) of a vertex.
     * @param vertex the common child of the parents
     * @return all the parents of the vertex
     */
    std::vector<VertexProperty> getParents(VertexProperty const& vertex) const
    {
        VertexDescriptor vtx;
        if (!getVertexDescriptor(vertex, vtx))
        {
            if constexpr (ThrowOnError)
            {
                throw vertex_existence_error("The vertex does not exist in the graph");
            }
            else
            {
                return {}; // Return an empty vector if the vertex does not exist
            }
        }

        std::vector<VertexProperty> parents;
        auto                        inEdges = boost::in_edges(vtx, graph_);
        for (auto it = inEdges.first; it != inEdges.second; ++it)
        {
            VertexDescriptor source = boost::source(*it, graph_);
            parents.push_back(graph_[source]);
        }

        return parents;
    }

    /**
     * @brief Get a collection of all edges described by their properties.
     * @return collection of all edge-properties with source and target properties
     */
    std::vector<std::tuple<VertexProperty, VertexProperty, EdgeProperty>> getEdgeProperties() const
    {
        std::vector<std::tuple<VertexProperty, VertexProperty, EdgeProperty>> edge_properties;

        // Iterate over all edges in the graph
        auto [edgeIter_begin, edgeIter_end] = boost::edges(graph_);
        for (auto it = edgeIter_begin; it != edgeIter_end; ++it)
        {
            // Get the source and target vertices for the current edge
            VertexDescriptor src = boost::source(*it, graph_);
            VertexDescriptor tgt = boost::target(*it, graph_);

            // Get the edge property for the current edge
            EdgeProperty edge_prop = graph_[*it];

            // Get the vertex properties for source and target
            VertexProperty src_prop = graph_[src];
            VertexProperty tgt_prop = graph_[tgt];

            // Add the tuple (source property, target property, edge property) to the result vector
            edge_properties.emplace_back(src_prop, tgt_prop, edge_prop);
        }

        return edge_properties;
    }

    /**
     * @brief Get a collection of all disconnected sub-graphs of the graph.
     * @return a collection of all disconnected sub-graphs
     */
    std::vector<directed_graph_base> getDisconnectedSubGraphs() const
    {
        // Create an undirected version of the graph
        using UndirectedGraph =
            boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty, EdgeProperty>;

        UndirectedGraph undirectedGraph;

        // Map from directed graph vertices to undirected graph vertices
        std::map<VertexDescriptor, typename boost::graph_traits<UndirectedGraph>::vertex_descriptor> vertex_map;

        // Copy vertices from the directed graph to the undirected graph
        auto [vertexIter_begin, vertexIter_end] = boost::vertices(graph_);
        for (auto it = vertexIter_begin; it != vertexIter_end; ++it)
        {
            // Add vertex in undirected graph and map it to the original vertex
            auto new_vertex = boost::add_vertex(graph_[*it], undirectedGraph);
            vertex_map[*it] = new_vertex;
        }

        // Copy edges from the directed graph to the undirected graph
        auto [edgeIter_begin, edgeIter_end] = boost::edges(graph_);
        for (auto it = edgeIter_begin; it != edgeIter_end; ++it)
        {
            EdgeProperty     edge = graph_[*it];
            VertexDescriptor src  = boost::source(*it, graph_);
            VertexDescriptor tgt  = boost::target(*it, graph_);

            // Use the mapped vertices from the undirected graph
            boost::add_edge(vertex_map[src], vertex_map[tgt], edge, undirectedGraph);
        }

        // Map to store the component for each vertex
        std::vector<int> component_map(boost::num_vertices(undirectedGraph));

        // Get the number of weakly connected components
        int num_components = boost::connected_components(undirectedGraph, &component_map[0]);

        // Create a vector of disconnected subGraphs (each sub-graph is a vector of vertices)
        std::vector<std::vector<VertexProperty>> subGraphs(num_components);

        // Map each vertex to its corresponding component
        for (auto it = vertexIter_begin; it != vertexIter_end; ++it)
        {
            int component_index = component_map[vertex_map[*it]]; // Use the mapped vertex in undirectedGraph
            subGraphs[component_index].push_back(graph_[*it]);
        }

        auto                             allEdges = getEdgeProperties();
        std::vector<directed_graph_base> revalGraphs{};
        for (auto const& vertexList: subGraphs)
        {
            directed_graph_base sub{};
            for (auto vertex: vertexList)
            {
                if (!sub.hasVertex(vertex))
                {
                    sub.addVertex(vertex);
                }
                for (auto edgePropTuple: allEdges)
                {
                    auto source = std::get<0>(edgePropTuple);
                    auto target = std::get<1>(edgePropTuple);
                    auto edge   = std::get<2>(edgePropTuple);
                    if (vertex == source)
                    {
                        if (!sub.hasVertex(target))
                        {
                            sub.addVertex(target);
                        }
                        sub.addEdge(source, target, edge);
                    }
                }
            }
            revalGraphs.push_back(sub);
        }
        return revalGraphs;
    }

    /**
     * @brief Function to return all paths from one vertex to another.
     * Each path includes both vertices and edge properties.
     *
     * @param vertexSource Source vertex
     * @param vertexTarget Target vertex
     * @return std::vector<std::vector<std::pair<VertexProperty, EdgeProperty>>> Collection of paths, where each path is
     * a sequence of {VertexProperty, EdgeProperty} pairs
     */
    std::vector<VertexEdgePath<VertexProperty, EdgeProperty>>
        getAllPaths(VertexProperty const& vertexSource, VertexProperty const& vertexTarget) const
    {
        // Check if both source and target vertices exist
        VertexDescriptor src, tgt;
        if (!getVertexDescriptor(vertexSource, src) || !getVertexDescriptor(vertexTarget, tgt))
        {
            return {}; // Return an empty vector if either vertex does not exist
        }

        // Store all paths
        std::vector<VertexEdgePath<VertexProperty, EdgeProperty>> paths;
        VertexEdgePath<VertexProperty, EdgeProperty>              current_path;
        std::unordered_set<VertexDescriptor>                      visited;

        // DFS helper to explore all paths
        std::function<void(VertexDescriptor)> dfs = [&](VertexDescriptor v) {
            visited.insert(v);

            // Explore outgoing edges
            auto outEdges = boost::out_edges(v, graph_);
            for (auto edgeIt = outEdges.first; edgeIt != outEdges.second; ++edgeIt)
            {
                VertexDescriptor source   = boost::source(*edgeIt, graph_);
                VertexDescriptor target   = boost::target(*edgeIt, graph_);
                EdgeProperty     edgeProp = graph_[*edgeIt]; // Get the edge property

                // Add the current vertex and edge to the path
                current_path.add(graph_[source], edgeProp, graph_[target]);

                // If the target vertex is the destination, save the full path
                if (target == tgt)
                {
                    paths.push_back(current_path);
                }
                else if (visited.find(target) == visited.end())
                {
                    // Continue the DFS if the target has not been visited yet
                    dfs(target);
                }
            }

            visited.erase(v);
        };

        // Start DFS from the source vertex
        dfs(src);

        return paths;
    }

  private:
    /**
     * @brief Safe retrieval of the vertex-descriptor (pointer/iterator/index/...) of a given vertex property.
     * This iterates through all vertices until it finds a matching vertex.
     * @param vertex vertex property
     * @param descriptor [out] the descriptor, if it can be found
     * @return true, if the descriptor was found, false otherwise
     */
    bool getVertexDescriptor(VertexProperty const& vertex, VertexDescriptor& descriptor) const
    {
        auto [vertIter_begin, vertIter_end] = boost::vertices(graph_);
        for (auto it = vertIter_begin; it != vertIter_end; ++it)
        {
            if (graph_[*it] == vertex)
            {
                descriptor = *it;
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Retrieve the vertex-descriptor (pointer/iterator/index/...) of a given vertex property, assuming it
     * exists.
     * @param vertex vertex property
     * @return the vertex descriptor
     */
    VertexDescriptor getVertexDescriptorUnsafe(VertexProperty const& vertex) const
    {
        VertexDescriptor descriptor;
        getVertexDescriptor(vertex, descriptor); // Assumes it's found
        return descriptor;
    }

    /**
     * @brief Cycle detection implementation.
     * @param vertexSource property of edge-source
     * @param vertexTarget property of edge-target
     * @param edge edge-property
     * @return true, if adding an edge from source to target would create a cycle in the graph, false otherwise
     */
    bool createsCycle(VertexProperty const& vertexSource, VertexProperty const& vertexTarget, EdgeProperty const& edge)
    {
        // Get vertex descriptors for source and target vertices
        VertexDescriptor src = getVertexDescriptorUnsafe(vertexSource);
        VertexDescriptor tgt = getVertexDescriptorUnsafe(vertexTarget);

        // Check if an edge between the source and target already exists
        auto [existingEdge, exists] = boost::edge(src, tgt, graph_);

        // If the edge already exists, no need to check for cycles
        if (exists)
        {
            return false; // Adding another parallel edge doesn't introduce a cycle
        }

        // Add the edge temporarily
        boost::add_edge(src, tgt, edge, graph_);

        // Check for cycles using DFS or BFS
        std::unordered_map<VertexDescriptor, bool> visited(boost::num_vertices(graph_));
        std::vector<VertexDescriptor>              recursion_stack;

        bool hasCycle = dfsCycleCheck(src, visited, recursion_stack);

        // Remove the temporarily added edge
        boost::remove_edge(src, tgt, graph_);

        return hasCycle;
    }

    /**
     * @brief Depth-first-search-based cycle check.
     * @param vertexDescriptor the storage-location of the vertex (pointer/iterator/...)
     * @param visited map to indicate whether a vertex has been visited (true) or not (false)
     * @param recursion_stack the work do do
     * @return true if a cycle has been detected
     */
    bool dfsCycleCheck(
        VertexDescriptor                            vertexDescriptor,
        std::unordered_map<VertexDescriptor, bool>& visited,
        std::vector<VertexDescriptor>&              recursion_stack
    )
    {
        if (!visited[vertexDescriptor])
        {
            visited[vertexDescriptor] = true;
            recursion_stack.push_back(vertexDescriptor);

            auto neighbors = boost::adjacent_vertices(vertexDescriptor, graph_);
            for (auto neighbor = neighbors.first; neighbor != neighbors.second; ++neighbor)
            {
                if ((!visited[*neighbor] && dfsCycleCheck(*neighbor, visited, recursion_stack)) ||
                    std::find(recursion_stack.begin(), recursion_stack.end(), *neighbor) != recursion_stack.end())
                {
                    return true;
                }
            }
        }

        recursion_stack.pop_back();
        return false;
    }
};

/**
 * @brief A typedef for all-purpose directed graphs with no hold barred.
 * <ul>
 * <li>vertices stored in hash-set/set/vector depending on @c VertexProperty (hash? comparable?)</li>
 * <li>edges stored in hash-set/set/vector depending on @c EdgeProperty (hash? comparable?)</li>
 * <li>no vertices with same property allowed</li>
 * <li>parallel edges are allowed</li>
 * <li>no cycles allowed</li>
 * <li>throwing on existing vertices or edges, on parallel edges, or cycles; return false instead</li>
 * <li>overwriting edge-properties</li>
 * </ul>
 * @tparam VertexProperty appropriate storage-selector for vertices
 * @tparam EdgeProperty appropriate storage-selector for edges
 */
template <typename VertexProperty, typename EdgeProperty, typename ThrowOnError = no_throw_on_error>
using directed_graph = directed_graph_base<
    VertexProperty,
    EdgeProperty,
    allow_multiple_vertices,
    allow_parallel_edges,
    allow_cycles_t,
    overwrite_edge_property,
    ThrowOnError>;

/**
 * @brief A typedef for directed acyclic graphs without parallel edges.
 * <ul>
 * <li>vertices stored in hash-set/set/vector depending on @c VertexProperty (hash? comparable?)</li>
 * <li>edges stored in hash-set/set/vector depending on @c EdgeProperty (hash? comparable?)</li>
 * <li>no vertices with same property allowed</li>
 * <li>parallel edges are not allowed</li>
 * <li>no cycles allowed</li>
 * <li>throwing on existing vertices or edges, on parallel edges, or cycles; return false instead</li>
 * <li>overwriting edge-properties</li>
 * </ul>
 * @tparam VertexProperty appropriate storage-selector for vertices
 * @tparam EdgeProperty appropriate storage-selector for edges
 */
template <typename VertexProperty, typename EdgeProperty, typename ThrowOnError = no_throw_on_error>
using directed_acyclic_graph = directed_graph_base<
    VertexProperty,
    EdgeProperty,
    disallow_multiple_vertices,
    disallow_parallel_edges,
    disallow_cycles,
    overwrite_edge_property,
    ThrowOnError>;

/**
 * @brief A typedef for directed acyclic graphs without parallel edges.
 * <ul>
 * <li>vertices stored in hash-set/set/vector depending on @c VertexProperty (hash? comparable?)</li>
 * <li>edges stored in hash-set/set/vector depending on @c EdgeProperty (hash? comparable?)</li>
 * <li>no vertices with same property allowed</li>
 * <li>parallel edges are allowed</li>
 * <li>no cycles allowed</li>
 * <li>throwing on existing vertices or edges, on parallel edges, or cycles; return false instead</li>
 * <li>overwriting edge-properties</li>
 * </ul>
 * @tparam VertexProperty appropriate storage-selector for vertices
 * @tparam EdgeProperty appropriate storage-selector for edges
 */
template <typename VertexProperty, typename EdgeProperty, typename ThrowOnError = no_throw_on_error_t>
using directed_acyclic_parallel_graph = directed_graph_base<
    VertexProperty,
    EdgeProperty,
    disallow_multiple_vertices,
    allow_parallel_edges,
    disallow_cycles,
    overwrite_edge_property,
    ThrowOnError>;

}; // namespace util

#endif // NS_UTIL_DIRECTED_GRAPH_H_INCLUDED
