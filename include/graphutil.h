/*
 * File Name:   graphutil.h
 * Description: graph utility functions
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

#ifndef NS_UTIL_GRAPHUTIL_H_INCLUDED
#define NS_UTIL_GRAPHUTIL_H_INCLUDED

#include "to_string.h"

#include <boost/concept_check.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/utility.hpp>
#include <concepts>
#include <exception>
#include <functional>
#include <ostream>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace util
{
/**
 * @brief Error handling for circles in an acyclic graph.
 */
struct circle_error : public std::logic_error
{
    circle_error(const std::string &node1Str, const std::string &node2Str)
    : std::logic_error("Cannot add edge " + node1Str + "->" + node2Str + " as that would create a circle.")
    {
    }
};

/**
 * @brief Error handling for parallel edges in an acyclic graph.
 */
struct parallel_error : public std::logic_error
{
    parallel_error(const std::string &node1Str, const std::string &node2Str)
    : std::logic_error("Cannot add edge " + node1Str + "->" + node2Str + " as that would create parallel edges.")
    {
    }
};

/**
 * @brief Helper class needed to avoid parallel edges in a directed graph
 * (undirected Edge).
 */
struct UndirEdge
{
    size_t n1_;
    size_t n2_;

    UndirEdge(size_t n1, size_t n2) : n1_(std::min(n1, n2)), n2_(std::max(n1, n2))
    {
    }
};

/**
 * @brief Generic ostream - &lt;&lt; operator for UndirEdge.
 */
inline std::ostream &operator<<(std::ostream &os, const UndirEdge &u)
{
    os << "(" << u.n1_ << "<=>" << u.n2_ << ")";

    return (os);
}

/**
 * @brief Hash functor for undirected edges.
 */
struct UndirEdge_hash
{
    size_t operator()(const UndirEdge &u) const
    {
        std::hash<size_t> hasher;

        return (hasher(u.n1_) ^ hasher(u.n2_));
    }
};

/**
 * @brief Equality for undirected edges.
 */
inline bool operator==(const UndirEdge &lhs, const UndirEdge &rhs)
{
    return ((lhs.n1_ == rhs.n1_) && (lhs.n2_ == rhs.n2_));
}

/**
 * @brief Abstract base class for nodes. Ensures that function hash() is implemented.
 */
struct NodeBase
{
    virtual ~NodeBase() = default;

    [[nodiscard]] virtual size_t hash() const = 0;
};

/**
 * @brief Generic hash-template function for nodes.
 *
 * @tparam NodeT_ node type
 * @param n node
 * @return size_t hash-value
 */
template<typename NodeT_>
inline size_t HashTFn(const NodeT_ &n)
{
    return (n.hash());
}

/**
 * @brief Generic hash-template functor for nodes.
 * Requires that template type is derived from NodeBase, hence implements hash().
 *
 * @tparam NodeT_
 */
template<typename NodeT_>
class HashTClass
{
    public:
    static_assert(std::is_base_of<NodeBase, NodeT_>::value,
                  "NodeT_ template parameter must be derived from class NodeBase");

    /**
     * @brief Call the generic template'd Hash-Function which in turn calls the
     * member hash() which we know is implemented because of concept-constraints.
     *
     * @param t
     * @return size_t
     */
    size_t operator()(const NodeT_ &t) const
    {
        return (HashTFn<NodeT_>(t));
    }
};

/**
 * @brief Plain old data implementation of NodeBase for ad-hoc use.
 */
template<typename T_>
struct PODNode : public NodeBase
{
    static_assert(std::is_default_constructible<T_>::value,
                  "template parameter of PODNode must be default-constructible");
    BOOST_CLASS_REQUIRE(T_, boost, EqualityComparableConcept);
    BOOST_CLASS_REQUIRE(T_, boost, LessThanComparableConcept);

    /**
     * @brief Default construct/construct from P.O.D - value
     *
     * @param v the value
     */
    PODNode(const T_ &v = T_()) : v_(v)
    {
    }

    /**
     * @brief Hash implementation using the hash template.
     * Fulfilling base-interface requirements.
     *
     * @return size_t hash-value
     */
    [[nodiscard]] size_t hash() const override
    {
        std::hash<T_> hasher;

        return (hasher(v_));
    }

