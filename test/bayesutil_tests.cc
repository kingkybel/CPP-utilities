/*
 * File:		bayesutilTest.cc
 * Description: Unit tests for string utilities
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

#include "bayesutil.h"

#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <stringutil.h>
#include <vector>

using namespace std;
using namespace util;

const string filename = "/tmp/test.csv";

class BayesUtilTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(BayesUtilTest, util_bayes_from_csv_test)
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
    ASSERT_TRUE(!bn.fullyDefined());

    const char*    shouldOrder[]     = {"Cloud", "Rain", "Sprinkler", "WetGrass"};
    vector<string> breadthFirstNodes = bn.breadthFirstNodeNames();
    for(size_t i = 0; i != breadthFirstNodes.size(); i++)
    {
        ASSERT_EQ(breadthFirstNodes[i], string(shouldOrder[i]));
    }

    BayesNet::NODE_SET connTo_1 = bn.connectedNodes(Node("Sprinkler"));
    ASSERT_EQ(connTo_1.size(), 2UL);
    ASSERT_TRUE(connTo_1.find(Node("Cloud")) != connTo_1.end());
    ASSERT_TRUE(connTo_1.find(Node("WetGrass")) != connTo_1.end());

    ASSERT_THROW(bn.addCauseEffect("Rain", "Cloud"), parallel_error);
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
    ASSERT_TRUE(bn.fullyDefined());

    // BOOST_TEST_MESSAGE(bn);

    ASSERT_TRUE(bn.P(CondEvent(Event("Cloud", true))) >= 0.0L);
    long double p = bn.P(CondEvent(Event("Cloud", true)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);
    p = bn.P(CondEvent(Event("Cloud", false)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);
    p = bn.P(CondEvent(Event("Rain", "heavy"), Event("Cloud", true)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);
    p = bn.P(CondEvent(Event("Rain", "none"), Event("Cloud", false)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);

    bn.clear();
    connTo_1 = bn.connectedNodes(Node("Sprinkler"));
    ASSERT_EQ(connTo_1.size(), 0UL);
    ASSERT_TRUE(connTo_1.find(Node("Cloud")) == connTo_1.end());
    ASSERT_TRUE(connTo_1.find(Node("WetGrass")) == connTo_1.end());
}

TEST_F(BayesUtilTest, util_bayes_manually_defined_test)
{
    BayesNet bn;

    bn.addNode("Cloud", EventValueRange(true), "Event describing whether there are clouds in the sky or not");
    bn.addNode("Rain", EventValueRange(VAR_UINT(0), 5), "Event describing the amount of rain falling");
    bn.addNode("Sprinkler",
               EventValueRange(VAR_UINT(0), 3),
               "Event describing the what stage the sprinkler is turned up to");
    bn.addNode("WetGrass", EventValueRange(true), "Event describing whether the grass is wet or not");
    bn.addCauseEffect("Cloud", "Rain");
    bn.addCauseEffect("Cloud", "Sprinkler");
    bn.addCauseEffect("Sprinkler", "WetGrass");
    bn.addCauseEffect("Rain", "WetGrass");

    // BOOST_TEST_MESSAGE(bn);
    BayesNet::NODE_SET connNodes = bn.connectedNodes(Node("Sprinkler"));
    ASSERT_EQ(connNodes.size(), 2UL);
    ASSERT_TRUE(connNodes.find(Node("Cloud")) != connNodes.end());
    ASSERT_TRUE(connNodes.find(Node("WetGrass")) != connNodes.end());
    bn.canonise();
    bn.normalise();
    ASSERT_TRUE(bn.fullyDefined());
    // BOOST_TEST_MESSAGE(bn);
    long double p = bn.P(CondEvent(Event("Cloud", true)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);
    p = bn.P(CondEvent(Event("Cloud", false)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);
    p = bn.P(CondEvent(Event("Rain", VAR_UINT(4)), Event("Cloud", true)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);
    p = bn.P(CondEvent(Event("Rain", VAR_UINT(2)), Event("Cloud", false)));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);

    p = bn.P(Event("Rain", VAR_UINT(2)) && Event("Cloud", false) && Event("Sprinkler", VAR_UINT(2))
             && Event("WetGrass", true));
    ASSERT_TRUE(p > 0.0L);
    ASSERT_TRUE(p <= 1.0L);

    EventCatenation irrelevant;
    CondEvent       e = bn.bayesBallAlgorithm(CondEvent(Event("Rain", VAR_UINT(4)), Event("Cloud", true)), irrelevant);
    e = bn.bayesBallAlgorithm(CondEvent(Event("Rain", VAR_UINT(4)), Event("Sprinkler", true)), irrelevant);
}

TEST_F(BayesUtilTest, util_bayes_ball_algorithm_test)
{
    BayesNet bn;
    bn.addNode("X", EventValueRange(true), "");
    bn.addNode("Y", EventValueRange(true), "");
    bn.addNode("Z", EventValueRange(true), "");
    bn.addCauseEffect("X", "Y");
    bn.addCauseEffect("Y", "Z");
    EventCatenation irrelevant;
    CondEvent       ce(Event("Z", true), Event("X", true) && Event("Y", true));
    CondEvent       e = bn.bayesBallAlgorithm(ce, irrelevant);

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
    ASSERT_TRUE(ce.containsCondition("5"));
    e = bn.bayesBallAlgorithm(ce, irrelevant);
}

