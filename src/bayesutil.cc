/*
 * File Name:   bayesutil.cc
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

#include <bayesutil.h>

namespace util
{
    using namespace boost;
    using namespace std;

    Node::Node(const string& name,
               const string& description,
               const EventValueRange& range)
    : name_(name)
    , description_(description)
    , distType_(range.type())
    , pDistribution_(0)
    , pAprioriDistribution_(0)
    , range_(range)
    {
    }

    Node::Node(const Node& rhs)
    : name_(rhs.name_)
    , description_(rhs.description_)
    , pDistribution_(0)
    , pAprioriDistribution_(0)
    , distType_(rhs.distType_)
    , range_(rhs.range_)
    {
        if (rhs.pDistribution_ != 0)
            pDistribution_ = rhs.pDistribution_->clone();
        if (rhs.pAprioriDistribution_ != 0)
            pAprioriDistribution_ = rhs.pAprioriDistribution_->clone();
    }

    Node& Node::operator=(const Node& rhs)
    {
        if (&rhs != this)
        {
            name_ = rhs.name_;
            description_ = rhs.description_;
            distType_ = rhs.distType_;
            range_ = rhs.range_;

            delete pDistribution_;
            pDistribution_ = (rhs.pDistribution_ == 0) ? 0 : rhs.pDistribution_->clone();
            pAprioriDistribution_ = (rhs.pAprioriDistribution_ == 0) ? 0 : rhs.pAprioriDistribution_->clone();
        }
        return *this;
    }

    Node::~Node()
    {
        delete pDistribution_;
        delete pAprioriDistribution_;
    }

    const EventValueRange& Node::range() const
    {
        return range_;
    }

    bool Node::addRangeValue(const Var& val)
    {
        bool reval = range_.add(val);
        if (dynamic_cast<DiscreteProbability*> (pDistribution_) != 0)
            dynamic_cast<DiscreteProbability*> (pDistribution_)->
            addValueToEventRange(name(), val);
        if (dynamic_cast<DiscreteProbability*> (pAprioriDistribution_) != 0)
            dynamic_cast<DiscreteProbability*> (pAprioriDistribution_)->
            addValueToEventRange(name(), val);
        return reval;
    }

    ProbabilityFunction* Node::distribution()
    {
        return pDistribution_;
    }

    ProbabilityFunction* Node::aprioriDistribution()
    {
        return pAprioriDistribution_;
    }

    bool Node::hasDistribution() const
    {
        return pDistribution_ != 0 && pDistribution_->isDistribution();
    }

    bool Node::hasAprioriDistribution() const
    {
        return pAprioriDistribution_ && pAprioriDistribution_->isDistribution();
    }

    string Node::setDescription(const string& descr)
    {
        description_ = descr;
        return description_;
    }

    string Node::description() const
    {
        return description_;
    }

    string Node::name() const
    {
        return name_;
    }

    bool Node::train(const CSVAnalyzer& csv, bool hasValue)
    {
        bool reval = !csv.empty();
        delete pDistribution_;
        pDistribution_ = 0;
        if (distType_ == EventValueRange::discrete)
        {
            DiscreteProbability* pDist = new DiscreteProbability();
            pDistribution_ = dynamic_cast<ProbabilityFunction*> (pDist);
        }
        else if (distType_ == EventValueRange::float_uniform)
        {
            UniformFloatFunction* pDist = new UniformFloatFunction();
            pDistribution_ = dynamic_cast<ProbabilityFunction*> (pDist);
        }
        else if (distType_ == EventValueRange::exponential)
        {
            ExponentialFunction* pDist = new ExponentialFunction();
            pDistribution_ = dynamic_cast<ProbabilityFunction*> (pDist);
        }
        else if (distType_ == EventValueRange::gaussian)
        {
            GaussFunction* pDist = new GaussFunction();
            pDistribution_ = dynamic_cast<ProbabilityFunction*> (pDist);
        }
        else
        {
            throw distribution_error("Distribution-type '" + asString(distType_) + "' not implemented.");
        }

        reval &= pDistribution_->train(csv, hasValue);
        if (reval && distType_)
            range_.setValues(csv.getRange(0));
        return reval;
    }

    bool Node::makeUniform(const VALUERANGES_TYPE& conditionValueRanges)
    {
        bool reval = false;

        if (distType_ != EventValueRange::discrete)
            return reval;

        DiscreteProbability* pDistribution = dynamic_cast<DiscreteProbability*> (distribution());
        if (pDistribution == 0)
        {
            VALUERANGES_TYPE eventValueRanges;
            if (range().empty())
                throw distribution_error(distribution_error::empty_uniform);
            eventValueRanges[name()] = range();
            pDistribution_ = new DiscreteProbability(eventValueRanges,
                                                     conditionValueRanges);
        }
        if (pDistribution != 0)
        {
            reval = pDistribution->makeUniform();
        }

        return reval;
    }

    bool Node::normalise(const VALUERANGES_TYPE& conditionValueRanges)
    {
        bool reval = false;

        if (distType_ != EventValueRange::discrete)
            return reval;

        DiscreteProbability* pDistribution = dynamic_cast<DiscreteProbability*> (distribution());
        if (pDistribution == 0)
        {
            VALUERANGES_TYPE eventValueRanges;
            if (range().empty())
                throw distribution_error(distribution_error::empty_normalise);
            eventValueRanges[name()] = range();
            pDistribution_ = new DiscreteProbability(eventValueRanges,
                                                     conditionValueRanges);
        }
        if (pDistribution != 0)
        {
            reval = pDistribution->normalise();
        }

        return reval;
    }

    bool Node::canonise(const VALUERANGES_TYPE& conditionValueRanges)
    {
        bool reval = false;

        if (distType_ != EventValueRange::discrete)
            return reval;

        DiscreteProbability* pDistribution = dynamic_cast<DiscreteProbability*> (distribution());
        if (pDistribution == 0)
        {
            VALUERANGES_TYPE eventValueRanges;
            if (range().empty())
                throw distribution_error(distribution_error::empty_canonise);
            eventValueRanges[name()] = range();
            pDistribution_ = new DiscreteProbability(eventValueRanges,
                                                     conditionValueRanges);
        }
        if (pDistribution != 0)
        {
            reval = pDistribution->canonise();
        }

        return reval;
    }

    bool Node::calculateAprioriDistribution(const set<Node>& incoming)
    {
        bool reval = false;

        return reval;
    }

    bool operator<(const Node& lhs, const Node& rhs)
    {
        return lhs.name() < rhs.name();
    }

    bool operator==(const Node& lhs, const Node& rhs)
    {
        return lhs.name() == rhs.name();
    }

    bool operator!=(const Node& lhs, const Node& rhs)
    {
        return !(lhs == rhs);
    }

    ostream& operator<<(ostream& os, const Node& node)
    {
        os << node.name() << " : ";
        if (!node.description().empty())
            os << node.description();
        if (!node.range().empty())
            if (node.description().empty())
                os << "range=" << node.range();
            else
                os << " (range=" << node.range() << ")";
        if (node.pDistribution_ != 0)
            os << *((DiscreteProbability*) node.pDistribution_);
        return os;
    }

    // TODO: handle arbitrary CondEvents

    long double Node::P(const CondEvent& ce) const
    {
        if (!hasDistribution())
            return 0.0L;
        return pDistribution_->P(ce);
    }

    Dependency::Dependency(const string& n1, const string& n2)
    : n1_(n1)
    , n2_(n2)
    {
    }

    string Dependency::condition() const
    {
        return n1_;
    }

    string Dependency::event() const
    {
        return n2_;
    }

    string Dependency::name() const
    {
        stringstream ss;
        ss << condition() << "->" << event();
        return ss.str();
    }

    bool operator<(const Dependency& lhs, const Dependency& rhs)
    {
        return lhs.n1_ < rhs.n1_ || (lhs.n1_ == rhs.n1_ && lhs.n2_ < rhs.n2_);
    }

    bool operator==(const Dependency& lhs, const Dependency& rhs)
    {
        return lhs.n1_ == rhs.n1_ && lhs.n2_ == rhs.n2_;
    }

    ostream& operator<<(ostream& os, const Dependency& dep)
    {
        os << dep.name();
        return os;
    }

    BayesNet::BayesNet()
    {
    }

    void BayesNet::clear()
    {
        g_.clear();
    }

    bool BayesNet::addNode(const string& name, const string& description)
    {
        VERTEX_RESULT reval = g_.addNode(Node(name, description));
        return reval.first;
    }

    bool BayesNet::addNode(const string& name,
                           const EventValueRange& range,
                           const string& description)
    {
        VERTEX_RESULT reval = g_.addNode(Node(name, description, range));
        return reval.first;
    }

    bool BayesNet::removeNode(const string& name)
    {
        return g_.removeNode(Node(name));
    }

    const Node* BayesNet::getNode(const string& name) const
    {
        return g_.getNode(Node(name));
    }

    Node* BayesNet::getNode(const string& name)
    {
        return g_.getNode(Node(name));
    }

    bool BayesNet::addCauseEffect(const string& cause, const string& effect)
    {
        return g_.addEdge(Node(cause), Node(effect));
    }

    bool BayesNet::hasCycle()
    {
        return g_.hasCycle();
    }

    BayesNet::NODE_SET BayesNet::connectedNodes(const Node& checkNode)
    {
        return g_.connectedNodes(checkNode);
    }

    const BayesNet::NODE_SET BayesNet::connectedNodes(const Node& checkNode) const
    {
        return g_.connectedNodes(checkNode);
    }

    BayesNet::NODE_SET BayesNet::childrenNodes(const Node& checkNode)
    {
        return g_.childrenNodes(checkNode);
    }

    const BayesNet::NODE_SET BayesNet::childrenNodes(const Node& checkNode) const
    {
        return g_.childrenNodes(checkNode);
    }

    BayesNet::NODE_SET BayesNet::parentNodes(const Node& checkNode)
    {
        return g_.parentNodes(checkNode);
    }

    const BayesNet::NODE_SET BayesNet::parentNodes(const Node& checkNode) const
    {
        return g_.parentNodes(checkNode);
    }

    set<string> BayesNet::nodes2names(const NODE_SET& nodes) const
    {
        set<string> reval;
        for (auto it = nodes.begin(); it != nodes.end(); it++)
            reval.insert(it->name());

        return reval;
    }

    // get the ranges of all conditions

    VALUERANGES_TYPE BayesNet::conditionRanges(const string& name) const
    {
        VALUERANGES_TYPE reval;
        NODE_SET parents = g_.parentNodes(Node(name));
        for (auto it = parents.begin(); it != parents.end(); it++)
        {
            reval[it->name()] = it->range();
        }

        return reval;
    }

    bool BayesNet::trainWithCsv(const CSVAnalyzer& data,
                                bool hasValue,
                                bool isDiscrete)
    {
        bool reval = true;

        NODE_PTR_VECTOR nodes = g_.getNodes();
        for (auto nIt = nodes.begin(); nIt != nodes.end(); nIt++)
        {
            vector<string> subHeaders;
            subHeaders.push_back((*nIt)->name());
            NODE_SET parentSet = parentNodes(*(*nIt));
            set<string> parentNames;
            for (auto pnIt = parentSet.begin(); pnIt != parentSet.end(); pnIt++)
                parentNames.insert(pnIt->name());

            copy(parentNames.begin(), parentNames.end(), back_inserter(subHeaders));
            if (hasValue)
            {
                if (data.type(data.columns() - 1) == "float")
                {
                    subHeaders.push_back(data.header(data.columns() - 1));
                }
                else
                {
                    hasValue = false;
                }
            }
            CSVAnalyzer nodeData = data.getSub(subHeaders);
            reval &= (*nIt)->train(nodeData, hasValue);
        }

        return reval;
    }

    bool BayesNet::trainWithCsv(const string& filename,
                                bool hasValue,
                                bool isDiscrete)
    {
        CSVAnalyzer data;
        bool reval = data.read(filename);
        reval &= trainWithCsv(data, hasValue, isDiscrete);

        return reval;
    }

    bool BayesNet::makeUniform()
    {
        bool reval = true;

        NODE_PTR_VECTOR nodes = g_.getNodes();
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            reval &= makeUniform(*(*it));
        }

        return reval;
    }

    bool BayesNet::normalise()
    {
        bool reval = true;
        NODE_PTR_VECTOR nodes = g_.getNodes();
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            reval &= normalise(*(*it));
        }
        return reval;
    }

    bool BayesNet::canonise()
    {
        bool reval = true;

        NODE_PTR_VECTOR nodes = g_.getNodes();
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            reval &= canonise(*(*it));
        }

        return reval;
    }

    // breadth-first calculation starting with independent Node's

    bool BayesNet::calculateAprioriDistributions()
    {
        bool reval = false;

        return reval;
    }

    bool BayesNet::fullyDefined() const
    {
        bool reval = true;

        NODE_PTR_VECTOR nodes = g_.getNodes();
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            reval &= (*it)->hasDistribution();
        }

        return reval;
    }

    /// Local helper class for recording node-names in breadth-first-order.

    struct BreadthFirstEl
    {
        size_t numParents_;
        string name_;

        BreadthFirstEl(size_t numParents = 0, string name = "")
        : numParents_(numParents)
        , name_(name)
        {
        }

        friend bool operator<(const BreadthFirstEl& lhs, const BreadthFirstEl& rhs)
        {
            return lhs.numParents_ < rhs.numParents_ ||
                    (lhs.numParents_ == rhs.numParents_ && lhs.name_ < rhs.name_);
        }

        friend bool operator==(const BreadthFirstEl& lhs, const BreadthFirstEl& rhs)
        {
            return lhs.numParents_ == rhs.numParents_ && lhs.name_ == rhs.name_;
        }
    };

    vector<string> BayesNet::breadthFirstNodeNames() const
    {
        vector<string> reval;
        set<BreadthFirstEl> sorted;
        NODE_PTR_VECTOR nodes = g_.getNodes();
        for (auto it = nodes.begin(); it != nodes.end(); it++)
        {
            size_t numParents = g_.inDegree(*(*it));
            sorted.insert(BreadthFirstEl(numParents, (*it)->name()));
        }
        for (auto it = sorted.begin(); it != sorted.end(); it++)
            reval.push_back(it->name_);

        return reval;
    }

    bool BayesNet::makeUniform(const string& name)
    {
        bool reval = true;

        Node* found = g_.getNode(Node(name));
        if (found != 0)
        {
            reval &= makeUniform(*found);
        }
        else
            reval = false;

        return reval;
    }

    bool BayesNet::normalise(const string& name)
    {
        bool reval = true;

        Node* found = g_.getNode(Node(name));
        if (found != 0)
        {
            reval &= normalise(*found);
        }
        else
            reval = false;

        return reval;
    }

    bool BayesNet::canonise(const string& name)
    {
        bool reval = true;

        Node* found = g_.getNode(Node(name));
        if (found != 0)
        {
            reval &= canonise(*found);
        }
        else
            reval = false;

        return reval;
    }

    bool BayesNet::makeUniform(Node& node)
    {
        VALUERANGES_TYPE condRanges = conditionRanges(node.name());
        return node.makeUniform(condRanges);
    }

    bool BayesNet::normalise(Node& node)
    {
        VALUERANGES_TYPE condRanges = conditionRanges(node.name());
        return node.normalise(condRanges);
    }

    bool BayesNet::canonise(Node& node)
    {
        VALUERANGES_TYPE condRanges = conditionRanges(node.name());
        return node.canonise(condRanges);
    }

    //
    // (1) Conditional probability definition
    // =====================================
    //
    //           P(A,B)
    // P(A|B) = --------
    //            P(B)
    //
    // (2) Bayes formula
    // =================
    //
    //           P(L|W)P(W)            P(L|W)P(W)
    // P(W|L) = ------------ = -------------------------
    //             P(L)         P(L|W)P(W) + P(L|M)P(M)
    //
    // (3) Chain rule of probability
    // =============================
    //
    // P(A,B) = P(B|A)P(A)
    // P(An,An-1,...,A1)=P(An|An-1,...,A1)P(An-1,...,A1)
    // P(A4,A3,A2,A1)=P(A4|A3,A2,A1)P(A3|A2,A1)P(A2|A1)P(A1)
    //
    // (4) Total probability
    // =====================
    //
    // P(A) = sum(P(A,B_i)) = sum(P(A|B_i)P(B_i)
    //
    // (5) Symmetry
    //
    // P(A | B,C) == P(A | B) [A independent of C] <==>
    // P(B | A,C) == P(B | A) [B independent of C]
    //

    long double BayesNet::P(const CondEvent& ce) const
    {
        long double reval = 0.0L;
        //

        if (ce.eventSize() == 1)
        {
            // find the correct node
            string nodeName = ce.event().cbegin()->name();
            const Node* pNode = getNode(nodeName);
            if (pNode == 0)
                return 0.0L;

            // remove all events from the condition that node is independent of
            CondEvent filtered = ce.filterConditions(nodes2names(parentNodes(Node(nodeName))));
            // TODO: this should use the Bayes Ball Algorithm
            // TODO: if not all conditions are present - build sum

            // and return the value from the distribution table
            return pNode->P(filtered);
        }
        else if (ce.eventSize() > 1)
        {
            CondEvent::CONDEVENT_LIST productElements;
            ce.chainRule(productElements, breadthFirstNodeNames());
            reval = 1.0L;
            for (auto it = productElements.begin();
                 it != productElements.end();
                 it++)
            {
                reval *= P(*it);
            }
        }

        return reval;
    }

    // Bayes-Ball Algorithm
    // ====================
    //
    // http://mlg.eng.cam.ac.uk/zoubin/course03/BayesBall.pdf
    // Engineering-Economic Systems and Operations Research Dept.
    // Stanford University
    // Stanford, CA 94305-4023
    // shachter@stanford.edu
    //
    // The Bayes Ball Algorithm determines the relevant nodes that need to be
    // considered given a conditional probability expression
    // It returns conditional independent nodes, irrelevant nodes and requisite
    // probability nodes
    // Answers questions like: P(x,z|y) When we condition on y, are x and z
    // independent?
    //
    // A structured belief network B = (N;A;F) consists of nodes N and directed
    // arcs A which together form a directed acyclic graph G(N;A), and a subset
    // F of the nodes that are deterministically (functionally) related to their
    // parents (that means: if I know the parents, then I can functionally
    // determine the value).
    // The algorithm explores a structured belief network B = (N; A; F) with
    // respect to the expression
    // P(X_J |X_K) and constructs the sets of relevant and requisite nodes.
    //
    // 1. Initialise all nodes as neither visited, nor marked on the top, nor
    //    marked on the bottom.
    // 2. Create a schedule of nodes to be visited, initialised with each node
    //    in J to be visited as if from one of its children. (all nodes in J)
    // 3. While there are still nodes scheduled to be visited:
    //    a. Pick any node j scheduled to be visited and remove it from the
    //       schedule. Either j was scheduled for a visit from a parent, a
    //       visit from a child, or both.
    //    b. Mark j as visited.
    //    c. If j not in K and the visit to j is from a child:
    //       i.	 if the top of j is not marked, then mark its top and
    //           schedule each of its parents to be visited
    //       ii. if j not in F and the bottom of j is not marked, then mark
    //           its bottom and schedule each of its children to be visited.
    //    d. If the visit to j is from a parent:
    //        i.  If j in K and the top of j is not marked, then mark its top
    //            and schedule each of its parents to be visited;
    //        ii. if j not in K and the bottom of j is not marked, then mark
    //            its bottom and schedule each of its children to be visited.
    // 4. The irrelevant nodes, Ni(J|K), are those nodes not marked on the bottom.
    // 5. The requisite probability nodes, Np(J|K), are those nodes marked on top.
    // 6. The requisite observation nodes, Ne(J|K), are those nodes in K marked
    //    as visited.

    struct schedNode
    {

        enum SchedType
        {
            byParent = 0x01, byChild = 0x02
        };

        explicit schedNode(SchedType type = byChild)
        : type_(type)
        , visited_(false)
        , top_(false)
        , bottom_(false)
        {
        }

        bool fromChild() const
        {
            return (type_ & byChild) == byChild;
        }

        bool fromParent() const
        {
            return (type_ & byParent) == byParent;
        }
        int type_;
        bool visited_;
        bool top_;
        bool bottom_;
    };

    CondEvent BayesNet::bayesBallAlgorithm(const CondEvent& ce,
                                           EventList& irrelevant) const
    {
        typedef map<string, schedNode> SCHEDULE_TYPE;
        SCHEDULE_TYPE sched;

        // 1. Initialise all nodes as neither visited, nor marked on the top, nor
        //    marked on the bottom.
        // 2. Create a schedule of nodes to be visited, initialised with each node
        //    in J to be visited as if from one of its children. (all nodes in J)
        deque<string> theQueue;
        for (auto eIt = ce.event().cbegin();
             eIt != ce.event().cend();
             eIt++)
        {
            sched[eIt->name()] = schedNode(schedNode::byChild);
            theQueue.push_back(eIt->name());
        }

        // 3. While there are still nodes scheduled to be visited:
        while (theQueue.size() > 0)
        {
            // a. Pick any node j scheduled to be visited and remove it from the
            //    schedule. Either j was scheduled for a visit from a parent, a
            //    visit from a child, or both.
            string name = theQueue.front();
            theQueue.pop_front();

            // b. Mark j as visited.
            sched[name].visited_ = true;

            // c. If j not in K and the visit to j is from a child:
            if (!ce.containsCondition(name) && sched[name].fromChild())
            {
                // i. if the top of j is not marked, then mark its top and
                //    schedule each of its parents to be visited
                if (!sched[name].top_)
                {
                    sched[name].top_ = true;
                    set<string> parents = nodes2names(parentNodes(Node(name)));
                    for (auto sIt = parents.begin();
                         sIt != parents.end();
                         sIt++)
                    {
                        sched[*sIt].type_ |= schedNode::byChild;
                        theQueue.push_back(*sIt);
                    }
                }
                // ii. if j not in F and the bottom of j is not marked, then mark
                //     its bottom and schedule each of its children to be visited.
                //     (we do not have F here!)
                if (!sched[name].bottom_)
                {
                    sched[name].bottom_ = true;
                    set<string> children = nodes2names(childrenNodes(Node(name)));
                    for (auto sIt = children.begin();
                         sIt != children.end();
                         sIt++)
                    {
                        sched[*sIt].type_ |= schedNode::byParent;
                        theQueue.push_back(*sIt);
                    }
                }
            }
            // d. If the visit to j is from a parent:
            if (sched[name].fromParent())
            {
                // i. If j in K and the top of j is not marked, then mark its top
                //    and schedule each of its parents to be visited;
                if (ce.containsCondition(name) && !sched[name].top_)
                {
                    sched[name].top_ = true;
                    set<string> parents = nodes2names(parentNodes(Node(name)));
                    for (auto sIt = parents.begin();
                         sIt != parents.end();
                         sIt++)
                    {
                        sched[*sIt].type_ |= schedNode::byChild;
                        theQueue.push_back(*sIt);
                    }
                }
                // ii. if j not in K and the bottom of j is not marked, then mark
                //     its bottom and schedule each of its children to be visited.
                if (!ce.containsCondition(name) && !sched[name].bottom_)
                {
                    sched[name].bottom_ = true;
                    set<string> children = nodes2names(childrenNodes(Node(name)));
                    for (auto sIt = children.begin();
                         sIt != children.end();
                         sIt++)
                    {
                        sched[*sIt].type_ |= schedNode::byParent;
                        theQueue.push_back(*sIt);
                    }
                }
            }
        }
        // blat out irrelevant list - just in case
        irrelevant = EventList();
        EventList requisiteEvents;
        EventList requisiteConditions;
        for (auto it = sched.begin(); it != sched.end(); it++)
        {
            Event evt = ce.event().eventByName(it->first);
            Event cond = ce.condition().eventByName(it->first);
            EventList el = evt;
            if (el.empty() && !cond.empty())
                el && cond;
            if (el.empty())
                el && Event::placeholderEvent(it->first);
            // 4. The irrelevant nodes, Ni(J|K), are those nodes not marked on
            //    the bottom.
            if (!it->second.bottom_)
            {
                irrelevant && el;
            }
                // 5. The requisite probability nodes, Np(J|K), are those nodes
                //    marked on top.
            else if (it->second.top_)
            {
                requisiteEvents && el;
            }
                // 6. The requisite observation nodes, Ne(J|K), are those nodes in
                //    K marked as visited.
            else if (it->second.visited_)
            {
                requisiteConditions && el;
            }
        }

        return CondEvent(requisiteEvents, requisiteConditions);
    }

    ostream& operator<<(ostream & os, const BayesNet& bn)
    {
        os << bn.g_;
        return os;
    }

}; // namespace util

