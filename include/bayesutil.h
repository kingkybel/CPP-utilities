/*
 * File Name:   bayesutil.h
 * Description: Bayes net utility functions
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
 * @date: 2014-02-04
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_BAYESUTIL_H_INCLUDED
#define NS_UTIL_BAYESUTIL_H_INCLUDED

#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/utility.hpp>
#include <exception>
#include <functional>
#include <graphutil.h>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <statutil.h>
#include <string>
#include <utility>
#include <vector>

namespace util
{
/**
 * Node class for Bayes networks that encapsulates distributions.
 */
class Node : public NodeBase
{
    public:
    /**
     * Default construct.
     * @param  name
     * @param description
     * @param value-range
     */
    explicit Node(std::string            name        = "",
                  std::string            description = "",
                  const EventValueRange &range       = EventValueRange());

    /**
     * Copy construct.
     */
    Node(const Node &rhs);

    /**
     * Assignment.
     */
    Node &operator=(const Node &rhs);

    /**
     * Destruction.
     */
    ~Node() override;

    /**
     * Modify the verbose description of the node.
     */
    std::string setDescription(const std::string &descr);

    /**
     * Retrieve the description
     */
    [[nodiscard]] std::string description() const;

    /**
     * Retrieve the name of the node.
     */
    [[nodiscard]] std::string name() const;

    /**
     * Estimate the distribution of the node using a CSVAnalyzer.
     */
    bool train(const CSVAnalyzer &csv, bool hasValue = false);

    /**
     * Set the (discrete) distribution to uniform.
     */
    bool makeUniform(const VALUERANGES_TYPE &conditionValueRanges);

    /**
     * Make a (discrete) distribution conform to probability requirements.
     */
    bool normalise(const VALUERANGES_TYPE &conditionValueRanges);

    /**
     * Complete the probability table of a (discrete) distribution with
     * all values in the range.
     */
    bool canonise(const VALUERANGES_TYPE &conditionValueRanges);

    /**
     * Calculate a-priori distribution of a Node, given all the a-priori
     * of Nodes it depends on already done.
     * Use the following: P(A,B)= P(A|B)P(B)
     * where P(A|B) is the distribution of this node and P(B) comes from the
     * incoming nodes.
     */
    bool calculateAprioriDistribution(const std::set<Node> &incoming);

    /**
     * Retrieve the range.
     */
    [[nodiscard]] const EventValueRange &range() const;

    /**
     * Add a value to the (discrete) range.
     */
    bool addRangeValue(const Var &val);

    /**
     * Retrieve the distribution.
     */
    ProbabilityFunction *distribution();

    /**
     * Retrieve the a-priori-distribution.
     */
    ProbabilityFunction *aprioriDistribution();

    /**
     * Check whether the distribution of this node is fully defined.
     */
    [[nodiscard]] bool hasDistribution() const;

    /**
     * Check whether the a-priori-distribution of this node is fully defined.
     */
    [[nodiscard]] bool hasAprioriDistribution() const;

    /**
     * Get a probability using the distribution of this node.
     */
    [[nodiscard]] long double P(const CondEvent &ce) const;

    /**
     *  satisfy the hash-value requirement of graph-nodes.
     */
    [[nodiscard]] size_t hash() const override
    {
        std::hash<std::string> hasher;

        return (hasher(name()));
    }

    /**
     *  Enable associative containers.
     */
    friend bool operator<(const Node &lhs, const Node &rhs);

    /**
     *  Enable associative containers.
     */
    friend bool operator==(const Node &lhs, const Node &rhs);

    /**
     * Enable associative containers.
     */
    friend bool operator!=(const Node &lhs, const Node &rhs);

    /**
     *  Generic ostream - &lt;&lt; operator for Node.
     */
    friend std::ostream &operator<<(std::ostream &os, const Node &node);

    private:
    std::string                       name_;
    std::string                       description_;
    EventValueRange::DistributionType distType_;
    EventValueRange                   range_;
    ProbabilityFunction *             pDistribution_{nullptr};
    ProbabilityFunction *             pAprioriDistribution_{nullptr};
};

/**
 *  Directed edge-type between nodes in the Bayes-net.
 */
class Dependency
{
    public:
    /**
     *  Default construct with start- and end-node.
     */
    Dependency(std::string n1 = "[no condition]", std::string n2 = "[no event]");
    /**
     *  Start-node.
     */
    [[nodiscard]] std::string condition() const;
    /**
     *  End-node.
     */
    [[nodiscard]] std::string event() const;
    /**
     *  Description of the edge. A->B
     */
    [[nodiscard]] std::string name() const;
    /**
     *  Enable associative containers.
     */
    friend bool operator<(const Dependency &lhs, const Dependency &rhs);
    /**
     *  Enable associative containers.
     */
    friend bool operator==(const Dependency &lhs, const Dependency &rhs);

