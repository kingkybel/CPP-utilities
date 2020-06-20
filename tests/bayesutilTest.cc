/*
 * File:		bayesutilTest.cc
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

#include "bayesutilTest.h"
#include <iostream>
#include <string>
#include <sstream>

#include <map>
#include <vector>
#include <set>
#include <algorithm>

#include <stringutil.h>
#include <bayesutil.h>

using namespace std;
using namespace util;

const string filename = "/tmp/test.csv";

CPPUNIT_TEST_SUITE_REGISTRATION(bayesutilTest);

bayesutilTest::bayesutilTest()
{
}

bayesutilTest::~bayesutilTest()
{
}

void bayesutilTest::setUp()
{
}

void bayesutilTest::tearDown()
{
}


void bayesutilTest::util_bayes_test()
{
    //BOOST_TEST_MESSAGE("");
    //BOOST_TEST_MESSAGE("====== Testing Bayes functions ========");
    {
        BayesNet bn;

        bn.addNode("Cloud", "Event describing whether there are clouds in the sky or not");
        bn.addNode("Cloud", "Event describing whether there are clouds in the sky or not");
        bn.addNode("Rain", "Event describing the amount of rain falling");
        bn.addNode("Sprinkler", "Event describing the what stage the sprinkler is turned up to");
        bn.addNode("WetGrass", "Event describing whether the grass is wet or not");
        bn.addCauseEffect("Cloud", "Rain");
        bn.addCauseEffect("Cloud", "Sprinkler");
        bn.addCauseEffect("Sprinkler", "WetGrass");
        bn.addCauseEffect("Rain", "WetGrass");
        CPPUNIT_ASSERT(!bn.fullyDefined());

        const char* shouldOrder[] = {"Cloud", "Rain", "Sprinkler", "WetGrass"};
        vector<string> breadthFirstNodes = bn.breadthFirstNodeNames();
        for (size_t i = 0; i != breadthFirstNodes.size(); i++)
        {
            CPPUNIT_ASSERT_EQUAL(breadthFirstNodes[i], string(shouldOrder[i]));
        }

        BayesNet::NODE_SET connTo_1 = bn.connectedNodes(Node("Sprinkler"));
        CPPUNIT_ASSERT_EQUAL(connTo_1.size(), 2UL);
        CPPUNIT_ASSERT(connTo_1.find(Node("Cloud")) != connTo_1.end());
        CPPUNIT_ASSERT(connTo_1.find(Node("WetGrass")) != connTo_1.end());

        CPPUNIT_ASSERT_THROW(bn.addCauseEffect("Rain", "Cloud"), parallel_error);
        //BOOST_TEST_MESSAGE(bn);
        {
            // write a sample data file
            std::ofstream ofs(filename.c_str());
            ofs << "Cloud , Rain         , Sprinkler , WetGrass, Prob " << endl;
            ofs << "bool  , string       , Uint      , bool,     float" << endl;
            ofs << "yes   , heavy        , 3         , yes,      0.999" << endl;
            ofs << "yes   , heavy        , 3         , no,       0.1  " << endl;
            ofs << "yes   , heavy        , 2         , yes,      0.9  " << endl;
            ofs << "yes   , heavy        , 2         , no,       0.1  " << endl;
            ofs << "yes   , heavy        , 1         , yes,      0.9  " << endl;
            ofs << "yes   , heavy        , 1         , no,       0.89 " << endl;
            ofs << "yes   , heavy        , 0         , yes,      0.88 " << endl;
            ofs << "yes   , heavy        , 0         , no,       0.101" << endl;
            ofs << "yes   , light        , 3         , yes,      0.79 " << endl;
            ofs << "yes   , light        , 3         , no,       0.2  " << endl;
            ofs << "yes   , light        , 2         , yes,      0.8  " << endl;
            ofs << "yes   , light        , 2         , no,       0.15 " << endl;
            ofs << "yes   , light        , 1         , yes,      0.75 " << endl;
            ofs << "yes   , light        , 1         , no,       0.1  " << endl;
            ofs << "yes   , light        , 0         , yes,      0.1  " << endl;
            ofs << "yes   , light        , 0         , no,       0.01 " << endl;
            ofs << "yes   , none         , 3         , yes,      0.55 " << endl;
            ofs << "yes   , none         , 3         , no,       0.3  " << endl;
            ofs << "yes   , none         , 2         , yes,      0.5  " << endl;
            ofs << "yes   , none         , 2         , no,       0.35 " << endl;
            ofs << "yes   , none         , 1         , yes,      0.4  " << endl;
            ofs << "yes   , none         , 1         , no,       0.5  " << endl;
            ofs << "yes   , none         , 0         , yes,      0.07 " << endl;
            ofs << "yes   , none         , 0         , no,       0.55 " << endl;
            ofs << "no    , heavy        , 3         , yes,      0.6  " << endl;
            ofs << "no    , heavy        , 3         , no,       0.21 " << endl;
            ofs << "no    , heavy        , 2         , yes,      0.55 " << endl;
            ofs << "no    , heavy        , 2         , no,       0.3  " << endl;
            ofs << "no    , heavy        , 1         , yes,      0.45 " << endl;
            ofs << "no    , heavy        , 1         , no,       0.5  " << endl;
            ofs << "no    , heavy        , 0         , yes,      0.45 " << endl;
            ofs << "no    , heavy        , 0         , no,       0.45 " << endl;
            ofs << "no    , light        , 3         , yes,      0.55 " << endl;
            ofs << "no    , light        , 3         , no,       0.5  " << endl;
            ofs << "no    , light        , 2         , yes,      0.65 " << endl;
            ofs << "no    , light        , 2         , no,       0.3  " << endl;
            ofs << "no    , light        , 1         , yes,      0.4  " << endl;
            ofs << "no    , light        , 1         , no,       0.5  " << endl;
            ofs << "no    , light        , 0         , yes,      0.25 " << endl;
            ofs << "no    , light        , 0         , no,       0.4  " << endl;
            ofs << "no    , none         , 3         , yes,      0.4  " << endl;
            ofs << "no    , none         , 1         , yes,      0.1  " << endl;
            ofs << "no    , none         , 1         , no,       0.6  " << endl;
            ofs << "no    , none         , 0         , yes,      0.02 " << endl;
            ofs << "no    , none         , 0         , no,       0.7  " << endl;
        }
        CSVAnalyzer data;
        data.read(filename);
        bn.trainWithCsv(data, true);
        CPPUNIT_ASSERT(bn.fullyDefined());

        //BOOST_TEST_MESSAGE(bn);

        CPPUNIT_ASSERT(bn.P(CondEvent(Event("Cloud", true))) >= 0.0L);
        long double p = bn.P(CondEvent(Event("Cloud", true)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);
        p = bn.P(CondEvent(Event("Cloud", false)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);
        p = bn.P(CondEvent(Event("Rain", "heavy"), Event("Cloud", true)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);
        p = bn.P(CondEvent(Event("Rain", "none"), Event("Cloud", false)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);

        bn.clear();
        connTo_1 = bn.connectedNodes(Node("Sprinkler"));
        CPPUNIT_ASSERT_EQUAL(connTo_1.size(), 0UL);
        CPPUNIT_ASSERT(connTo_1.find(Node("Cloud")) == connTo_1.end());
        CPPUNIT_ASSERT(connTo_1.find(Node("WetGrass")) == connTo_1.end());
    }
    //BOOST_TEST_MESSAGE("====== Creating BayesNet without reading csv ========");
    {
        BayesNet bn;

        bn.addNode("Cloud", EventValueRange(true), "Event describing whether there are clouds in the sky or not");
        bn.addNode("Rain", EventValueRange(VAR_UINT(0), 5), "Event describing the amount of rain falling");
        bn.addNode("Sprinkler", EventValueRange(VAR_UINT(0), 3), "Event describing the what stage the sprinkler is turned up to");
        bn.addNode("WetGrass", EventValueRange(true), "Event describing whether the grass is wet or not");
        bn.addCauseEffect("Cloud", "Rain");
        bn.addCauseEffect("Cloud", "Sprinkler");
        bn.addCauseEffect("Sprinkler", "WetGrass");
        bn.addCauseEffect("Rain", "WetGrass");

        //BOOST_TEST_MESSAGE(bn);
        BayesNet::NODE_SET connNodes = bn.connectedNodes(Node("Sprinkler"));
        CPPUNIT_ASSERT_EQUAL(connNodes.size(), 2UL);
        CPPUNIT_ASSERT(connNodes.find(Node("Cloud")) != connNodes.end());
        CPPUNIT_ASSERT(connNodes.find(Node("WetGrass")) != connNodes.end());
        bn.canonise();
        bn.normalise();
        CPPUNIT_ASSERT(bn.fullyDefined());
        //BOOST_TEST_MESSAGE(bn);
        long double p = bn.P(CondEvent(Event("Cloud", true)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);
        p = bn.P(CondEvent(Event("Cloud", false)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);
        p = bn.P(CondEvent(Event("Rain", VAR_UINT(4)), Event("Cloud", true)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);
        p = bn.P(CondEvent(Event("Rain", VAR_UINT(2)), Event("Cloud", false)));
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);

        p = bn.P(Event("Rain", VAR_UINT(2)) &&
                 Event("Cloud", false) &&
                 Event("Sprinkler", VAR_UINT(2)) &&
                 Event("WetGrass", true)
                 );
        CPPUNIT_ASSERT(p > 0.0L);
        CPPUNIT_ASSERT(p <= 1.0L);

        EventList irrelevant;
        CondEvent
        e = bn.bayesBallAlgorithm(CondEvent(Event("Rain", VAR_UINT(4)),
                                            Event("Cloud", true)),
                                  irrelevant);
        e = bn.bayesBallAlgorithm(CondEvent(Event("Rain", VAR_UINT(4)),
                                            Event("Sprinkler", true)),
                                  irrelevant);
    }
    //BOOST_TEST_MESSAGE("====== BayesNet check BayesBallAlgorithm works ========");
    {
        BayesNet bn;
        bn.addNode("X", EventValueRange(true), "");
        bn.addNode("Y", EventValueRange(true), "");
        bn.addNode("Z", EventValueRange(true), "");
        bn.addCauseEffect("X", "Y");
        bn.addCauseEffect("Y", "Z");
        EventList irrelevant;
        CondEvent ce(Event("Z", true), Event("X", true) && Event("Y", true));
        CondEvent e = bn.bayesBallAlgorithm(ce, irrelevant);

        bn.clear();

        bn.addNode("1", EventValueRange(true), "");
        bn.addNode("2", EventValueRange(true), "");
        bn.addNode("3", EventValueRange(true), "");
        bn.addNode("4", EventValueRange(true), "");
        bn.addNode("5", EventValueRange(true), "");
        bn.addNode("6", EventValueRange(true), "");
        bn.addCauseEffect("1", "2");
        bn.addCauseEffect("3", "2");
        bn.addCauseEffect("3", "6");
        bn.addCauseEffect("5", "6");
        bn.addCauseEffect("5", "4");
        ce = CondEvent(Event("6", true), Event("2", true) && Event("5", true));
        CPPUNIT_ASSERT(ce.containsCondition("5"));
        e = bn.bayesBallAlgorithm(ce, irrelevant);
    }
}