    /**
     * @brief Retrieve the underlying value.
     *
     * @return T_& value
     */
    T_ &pod()
    {
        return (v_);
    }

    /**
     * @brief Less-operator needed to enable associative containers.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     * @return true, if lhs is less than rhs, false otherwise
     */
    friend bool operator<(const PODNode<T_> &lhs, const PODNode<T_> &rhs)
    {
        return (lhs.v_ < rhs.v_);
    }

    /**
     * @brief Equality-operator needed to enable associative containers.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     * @return true, if lhs is equal to rhs, false otherwise
     */
    friend bool operator==(const PODNode<T_> &lhs, const PODNode<T_> &rhs)
    {
        return (lhs.v_ == rhs.v_);
    }

    /**
     * @brief Inequality-operator.
     *
     * @param lhs left-hand-side
     * @param rhs right-hand-side
     * @return true, if lhs is not equal to rhs, false otherwise
     */
    friend bool operator!=(const PODNode<T_> &lhs, const PODNode<T_> &rhs)
    {
        return (lhs.v_ != rhs.v_);
    }

    /**
     * @brief Generic ostream - &lt;&lt; operator for PODNode.
     *
     * @param os output stream to be modified
     * @param node node to stream
     * @return std::ostream& the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const PODNode<T_> &node)
    {
        os << node.v_;

        return (os);
    }

    private:
    T_ v_;
};

/**
 * @brief Wrapper for edge-descriptors that keep a back-pointer to node-descriptors.
 */
template<typename NodeT_, typename EdgeT_>
struct EdgeDescriptor
{
    static_assert(std::is_base_of<NodeBase, NodeT_>::value,
                  "NodeT_ template parameter must be derived from class NodeBase");
    static_assert(std::is_default_constructible<NodeT_>::value,
                  "NodeT_ template parameter must be default-constructible type");
    static_assert(std::is_default_constructible<EdgeT_>::value,
                  "EdgeT_ template parameter must be default-constructible type");
    BOOST_CLASS_REQUIRE(NodeT_, boost, EqualityComparableConcept);
    BOOST_CLASS_REQUIRE(NodeT_, boost, LessThanComparableConcept);

    /**
     * Construct an edge descriptor with start- and end-node and edge-description.
     */
    EdgeDescriptor(NodeT_ start = NodeT_(), NodeT_ finish = NodeT_(), EdgeT_ e = EdgeT_())
    : start_(start)
    , finish_(finish)
    , e_(std::move(e))
    {
    }

    NodeT_ start_;
    NodeT_ finish_;
    EdgeT_ e_;
};

/**
 * @brief Generic ostream - &lt;&lt; operator for EdgeDescriptor.
 *
 * @tparam NodeT_ node type
 * @tparam EdgeT_ edge type
 * @param os output stream to modify
 * @param ed edge-descriptor to stream
 * @return std::ostream& the modified stream
 */
template<typename NodeT_, typename EdgeT_>
std::ostream &operator<<(std::ostream &os, const EdgeDescriptor<NodeT_, EdgeT_> &ed)
{
    os << "[ " << ed.start_ << "->" << ed.finish_ << " ]:-\"" << ed.e_ << "\"";

    return (os);
}

/**
 * @brief Class template for directed graphs.
 */
template<typename NodeT_, typename EdgeT_ = std::string>
class DirectedGraph
{
    public:
    /**
     * @brief Construct a new Directed Graph object.
     *
     * @param allowCycles whether (true) or not (false) to allow cycles in the graph
     * @param allowParallelEdges  whether (true) or not (false) to allow parallel edges in the graph
     */
    DirectedGraph(bool allowCycles = false, bool allowParallelEdges = false)
    : allowCycles_(allowCycles)
    , allowParallelEdges_(allowParallelEdges)
    {
    }

    static_assert(std::is_base_of<NodeBase, NodeT_>::value,
                  "NodeT_ template parameter must be derived from class NodeBase");
    static_assert(std::is_copy_constructible<NodeT_>::value,
                  "NodeT_ template parameter must be copy-constructible class NodeBase");
    BOOST_CLASS_REQUIRE(NodeT_, boost, EqualityComparableConcept);
    BOOST_CLASS_REQUIRE(NodeT_, boost, LessThanComparableConcept);