    private:
    std::string n1_;  ///< Name of start-node
    std::string n2_;  ///< Name of end-node
};

/**
 * Bayes-net (believe-net).
 * A uni-directed graph where nodes have distributions attached.
 */
class BayesNet
{
    public:
    using DAG_TYPE        = DirectedGraph<Node, Dependency>;
    using VERTEX_RESULT   = DAG_TYPE::VERTEX_RESULT;
    using NODE_SET        = DAG_TYPE::NODE_SET;
    using NODE_PTR_VECTOR = DAG_TYPE::NODE_PTR_VECTOR;

    /**
     * Default construct.
     */
    BayesNet();

    /**
     * Reset the Believe net.
     */
    void clear();

    /**
     * Add a node named name with verbose description.
     */
    bool addNode(const std::string &name, const std::string &description = "");

    /**
     * Add a node named name with a value range and verbose description.
     */
    bool addNode(const std::string &name, const EventValueRange &range, const std::string &description = "");

    /**
     * Remove node named name from the BN.
     */
    bool removeNode(const std::string &name);

    /**
     *  Retrieve a node by name from the net (constant).
     */
    const Node *getNode(const std::string &name) const;

    /**
     * Retrieve a node by name from the net.
     */
    Node *getNode(const std::string &name);

    /**
     * Add a cause/effect connection between two nodes. If the nodes do not yet
     * exist then create them.
     */
    bool addCauseEffect(const std::string &cause, const std::string &effect);

    /**
     * Check whether adding a cause/effect connection created a cycle, which
     * is not permissable.
     */
    bool hasCycle();

    /**
     * Retrieve the set of nodes connected to a node (in and out edges)
     */
    NODE_SET connectedNodes(const Node &checkNode);

    /**
     * Retrieve the set of children nodes connected to a node (out edges)
     */
    NODE_SET childrenNodes(const Node &checkNode);

    /**
     * Retrieve the set of parent nodes connected to a node (in edges)
     */
    NODE_SET parentNodes(const Node &checkNode);

    /**
     * Retrieve the set of nodes connected to a node (in and out edges)
     */
    const NODE_SET connectedNodes(const Node &checkNode) const;

    /**
     * Retrieve the set of children nodes connected to a node (out edges)
     */
    const NODE_SET childrenNodes(const Node &checkNode) const;

    /**
     * Retrieve the set of parent nodes connected to a node (in edges)
     */
    const NODE_SET parentNodes(const Node &checkNode) const;

    /**
     * Extract a list of node-name from a list of nodes.
     */
    std::set<std::string> nodes2names(const NODE_SET &nodes) const;

    /**
     * Retrieve the ranges of the incoming nodes of node named name.
     */
    VALUERANGES_TYPE conditionRanges(const std::string &name) const;

    /**
     * Use a CSVAnalyzer to estimate node distributions of the Bayes net.
     */
    bool trainWithCsv(const CSVAnalyzer &csv, bool hasValue = false, bool isDiscrete = true);

    /**
     * Use a CSVAnalyzer-file to estimate node distributions of the Bayes net.
     */
    bool trainWithCsv(const std::string &filename, bool hasValue = false, bool isDiscrete = true);

    /**
     * Make all nodes uniform.
     */
    bool makeUniform();

    /**
     * Normalise all nodes.
     */
    bool normalise();

    /**
     * Canonise all nodes,
     */
    bool canonise();

    /**
     * breadth-first calculation starting with independent Node's
     */
    bool calculateAprioriDistributions();

    /**
     * Make node named name uniform.
     */
    bool makeUniform(const std::string &name);

    /**
     * Normalise node named name.
     */
    bool normalise(const std::string &name);

    /**
     * Canonise node named name.
     */
    bool canonise(const std::string &name);

    /**
     * Check whether the model is fully defined with all distributions present.
     */
    bool fullyDefined() const;

    /**
     * Retrieve a list of node names (breadth-first).
     */
    std::vector<std::string> breadthFirstNodeNames() const;

    /**
     * Calculate a conditional event probability using the Bayes net.
     */
    long double P(const CondEvent &ce) const;

    /**
     * Calculate conditionally independent sets of nodes (d-separated nodes).
     */
    CondEvent bayesBallAlgorithm(const CondEvent &ce, EventList &irrelevant) const;

    /**
     * Generic ostream - &lt;&lt; operator for BayesNet.
     */
    friend std::ostream &operator<<(std::ostream &os, const BayesNet &bn);

    protected:
    /**
     * Make node uniform.
     */
    bool makeUniform(Node &node);

    /**
     * Normalise node.
     */
    bool normalise(Node &node);

    /**
     * Canonise node,
     */
    bool canonise(Node &node);

    private:
    DirectedGraph<Node, Dependency> g_;  ///< The underlying graph.
};

};
// namespace util

#endif  // NS_UTIL_BAYESUTIL_H_INCLUDED
