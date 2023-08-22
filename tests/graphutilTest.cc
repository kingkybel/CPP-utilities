/*
 * File:		graphutilTest.cc
 * Description:         Unit tests for string utilities
 *
 * Copyright (C) 2020 Dieter J Kybelksties <github@kybelksties.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @date: 2020-06-17
 * @author: Dieter J Kybelksties
 */

#include "graphutilTest.h"

#include <boost/assign/std/vector.hpp>
#include <boost/bind.hpp>
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS
#include <anyutil.h>
#include <bayesutil.h>
#include <cmath>
#include <cstdlib>
#include <csvutil.h>
#include <dateutil.h>
#include <graphutil.h>
#include <iostream>
#include <statutil.h>
#include <string>
#include <stringutil.h>

using namespace std;
using namespace util;
using namespace util::datescan;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::assign;
using namespace boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION(graphutilTest);

graphutilTest::graphutilTest()
{
}

graphutilTest::~graphutilTest()
{
}

void graphutilTest::setUp()
{
}

void graphutilTest::tearDown()
{
}

void graphutilTest::util_graph_test()
{
    // BOOST_TEST_MESSAGE("");
    // BOOST_TEST_MESSAGE("====== Testing graph functions ========");
    {
        DirectedGraph<PODNode<string>, string> g(false, false);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        CPPUNIT_ASSERT_EQUAL(g.addNode(A).first, g.addNode(A).first);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "first"), true);
        CPPUNIT_ASSERT_THROW(g.addEdge(B, C, "first"), parallel_error);
        CPPUNIT_ASSERT_THROW(g.addEdge(B, C, "second"), parallel_error);
        CPPUNIT_ASSERT_THROW(g.addEdge(C, B, "third"), parallel_error);
        g.removeNode(B);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        CPPUNIT_ASSERT_THROW(g.addEdge(A, A, "zeroth"), circle_error);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(A, B, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "second"), true);
        CPPUNIT_ASSERT_THROW(g.addEdge(C, A, "third"), circle_error);

        CPPUNIT_ASSERT(nullptr == g.getEdge(A, A));
        CPPUNIT_ASSERT_EQUAL(*(g.getEdge(A, B)), string("first"));
        CPPUNIT_ASSERT_EQUAL(*(g.getEdge(B, C)), string("second"));
        CPPUNIT_ASSERT(nullptr == g.getEdge(C, A));
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for(auto it = ev.begin(); it != ev.end(); it++)
        {
            // BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");
        g.removeEdge(B, C);
        CPPUNIT_ASSERT(nullptr == g.getEdge(B, C));
    }
    {
        DirectedGraph<PODNode<string>, string> g(false, true);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        CPPUNIT_ASSERT_EQUAL(g.addNode(A).first, g.addNode(A).first);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "second"), true);
        CPPUNIT_ASSERT_THROW(g.addEdge(C, B, "third"), circle_error);
        g.removeNode(B);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        CPPUNIT_ASSERT_THROW(g.addEdge(A, A, "zeroth"), circle_error);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(A, B, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "second"), true);
        CPPUNIT_ASSERT_THROW(g.addEdge(C, A, "third"), circle_error);

        CPPUNIT_ASSERT(nullptr == g.getEdge(A, A));
        CPPUNIT_ASSERT_EQUAL(*(g.getEdge(A, B)), string("first"));
        CPPUNIT_ASSERT_EQUAL(*(g.getEdge(B, C)), string("second"));
        CPPUNIT_ASSERT(nullptr == g.getEdge(C, A));
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for(auto it = ev.begin(); it != ev.end(); it++)
        {
            // BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");

        g.removeEdge(B, C);
        CPPUNIT_ASSERT(nullptr == g.getEdge(B, C));
    }
    {
        DirectedGraph<PODNode<string>, string> g(true, false);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        CPPUNIT_ASSERT_EQUAL(g.addNode(A).first, g.addNode(A).first);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "first"), true);
        CPPUNIT_ASSERT_THROW(g.addEdge(B, C, "first"), parallel_error);
        CPPUNIT_ASSERT_THROW(g.addEdge(B, C, "second"), parallel_error);
        CPPUNIT_ASSERT_THROW(g.addEdge(C, B, "third"), parallel_error);
        g.removeNode(B);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        CPPUNIT_ASSERT_EQUAL(g.addEdge(A, A, "zeroth"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(A, B, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "second"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(C, A, "third"), true);

        CPPUNIT_ASSERT_EQUAL(*g.getEdge(A, A), string("zeroth"));
        CPPUNIT_ASSERT_EQUAL(*(g.getEdge(A, B)), string("first"));
        CPPUNIT_ASSERT_EQUAL(*(g.getEdge(B, C)), string("second"));
        CPPUNIT_ASSERT_EQUAL(*g.getEdge(C, A), string("third"));
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for(auto it = ev.begin(); it != ev.end(); it++)
        {
            // BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");
        g.removeEdge(B, C);
        CPPUNIT_ASSERT(nullptr == g.getEdge(B, C));
    }
    {
        DirectedGraph<PODNode<string>, string> g(true, true);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        CPPUNIT_ASSERT_EQUAL(g.addNode(A).first, g.addNode(A).first);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "second"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(C, B, "third"), true);

        g.removeNode(B);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        CPPUNIT_ASSERT_EQUAL(g.addEdge(A, A, "zeroth"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(A, B, "first"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(B, C, "second"), true);
        CPPUNIT_ASSERT_EQUAL(g.addEdge(C, A, "third"), true);

        CPPUNIT_ASSERT_EQUAL(*g.getEdge(A, A), string("zeroth"));
        CPPUNIT_ASSERT_EQUAL(*g.getEdge(A, B), string("first"));
        CPPUNIT_ASSERT_EQUAL(*g.getEdge(B, C), string("second"));
        CPPUNIT_ASSERT_EQUAL(*g.getEdge(C, A), string("third"));
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for(auto it = ev.begin(); it != ev.end(); it++)
        {
            // BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");

        g.removeEdge(B, C);
        CPPUNIT_ASSERT(nullptr == g.getEdge(B, C));
    }
}

struct AlgoNode : public NodeBase
{
    AlgoNode(const string &name = "") : name_(name)
    {
    }

    size_t hash() const
    {
        std::hash<string> hasher;
        return hasher(name_);
    }

    friend bool operator<(const AlgoNode &lhs, const AlgoNode &rhs)
    {
        return lhs.name_ < rhs.name_;
    }

    friend bool operator==(const AlgoNode &lhs, const AlgoNode &rhs)
    {
        return lhs.name_ == rhs.name_;
    }

    friend bool operator!=(const AlgoNode &lhs, const AlgoNode &rhs)
    {
        return lhs.name_ != rhs.name_;
    }

    friend ostream &operator<<(ostream &os, const AlgoNode &n)
    {
        os << n.name_;
        return os;
    }
    string name_;
};

struct dfVis : public boost::dfs_visitor<>
{
    dfVis(vector<AlgoNode> &v) : visitList(v)
    {
    }
    vector<AlgoNode> &visitList;

    template<class Vertex, class Graph>
    void discover_vertex(Vertex v, Graph &g)
    {
        visitList.push_back(g[v]);
    }
};

struct bfVis : public boost::bfs_visitor<>
{
    bfVis(vector<AlgoNode> &v) : visitList(v)
    {
    }
    vector<AlgoNode> &visitList;

    template<class Vertex, class Graph>
    void discover_vertex(Vertex v, Graph &g)
    {
        visitList.push_back(g[v]);
    }
};

void graphutilTest::util_graph_algo_test()
{
    // BOOST_TEST_MESSAGE("");
    // BOOST_TEST_MESSAGE("====== Testing graph algorithm functions ========");

    DirectedGraph<AlgoNode> g;

    /*
              a   h
             /|\ / \     
            / | c   \ 
           /  |/ \   \ 
          b   f   d  /
           \ /|  /  /
            g | /  /
             \|/  /
              e__/
    */
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("a"), AlgoNode("b"), "01"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("a"), AlgoNode("c"), "02"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("a"), AlgoNode("f"), "03"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("h"), AlgoNode("c"), "04"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("h"), AlgoNode("e"), "05"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("b"), AlgoNode("g"), "06"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("c"), AlgoNode("d"), "07"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("c"), AlgoNode("f"), "08"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("d"), AlgoNode("e"), "09"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("f"), AlgoNode("e"), "10"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("f"), AlgoNode("g"), "11"), true);
    CPPUNIT_ASSERT_EQUAL(g.addEdge(AlgoNode("g"), AlgoNode("e"), "12"), true);

    vector<AlgoNode> result;
    dfVis            vis(result);
    g.applyDepthFirst(vis);
}