    using GRAPH_TYPE = boost::adjacency_list<boost::setS,
                                             boost::vecS,
                                             boost::bidirectionalS,
                                             NodeT_,
                                             EdgeDescriptor<NodeT_, EdgeT_>,
                                             boost::setS>;

    using vertex_t = typename boost::graph_traits<GRAPH_TYPE>::vertex_descriptor;
    using edge_t   = typename boost::graph_traits<GRAPH_TYPE>::edge_descriptor;

    using NODE_SET        = std::unordered_set<NodeT_, HashTClass<NodeT_>>;
    using INDEX_MAP       = std::unordered_map<NodeT_, vertex_t, HashTClass<NodeT_>>;
    using UNDIR_EDGE_SET  = std::unordered_set<UndirEdge, UndirEdge_hash>;
    using NODE_PTR_VECTOR = std::vector<NodeT_ *>;
    using EDGE_PTR_VECTOR = std::vector<EdgeDescriptor<NodeT_, EdgeT_> *>;
    using VERTEX_RESULT   = std::pair<vertex_t, bool>;

    /**
     * @brief Helper visitor class to help detect cycles in the graph.
     */
    struct cycle_detector : public boost::dfs_visitor<>
    {
        /**
         * @brief Construct a new cycle detector object
         *
         * @param has_cycle result as reference. Should be initialised with "false" by caller.
         */
        explicit cycle_detector(bool &has_cycle) : has_cycle_(has_cycle)
        {
        }

        cycle_detector(const cycle_detector &rhs) = default;

        cycle_detector(const cycle_detector &&)           = delete;
        cycle_detector operator=(const cycle_detector &)  = delete;
        cycle_detector operator=(const cycle_detector &&) = delete;

        /**
         * @brief Overridden dfs_visitor - method for back-edge event.
         *
         * @tparam Edge edge type
         * @tparam Graph graph type
         * @param e edge
         * @param g graph
         */
        template<typename Edge, typename Graph>
        void back_edge(Edge e, Graph &g)
        {
            has_cycle_ = true;
        }

        protected:
        bool &has_cycle_;
    };

    /**
     * @brief Reset the graph.
     */
    void clear()
    {
        graph_.clear();
        node2index_.clear();
        edges_.clear();
    }

    /**
     * @brief Add a node with value "node" to the graph.
     *
     * @param node the note to add to the graph
     * @return VERTEX_RESULT the added/modified vertex and whether (true) it was added or (false) only modified
     */
    VERTEX_RESULT addNode(NodeT_ node)
    {
        VERTEX_RESULT reval;
        auto          found = node2index_.find(node);

        if(found == node2index_.end())
        {
            node2index_[node] = boost::add_vertex(node, graph_);
            reval.first       = node2index_[node];
            reval.second      = true;
        }
        else
        {
            reval.first  = found->second;
            reval.second = false;
        }

        return (reval);
    }

    /**
     * @brief Retrieve a pointer to the value of a node using the vertex.
     *
     * @param v vertex
     * @return NodeT_* pointer to the node
     */
    NodeT_ *getNode(const vertex_t &v)
    {
        return (&graph_[v]);
    }

    /**
     * @brief Retrieve a pointer to the value of a node using a copy of the value.
     *
     * @param node the node to search
     * @return NodeT_* pointer to the value of the node
     */
    NodeT_ *getNode(const NodeT_ &node)
    {
        NodeT_ *reval = nullptr;
        auto    found = node2index_.find(node);
        if(found != node2index_.end())
        {
            reval = &graph_[found->second];
        }

        return (reval);
    }

    /**
     * @brief Retrieve a read-only pointer to the value of a node using a copy of the value.
     *
     * @param node the node to search
     * @return const NodeT_* pointer to the node
     */
    const NodeT_ *getNode(const NodeT_ &node) const
    {
        const NodeT_ *reval = nullptr;
        auto          found = node2index_.find(node);

        if(found != node2index_.end())
        {
            reval = &graph_[found->second];
        }

        return (reval);
    }

