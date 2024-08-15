/*
 * Repository:  https://github.com/kingkybel/CPP-utilities
 * File Name:   test/statutilTest.cc
 * Description: Unit tests for statistics utilities
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

#include <boost/assign/std/vector.hpp>
#include <boost/bind.hpp>
#include <gtest/gtest.h>
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

const string filename = "/tmp/test.csv";

using namespace std;
using namespace util;

class StatUtilTest : public ::testing::Test
{
    protected:
    void SetUp() override
    {
        initDateFormats();
    }

    void TearDown() override
    {
    }
};

TEST_F(StatUtilTest, util_event_catenation_test)
{
    // "Two different EventLists of size 1");
    EventCatenation el1 = Event("E1", true);
    EventCatenation el2 = Event("E3", false);
    ASSERT_TRUE(el1 < el2);

    // "Two *IDENTICAL* EventLists of size 1");
    el1 = Event("E1", true);
    el2 = Event("E1", true);
    ASSERT_FALSE((el1 < el2));
    ASSERT_TRUE(Event("E1", true).notConflicting(Event("E1", true)));
    ASSERT_FALSE(Event("E1", true).notConflicting(Event("E1", false)));
    ASSERT_TRUE(Event("E1", true).notConflicting(Event("E2", true)));
    ASSERT_TRUE(Event("E1", true).notConflicting(Event("E2", false)));

    // "Two EventLists of different size where one is front of other");
    el1 = Event("E1", true);
    el2 = Event("E1", true) && Event("E2", false);
    ASSERT_TRUE(el1 < el2);
    ASSERT_FALSE((el2 < el1));
    ASSERT_TRUE(el1.notConflicting(el1));
    ASSERT_TRUE(el2.notConflicting(el2));
    ASSERT_TRUE(el2.notConflicting(el1));
    ASSERT_TRUE(el1.notConflicting(el2));

    // "Two EventLists *IDENTICAL* to a certain size then different, but same length");
    el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false);
    el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
    ASSERT_TRUE(el1 < el2);
    ASSERT_FALSE((el2 < el1));
    el1 = Event("E1", true) && Event("E2", false) && Event("E5", false) && Event("E4", false);
    el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
    ASSERT_TRUE(el2 < el1);
    ASSERT_FALSE((el1 < el2));

    // "Two EventLists *IDENTICAL* to a certain size then different, different length");
    el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false) && Event("E6", false);
    el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
    ASSERT_TRUE(el1 < el2);
    ASSERT_FALSE((el2 < el1));
    el1 = Event("E1", true) && Event("E2", false) && Event("E5", false) && Event("E4", false);
    el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
    ASSERT_TRUE(el2 < el1);
    ASSERT_FALSE((el1 < el2));
    ASSERT_TRUE(el1.notConflicting(el1));
    ASSERT_TRUE(el2.notConflicting(el2));
    ASSERT_TRUE(el2.notConflicting(el1));
    ASSERT_TRUE(el1.notConflicting(el2));

    el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false) && Event("E6", false);
    el2 = Event("E1", false) && Event("E2", false) && Event("E3", true) && Event("E4", false);
    ASSERT_TRUE(el1.notConflicting(el1));
    ASSERT_TRUE(el2.notConflicting(el2));
    ASSERT_FALSE(el2.notConflicting(el1));
    ASSERT_FALSE(el1.notConflicting(el2));
}

TEST_F(StatUtilTest, util_event_eq_and_le_test)
{
    // "Check Events/EventLists creation");
    EventCatenation              el1 = Event("E1", true) && Event("E2", VAR_STRING("fdsa"));
    EventCatenation              el2 = Event("E3", false) && Event("E4", VAR_STRING("dfsg"));
    map<EventCatenation, string> elMap;
    elMap[el1] = VAR_STRING("1st");
    elMap[el2] = VAR_STRING("2nd");

    // "Check Events/EventLists == and < operators");
    ASSERT_EQ(el1, Event("E1", true) && Event("E2", VAR_STRING("fdsa")));
    ASSERT_TRUE(el1 < el2);
    ASSERT_EQ(el2, Event("E3", false) && Event("E4", VAR_STRING("dfsg")));
    ASSERT_EQ(el1, el1);
    ASSERT_EQ(el2, el2);

    ASSERT_TRUE(elMap.find(el1) != elMap.end());
    ASSERT_TRUE(elMap.find(el2) != elMap.end());
    ASSERT_EQ(elMap[el1], VAR_STRING("1st"));
    ASSERT_EQ(elMap[el2], VAR_STRING("2nd"));
}

TEST_F(StatUtilTest, util_accu_map)
{
    ACCUMULATION_MAP accMap;
    EventCatenation  el1 = Event("E1", true) && Event("E2", VAR_STRING("fdsa"));
    EventCatenation  el2 = Event("E3", false) && Event("E4", VAR_STRING("dfsg"));
    accMap[el1]          = ACCUMULATION_DATA(0, 0);
}

TEST_F(StatUtilTest, util_creation_with_empty_event)
{
    Event e;
    ASSERT_TRUE(e.empty());
    e("SomeName", true);
    ASSERT_FALSE(e.empty());
}

TEST_F(StatUtilTest, util_condition_events_test)
{
    // "Check CondEvents");
    Event           e;
    EventCatenation el(e);  // empty event is not added so results in empty list
    ASSERT_TRUE(el.empty());

    el = Event("SomeName", true);  // list is initialised with one element
    ASSERT_FALSE(el.empty());
    el&& Event("SomeMore", (VAR_FLOAT)3.14159365) && Event("EvenSomeMore", VAR_STRING("XXX"));
    ASSERT_FALSE(el.empty());

    CondEvent c = el;
    ASSERT_FALSE(c.empty());
    ASSERT_EQ(c.eventSize(), 3UL);
    ASSERT_EQ(c.event(), el);
    ASSERT_EQ(c.conditionSize(), 0UL);

    CondEvent c2(el, el);
    ASSERT_FALSE(c2.empty());

    ASSERT_EQ(c2.eventSize(), 3UL);
    ASSERT_EQ(c2.event(), el);
    ASSERT_EQ(c2.conditionSize(), 3UL);
    ASSERT_EQ(c2.condition(), el);

    // "Check CondEvents creation by csv");

    CSVAnalyzer csv("FEvent, BCond, CCond, Value", "f,b,c,f");
    csv << " 1.0, yes,  a, 0.5";
    csv << " 5.0, No,   B, 2.5";

    CondEvent c3(csv, 0, 0, true);
    ASSERT_TRUE(c3.event() == (Event("FEvent", 1.0L)));
    ASSERT_EQ(c3.condition(), (Event("BCond", true) && Event("CCond", 'a')));
    CondEvent c4(csv, 1, 1, true);
    ASSERT_EQ(c4.event(), (Event("FEvent", 5.0L) && Event("BCond", false)));
    ASSERT_TRUE(c4.condition() == (Event("CCond", 'B')));

    ASSERT_THROW(CondEvent(Event("E1", true) && Event("E1", false)), eventlist_conflict_error);
}

TEST_F(StatUtilTest, util_condition_event_manip_test)
{
    // "Check CondEvent - manipulation");
    CondEvent                 ce(Event("E1", true) && Event("E2", true));
    CondEvent::CONDEVENT_LIST l;
    ASSERT_TRUE(ce.chainRule(l, "E1"));

    ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);
    vector<string> order;
    order.push_back("E2");
    order.push_back("E1");
    order.push_back("E4");
    order.push_back("E3");
    ce.chainRule(l, order);
    for(CondEvent::CONDEVENT_LIST_CITER it = l.begin(); it != l.end(); it++)
    {
        ASSERT_EQ(it->eventSize(), 1UL);
    }

    set<string> strSet;
    strSet.insert("E5");
    strSet.insert("E6");
    ce = ce.filterConditions(strSet);
    // "Check needed events added as place-holders");
    ASSERT_TRUE(ce.hasEvent("E1"));
    ASSERT_TRUE(ce.hasEvent("E2"));
    ASSERT_TRUE(ce.hasEvent("E3"));
    ASSERT_TRUE(ce.hasEvent("E4"));
    ASSERT_TRUE(ce.hasCondition("E5"));
    ASSERT_TRUE(ce.hasCondition("E6"));

    strSet.insert("E1");
    strSet.insert("E4");
    // "Check that filtering does not create conflicts");
    ASSERT_THROW(ce.filterConditions(strSet), eventlist_conflict_error);

    // "Check that removes unneeded conditions");
    ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);

    strSet.clear();
    strSet.insert("E3");
    strSet.insert("E5");
    ce = Event("E1", true) || (Event("E3", true) && Event("E4", true) && Event("E5", true));
    ce = ce.filterConditions(strSet);
    ASSERT_TRUE(ce.hasEvent("E1"));
    ASSERT_TRUE(ce.hasCondition("E3"));
    ASSERT_FALSE(ce.hasCondition("E4"));
    ASSERT_TRUE(ce.hasCondition("E5"));

    ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);
    ce.chainRule(l, order);
    for(CondEvent::CONDEVENT_LIST_CITER it = l.begin(); it != l.end(); it++)
    {
        ASSERT_EQ(it->eventSize(), 1UL);
    }
}

TEST_F(StatUtilTest, util_event_date_matcher_test)
{
    // "Two EventLists of size 1");
    // "match to interval");
    {
        Event itv_20140203_20150203 = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), toDate(2015, 2, 3)));
        ASSERT_TRUE(Event("E1", toDate(2014, 2, 3)).matches(itv_20140203_20150203));
        ASSERT_TRUE(Event("E1", toDate(2015, 2, 3)).matches(itv_20140203_20150203));
        ASSERT_TRUE(Event("E1", toDate(2014, 3, 3)).matches(itv_20140203_20150203));
        ASSERT_FALSE(Event("E1", toDate(2014, 2, 2)).matches(itv_20140203_20150203));
        ASSERT_FALSE(Event("E1", toDate(2015, 2, 4)).matches(itv_20140203_20150203));
    }
    {
        // comparator1 = [2014-02-03, oo)

        Event itv_20140203_oo = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), {finiteMin}));
        ASSERT_TRUE(Event("E1", toDate(2014, 2, 3)).matches(itv_20140203_oo));
        ASSERT_TRUE(Event("E1", toDate(2015, 2, 3)).matches(itv_20140203_oo));
        ASSERT_TRUE(Event("E1", toDate(2014, 3, 3)).matches(itv_20140203_oo));
        ASSERT_FALSE(Event("E1", toDate(2014, 2, 2)).matches(itv_20140203_oo));  // outside the interval
        ASSERT_TRUE(Event("E1", toDate(2015, 2, 2)).matches(itv_20140203_oo));
    }
    {
        Event itv_oo_20140203 = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), {infiniteMin, rightClosed}));
        ASSERT_TRUE(Event("E1", toDate(2014, 2, 3)).matches(itv_oo_20140203));
        ASSERT_FALSE(Event("E1", toDate(2015, 2, 3)).matches(itv_oo_20140203));
        ASSERT_FALSE(Event("E1", toDate(2014, 3, 3)).matches(itv_oo_20140203));
        ASSERT_TRUE(Event("E1", toDate(2014, 2, 2)).matches(itv_oo_20140203));
        ASSERT_FALSE(Event("E1", toDate(2015, 2, 4)).matches(itv_oo_20140203));
    }
}

TEST_F(StatUtilTest, util_event_POD_matcher_test)
{
    // "match bool with less-operator";
    Event bool_lt_false = Event("E2", VAR_BOOL(false), &Event::less);
    // are not less than false: {false, true}
    ASSERT_FALSE(Event("E2", VAR_BOOL(false)).matches(bool_lt_false));
    ASSERT_FALSE(Event("E2", VAR_BOOL(true)).matches(bool_lt_false));
    // are not less than 11: {11, 12}
    Event bool_lt_true = Event("E2", VAR_BOOL(true), &Event::less);
    ASSERT_TRUE(Event("E2", VAR_BOOL(false)).matches(bool_lt_true));
    ASSERT_FALSE(Event("E2", VAR_BOOL(true)).matches(bool_lt_true));
    // --------

    // "match int with less-operator";
    Event comparator_lt = Event("E2", VAR_INT(11), &Event::less);
    // are less than 11: {4, 10}
    ASSERT_TRUE(Event("E2", VAR_INT(4)).matches(comparator_lt));
    ASSERT_TRUE(Event("E2", VAR_INT(10)).matches(comparator_lt));
    // are not less than 11: {11, 12}
    ASSERT_FALSE(Event("E2", VAR_INT(11)).matches(comparator_lt));
    ASSERT_FALSE(Event("E2", VAR_INT(12)).matches(comparator_lt));
    // --------

    // "match unsigned int with  lessEqual-operator";
    Event comparator_le = Event("E3", VAR_UINT(11), &Event::lessEqual);
    // are less or equal to 11: {4, 10, 11}
    ASSERT_TRUE(Event("E3", VAR_UINT(4)).matches(comparator_le));
    ASSERT_TRUE(Event("E3", VAR_UINT(10)).matches(comparator_le));
    ASSERT_TRUE(Event("E3", VAR_UINT(11)).matches(comparator_le));
    // are not less or equal to 11: {12, 113}
    ASSERT_FALSE(Event("E3", VAR_UINT(12)).matches(comparator_le));
    ASSERT_FALSE(Event("E3", VAR_UINT(113)).matches(comparator_le));
    // --------

    // "match char with greater-operator";
    Event comparator_gt = Event("E4", VAR_CHAR('t'), &Event::greater);
    // are not greater to 't': {'a', 't'}
    ASSERT_FALSE(Event("E4", VAR_CHAR('a')).matches(comparator_gt));
    ASSERT_FALSE(Event("E4", VAR_CHAR('t')).matches(comparator_gt));
    // are greater than 't': {'u', 'z'}
    ASSERT_TRUE(Event("E4", VAR_CHAR('u')).matches(comparator_gt));
    ASSERT_TRUE(Event("E4", VAR_CHAR('z')).matches(comparator_gt));
    // --------

    // "match string with greaterEqual-operator";
    Event comparator_ge = Event("E5", VAR_STRING("dieter"), &Event::greaterEqual);
    // are greater or equal to "dieter": {"dieter", "freedom"}
    ASSERT_TRUE(Event("E5", VAR_STRING("dieter")).matches(comparator_ge));
    ASSERT_TRUE(Event("E5", VAR_STRING("freedom")).matches(comparator_ge));
    // are not greater or equal to "dieter": {"diet", "angry"}
    ASSERT_FALSE(Event("E5", VAR_STRING("diet")).matches(comparator_ge));  // "diet" not >= "dieter"
    ASSERT_FALSE(Event("E5", VAR_STRING("angry")).matches(comparator_ge));
}

TEST_F(StatUtilTest, util_eventlist_matcher_test)
{
    // "Two EventLists of equal size >1";
    // "match to interval";

    EventCatenation el1;  // E1 in [2014-02-03..2015-02-03], E2 < 11 , E3 >= "dieter"
    el1&&           Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), toDate(2015, 2, 3)));
    el1&&           Event("E2", VAR_INT(11), &Event::less);
    el1&&           Event("E3", VAR_STRING("dieter"), &Event::greaterEqual);

    // match only if *ALL* events are matching
    EventCatenation el2;
    el2 = Event("E1", toDate(2014, 2, 3));
    el2&& Event("E2", VAR_INT(10));
    el2&& Event("E3", VAR_STRING("dieter"));
    ASSERT_TRUE(el2.matches(el1));

    el2 = Event("E1", toDate(2015, 2, 3));
    el2&& Event("E2", VAR_INT(-5));
    el2&& Event("E3", VAR_STRING("freedom"));
    ASSERT_TRUE(el2.matches(el1));

    el2 = Event("E1", toDate(2013, 2, 3));
    el2&& Event("E2", VAR_INT(-5));
    el2&& Event("E3", VAR_STRING("freedom"));
    ASSERT_FALSE(el2.matches(el1));

    el2 = Event("E1", toDate(2014, 2, 3));
    el2&& Event("E2", VAR_INT(23));
    el2&& Event("E3", VAR_STRING("freedom"));
    ASSERT_FALSE(el2.matches(el1));

    el2 = Event("E1", toDate(2014, 2, 3));
    el2&& Event("E2", VAR_INT(-5));
    el2&& Event("E3", VAR_STRING("angry"));
    ASSERT_TRUE(!el2.matches(el1));
}

TEST_F(StatUtilTest, util_stat_test)
{
    // "====== Testing statistical functions ========");
    {
        CSVAnalyzer csv("FEvent, BCond, CCond, Value", "f,b,c,f");
        csv << " 1.0, yes,  a, 0.5";
        csv << " 5.0, No,   B, 2.5";
        csv << " 7.0, No,   B, 2.6";
        csv << " 1.0, yes,  a, 4.5";
        csv << " 5.0, No,   B, 2.3";
        csv << " 7.0, No,   B, 1.6";
        csv << " 1.0, yes,  a, 7";
        csv << " 5.0, No,   B, 1.1";
        csv << " 7.0, No,   B, 1.01";

        DiscreteProbability d;

        d.train(csv, true);
        ASSERT_FALSE(d.isModified());
        ASSERT_FALSE(d.isUniform());
        ASSERT_TRUE(d.isDistribution());
    }
    {
        CSVAnalyzer csv("FEvent, BCond, CCond", "f,b,c");
        csv << " 1.0, yes, A";
        csv << " 2.0, No,  A";
        csv << " 3.0, yes, B";
        csv << " 4.0, No,  B";
        csv << " 4.0, yes, A";
        csv << " 7.0, No,  A";
        csv << " 7.0, No,  c";
        csv << " 3.0, No,  c";
        csv << " 4.0, No,  c";
        csv << " 1.0, yes, B";
        csv << " 3.0, No,  B";
        csv << " 4.0, No,  c";
        csv << " 3.0, No,  B";
        csv << " 4.0, No,  c";
        csv << " 4.0, No,  B";
        csv << " 4.0, yes, A";
        csv << " 7.0, No,  A";
        csv << " 7.0, No,  c";
        csv << " 3.0, No,  c";
        csv << " 4.0, No,  c";
        csv << " 1.0, yes, B";
        csv << " 3.0, No,  B";
        csv << " 4.0, No,  c";
        csv << " 1.0, yes, B";
        csv << " 3.0, No,  B";
        csv << " 4.0, No,  B";
        csv << " 5.0, No,  B";

        DiscreteProbability d;

        d.train(csv, false);
        ASSERT_FALSE(d.isModified());
        ASSERT_FALSE(d.isUniform());
        ASSERT_TRUE(d.isDistribution());
        ASSERT_TRUE(d.P(Event("FEvent", 5.0L) || (Event("CCond", 'B') && Event("BCond", false))) < 1.0L);
        ASSERT_TRUE(d.P(Event("FEvent", 5.0L) || (Event("CCond", 'B') && Event("BCond", false))) > 0.0L);
        // d);
    }
    {
        CSVAnalyzer csv("FEvent, BCond, CCond", "f,b,c");
        csv << " 1.0, yes,  A";
        csv << " 2.0, No,   A";
        csv << " 3.0, yes,  B";
        DiscreteProbability d;
        d.clear();
        d.train(csv);
        // d);
        d.canonise();
        d.normalise();
        // d);
        ASSERT_FALSE(d.isModified());
        ASSERT_FALSE(d.isUniform());
        ASSERT_TRUE(d.isDistribution());
    }
    {
        VALUERANGES_TYPE eventValRanges;
        VALUERANGES_TYPE condValRanges;
        eventValRanges["1stEventUint"] = EventValueRange(VAR_UINT(0), 5);
        eventValRanges["2ndEventInt"]  = EventValueRange(VAR_INT(-3), 3);
        condValRanges["boolCond"]      = EventValueRange(true);
        condValRanges["charCond"]      = EventValueRange('a', 'h');

        DiscreteProbability d(eventValRanges, condValRanges);
        d.canonise();
        d.normalise();

        // d);
        ASSERT_FALSE(d.isModified());
        ASSERT_FALSE(d.isUniform());
        ASSERT_TRUE(d.isDistribution());
    }
}

TEST_F(StatUtilTest, util_continuous_stat_test)
{
    // "");
    // "====== Testing continuous statistical functions ========");
    GaussFunction norm(0.0L, 1.0L);
    // probability P([mu-sigma, mu+sigma])
    const long double p_m_var_prob = 0.682689492137L;

    VAR_FLOAT prob = norm.P(Event("E", Interval<long double>()));
    ASSERT_DOUBLE_EQ(prob, 1.0L);
    prob = norm.P(Event("E", Interval<long double>(0.0L)));
    ASSERT_DOUBLE_EQ(prob, 0.5L);
    prob = norm.P(Event("E", Interval<long double>(0.0L, {infiniteMin})));
    ASSERT_DOUBLE_EQ(prob, 0.5L);
    prob            = norm.P(Event("E", Interval<long double>(0.0L, 1.0L)));
    VAR_FLOAT prob2 = norm.P(Event("E", Interval<long double>(-1.0L, 0.0L)));
    ASSERT_DOUBLE_EQ(prob, prob2);

    prob = norm.P(Event("E", Interval<long double>(norm.mu() - norm.sigma(), norm.mu() + norm.sigma())));
    ASSERT_LE(prob - p_m_var_prob, 1E-10);

    CSVAnalyzer         csv;
    vector<long double> sample;
    sample += -1.0L, -0.5L, -0.1L, -1.0L, -0.2L, -0.7L, 1.0L;
    csv.appendColumn("E", sample);
    norm.train(csv, false);
    ASSERT_DOUBLE_EQ(norm.P(Event("E", Interval<long double>())), 1.0L);
    ASSERT_DOUBLE_EQ(norm.P(Event("E", Interval<long double>(norm.mu()))), 0.5L);

    ASSERT_DOUBLE_EQ(norm.P(Event("E", Interval<long double>(norm.mu(), {infiniteMin}))), 0.5L);

    ASSERT_LE(norm.P(Event("E", Interval<long double>(norm.mu() - norm.sigma(), norm.mu() + norm.sigma())))
               - p_m_var_prob,
              1E-10);

    ExponentialFunction ed(1.0L);
    ASSERT_DOUBLE_EQ(ed.P(Event("E", Interval<long double>(0.0L))), 1.0L);
    ASSERT_DOUBLE_EQ(ed.P(Event("E", Interval<long double>(0.0L, 0.0L))), 0.0L);
    ASSERT_DOUBLE_EQ(ed.P(Event("E", Interval<long double>(0.0L, {finiteMin}))), 1.0L);

    ASSERT_DOUBLE_EQ(ed.P(Event("E", Interval<long double>(ed.ln2ByLambda(), {infiniteMin}))), 0.5L);
    ASSERT_THROW(ed.train(csv, false), event_range_error);
    csv.clear();
    sample.clear();
    sample += 1.0L, 0.5L, 0.1L, 1.0L, 0.2L, 5.7L, 7.0L;
    csv.appendColumn("E", sample);
    ed.train(csv, false);

    UniformFloatFunction uf(0.0L, 1.0L);
    ASSERT_DOUBLE_EQ(uf.P(Event("E", Interval<long double>(0.0L, {infiniteMax}))), 1.0L);
    ASSERT_DOUBLE_EQ(uf.P(Event("E", Interval<long double>(0.0L, {infiniteMin}))), 0.0L);
    ASSERT_DOUBLE_EQ(uf.P(Event("E", Interval<long double>(0.0L, {infiniteMax}))), 1.0L);

    ASSERT_DOUBLE_EQ(uf.P(Event("E", Interval<long double>(0.1234L, {infiniteMin}))), 0.1234L);
    ASSERT_DOUBLE_EQ(uf.P(Event("E", Interval<long double>(0.1234L, {infiniteMax}))), 1.0L - 0.1234L);
    uf.train(csv, false);
}
