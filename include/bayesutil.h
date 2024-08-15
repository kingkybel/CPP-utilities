/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   include/bayesutil.h
 * Description: Bayes net utility functions
 *
 * Copyright (C) 2023 Dieter J Kybelksties
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

#ifndef NS_UTIL_BAYESUTIL_H_INCLUDED
#define NS_UTIL_BAYESUTIL_H_INCLUDED

#include "graphutil.h"
#include "statutil.h"

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
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace util
{
/**
 * @brief Node class for Bayes networks that encapsulates distributions.
 */
class Node : public NodeBase
{
    public:
    /**
     * @brief Default construct.
     * @param  name
     * @param description
     * @param value-range
     */
    explicit Node(std::string            name        = "",
                  std::string            description = "",
                  const EventValueRange &range       = EventValueRange());

    /**
     * @brief Copy construct.
     */
    Node(const Node &rhs);

    /**
     * @brief Assignment.
     */
    Node &operator=(const Node &rhs);

    /**
     * @brief Destruction.
     */
    ~Node() override;

    /**
     * @brief Modify the verbose description of the node.
     *
     * @param descr
     * @return std::string
     */
    std::string setDescription(const std::string &descr);

    /**
     * @brief Retrieve the description
     */
    [[nodiscard]] std::string description() const;

    /**
     * @brief Retrieve the name of the node.
     */
    [[nodiscard]] std::string name() const;

    /**
     * @brief Estimate the distribution of the node using a CSVAnalyzer.
     *
     * @param csv a csv object
     * @param hasValue true, if the CSV a value column
     * @return true, it training successful, false otherwise
     */
    bool train(const CSVAnalyzer &csv, bool hasValue = false);

    /**
     * @brief Set the (discrete) distribution to uniform.
     *
     * @param conditionValueRanges ranges to base the uniform-ness on
     * @return true, if making uniform was successful, false otherwise
     */
    bool makeUniform(const VALUERANGES_TYPE &conditionValueRanges);

    /**
     * @brief Make a (discrete) distribution conform to probability requirements.
     *
     * @param conditionValueRanges ranges to base the normalisation on
     * @return true, if normalisation was successful, false otherwise
     */
    bool normalise(const VALUERANGES_TYPE &conditionValueRanges);

    /**
     * @brief Complete the probability table of a (discrete) distribution with
     * all values in the range.
     *
     * @param conditionValueRanges ranges to base the canonisation on
     * @return true, if canonisation was successful, false otherwise
     */
    bool canonise(const VALUERANGES_TYPE &conditionValueRanges);

    /**
     * @brief Calculate a-priori distribution of a Node, given all the a-priori
     * of Nodes it depends on already done.
     * Use the following: P(A,B)= P(A|B)P(B)
     * where P(A|B) is the distribution of this node and P(B) comes from the
     * incoming nodes.
     *
     * @param incoming the set of incoming nodes
     * @return true, if canlculation was successful, false otherwise
     */
    bool calculateAprioriDistribution(const std::set<Node> &incoming);

    /**
     * @brief Retrieve the range.
     */
    [[nodiscard]] const EventValueRange &range() const;

    /**
     * @brief Add a value to the (discrete) range.
     *
     * @param val value to add to the discrete value range
     * @return true, if successfully added, false otherwise
     */
    bool addRangeValue(const Var &val);

    /**
     * @brief Retrieve the distribution.
     *
     * @return ProbabilityFunction* the probability function
     */
    ProbabilityFunction *distribution();

    /**
     * @brief Retrieve the a-priori-distribution.
     *
     * @return ProbabilityFunction* the a-priori distribution
     */
    ProbabilityFunction *aprioriDistribution();

    /**
     * @brief Check whether the distribution of this node is fully defined.
     *
     * @return true, if so, false otherwise
     */
    [[nodiscard]] bool hasDistribution() const;

    /**
     * @brief Check whether the a-priori-distribution of this node is fully defined.
     *
     * @return true, if so, false otherwise
     */
    [[nodiscard]] bool hasAprioriDistribution() const;

    /**
     * @brief Get a probability using the distribution of this node.
     *
     * @param ce the conditional event to calculate the probability for
     * @return long double the calculated probability
     */
    [[nodiscard]] long double P(const CondEvent &ce) const;

    /**
     * @brief satisfy the hash-value requirement of graph-nodes.
     *
     * @return size_t hash value
     */
    [[nodiscard]] size_t hash() const override
    {
        std::hash<std::string> hasher;

        return (hasher(name()));
    }

    /**
     * @brief less operator for Node objects.
     * Needed to enable associative containers.
     * @param lhs left-hand-side Node
     * @param rhs  left-hand-side Node
     * @return true, if lhs is less than rhs, false otherwise
     */
    friend bool operator<(const Node &lhs, const Node &rhs);

    /**
     * @brief equal operator for Node objects.
     * Needed to enable associative containers.
     * @param lhs left-hand-side Node
     * @param rhs  left-hand-side Node
     * @return true, if lhs is equal to rhs, false otherwise
     */
    friend bool operator==(const Node &lhs, const Node &rhs);

    /**
     * @brief not equal operator for Node objects.
     * Needed to enable associative containers.
     * @param lhs left-hand-side Node
     * @param rhs  left-hand-side Node
     * @return true, if lhs is not equal rhs, false otherwise
     */
    friend bool operator!=(const Node &lhs, const Node &rhs);

    /**
     * @brief Generic ostream - &lt;&lt; operator for Node.
     *
     * @param os output stream
     * @param node Node object
     * @return std::ostream& the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const Node &node);

    private:
    std::string                       name_;
    std::string                       description_;
    EventValueRange::DistributionType distType_;
    EventValueRange                   range_;
    ProbabilityFunction              *pDistribution_{nullptr};
    ProbabilityFunction              *pAprioriDistribution_{nullptr};
};

/**
 * @brief Directed edge-type between nodes in the Bayes-net.
 */
class Dependency
{
    public:
    /**
     * @brief Default construct with start- and end-node.
     *
     * @param n1 Node that n2 depends on
     * @param n2 dependent Node
     */
    Dependency(std::string n1 = "[no condition]", std::string n2 = "[no event]");

    /**
     * @brief Retrieve the condition part of the dependency
     *
     * @return std::string
     */
    [[nodiscard]] std::string condition() const;

    /**
     * @brief Retrieve the dependent part of the dependency
     *
     * @return std::string
     */
    [[nodiscard]] std::string event() const;

    /**
     * @brief Description of the edge. A->B
     *
     * @return std::string
     */
    [[nodiscard]] std::string name() const;

    /**
     * @brief less operator for depencies.
     * Needed to enable associative containers.
     * @param lhs left-hand-side dependency
     * @param rhs right-hand-side dependency
     * @return true, if lhs is less rhs, false otherwise
     */
    friend bool operator<(const Dependency &lhs, const Dependency &rhs);

    /**
     * @brief equal operator for depencies.
     * Needed to enable associative containers.
     * @param lhs left-hand-side dependency
     * @param rhs right-hand-side dependency
     * @return true, if lhs is equal rhs, false otherwise
     */
    friend bool operator==(const Dependency &lhs, const Dependency &rhs);

    private:
    std::string n1_;  ///< Name of start-node
    std::string n2_;  ///< Name of end-node
};

/**
 * @brief Bayes-net (believe-net).
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
     * @brief Default construct.
     */
    BayesNet();

    /**
     * @brief Reset the Believe net.
     */
    void clear();

    /**
     * @brief Add a node named name with verbose description.
     *
     * @param name name of the Node
     * @param description description of the node
     * @return true, if successful, false otherwise
     */
    bool addNode(const std::string &name, const std::string &description = "");

    /**
     * @brief Add a node named name with a value range and verbose description.
     *
     * @param name name of the Node
     * @param range a value range
     * @param description description of the node
     * @return true, if successful, false otherwise
     */
    bool addNode(const std::string &name, const EventValueRange &range, const std::string &description = "");

    /**
     * @brief Remove node named name from the BN.
     *
     * @param name name of the node
     * @return true, if successful, false otherwise
     */
    bool removeNode(const std::string &name);

    /**
     * @brief Retrieve a node by name from the net (constant).
     *
     * @param name name of the node
     * @return const Node* a pointer to the node, if it exists, nullptr otherwise
     */
    const Node *getNode(const std::string &name) const;

    /**
     * @brief Retrieve a node by name from the net.
     *
     * @param name name of the node
     * @return (non-constant) Node* a pointer to the node, if it exists, nullptr otherwise
     */
    Node *getNode(const std::string &name);

    /**
     * @brief Add a cause/effect connection between two nodes. If the nodes do not yet
     * exist then create them.
     *
     * @param cause cause node
     * @param effect effect node
     * @return true, if successful, false otherwise
     */
    bool addCauseEffect(const std::string &cause, const std::string &effect);

    /**
     * @brief Check whether adding a cause/effect connection created a cycle, which
     * is not permissable.
     *
     * @return true, if the BN-graph has a cycle, false otherwise
     */
    bool hasCycle();

    /**
     * @brief Retrieve the set of nodes connected to a node (in and out edges)
     *
     * @param checkNode the node to check
     * @return NODE_SET a set of all connected nodes
     */
    NODE_SET connectedNodes(const Node &checkNode);

    /**
     * @brief Retrieve the set of all connected to a node (out edges) constant version
     *
     * @param checkNode the node to check
     * @return NODE_SET a set of all connected nodes
     */
    const NODE_SET connectedNodes(const Node &checkNode) const;

    /**
     * @brief Retrieve the set of children nodes connected to a node (out edges)
     *
     * @param checkNode the node to check
     * @return NODE_SET a set of all children nodes
     */
    NODE_SET childrenNodes(const Node &checkNode);

    /**
     * @brief Retrieve the set of children nodes connected to a node (out edges) constant version
     *
     * @param checkNode the node to check
     * @return NODE_SET a set of all children nodes
     */
    const NODE_SET childrenNodes(const Node &checkNode) const;

    /**
     * @brief Retrieve the set of parent nodes connected to a node (in edges)
     *
     * @param checkNode the node to check
     * @return NODE_SET a set of all parent nodes
     */
    NODE_SET parentNodes(const Node &checkNode);

    /**
     * @brief Retrieve the set of parent nodes connected to a node (in edges) constant version
     *
     * @param checkNode the node to check
     * @return NODE_SET a set of all parent nodes
     */
    const NODE_SET parentNodes(const Node &checkNode) const;

    /**
     * @brief Extract a list of node-name from a list of nodes.
     *
     * @param nodes set of nodes
     * @return std::set<std::string> set of node-names
     */
    std::set<std::string> nodes2names(const NODE_SET &nodes) const;

    /**
     * @brief Retrieve the ranges of the incoming nodes of node named name.
     *
     * @param name name of a node to check
     * @return VALUERANGES_TYPE rangs of the incoming nodes
     */
    VALUERANGES_TYPE conditionRanges(const std::string &name) const;

    /**
     * @brief Use a CSVAnalyzer to estimate node distributions of the Bayes net.
     *
     * @param csv csv object
     * @param hasValue
     * @param isDiscrete
     * @return true, if successful, false otherwise
     */
    bool trainWithCsv(const CSVAnalyzer &csv, bool hasValue = false, bool isDiscrete = true);

    /**
     * @brief Use a CSVAnalyzer-file to estimate node distributions of the Bayes net.
     *
     * @param filename filename of a csv
     * @param hasValue
     * @param isDiscrete
     * @return true, if successful, false otherwise
     */
    bool trainWithCsv(const std::string &filename, bool hasValue = false, bool isDiscrete = true);

    /**
     * @brief Make all nodes uniform.
     *
     * @return true, if successful, false otherwise
     */
    bool makeUniform();

    /**
     * @brief Normalise all nodes.
     *
     * @return true, if successful, false otherwise
     */
    bool normalise();

    /**
     * @brief Canonise all nodes,
     *
     * @return true, if successful, false otherwise
     */
    bool canonise();

    /**
     * breadth-first calculation starting with independent Node's
     */

    /**
     * @brief breadth-first calculation starting with independent Node's
     *
     * @return true, if successful, false otherwise
     */
    bool calculateAprioriDistributions();

    /**
     * @brief Make node named name uniform.
     *
     * @param name name of a node
     * @return true, if successful, false otherwise
     */
    bool makeUniform(const std::string &name);

    /**
     * @brief Normalise node named name.
     *
     * @param name name of a node
     * @return true, if successful, false otherwise
     */
    bool normalise(const std::string &name);

    /**
     * @brief Canonise node named name.
     *
     * @param name name of a node
     * @return true, if successful, false otherwise
     */
    bool canonise(const std::string &name);

    /**
     * @brief Check whether the model is fully defined with all distributions present.
     *
     * @return true, if so, false otherwise
     */
    bool fullyDefined() const;

    /**
     * @brief Retrieve a list of node names (breadth-first).
     *
     * @return std::vector<std::string> list of nodenames
     */
    std::vector<std::string> breadthFirstNodeNames() const;

    /**
     * @brief Calculate a conditional event probability using the Bayes net.
     *
     * @param ce conditional event
     * @return long double probability
     */
    long double P(const CondEvent &ce) const;

    /**
     * @brief Calculate conditionally independent sets of nodes (d-separated nodes).
     *
     * @param ce conditional event
     * @param irrelevant [inout] irrelevant events
     * @return CondEvent a conditional event need for calculation of probability
     */
    CondEvent bayesBallAlgorithm(const CondEvent &ce, EventCatenation &irrelevant) const;

    /**
     * @brief Generic ostream - &lt;&lt; operator for BayesNet.
     *
     * @param os output stream
     * @param bn bayes net
     * @return std::ostream& the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const BayesNet &bn);

    protected:
    /**
     * @brief Make node uniform.
     *
     * @param node node to modify
     * @return true, if successful, fast otherwise
     */
    bool makeUniform(Node &node);

    /**
     * @brief Normalise node.
     *
     * @param node node to modify
     * @return true, if successful, fast otherwise
     */
    bool normalise(Node &node);

    /**
     * @brief Canonise node,
     *
     * @param node node to modify
     * @return true, if successful, fast otherwise
     */
    bool canonise(Node &node);

    private:
    DirectedGraph<Node, Dependency> g_;  ///< The underlying graph.
};

};  // namespace util

#endif  // NS_UTIL_BAYESUTIL_H_INCLUDED