    /**
     * @brief Retrieve the number of in-edges to a node.
     *
     * @param checkNode the node to check
     * @return size_t the in-degree for the node
     */
    size_t inDegree(const NodeT_ &checkNode) const
    {
        auto found = node2index_.find(checkNode);

        return (found == node2index_.end() ? 0ul : static_cast<size_t>(in_degree(found->second, graph_)));
    }

    /**
     * @brief Determine whether the graph has a cycle.
     * This can only happen when we just added an edge, but since we'd like
     * our graph to be acyclic we will remove the offending edge.
     * Any non-member caller of this will always receive a false.
     *
     * @return true, if the graph has got a cycle, false otherwise
     */
    bool hasCycle() const
    {
        bool           has_cycle = false;
        cycle_detector vis(has_cycle);

        boost::depth_first_search(graph_, visitor(vis));

        return (has_cycle);
    }

    /**
     * @brief Do a depth first search on the graph.
     *
     * @tparam VisitorT_ visitor type
     * @param vis the visitor
     */
    template<typename VisitorT_>
    void applyDepthFirst(VisitorT_ &vis)
    {
        boost::depth_first_search(graph_, visitor(vis));
    }

    /**
     * @brief Internal function to re-jig the map after a deletion.
     */
    void reorganiseIndexMap()
    {
        node2index_.clear();

        typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator vi;
        typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator viend;

        for(boost::tie(vi, viend) = vertices(graph_); vi != viend; ++vi)
        {
            node2index_[graph_[*vi]] = *vi;
        }
    }

    /**
     * @brief Retrieve pointers to all nodes in the graph ordered in a depth first fashion.
     *
     * @return NODE_PTR_VECTOR vector of all nodes
     */
    NODE_PTR_VECTOR getNodes()
    {
        NODE_PTR_VECTOR reval;

        typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator vi;
        typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator viend;

        for(boost::tie(vi, viend) = vertices(graph_); vi != viend; ++vi)
        {
            reval.push_back(&graph_[*vi]);
        }

        return (reval);
    }

    /**
     * @brief Retrieve read-only pointers to all nodes in the graph ordered in a
     * depth first order.
     *
     * @return const NODE_PTR_VECTOR vector of all nodes
     */
    const NODE_PTR_VECTOR getNodes() const
    {
        NODE_PTR_VECTOR reval;

        typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator vi;
        typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator viend;

        for(boost::tie(vi, viend) = vertices(graph_); vi != viend; ++vi)
        {
            reval.push_back(const_cast<NodeT_ *>(&graph_[*vi]));
        }

        return (reval);
    }

    /**
     * @brief Remove a node from the graph.
     *
     * @param node the node to remove
     * @return true, if the node was removed, false if the node was not part of the graph
     */
bool removeNode(const NodeT_ &node)
    {
        bool reval = false;
        auto found = node2index_.find(node);

        if(found != node2index_.end())
        {
            // also remove all edges into and out of the node
            NODE_SET parents = parentNodes(node);

            for(auto it = parents.begin(); it != parents.end(); it++)
            {
                auto eIt = edges_.find(UndirEdge(node2index_[*it], found->second));

                remove_edge(node2index_[*it], found->second, graph_);

                if(eIt != edges_.end())
                    edges_.erase(eIt);
            }

            NODE_SET children = childrenNodes(node);

            for(auto it = children.begin(); it != children.end(); it++)
            {
                auto eIt = edges_.find(UndirEdge(node2index_[*it], found->second));

                remove_edge(found->second, node2index_[*it], graph_);

                if(eIt != edges_.end())
                    edges_.erase(eIt);
            }

            // then remove the node itself
            remove_vertex(found->second, graph_);

            reorganiseIndexMap();
            reval = true;
        }

        return (reval);
    }
    /**
     * @brief Add an edge from node1 to node2 with edge-information
     *
     * @param node1 first node
     * @param node2 second node
     * @param edge edge between the nodes
     * @return true, is successfully added
     * @throw circle_error, if inserting the edge would create a circle
     *        parallel_error,  if inserting the edge would create parallel edges
     */
    bool addEdge(NodeT_ node1, NodeT_ node2, EdgeT_ edge = EdgeT_())
    {
        bool          reval     = true;
        VERTEX_RESULT startNode = addNode(node1);
        VERTEX_RESULT endNode   = addNode(node2);
        UndirEdge     p(node2index_[node1], node2index_[node2]);
        auto          found = edges_.find(p);

        // don't add parallel edges
        if(found == edges_.end() || allowParallelEdges_)
        {
            EdgeDescriptor<NodeT_, EdgeT_> newGraphEdge(graph_[startNode.first], graph_[endNode.first], edge);
            std::pair<edge_t, bool>        added_edge = add_edge(startNode.first, endNode.first, newGraphEdge, graph_);

            if(!allowCycles_ && hasCycle())
            {
                // we just created a cycle - this must not happen: we got to reverse
                // the changes
                remove_edge(added_edge.first, graph_);

                if(startNode.second)
                    removeNode(node1);

                if(endNode.second)
                    removeNode(node2);

                throw circle_error(toString(node1), toString(node2));
            }
            edges_.insert(p);
        }
        else
        {
            throw parallel_error(toString(node1), toString(node2) + " [" + toString(edges_) + "]");
        }

        return (reval);
    }

