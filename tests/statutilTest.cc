/*
 * File:		statutilTest.cc
 * Description:         Unit tests for statistics utilities
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

#include "statutilTest.h"

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

const string filename = "/tmp/test.csv";

using namespace std;
using namespace util;

CPPUNIT_TEST_SUITE_REGISTRATION(statutilTest);

statutilTest::statutilTest()
{
}

statutilTest::~statutilTest()
{
}

void statutilTest::setUp()
{
}

void statutilTest::tearDown()
{
}

void statutilTest::util_event_test()
{
    initDateFormats();
    ////BOOST_TEST_MESSAGE("");
    ////BOOST_TEST_MESSAGE("====== Testing event/eventlist/condition event functions ========");
    {
        ////BOOST_TEST_MESSAGE("Two different EventLists of size 1");
        EventList el1 = Event("E1", true);
        EventList el2 = Event("E3", false);
        CPPUNIT_ASSERT(el1 < el2);

        ////BOOST_TEST_MESSAGE("Two *IDENTICAL* EventLists of size 1");
        el1 = Event("E1", true);
        el2 = Event("E1", true);
        CPPUNIT_ASSERT(!(el1 < el2));
        CPPUNIT_ASSERT(Event("E1", true).notConflicting(Event("E1", true)));
        CPPUNIT_ASSERT(!Event("E1", true).notConflicting(Event("E1", false)));
        CPPUNIT_ASSERT(Event("E1", true).notConflicting(Event("E2", true)));
        CPPUNIT_ASSERT(Event("E1", true).notConflicting(Event("E2", false)));

        ////BOOST_TEST_MESSAGE("Two EventLists of different size where one is front of other");
        el1 = Event("E1", true);
        el2 = Event("E1", true) && Event("E2", false);
        CPPUNIT_ASSERT(el1 < el2);
        CPPUNIT_ASSERT(!(el2 < el1));
        CPPUNIT_ASSERT(el1.notConflicting(el1));
        CPPUNIT_ASSERT(el2.notConflicting(el2));
        CPPUNIT_ASSERT(el2.notConflicting(el1));
        CPPUNIT_ASSERT(el1.notConflicting(el2));

        ////BOOST_TEST_MESSAGE("Two EventLists *IDENTICAL* to a certain size then different, but same length");
        el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        CPPUNIT_ASSERT(el1 < el2);
        CPPUNIT_ASSERT(!(el2 < el1));
        el1 = Event("E1", true) && Event("E2", false) && Event("E5", false) && Event("E4", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        CPPUNIT_ASSERT(el2 < el1);
        CPPUNIT_ASSERT(!(el1 < el2));

        ////BOOST_TEST_MESSAGE("Two EventLists *IDENTICAL* to a certain size then different, different length");
        el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false) && Event("E6", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        CPPUNIT_ASSERT(el1 < el2);
        CPPUNIT_ASSERT(!(el2 < el1));
        el1 = Event("E1", true) && Event("E2", false) && Event("E5", false) && Event("E4", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        CPPUNIT_ASSERT(el2 < el1);
        CPPUNIT_ASSERT(!(el1 < el2));
        CPPUNIT_ASSERT(el1.notConflicting(el1));
        CPPUNIT_ASSERT(el2.notConflicting(el2));
        CPPUNIT_ASSERT(el2.notConflicting(el1));
        CPPUNIT_ASSERT(el1.notConflicting(el2));

        el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false) && Event("E6", false);
        el2 = Event("E1", false) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        CPPUNIT_ASSERT(el1.notConflicting(el1));
        CPPUNIT_ASSERT(el2.notConflicting(el2));
        CPPUNIT_ASSERT(!el2.notConflicting(el1));
        CPPUNIT_ASSERT(!el1.notConflicting(el2));
    }
    {
        ////BOOST_TEST_MESSAGE("Check Events/EventLists creation");
        EventList              el1 = Event("E1", true) && Event("E2", VAR_STRING("fdsa"));
        EventList              el2 = Event("E3", false) && Event("E4", VAR_STRING("dfsg"));
        map<EventList, string> elMap;
        elMap[el1] = VAR_STRING("1st");
        elMap[el2] = VAR_STRING("2nd");

        ////BOOST_TEST_MESSAGE("Check Events/EventLists == an < operators");
        CPPUNIT_ASSERT_EQUAL(el1, Event("E1", true) && Event("E2", VAR_STRING("fdsa")));
        CPPUNIT_ASSERT(el1 < el2);
        CPPUNIT_ASSERT_EQUAL(el2, Event("E3", false) && Event("E4", VAR_STRING("dfsg")));
        CPPUNIT_ASSERT_EQUAL(el1, el1);
        CPPUNIT_ASSERT_EQUAL(el2, el2);

        CPPUNIT_ASSERT(elMap.find(el1) != elMap.end());
        CPPUNIT_ASSERT(elMap.find(el2) != elMap.end());
        CPPUNIT_ASSERT_EQUAL(elMap[el1], VAR_STRING("1st"));
        CPPUNIT_ASSERT_EQUAL(elMap[el2], VAR_STRING("2nd"));
    }
    {
        ACCUMULATION_MAP accMap;
        EventList        el1 = Event("E1", true) && Event("E2", VAR_STRING("fdsa"));
        EventList        el2 = Event("E3", false) && Event("E4", VAR_STRING("dfsg"));
        accMap[el1]          = ACCUMULATION_DATA(0, 0);
    }
    {
        Event e;
        CPPUNIT_ASSERT(e.empty());
        e("SomeName", true);
        CPPUNIT_ASSERT(!e.empty());
    }
    {
        ////BOOST_TEST_MESSAGE("Check CondEvents");
        Event     e;
        EventList el(e);  // empty event is not added so results in empty list
        CPPUNIT_ASSERT(el.empty());

        el = Event("SomeName", true);  // list is initialised with one element
        CPPUNIT_ASSERT(!el.empty());
        el&& Event("SomeMore", (VAR_FLOAT)3.14159365) && Event("EvenSomeMore", VAR_STRING("XXX"));
        CPPUNIT_ASSERT(!el.empty());

        CondEvent c = el;
        CPPUNIT_ASSERT(!c.empty());
        CPPUNIT_ASSERT_EQUAL(c.eventSize(), 3UL);
        CPPUNIT_ASSERT_EQUAL(c.event(), el);
        CPPUNIT_ASSERT_EQUAL(c.conditionSize(), 0UL);

        CondEvent c2(el, el);
        CPPUNIT_ASSERT(!c2.empty());

        CPPUNIT_ASSERT_EQUAL(c2.eventSize(), 3UL);
        CPPUNIT_ASSERT_EQUAL(c2.event(), el);
        CPPUNIT_ASSERT_EQUAL(c2.conditionSize(), 3UL);
        CPPUNIT_ASSERT_EQUAL(c2.condition(), el);

        ////BOOST_TEST_MESSAGE("Check CondEvents creation by csv");

        CSVAnalyzer csv("FEvent, BCond, CCond, Value", "f,b,c,f");
        csv << " 1.0, yes,  a, 0.5";
        csv << " 5.0, No,   B, 2.5";

        CondEvent c3(csv, 0, 0, true);
        CPPUNIT_ASSERT(c3.event() == (Event("FEvent", 1.0L)));
        CPPUNIT_ASSERT_EQUAL(c3.condition(), (Event("BCond", true) && Event("CCond", 'a')));
        CondEvent c4(csv, 1, 1, true);
        CPPUNIT_ASSERT_EQUAL(c4.event(), (Event("FEvent", 5.0L) && Event("BCond", false)));
        CPPUNIT_ASSERT(c4.condition() == (Event("CCond", 'B')));

        CPPUNIT_ASSERT_THROW(CondEvent(Event("E1", true) && Event("E1", false)), eventlist_conflict_error);
    }
    {
        ////BOOST_TEST_MESSAGE("Check CondEvent - manipulation");
        CondEvent                 ce(Event("E1", true) && Event("E2", true));
        CondEvent::CONDEVENT_LIST l;
        CPPUNIT_ASSERT(ce.chainRule(l, "E1"));

        ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);
        vector<string> order;
        order.push_back("E2");
        order.push_back("E1");
        order.push_back("E4");
        order.push_back("E3");
        ce.chainRule(l, order);
        for(CondEvent::CONDEVENT_LIST_CITER it = l.begin(); it != l.end(); it++)
        {
            CPPUNIT_ASSERT_EQUAL(it->eventSize(), 1UL);
        }

        set<string> strSet;
        strSet.insert("E5");
        strSet.insert("E6");
        ce = ce.filterConditions(strSet);
        ////BOOST_TEST_MESSAGE("Check needed events added as place-holders");
        CPPUNIT_ASSERT(ce.hasEvent("E1"));
        CPPUNIT_ASSERT(ce.hasEvent("E2"));
        CPPUNIT_ASSERT(ce.hasEvent("E3"));
        CPPUNIT_ASSERT(ce.hasEvent("E4"));
        CPPUNIT_ASSERT(ce.hasCondition("E5"));
        CPPUNIT_ASSERT(ce.hasCondition("E6"));

        strSet.insert("E1");
        strSet.insert("E4");
        ////BOOST_TEST_MESSAGE("Check that filtering does not create conflicts");
        CPPUNIT_ASSERT_THROW(ce.filterConditions(strSet), eventlist_conflict_error);

        ////BOOST_TEST_MESSAGE("Check that removes unneeded conditions");
        ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);

        strSet.clear();
        strSet.insert("E3");
        strSet.insert("E5");
        ce = Event("E1", true) || Event("E3", true) && Event("E4", true) && Event("E5", true);
        ce = ce.filterConditions(strSet);
        CPPUNIT_ASSERT(ce.hasEvent("E1"));
        CPPUNIT_ASSERT(ce.hasCondition("E3"));
        CPPUNIT_ASSERT(!ce.hasCondition("E4"));
        CPPUNIT_ASSERT(ce.hasCondition("E5"));

        ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);
        ce.chainRule(l, order);
        for(CondEvent::CONDEVENT_LIST_CITER it = l.begin(); it != l.end(); it++)
        {
            CPPUNIT_ASSERT_EQUAL(it->eventSize(), 1UL);
        }
    }
}

void statutilTest::util_event_operation_test()
{
    initDateFormats();
    ////BOOST_TEST_MESSAGE("");
    ////BOOST_TEST_MESSAGE("====== Testing event explicit operations ========");
    {
        ////BOOST_TEST_MESSAGE("Two EventLists of size 1");
        ////BOOST_TEST_MESSAGE("match to interval");
        {
            Event itv_20140203_20150203 = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), toDate(2015, 2, 3)));
            CPPUNIT_ASSERT(Event("E1", toDate(2014, 2, 3)).matches(itv_20140203_20150203));
            CPPUNIT_ASSERT(Event("E1", toDate(2015, 2, 3)).matches(itv_20140203_20150203));
            CPPUNIT_ASSERT(Event("E1", toDate(2014, 3, 3)).matches(itv_20140203_20150203));
            CPPUNIT_ASSERT(!Event("E1", toDate(2014, 2, 2)).matches(itv_20140203_20150203));
            CPPUNIT_ASSERT(!Event("E1", toDate(2015, 2, 4)).matches(itv_20140203_20150203));
        }
        {
            // comparator1 = [2014-02-03, oo)

            Event itv_20140203_oo = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), {finiteMin}));
            CPPUNIT_ASSERT(Event("E1", toDate(2014, 2, 3)).matches(itv_20140203_oo));
            CPPUNIT_ASSERT(Event("E1", toDate(2015, 2, 3)).matches(itv_20140203_oo));
            CPPUNIT_ASSERT(Event("E1", toDate(2014, 3, 3)).matches(itv_20140203_oo));
            CPPUNIT_ASSERT(!Event("E1", toDate(2014, 2, 2)).matches(itv_20140203_oo));  // outside the interval
            CPPUNIT_ASSERT(Event("E1", toDate(2015, 2, 2)).matches(itv_20140203_oo));
        }
        {
            Event itv_oo_20140203 = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), {infiniteMin, rightClosed}));
            CPPUNIT_ASSERT(Event("E1", toDate(2014, 2, 3)).matches(itv_oo_20140203));
            CPPUNIT_ASSERT(!Event("E1", toDate(2015, 2, 3)).matches(itv_oo_20140203));
            CPPUNIT_ASSERT(!Event("E1", toDate(2014, 3, 3)).matches(itv_oo_20140203));
            CPPUNIT_ASSERT(Event("E1", toDate(2014, 2, 2)).matches(itv_oo_20140203));
            CPPUNIT_ASSERT(!Event("E1", toDate(2015, 2, 4)).matches(itv_oo_20140203));
        }

        ////BOOST_TEST_MESSAGE("match to less-operator");

        Event comparator2 = Event("E2", VAR_INT(11), &Event::less);
        CPPUNIT_ASSERT(Event("E2", VAR_INT(4)).matches(comparator2));
        CPPUNIT_ASSERT(Event("E2", VAR_INT(10)).matches(comparator2));
        CPPUNIT_ASSERT(!Event("E2", VAR_INT(11)).matches(comparator2));
        CPPUNIT_ASSERT(!Event("E2", VAR_INT(12)).matches(comparator2));

        ////BOOST_TEST_MESSAGE("match to lessEqual-operator");

        Event comparator3 = Event("E3", VAR_UINT(11), &Event::lessEqual);
        CPPUNIT_ASSERT(Event("E3", VAR_UINT(4)).matches(comparator3));
        CPPUNIT_ASSERT(Event("E3", VAR_UINT(11)).matches(comparator3));
        CPPUNIT_ASSERT(Event("E3", VAR_UINT(10)).matches(comparator3));
        CPPUNIT_ASSERT(!Event("E3", VAR_UINT(113)).matches(comparator3));
        CPPUNIT_ASSERT(!Event("E3", VAR_UINT(12)).matches(comparator3));

        ////BOOST_TEST_MESSAGE("match to greater-operator");

        Event comparator4 = Event("E4", VAR_CHAR('t'), &Event::greater);
        CPPUNIT_ASSERT(Event("E4", VAR_CHAR('u')).matches(comparator4));
        CPPUNIT_ASSERT(Event("E4", VAR_CHAR('z')).matches(comparator4));
        CPPUNIT_ASSERT(!Event("E4", VAR_CHAR('a')).matches(comparator4));
        CPPUNIT_ASSERT(!Event("E4", VAR_CHAR('t')).matches(comparator4));

        ////BOOST_TEST_MESSAGE("match to greaterEqual-operator");

        Event comparator5 = Event("E5", VAR_STRING("dieter"), &Event::greaterEqual);
        CPPUNIT_ASSERT(Event("E5", VAR_STRING("dieter")).matches(comparator5));
        CPPUNIT_ASSERT(Event("E5", VAR_STRING("freedom")).matches(comparator5));
        CPPUNIT_ASSERT(!Event("E5", VAR_STRING("diet")).matches(comparator5));
        CPPUNIT_ASSERT(!Event("E5", VAR_STRING("angry")).matches(comparator5));
    }
    {
        ////BOOST_TEST_MESSAGE("Two EventLists of equal size >1");
        ////BOOST_TEST_MESSAGE("match to interval");

        EventList el1;  // E1 in [2014-02-03..2015-02-03], E2 < 11 , E3 >= "dieter"
        el1&&     Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), toDate(2015, 2, 3)));
        el1&&     Event("E2", VAR_INT(11), &Event::less);
        el1&&     Event("E3", VAR_STRING("dieter"), &Event::greaterEqual);

        // match only if *ALL* events are matching
        EventList el2;
        el2 = Event("E1", toDate(2014, 2, 3));
        el2&& Event("E2", VAR_INT(10));
        el2&& Event("E3", VAR_STRING("dieter"));
        CPPUNIT_ASSERT(el2.matches(el1));

        el2 = Event("E1", toDate(2015, 2, 3));
        el2&& Event("E2", VAR_INT(-5));
        el2&& Event("E3", VAR_STRING("freedom"));
        CPPUNIT_ASSERT(el2.matches(el1));

        el2 = Event("E1", toDate(2013, 2, 3));
        el2&& Event("E2", VAR_INT(-5));
        el2&& Event("E3", VAR_STRING("freedom"));
        CPPUNIT_ASSERT(!el2.matches(el1));

        el2 = Event("E1", toDate(2014, 2, 3));
        el2&& Event("E2", VAR_INT(23));
        el2&& Event("E3", VAR_STRING("freedom"));
        CPPUNIT_ASSERT(!el2.matches(el1));

        el2 = Event("E1", toDate(2014, 2, 3));
        el2&& Event("E2", VAR_INT(-5));
        el2&& Event("E3", VAR_STRING("angry"));
        CPPUNIT_ASSERT(!el2.matches(el1));
    }
}

void statutilTest::util_stat_test()
{
    initDateFormats();
    ////BOOST_TEST_MESSAGE("");
    ////BOOST_TEST_MESSAGE("====== Testing statistical functions ========");
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
        CPPUNIT_ASSERT(!d.isModified());
        CPPUNIT_ASSERT(!d.isUniform());
        CPPUNIT_ASSERT(d.isDistribution());
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
        CPPUNIT_ASSERT(!d.isModified());
        CPPUNIT_ASSERT(!d.isUniform());
        CPPUNIT_ASSERT(d.isDistribution());
        CPPUNIT_ASSERT(d.P(Event("FEvent", 5.0L) || Event("CCond", 'B') && Event("BCond", false)) < 1.0L);
        CPPUNIT_ASSERT(d.P(Event("FEvent", 5.0L) || Event("CCond", 'B') && Event("BCond", false)) > 0.0L);
        ////BOOST_TEST_MESSAGE(d);
    }
    {
        CSVAnalyzer csv("FEvent, BCond, CCond", "f,b,c");
        csv << " 1.0, yes,  A";
        csv << " 2.0, No,   A";
        csv << " 3.0, yes,  B";
        DiscreteProbability d;
        d.clear();
        d.train(csv);
        ////BOOST_TEST_MESSAGE(d);
        d.canonise();
        d.normalise();
        ////BOOST_TEST_MESSAGE(d);
        CPPUNIT_ASSERT(!d.isModified());
        CPPUNIT_ASSERT(!d.isUniform());
        CPPUNIT_ASSERT(d.isDistribution());
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

        ////BOOST_TEST_MESSAGE(d);
        CPPUNIT_ASSERT(!d.isModified());
        CPPUNIT_ASSERT(!d.isUniform());
        CPPUNIT_ASSERT(d.isDistribution());
    }
}

void statutilTest::util_continuous_stat_test()
{
    ////BOOST_TEST_MESSAGE("");
    ////BOOST_TEST_MESSAGE("====== Testing continuous statistical functions ========");
    GaussFunction norm(0.0L, 1.0L);
    // probability P([mu-sigma, mu+sigma])
    const long double p_m_var_prob = 0.682689492137L;

    VAR_FLOAT prob = norm.P(Event("E", Interval<long double>()));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(prob, 1.0L, 1e-10L);
    prob = norm.P(Event("E", Interval<long double>(0.0L)));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(prob, 0.5L, 1e-10L);
    prob = norm.P(Event("E", Interval<long double>(0.0L, {infiniteMin})));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(prob, 0.5L, 1e-10L);
    prob            = norm.P(Event("E", Interval<long double>(0.0L, 1.0L)));
    VAR_FLOAT prob2 = norm.P(Event("E", Interval<long double>(-1.0L, 0.0L)));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(prob, prob2, 1e-10L);

    prob = norm.P(Event("E", Interval<long double>(norm.mu() - norm.sigma(), norm.mu() + norm.sigma())));
    CPPUNIT_ASSERT_DOUBLES_EQUAL(prob, p_m_var_prob, 1e-10L);

    CSVAnalyzer         csv;
    vector<long double> sample;
    sample += -1.0L, -0.5L, -0.1L, -1.0L, -0.2L, -0.7L, 1.0L;
    csv.appendColumn("E", sample);
    norm.train(csv, false);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(norm.P(Event("E", Interval<long double>())), 1.0L, 1e-10L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(norm.P(Event("E", Interval<long double>(norm.mu()))), 0.5L, 1e-10L);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(norm.P(Event("E", Interval<long double>(norm.mu(), {infiniteMin}))), 0.5L, 1e-10L);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
     norm.P(Event("E", Interval<long double>(norm.mu() - norm.sigma(), norm.mu() + norm.sigma()))),
     p_m_var_prob,
     1e-10L);

    ExponentialFunction ed(1.0L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ed.P(Event("E", Interval<long double>(0.0L))), 1.0L, 1e-10L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ed.P(Event("E", Interval<long double>(0.0L, 0.0L))), 0.0L, 1e-10L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(ed.P(Event("E", Interval<long double>(0.0L, {finiteMin}))), 1.0L, 1e-10L);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(ed.P(Event("E", Interval<long double>(ed.ln2ByLambda(), {infiniteMin}))),
                                 0.5L,
                                 1e-10L);
    CPPUNIT_ASSERT_THROW(ed.train(csv, false), event_range_error);
    csv.clear();
    sample.clear();
    sample += 1.0L, 0.5L, 0.1L, 1.0L, 0.2L, 5.7L, 7.0L;
    csv.appendColumn("E", sample);
    ed.train(csv, false);

    UniformFloatFunction uf(0.0L, 1.0L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(uf.P(Event("E", Interval<long double>(0.0L, {infiniteMax}))), 1.0L, 1e-10L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(uf.P(Event("E", Interval<long double>(0.0L, {infiniteMin}))), 0.0L, 1e-10L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(uf.P(Event("E", Interval<long double>(0.0L, {infiniteMax}))), 1.0L, 1e-10L);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(uf.P(Event("E", Interval<long double>(0.1234L, {infiniteMin}))), 0.1234L, 1e-10L);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(uf.P(Event("E", Interval<long double>(0.1234L, {infiniteMax}))),
                                 1.0L - 0.1234L,
                                 1e-10L);
    uf.train(csv, false);
}
