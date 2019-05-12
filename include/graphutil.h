/*
 * File Name:   graphutil.h
 * Description: graph utility functions
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
 * @date: 2011-12-13
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_GRAPHUTIL_H_INCLUDED
#define NS_UTIL_GRAPHUTIL_H_INCLUDED

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/utility.hpp>
#include <boost/concept_check.hpp>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <ostream>
#include <vector>
#include <set>
#include <exception>

namespace util
{

    /**
     * Error handling for circles in an acyclic graph.
     */
    struct circle_error : public std::logic_error
    {

        circle_error(const std::string& node1Str, const std::string& node2Str)
        : std::logic_error("Cannot add edge "
                           + node1Str + "->" + node2Str
                           + " as that would create a circle.")
        {
        }
    };

    /**
     * Error handling for parallel edges in an acyclic graph.
     */
    struct parallel_error : public std::logic_error
    {

        parallel_error(const std::string& node1Str, const std::string& node2Str)
        : std::logic_error("Cannot add edge "
                           + node1Str + "->" + node2Str
                           + " as that would create parallel edges.")
        {
        }
    };

    /**
     * Helper class needed to avoid parallel edges in a directed graph
     * (undirected Edge).
     */
    struct UndirEdge
    {
        size_t n1_;
        size_t n2_;

        UndirEdge(size_t n1, size_t n2)
        : n1_(std::min(n1, n2))
        , n2_(std::max(n1, n2))
        {
        }
    };

    /**
     * Generic ostream - \<\< operator for UndirEdge.
     */
    inline std::ostream& operator<<(std::ostream& os, const UndirEdge& u)
    {
        os << "(" << u.n1_ << "<=>" << u.n2_ << ")";
        return os;
    }

    /**
     * Hash functor for undirected edges.
     */
    struct UndirEdge_hash
    {
        /// use the

        size_t operator()(const UndirEdge& u) const
        {
            std::hash<size_t> hasher;
            return hasher(u.n1_) ^ hasher(u.n2_);
        }
    };

    /**
     * Equality for undirected edges.
     */
    inline bool operator==(const UndirEdge& lhs, const UndirEdge& rhs)
    {
        return lhs.n1_ == rhs.n1_ && lhs.n2_ == rhs.n2_;
    }

    /**
     * Abstract base class for nodes. Ensures that function hash() is implemented.
     */
    struct NodeBase
    {
        virtual size_t hash() const = 0;
    };

    /**
     * Generic hash-template function for nodes.
     */
    template<typename N_>
    inline size_t HashTFn(const N_& n)
    {
        return n.hash();
    }

    /**
     * Generic hash-template functor for nodes.
     * Requires that template type is derived from NodeBase, hence implements hash().
     */
    template<typename T_>
    class HashTClass
    {
    public:
        BOOST_CLASS_REQUIRE2(T_*, NodeBase*, boost, ConvertibleConcept);

        /**
         * Call the generic template'd Hash-Function which in turn calls the
         * member hash() which we know is implemented because of concept-constraints.)
         */
        size_t operator()(const T_& t) const
        {
            return HashTFn<T_>(t);
        }
    };

    /**
     * Plain old data implementation of NodeBase for ad-hoc use.
     */
    template<typename T_>
    struct PODNode : public NodeBase
    {
        BOOST_CLASS_REQUIRE(T_, boost, DefaultConstructibleConcept);
        BOOST_CLASS_REQUIRE(T_, boost, EqualityComparableConcept);
        BOOST_CLASS_REQUIRE(T_, boost, LessThanComparableConcept);

        /**
         * Default construct/construct from P.O.D - value
         */
        PODNode(const T_& v = T_())
        : v_(v)
        {
        }

        /**
         * Hash implementation using the hash template.
         * Fulfilling base-interface requirements.
         */
        virtual size_t hash() const
        {
            std::hash<T_> hasher;
            return hasher(v_);
        }

        /**
         * Retrieve the underlying value.
         */
        T_& pod()
        {
            return v_;
        }

        /**
         * Enable associative containers.
         */
        friend bool operator<(const PODNode<T_>& lhs, const PODNode<T_>& rhs)
        {
            return lhs.v_ < rhs.v_;
        }

        /**
         * Enable associative containers.
         */
        friend bool operator==(const PODNode<T_>& lhs, const PODNode<T_>& rhs)
        {
            return lhs.v_ == rhs.v_;
        }

        /**
         * Enable associative containers.
         */
        friend bool operator!=(const PODNode<T_>& lhs, const PODNode<T_>& rhs)
        {
            return lhs.v_ != rhs.v_;
        }

        /**
         * Generic ostream - \<\< operator for PODNode.
         */
        friend std::ostream& operator<<(std::ostream& os, const PODNode<T_>& n)
        {
            os << n.v_;
            return os;
        }
    private:
        T_ v_;
    };

    /**
     * Wrapper for edge-descriptors that keep a back-pointer to node-descriptors.
     */
    template<typename NodeT_, typename EdgeT_>
    struct EdgeDescriptor
    {
        BOOST_CLASS_REQUIRE2(NodeT_*, NodeBase*, boost, ConvertibleConcept);
        BOOST_CLASS_REQUIRE(NodeT_, boost, DefaultConstructibleConcept);
        BOOST_CLASS_REQUIRE(NodeT_, boost, EqualityComparableConcept);
        BOOST_CLASS_REQUIRE(NodeT_, boost, LessThanComparableConcept);
        BOOST_CLASS_REQUIRE(EdgeT_, boost, DefaultConstructibleConcept);

        /**
         * Construct an edge descriptor with start- and end-node and edge-description.
         */
        EdgeDescriptor(NodeT_ start = NodeT_(),
                       NodeT_ finish = NodeT_(),
                       const EdgeT_& e = EdgeT_())
        : start_(start)
        , finish_(finish)
        , e_(e)
        {
        }

        EdgeT_ e_;
        NodeT_ start_;
        NodeT_ finish_;
    };

    /**
     * Generic ostream - \<\< operator for EdgeDescriptor.
     */
    template<typename NodeT_, typename EdgeT_>
    std::ostream& operator<<(std::ostream& os,
                             const EdgeDescriptor<NodeT_, EdgeT_>& ed)
    {
        os << "[ " << ed.start_ << "->" << ed.finish_ << " ]:-\"" << ed.e_ << "\"";
        return os;
    }

    /**
     * Class template for directed graphs.
     */
    template<typename NodeT_, typename EdgeT_ = std::string>
    class DirectedGraph
    {
    public:

        DirectedGraph(bool allowCycles = false, bool allowParallelEdges = false)
        : allowCycles_(allowCycles)
        , allowParallelEdges_(allowParallelEdges)
        {
        }

        BOOST_CLASS_REQUIRE2(NodeT_*, NodeBase*, boost, ConvertibleConcept);
        BOOST_CLASS_REQUIRE(NodeT_, boost, DefaultConstructibleConcept);
        BOOST_CLASS_REQUIRE(NodeT_, boost, EqualityComparableConcept);
        BOOST_CLASS_REQUIRE(NodeT_, boost, LessThanComparableConcept);
        typedef boost::adjacency_list< boost::setS,
        boost::vecS,
        boost::bidirectionalS,
        NodeT_,
        EdgeDescriptor<NodeT_, EdgeT_>,
        boost::setS> GRAPH_TYPE;
        typedef typename boost::graph_traits<GRAPH_TYPE>::vertex_descriptor vertex_t;
        typedef typename boost::graph_traits<GRAPH_TYPE>::edge_descriptor edge_t;

        typedef std::unordered_set<NodeT_, HashTClass<NodeT_> > NODE_SET;
        typedef std::unordered_map<NodeT_, vertex_t, HashTClass<NodeT_> > INDEX_MAP;
        typedef std::unordered_set<UndirEdge, UndirEdge_hash> UNDIR_EDGE_SET;
        typedef std::vector<NodeT_*> NODE_PTR_VECTOR;
        typedef std::vector<EdgeDescriptor<NodeT_, EdgeT_>*> EDGE_PTR_VECTOR;
        typedef std::pair<vertex_t, bool> VERTEX_RESULT;

        /**
         * Helper visitor class to help detect cycles in the graph.
         */
        struct cycle_detector : public boost::dfs_visitor<>
        {
            /// Pass the result as reference. Should be initialised with "false" by caller.

            cycle_detector(bool& has_cycle)
            : has_cycle_(has_cycle)
            {
            }

            /**
             * Overriden dfs_visitor - method for back-edge- event.
             */
            template <class Edge, class Graph>
            void back_edge(Edge e, Graph& g)
            {
                has_cycle_ = true;
            }

        protected:
            bool& has_cycle_;
        };

        /**
         * Reset the graph.
         */
        void clear()
        {
            graph_.clear();
            node2index_.clear();
            edges_.clear();
        }

        /**
         * Add a node with value "node" to the graph.
         */
        VERTEX_RESULT addNode(NodeT_ node)
        {
            VERTEX_RESULT reval;
            auto found = node2index_.find(node);
            if (found == node2index_.end())
            {
                node2index_[node] = boost::add_vertex(node, graph_);
                reval.first = node2index_[node];
                reval.second = true;
            }
            else
            {
                reval.first = found->second;
                reval.second = false;
            }
            return reval;
        }

        /**
         * Retrieve a pointer to the value of a node using the vertex.
         */
        NodeT_* getNode(const vertex_t& v)
        {
            return &graph_[v];
        }

        /**
         * Retrieve a pointer to the value of a node using a copy of the value.
         */
        NodeT_* getNode(const NodeT_& node)
        {
            NodeT_* reval = 0;
            auto found = node2index_.find(node);
            if (found != node2index_.end())
            {
                reval = &graph_[found->second];
            }

            return reval;
        }

        /**
         * Retrieve a read-only pointer to the value of a node using a copy of the value.
         */
        const NodeT_* getNode(const NodeT_& node) const
        {
            const NodeT_* reval = 0;
            auto found = node2index_.find(node);
            if (found != node2index_.end())
            {
                reval = &graph_[found->second];
            }

            return reval;
        }

        /**
         * Retrieve the number of in-edges to a node.
         */
        size_t inDegree(const NodeT_& checkNode) const
        {
            auto found = node2index_.find(checkNode);
            return found == node2index_.end() ? 0 :
                (size_t) in_degree(found->second, graph_);
        }

        /**
         * Determine whether the graph has a cycle.
         * This can only happen when we just added an edge, but since we'd like
         * our graph to be acyclic we will remove the offending edge.
         * Any non-member caller of this will always receive a false.
         */
        bool hasCycle() const
        {
            bool has_cycle = false;
            cycle_detector vis(has_cycle);
            boost::depth_first_search(graph_, visitor(vis));
            return has_cycle;
        }

        /**
         * Do a depth first search on the graph.
         */
        template<typename VisitorT_>
        void applyDepthFirst(VisitorT_& vis)
        {
            boost::depth_first_search(graph_, visitor(vis));
        }

        /**
         * Internal function to re-jig the map after a deletion.
         */
        void reorganiseIndexMap()
        {
            node2index_.clear();
            typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator vi, viend;
            for (boost::tie(vi, viend) = vertices(graph_); vi != viend; ++vi)
            {
                node2index_[graph_[*vi]] = *vi;
            }
        }

        /**
         * Retrieve pointers to all nodes in the graph ordered in a depth first fashion.
         */
        NODE_PTR_VECTOR getNodes()
        {
            NODE_PTR_VECTOR reval;

            typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator vi, viend;
            for (boost::tie(vi, viend) = vertices(graph_); vi != viend; ++vi)
            {
                reval.push_back(&graph_[*vi]);
            }

            return reval;
        }

        /**
         * Retrieve read-only pointers to all nodes in the graph ordered in a
         * depth first order.
         */
        const NODE_PTR_VECTOR getNodes() const
        {
            NODE_PTR_VECTOR reval;

            typename boost::graph_traits<GRAPH_TYPE>::vertex_iterator vi, viend;
            for (boost::tie(vi, viend) = vertices(graph_); vi != viend; ++vi)
            {
                reval.push_back(const_cast<NodeT_*> (&graph_[*vi]));
            }

            return reval;
        }

        /**
         *  Remove a node from the graph.
         */
        bool removeNode(const NodeT_& node)
        {
            bool reval = false;
            auto found = node2index_.find(node);
            if (found != node2index_.end())
            {
                TRACE1(*found);
                // also remove all edges into and out of the node
                NODE_SET parents = parentNodes(node);
                for (auto it = parents.begin(); it != parents.end(); it++)
                {
                    auto eIt = edges_.find(UndirEdge(node2index_[*it], found->second));
                    remove_edge(node2index_[*it], found->second, graph_);
                    if (eIt != edges_.end())
                        edges_.erase(eIt);
                }
                NODE_SET children = childrenNodes(node);
                for (auto it = children.begin(); it != children.end(); it++)
                {
                    auto eIt = edges_.find(UndirEdge(node2index_[*it], found->second));
                    remove_edge(found->second, node2index_[*it], graph_);
                    if (eIt != edges_.end())
                        edges_.erase(eIt);
                }
                // then remove the node itself
                remove_vertex(found->second, graph_);

                reorganiseIndexMap();
                reval = true;
            }

            return reval;
        }

        /**
         * Add an edge from node1 to node2 with edge-information
         */
        bool addEdge(NodeT_ node1, NodeT_ node2, EdgeT_ edge = EdgeT_())
        {
            bool reval = true;
            VERTEX_RESULT startNode = addNode(node1);
            VERTEX_RESULT endNode = addNode(node2);
            UndirEdge p(node2index_[node1], node2index_[node2]);
            auto found = edges_.find(p);

            // don't add parallel edges
            if (found == edges_.end() || allowParallelEdges_)
            {
                EdgeDescriptor<NodeT_, EdgeT_> newGraphEdge(graph_[startNode.first],
                                                            graph_[endNode.first],
                                                            edge);
                std::pair<edge_t, bool> added_edge = add_edge(startNode.first,
                                                              endNode.first,
                                                              newGraphEdge,
                                                              graph_);
                if (!allowCycles_ && hasCycle())
                {
                    // we just created a cycle - this must not happen: we got to reverse
                    // the changes
                    remove_edge(added_edge.first, graph_);
                    if (startNode.second)
                        removeNode(node1);
                    if (endNode.second)
                        removeNode(node2);
                    throw circle_error(asString(node1), asString(node2));
                }
                edges_.insert(p);
            }
            else
            {
                throw parallel_error(asString(node1), asString(node2) + " [" + asString(edges_) + "]");
            }

            return reval;
        }

        /**
         * Remove an edge from the graph.
         */
        bool removeEdge(const NodeT_& node1, const NodeT_& node2)
        {
            auto n1 = node2index_.find(node1);
            auto n2 = node2index_.find(node2);
            bool reval = n1 != node2index_.end() && n2 != node2index_.end();
            if (reval)
            {
                auto eIt = edges_.find(UndirEdge(n1->second, n2->second));
                if (eIt != edges_.end())
                    edges_.erase(eIt);
                remove_edge(node2index_[node1], node2index_[node2], graph_);
            }
            return reval;
        }

        /**
         * Retrieve a pointer to an edge from start to finish (if such an edge
         * exists, 0 otherwise).
         */
        const EdgeT_* getEdge(const NodeT_& start, const NodeT_& finish) const
        {
            auto found = node2index_.find(start);
            if (found != node2index_.end() && out_degree(found->second, graph_) > 0)
            {
                typename GRAPH_TYPE::out_edge_iterator j, j_end;
                for (boost::tie(j, j_end) = out_edges(found->second, graph_);
                     j != j_end;
                     ++j)
                {
                    if (finish == graph_[*j].finish_)
                        return &(graph_[*j].e_);
                }
            }

            return (EdgeT_*) 0;
        }

        /**
         * Retrieve pointers to all edges of the graph.
         */
        EDGE_PTR_VECTOR getEdges()
        {
            EDGE_PTR_VECTOR reval;

            typename boost::graph_traits<GRAPH_TYPE>::edge_iterator ei, eiend;
            for (boost::tie(ei, eiend) = edges(graph_); ei != eiend; ++ei)
            {
                reval.push_back(&graph_[*ei]);
            }

            return reval;
        }

        /**
         * Retrieve read-only pointers to all edges of the graph.
         */
        const EDGE_PTR_VECTOR getEdges() const
        {
            EDGE_PTR_VECTOR reval;

            typename boost::graph_traits<GRAPH_TYPE>::edge_iterator ei, eiend;
            for (boost::tie(ei, eiend) = edges(graph_); ei != eiend; ++ei)
            {
                reval.push_back(const_cast<EdgeT_*> (&graph_[*ei]));
            }

            return reval;
        }

        /**
         * Retrieve all nodes connected to checkNode where checkNode is the start-point.
         */
        NODE_SET childrenNodes(const NodeT_& checkNode) const
        {
            NODE_SET reval;

            auto found = node2index_.find(checkNode);
            if (found != node2index_.end() && out_degree(found->second, graph_) > 0)
            {
                typename GRAPH_TYPE::out_edge_iterator j, j_end;
                for (boost::tie(j, j_end) = out_edges(found->second, graph_);
                     j != j_end;
                     ++j)
                {
                    reval.insert(graph_[*j].finish_);
                }
            }

            return reval;
        }

        /**
         * Retrieve all nodes connected to checkNode where checkNode is the end-point.
         */
        NODE_SET parentNodes(const NodeT_& checkNode) const
        {
            NODE_SET reval;

            auto found = node2index_.find(checkNode);
            if (found != node2index_.end() && in_degree(found->second, graph_) > 0)
            {
                typename GRAPH_TYPE::in_edge_iterator j, j_end;
                for (boost::tie(j, j_end) = in_edges(found->second, graph_);
                     j != j_end;
                     ++j)
                {
                    reval.insert(graph_[*j].start_);
                }
            }

            return reval;
        }

        /**
         * Retrieve all nodes connected to checkNode.
         */
        NODE_SET connectedNodes(const NodeT_& checkNode) const
        {
            NODE_SET reval;

            auto found = node2index_.find(checkNode);
            if (found != node2index_.end())
            {
                if (out_degree(found->second, graph_) > 0)
                {
                    typename GRAPH_TYPE::out_edge_iterator jo, jo_end;
                    for (boost::tie(jo, jo_end) = out_edges(found->second, graph_); jo != jo_end; ++jo)
                    {
                        reval.insert(graph_[*jo].finish_);
                    }
                }
                if (in_degree(found->second, graph_) > 0)
                {
                    typename GRAPH_TYPE::in_edge_iterator ji, ji_end;
                    for (boost::tie(ji, ji_end) = in_edges(found->second, graph_); ji != ji_end; ++ji)
                    {
                        reval.insert(graph_[*ji].start_);
                    }
                }
            }

            return reval;
        }

        template<typename NodeTT_, typename EdgeTT_>
        friend std::ostream& operator<<(std::ostream& os,
                                        const DirectedGraph<NodeTT_, EdgeTT_>& dag);

    private:
        GRAPH_TYPE graph_;
        INDEX_MAP node2index_;
        UNDIR_EDGE_SET edges_;
        bool allowCycles_;
        bool allowParallelEdges_;
    };

    /**
     * Generic ostream - \<\< operator for DirectedAcylicGraph.
     */
    template<typename NodeT_, typename EdgeT_>
    std::ostream& operator<<(std::ostream& os,
                             const DirectedGraph<NodeT_, EdgeT_>& dag)
    {
        typedef typename DirectedGraph<NodeT_, EdgeT_>::GRAPH_TYPE GRAPH_TYPE;
        typedef typename boost::graph_traits<GRAPH_TYPE> TRAITS_TYPE;

        typename TRAITS_TYPE::vertex_iterator vi, viend;
        for (boost::tie(vi, viend) = vertices(dag.graph_); vi != viend; ++vi)
        {
            NodeT_ v = dag.graph_[*vi];
            os << "Node: " << v << std::endl
                << "\tchildren: " << dag.childrenNodes(v) << std::endl
                << "\tparents : " << dag.parentNodes(v) << std::endl
                ;
        }
        return os;
    }
};

#endif // NS_UTIL_GRAPHUTIL_H_INCLUDED