    /**
     * @brief Remove an edge from the graph.
     *
     * @param node1 first node of the edge
     * @param node2 second node of the edge
     * @return true, if the edge was successfully removed, false otherwise
     */
    bool removeEdge(const NodeT_ &node1, const NodeT_ &node2)
    {
        auto n1    = node2index_.find(node1);
        auto n2    = node2index_.find(node2);
        bool reval = n1 != node2index_.end() && n2 != node2index_.end();

        if(reval)
        {
            auto eIt = edges_.find(UndirEdge(n1->second, n2->second));

            if(eIt != edges_.end())
                edges_.erase(eIt);

            remove_edge(node2index_[node1], node2index_[node2], graph_);
        }

        return (reval);
    }

    /**
     * @brief Retrieve a pointer to an edge from start to finish (if such an edge
     * exists, 0 otherwise).
     *
     * @param start start node of the edge
     * @param finish end node of the edge
     * @return const EdgeT_* pointer to the edge
     */
    const EdgeT_ *getEdge(const NodeT_ &start, const NodeT_ &finish) const
    {
        auto found = node2index_.find(start);

        if(found != node2index_.end() && out_degree(found->second, graph_) > 0)
        {
            typename GRAPH_TYPE::out_edge_iterator j;
            typename GRAPH_TYPE::out_edge_iterator j_end;

            for(boost::tie(j, j_end) = out_edges(found->second, graph_); j != j_end; ++j)
            {
                if(finish == graph_[*j].finish_)
                    return (&(graph_[*j].e_));
            }
        }

        return (nullptr);
    }

    /**
     * @brief Retrieve pointers to all edges of the graph.
     *
     * @return EDGE_PTR_VECTOR the edge pointers
     */
    EDGE_PTR_VECTOR getEdges()
    {
        EDGE_PTR_VECTOR                                         reval;
        typename boost::graph_traits<GRAPH_TYPE>::edge_iterator ei;
        typename boost::graph_traits<GRAPH_TYPE>::edge_iterator eiend;

        for(boost::tie(ei, eiend) = edges(graph_); ei != eiend; ++ei)
        {
            reval.push_back(&graph_[*ei]);
        }

        return (reval);
    }

    /**
     * @brief Retrieve read-only pointers to all edges of the graph.
     *
     * @return const EDGE_PTR_VECTOR  the edge pointers
     */
    const EDGE_PTR_VECTOR getEdges() const
    {
        EDGE_PTR_VECTOR                                         reval;
        typename boost::graph_traits<GRAPH_TYPE>::edge_iterator ei;
        typename boost::graph_traits<GRAPH_TYPE>::edge_iterator eiend;

        for(boost::tie(ei, eiend) = edges(graph_); ei != eiend; ++ei)
        {
            reval.push_back(const_cast<EdgeT_ *>(&graph_[*ei]));
        }

        return (reval);
    }

    /**
     * @brief Retrieve all nodes connected to checkNode where checkNode is the start-point.
     *
     * @param checkNode the node to check
     * @return NODE_SET the children nodes of checkNode
     */
    NODE_SET childrenNodes(const NodeT_ &checkNode) const
    {
        NODE_SET reval;
        auto     found = node2index_.find(checkNode);

        if(found != node2index_.end() && out_degree(found->second, graph_) > 0)
        {
            typename GRAPH_TYPE::out_edge_iterator j;
            typename GRAPH_TYPE::out_edge_iterator j_end;

            for(boost::tie(j, j_end) = out_edges(found->second, graph_); j != j_end; ++j)
            {
                reval.insert(graph_[*j].finish_);
            }
        }

        return (reval);
    }

    /**
     * @brief Retrieve all nodes connected to checkNode where checkNode is the end-point.
     *
     * @param checkNode the node to check
     * @return NODE_SET the parent nodes of checkNode
     */
    NODE_SET parentNodes(const NodeT_ &checkNode) const
    {
        NODE_SET reval;
        auto     found = node2index_.find(checkNode);

        if(found != node2index_.end() && in_degree(found->second, graph_) > 0)
        {
            typename GRAPH_TYPE::in_edge_iterator j;
            typename GRAPH_TYPE::in_edge_iterator j_end;

            for(boost::tie(j, j_end) = in_edges(found->second, graph_); j != j_end; ++j)
            {
                reval.insert(graph_[*j].start_);
            }
        }

        return (reval);
    }

    /**
     * @brief Retrieve all nodes connected to checkNode.
     *
     * @param checkNode the node to check
     * @return NODE_SET all nodes connected to checkNode
     */
    NODE_SET connectedNodes(const NodeT_ &checkNode) const
    {
        NODE_SET reval;
        auto     found = node2index_.find(checkNode);

        if(found != node2index_.end())
        {
            if(out_degree(found->second, graph_) > 0)
            {
                typename GRAPH_TYPE::out_edge_iterator jo;
                typename GRAPH_TYPE::out_edge_iterator jo_end;

                for(boost::tie(jo, jo_end) = out_edges(found->second, graph_); jo != jo_end; ++jo)
                {
                    reval.insert(graph_[*jo].finish_);
                }
            }
            if(in_degree(found->second, graph_) > 0)
            {
                typename GRAPH_TYPE::in_edge_iterator ji;
                typename GRAPH_TYPE::in_edge_iterator ji_end;

                for(boost::tie(ji, ji_end) = in_edges(found->second, graph_); ji != ji_end; ++ji)
                {
                    reval.insert(graph_[*ji].start_);
                }
            }
        }

        return (reval);
    }

    template<typename NodeTT_, typename EdgeTT_>
    friend std::ostream &operator<<(std::ostream &os, const DirectedGraph<NodeTT_, EdgeTT_> &dag);

    private:
    GRAPH_TYPE     graph_;
    INDEX_MAP      node2index_;
    UNDIR_EDGE_SET edges_;
    bool           allowCycles_;
    bool           allowParallelEdges_;
};

/**
 * @brief Generic ostream - &lt;&lt; operator for DirectedAcylicGraph.
 *
 * @tparam NodeT_ node type
 * @tparam EdgeT_ edge type
 * @param os the output stream to modify
 * @param dag the directed acycic graph to stream
 * @return std::ostream& the modified stream
 */
template<typename NodeT_, typename EdgeT_>
std::ostream &operator<<(std::ostream &os, const DirectedGraph<NodeT_, EdgeT_> &dag)
{
    using GRAPH_TYPE  = typename DirectedGraph<NodeT_, EdgeT_>::GRAPH_TYPE;
    using TRAITS_TYPE = typename boost::graph_traits<GRAPH_TYPE>;

    typename TRAITS_TYPE::vertex_iterator vi;
    typename TRAITS_TYPE::vertex_iterator viend;

    for(boost::tie(vi, viend) = vertices(dag.graph_); vi != viend; ++vi)
    {
        NodeT_ v = dag.graph_[*vi];

        os << "Node: " << v << std::endl
           << "\tchildren: " << dag.childrenNodes(v) << std::endl
           << "\tparents : " << dag.parentNodes(v) << std::endl;
    }

    return (os);
}
};  // namespace util

#endif  // NS_UTIL_GRAPHUTIL_H_INCLUDED
