/*
 * File Name:   statutil.h
 * Description: statistic utility functions
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

#ifndef NS_UTIL_STATUTIL_H_INCLUDED
#define NS_UTIL_STATUTIL_H_INCLUDED

#include "anyutil.h"
#include "csvutil.h"
#include "to_string.h"

#include <algorithm>
#include <boost/math/distributions/exponential.hpp>
#include <boost/math/distributions/normal.hpp>
#include <cmath>
#include <exception>
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
 * Error handling for event-range errors.
 */
struct event_range_error : public std::logic_error
{
    enum conflict_type
    {
        exponential_range,  ///< Outside the range of the exponential function [0..oo)
        gaussian_range,     ///< Outside the range of the gaussian function (-oo..oo)
        uniform_range       ///< Outside the range of a uniform function [min..max]
    };

    event_range_error(VAR_FLOAT f, size_t rangeDescriptors)
    : std::logic_error("Floatrange-test for '" + toString(f) + "': the float description"
                       + " must be empty or exactly min and max values. Found " + toString(rangeDescriptors))
    {
    }

    event_range_error(const std::string &str = "Invalid range") : std::logic_error(str)
    {
    }

    event_range_error(conflict_type tp, VAR_FLOAT f1, VAR_FLOAT f2 = 0.0L, VAR_FLOAT f3 = 0.0L)
    : std::logic_error(tp == exponential_range ? "Range for exponential function is [0..oo) but found " + toString(f1) :
                       tp == uniform_range ? "Range for exponential function is [" + toString(f1) + ".." + toString(f2)
                                              + "] but found " + toString(f3) :
                                             "Invalid range.")
    {
    }
};

/**
 * Error handling for event errors.
 */
struct event_error : public std::logic_error
{
    event_error(const std::string &str) : std::logic_error(str)
    {
    }

    event_error(const std::type_info &t1, const std::type_info &t2)
    : std::logic_error(
     event_error("Cannot get interval of type " + toString(t1.name()) + " from Event of type " + toString(t2.name())))
    {
    }
};

class EventCatenation;

/**
 * Error handling for event lists with mutually exclusive events.
 */
struct eventlist_conflict_error : public std::logic_error
{
    enum conflict_type
    {
        evt,      ///< Event list conflicts with itself
        cond,     ///< condition list conflicts with itself
        evt_cond  ///< event- and condition-lists conflict with each other
    };

    eventlist_conflict_error(conflict_type tp, const EventCatenation &e1);
    eventlist_conflict_error(const EventCatenation &e1, const EventCatenation &e2);
};

/**
 * Error handling for general distribution errors like breach of probability requirements.
 */
struct distribution_error : public std::logic_error
{
    enum conflict_type
    {
        empty_uniform,
        empty_normalise,
        empty_canonise
    };

    distribution_error(conflict_type tp)
    : std::logic_error(std::string(tp == empty_uniform   ? "Make uniform" :
                                   tp == empty_normalise ? "Normalise" :
                                                           "Canonise")
                       + ": cannot modify distribution as node-distribution "
                         "is empty and range is empty.")
    {
    }

    distribution_error(const std::string &err = "") : std::logic_error(err.empty() ? "Not a distribution!" : err)
    {
    }

    distribution_error(long double val)
    : std::logic_error("Probability value " + toString(val) + " is outside range [0.0..1.0]")
    {
    }
};

/**
 * Formalize one statistical Event.
 * Consists of the name of the event, a match-operation and a match-value.
 *
 * <ul>
 * <li>x &le; 10</li>
 * <li>x in [-3.1415, 3.1415]</li>
 *  <li>y == "1st Jan 2014"</li>
 * </ul>
 */
class Event
{
    public:
    static Equals        equals;         ///< Default Equality operation
    static Less          less;           ///< Default Less operation
    static LessEqual     lessEqual;      ///< Default Less Equal operation
    static Greater       greater;        ///< Default Greater operation
    static GreaterEqual  greaterEqual;   ///< Default Greater Equal operation
    static IsElementOf   isElementOf;    ///< Default Element inclusion operation
    static PlaceHolderOp placeHolderOp;  ///< Default Placeholder operation

    /**
     * Default construct empty Event.
     *
     * @param op Default-operation: Placeholder - returns false
     */
    Event(Operation *op = &placeHolderOp);

    /**
     * Construct boolean Event.
     *
     * @param name event name
     * @param b boolean value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_BOOL b, Operation *op = &equals);

    /**
     * Construct character Event.
     *
     * @param name event name
     * @param c character value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_CHAR c, Operation *op = &equals);

    /**
     * Construct signed integer Event.
     *
     * @param name event name
     * @param i integral value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_INT i, Operation *op = &equals);

    /**
     * Construct unsigned integer Event.
     *
     * @param name event name
     * @param u unsigned integral value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_UINT u, Operation *op = &equals);

    /**
     * Construct floating point Event.
     *
     * @param name event name
     * @param f floating point value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_FLOAT f, Operation *op = &equals);

    /**
     * Construct date Event.
     *
     * @param name event name
     * @param d date value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_DATE d, Operation *op = &equals);

    /**
     * Construct string Event.
     *
     * @param name event name
     * @param s string value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_STRING s, Operation *op = &equals);

    /**
     * Construct boolean interval Event.
     *
     * @param name event name
     * @param bi boolean interval value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, VAR_BOOL_INTERVAL bi, Operation *op = &isElementOf);

    /**
     * Construct character interval Event.
     *
     * @param name event name
     * @param ci character interval value
     * @param op operation, default: Containment.
     */
    Event(const std::string &name, VAR_CHAR_INTERVAL ci, Operation *op = &isElementOf);

    /**
     * Construct signed integer interval Event.
     *
     * @param name event name
     * @param ii integer interval value
     * @param op operation, default: Containment.
     */
    Event(const std::string &name, VAR_INT_INTERVAL ii, Operation *op = &isElementOf);

    /**
     * Construct unsigned integer interval Event.
     *
     * @param name event name
     * @param ui unsigned integer interval value
     * @param op operation, default: Containment.
     */
    Event(const std::string &name, VAR_UINT_INTERVAL ui, Operation *op = &isElementOf);

    /**
     * Construct floating point interval Event.
     *
     * @param name event name
     * @param fi floating point interval value
     * @param op operation, default: Containment..
     */
    Event(const std::string &name, VAR_FLOAT_INTERVAL fi, Operation *op = &isElementOf);

    /**
     * Construct date interval Event.
     *
     * @param name event name
     * @param di date interval value
     * @param op operation, default: Containment.
     */
    Event(const std::string &name, VAR_DATE_INTERVAL di, Operation *op = &isElementOf);

    /**
     * Construct character-string interval Event.
     *
     * @param name event name
     * @param si string interval value
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, const char *si, Operation *op = &equals);

    /**
     * Construct Var-type Event.
     *
     * @param name event name
     * @param a variant
     * @param dummyConfirm Needed to ensure only valid types are used.
     * @param op operation, default: Equality.
     */
    Event(const std::string &name, const Var &a, bool dummyConfirm, Operation *op = &equals);

    /**
     * Copy-construct Event.
     *
     * @param rhs right-hand side
     */
    Event(const Event &rhs) = default;

    /**
     * Assign Event.
     * @param rhs right-hand side
     */
    Event &operator=(const Event &rhs) = default;

    /**
     * Change Event - parameters name/value/operation.
     *
     * @param name new name
     * @param value new value
     * @param op new operation
     *
     * @return the modified *this
     */
    Event &operator()(const std::string &name = "", const Var &value = Var(), Operation *op = &equals);

    /**
     * Explicitly create an empty (placeholder) Event.
     *
     * @param name new name
     *
     * @return the place holder
     */
    static Event placeholderEvent(const std::string &name);

    /**
     * Check whether this Event is a placeholder.
     *
     * @return true if so, false otherwise
     */
    [[nodiscard]] bool isPlaceholder() const;

    /**
     * Check whether all the parameters are undefined
     *
     * @return true if so, false otherwise
     */
    [[nodiscard]] bool empty() const;

    /**
     * Check whether this Event conflicts with the other.
     * two Events conflict if they have the same name but different values, e.g
     * P(Rain=strong | Rain=light)
     * P(Rain=strong | Rain>strong)
     * but not conflicting
     * P(Rain=strong | Rain=strong)
     *
     * @param e the event to check against
     *
     * @return true if *this and e do not conflict, false otherwise
     */
    [[nodiscard]] bool notConflicting(const Event &event) const;

    /**
     * Retrieve the name only.
     *
     * @return the name
     */
    [[nodiscard]] std::string name() const;

    /**
     * Retrieve the value as template-type T_.
     *
     * @return the native value
     */
    template<typename T_>
    T_ value() const
    {
        return (toNative<T_>(value_));
    }

    /**
     * Retrieve the value as interval of template-type T_.
     * If the value is indeed a single value, then return a single value interval.
     *
     * @return the type-T_ - interval
     */
    template<typename T_>
    [[nodiscard]] Interval<T_> interval() const
    {
        if(value_.type() == typeid(Interval<T_>))
            return (value_.get<Interval<T_>>());
        else if(value_.type() == typeid(T_))
        {
            T_ v = value_.get<T_>();

            return (Interval<T_>(v, v));
        }
        else
            throw event_error(typeid(T_), value_.type());
    }

    /**
     * Retrieve the value as variant.
     * @return the value as variant
     */
    [[nodiscard]] Var varValue() const;

    /**
     * Check whether an Event matches this event considering the name/value/operation.
     *
     * @param event the event to match against
     *
     * @return true if e matches *this, false otherwise
     */
    [[nodiscard]] bool matches(const Event &event) const;

    /**
     * Propagate the operation description to the users of this.
     *
     * @return the operation description
     */
    [[nodiscard]] std::string opDesc() const;

    /**
     * Equality operation needed to enable associative containers.
     *
     * @param lhs left hand side
     * @param rhs right hand side
     *
     * @return true if equal, false otherwise
     */
    friend bool operator==(const Event &lhs, const Event &rhs);

    /**
     * Less than operation needed to enable associative containers.
     *
     * @param lhs left hand side
     * @param rhs right hand side
     *
     * @return true if equal, false otherwise
     */
    friend bool operator<(const Event &lhs, const Event &rhs);

    friend bool operator<=(const Event &lhs, const Event &rhs)
    {
        return (lhs < rhs) || (lhs == rhs);
    }

    friend bool operator>(const Event &lhs, const Event &rhs)
    {
        return !(lhs <= rhs);
    }

    friend bool operator>=(const Event &lhs, const Event &rhs)
    {
        return !(lhs < rhs);
    }

    /**
     * Generic ostream - &lt;&lt; operator for Events.
     *
     * @param os the out-stream
     * @param the right-hand-side event
     *
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const Event &rhs);

    private:
    std::string name_;
    Var         value_;
    Operation  *operation_;
    bool        isPlaceHolder_ = false;
};

using EventCollection = std::set<Event>;

/**
 * The range that the value of an an Event can assume.
 * This can be a set of disjunct (enumerated) values or a continuous (float)
 * interval.
 */
class EventValueRange
{
    public:
    enum DistributionType
    {
        discrete,       ///< discrete, enumerated distribution of distinct values
        float_uniform,  ///< uniform distribution on a finite floating point interval
        gaussian,       ///< gaussian bell distribution on the float range
        exponential     ///< exponential distribution on the positive float range
    };
    using RangeValues = std::set<Var>;

    /**
     * Default construct a boolean range (or not if haveBoolRange == false).
     *
     * @param haveBoolRange whether or not a boolean range should be created
     */
    explicit EventValueRange(bool haveBoolRange = false);

    /**
     * Construct a range of enumerated character values.
     *
     * @param values the set of characters to initialize the range
     */
    EventValueRange(const std::set<VAR_CHAR> &values);

    /**
     * Construct a range of enumerated signed integer values.
     *
     * @param values the set of integrals to initialize the range
     */
    EventValueRange(const std::set<VAR_INT> &values);

    /**
     * Construct a range of enumerated unsigned integer values.
     *
     * @param values the set of unsigned integrals to initialize the range
     */
    EventValueRange(const std::set<VAR_UINT> &values);

    /**
     * Construct a range of enumerated floating point values.
     *
     * @param values the set of floating point to initialize the range
     */
    EventValueRange(const std::set<VAR_FLOAT> &values);

    /**
     * Construct a range of enumerated date values.
     *
     * @param values the set of dates to initialize the range
     */
    EventValueRange(const std::set<VAR_DATE> &values);

    /**
     * Construct a range of enumerated string values.
     *
     * @param values the set of strings to initialize the range
     */
    EventValueRange(const std::set<VAR_STRING> &values);

    /**
     * Construct a range of enumerated character values as an interval from
     * lowest to highest.
     *
     * @param lowest lowest character of the range
     * @param highest highest character of the range
     */
    EventValueRange(VAR_CHAR lowest, VAR_CHAR highest);

    /**
     * Construct a range of enumerated signed integer values as an interval
     * lowest to highest.
     *
     * @param lowest lowest integer of the range
     * @param highest highest integer of the range
     */
    EventValueRange(VAR_INT lowest, VAR_INT highest);

    /**
     * Construct a range of enumerated unsigned integer values as an interval
     * lowest to highest.
     *
     * @param lowest lowest unsigned integer of the range
     * @param highest highest unsigned integer of the range
     */
    EventValueRange(VAR_UINT lowest, VAR_UINT highest);

    /**
     * Construct a range of continuous uniform floating point values as an
     * interval lowest to highest.
     *
     * @param lowest lowest unsigned integer of the range
     * @param highest highest unsigned integer of the range
     */
    EventValueRange(VAR_FLOAT lowest, VAR_FLOAT highest);

    /**
     * Construct a range of continuous uniform floating point values as an
     * interval.
     *
     * @param interval the floating point interval
     */
    EventValueRange(VAR_FLOAT_INTERVAL interval);

    /**
     * Construct a range of continuous for distribution-type.
     *
     * @param tp the type of distribution which defines the range implicitly
     */
    EventValueRange(DistributionType tp);

    /**
     * Check whether the range is empty.
     *
     * @return true if so, false otherwise
     */
    [[nodiscard]] bool empty() const
    {
        return (values_.empty());
    }

    /**
     * Populate the enumerated range from a set.
     * @param values a set of values to add to the range
     */
    template<typename T_>
    void setValues(const std::set<T_> &values)
    {
        for(auto it = values.begin(); it != values.end(); it++)
        {
            values_.insert(Var(*it));
        }
    }

    /**
     * Add a value if the type is valid.
     * @param val a potential new value, if type is ok
     * @return true, if value was added, false otherwise
     */
    bool add(const Var &val);

    /**
     * Add a range of values to the range.
     * @param lowest lowest T_ value
     * @param highest highest T_ value
     * true if all value of between lowest and highest were added, false
     *       otherwise
     */
    template<typename T_>
    bool addRange(T_ lowest, T_ highest)
    {
        if(highest < lowest)
            std::swap(highest, lowest);

        bool reval = true;

        for(T_ i = lowest; i <= highest && reval; i++)
            reval &= insert(i);

        return (reval);
    }

    /**
     * Add a range of values to the range specialised for floating point
     * (continuous) range.
     *
     * @param lowest lowest floating point value
     * @param highest highest floating point value
     *
     * @return true if all value of between lowest and highest were added, false
     *       otherwise
     */
    bool addRange(VAR_FLOAT lowest, VAR_FLOAT highest)
    {
        if(highest < lowest)
            std::swap(highest, lowest);

        bool reval = (highest != lowest);  // @suppress("Direct float comparison")

        values_.clear();
        insert(lowest);
        insert(highest);

        return (reval);
    }

    /**
     * Get the size of the range. Continuous ranges are always 0 or 2
     * (lowest,highest)
     *
     * @return the size of the range
     */
    [[nodiscard]] size_t size() const;

    /**
     * Check whether the range is continuous
     *
     * @return true, if so, false otherwise
     */
    [[nodiscard]] bool isContinuous() const;

    /**
     * Retrieve the type of distribution for this range.
     *
     * @return the type of distribution
     */
    [[nodiscard]] DistributionType type() const;

    /**
     * Check whether a value is in range.
     *
     * @param value the value to check
     *
     * @return true if so, false otherwise
     */
    [[nodiscard]] bool validValue(const Var &value) const;

    /**
     * Check whether a value has the correct type for this range.
     * If no value has been added to the range yet then any Var is valid
     * otherwise all have to be of the same type.
     *
     * @param value the value to check
     *
     * @return true if so, false otherwise
     */
    [[nodiscard]] bool validType(const Var &value) const;

    /**
     * Create a list of Events Event(name,x) where x is in the range
     * by conjugating the name with all possible values.
     * Helper for the creation of canonised (full) probability table.
     *
     * @param name name of the new event
     *
     * @return the created list of events
     */
    [[nodiscard]] EventCollection makeEventSet(const std::string &name) const;

    /**
     * Generic ostream - &lt;&lt; operator for EventValueRange.
     *
     * @param os the ostream
     * @param evr the event value range to write on the stream
     *
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const EventValueRange &evr);

    private:
    /**
     * Template helper to ensure only Var's of the same type can be inserted
     * into the collection.
     *
     * @param v the value to insert
     *
     * @return true, it insertion successful, false otherwise
     */
    template<typename T_>
    bool insert(const T_ &v)
    {
        bool reval = false;
        if(values_.empty() || sameType(v, *(values_.begin())))
        {
            values_.insert(v);
            reval = true;
        }

        return (reval);
    }

    RangeValues      values_;          ///< Collection describing the range.
    DistributionType type_{discrete};  ///< Indicates the type range.
};

/**
 * Tag for a list of statistical Events (Event_1 and Event_2 and ...).
 * This enables statistical expressions like P(E1=e1,E2 &lt; e2,...)
 */
class EventCatenation
{
    public:
    using EventsCollection    = std::set<Event>;
    using EventsIterator      = EventsCollection::iterator;
    using EventsConstIterator = EventsCollection::const_iterator;

    /**
     * Default construct empty event-list.
     */
    EventCatenation() = default;

    /**
     * Construct a one-element event-list (if Event is not empty).
     *
     * @param event an event
     */
    EventCatenation(const Event &event);

    /**
     * Copy construct an EventList.
     *
     * @param rhs the right-hand-side event
     */
    EventCatenation(const EventCatenation &rhs) = default;

    /**
     * Assign the right-hand-side to this.
     *
     * @param rhs the right-hand-side event
     *
     * @return the modified Event-list
     */
    EventCatenation &operator=(const EventCatenation &rhs) = default;

    /**
     * Append a single Event to the this.
     *
     * @param event an event
     *
     * @return the modified Event-list
     */
    EventCatenation &operator&&(const Event &event);

    /**
     * Append a list of Events to this.
     *
     * @param el an event list
     *
     * @return the modified Event-list
     */
    EventCatenation &operator&&(const EventCatenation &el);

    /**
     * Check for emptiness.
     */
    [[nodiscard]] bool empty() const;

    /**
     * Get the number of Events in the list.
     */
    [[nodiscard]] size_t size() const;

    /**
     * Check whether two lists are not conflicting (using the Event-definition of conflict).
     */
    [[nodiscard]] bool notConflicting(const EventCatenation &eList) const;

    /**
     * Check whether this list matches the other EventCatenation.
     */
    [[nodiscard]] bool matches(const EventCatenation &eList) const;

    /**
     * Retrieve an event from this list by name.
     */
    [[nodiscard]] Event eventByName(const std::string &name) const;

    /**
     * Retrieve the collection of events.
     */
    EventsCollection events() const
    {
        return evts_;
    }

    /**
     * Move name-specified event from this list to the other.
     */
    bool moveEvent(const std::string &name, EventCatenation &el);

    /**
     * Check whether an event named name is in this list.
     */
    [[nodiscard]] bool hasEvent(const std::string &event) const;

    /**
     * Iterator pointing to the start of the list of events.
     */
    EventsIterator begin();

    /**
     * Iterator pointing to the start of the list of events (constant version).
     */
    [[nodiscard]] EventsConstIterator cbegin() const;

    /**
     * Iterator pointing to the end of the list of events.
     */
    EventsIterator end();

    /**
     * Iterator pointing to the end of the list of events (constant version).
     */
    [[nodiscard]] EventsConstIterator cend() const;

    /**
     * Remove the Event pointed to by the iterator from this list.
     */
    void erase(EventsIterator it);

    /**
     * Enable associative containers.
     */
    friend bool operator==(const EventCatenation &lhs, const EventCatenation &rhs);

    /**
     * Enable associative containers.
     */
    friend bool operator<(const EventCatenation &lhs, const EventCatenation &rhs);

    /**
     * Generic ostream - &lt;&lt; operator for EventList.
     *
     * @param os the ostream
     * @param evr the event list to write on the stream
     *
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &, const EventCatenation &eList);

    private:
    EventsCollection evts_;  ///< Ordered list of Events.
};

/**
 * Global operator that creates an EventList from two Events.
 *
 * @param lhs left-hand-side
 * @param rhs right-hand-side
 *
 * @return the newly created list of events
 */
EventCatenation operator&&(const Event &lhs, const Event &rhs);

/**
 * Tag for a list of statistical conditional Events.
 * his enables statistical expressions like P(E1=e1,E2\<e2|E3\>e3,E4 in [0..11])
 */
class CondEvent
{
    public:
    using CONDEVENT_LIST       = std::deque<CondEvent>;
    using CONDEVENT_LIST_ITER  = CONDEVENT_LIST::iterator;
    using CONDEVENT_LIST_CITER = CONDEVENT_LIST::const_iterator;

    /**
     * Default construct from two event-lists: first for the event, second
     * for the condition.
     *
     * @param events an event list
     * @param conds a condition list
     */
    CondEvent(const EventCatenation &events = EventCatenation(), const EventCatenation &conds = EventCatenation());

    /**
     * Construct from a csv-object, more precisely from the column- headers
     * and types. The events and conditions (P(events-list|condition-list)
     * are separated by the last eventIndex, Condition list can be empty.
     *
     * @param csv a csv-reader
     * @param row header row index
     * @param lastEventIndex last column index that describes events
     * @param isAccumulativeCSV if isAccummulative id true, then the last
     *                          column are probability-values
     */
    CondEvent(const CSVAnalyzer &csv, size_t row = 0, size_t lastEventIndex = 0, bool isAccumulativeCSV = false);

    /**
     * Copy-construct CondEvent.
     *
     * @param rhs right-hand-side condition event
     */
    CondEvent(const CondEvent &rhs) = default;

    /**
     * Assign right-hand-side to this.
     *
     * @param rhs right-hand-side condition event
     */
    CondEvent &operator=(const CondEvent &rhs) = default;

    /**
     * Check for emptiness.
     *
     * @return true if is empty, false otherwise
     */
    [[nodiscard]] bool empty() const;

    /**
     * Retrieve the number of events (not conditions) in this CondEvent.
     *
     * @return the number of events
     */
    [[nodiscard]] size_t eventSize() const;

    /**
     * Retrieve the number of conditions (not events) in this CondEvent.
     *
     * @return the number of conditions
     */
    [[nodiscard]] size_t conditionSize() const;

    /**
     * Retrieve the event-part.
     *
     * @return the events list
     */
    [[nodiscard]] const EventCatenation &event() const;

    /**
     * Retrieve the condition-part.
     *
     * @return the condition list
     */
    [[nodiscard]] const EventCatenation &condition() const;

    /**
     * Filter out conditions with names in the list "conds".
     *
     * @param conds the list of conditions to filter out
     *
     * @return the filtered condition event
     */
    [[nodiscard]] CondEvent filterConditions(const std::set<std::string> &conds) const;

    /**
     * Check whether the right-hand-side is a match to this.
     * @param ce the condition event to match against
     * @return true if matching, false otherwise
     */
    [[nodiscard]] bool isMatch(const CondEvent &ce) const;

    /**
     * Check whether the condition-part contains an event named name.
     *
     * @param name the event name to check
     *
     * @return true if a condition of this name is contained, false else
     */
    [[nodiscard]] bool containsCondition(const std::string &name) const;

    /**
     * Apply the chain rule of probability. Result in referenced parameter.
     * one application of the chain rule
     * iteratively use this to reduce probabilities to a list where all
     * probabilities are of the form
     * P(E|C1,C2,....,Cn) with E and C_i 1&le; i &le; n all single events
     *
     * P(A,B) = P(B|A)P(A)
     * P(An,An-1,...,A1)=P(An|An-1,...,A1)P(An-1,...,A1)
     * P(A4,A3,A2,A1)=P(A4|A3,A2,A1)P(A3|A2,A1)P(A2|A1)P(A1)
     *
     * P(A3,A2,A1|B1,B2,B3)=P(A1|A2,A3,B1,B2,B3)P(A2,A3,B1,B2,B3)
     *
     * precondition:
     * <ul>
     *  <li> CondEvent list is empty *OR*</li>
     *  <li> all but (possibly) the last are already of the required form</li>
     * </ul>
     *
     * @param cel the result of the application of the chain-rule
     * @param name the name of the event for which to apply the chain rul
     * @return true if success, false otherwise
     */
    bool chainRule(CONDEVENT_LIST &cel, const std::string &name) const;

    /**
     * Apply the chain rule of probability. Result in referenced parameter.
     * one application of the chain rule
     * iteratively use this to reduce probabilities to a list where all
     * probabilities are of the form
     * P(E|C1,C2,....,Cn) with E and C_i 1&le; i &le; n all single events
     *
     * P(A,B) = P(B|A)P(A)
     * P(An,An-1,...,A1)=P(An|An-1,...,A1)P(An-1,...,A1)
     * P(A4,A3,A2,A1)=P(A4|A3,A2,A1)P(A3|A2,A1)P(A2|A1)P(A1)
     *
     * P(A3,A2,A1|B1,B2,B3)=P(A1|A2,A3,B1,B2,B3)P(A2,A3,B1,B2,B3)
     *
     * precondition:
     * <ul>
     *  <li> CondEvent list is empty *OR*</li>
     *  <li> all but (possibly) the last are already of the required form</li>
     * </ul>
     *
     * @param cel the result of the application of the chain-rule
     * @param nameList the names of  events for which to apply the chain rule
     *
     * @return true if success, false otherwise
     */
    bool chainRule(CONDEVENT_LIST &cel, const std::vector<std::string> &nameList) const;

    /**
     * Append an event to event-part.
     *
     * @param el the event to append
     *
     * @return the new condition event
     */
    CondEvent &operator&&(const Event &el);

    /**
     * Append an event to the condition-part.
     *
     * @param el the condition to append
     *
     * @return the new condition event
     */
    CondEvent &operator||(const Event &el);

    /**
     * Check whether the event-part contains an Event named name.
     *
     * @param name the name of the event to check
     *
     * @return true, if the events contain it, false otherwise
     */
    [[nodiscard]] bool hasEvent(const std::string &name) const;

    /**
     * Check whether the condition-part contains an Event named name.
     *
     * @param name the name of the condition to check
     *
     * @return true, if the conditions contain it, false otherwise
     */
    [[nodiscard]] bool hasCondition(const std::string &name) const;

    /**
     * Equality operator. Needed in order to to enable associative containers.
     *
     * @param lhs left-hand-side condition event
     * @param rhs right-hand-side condition event
     *
     * @return true if equal, false otherwise
     */
    friend bool operator==(const CondEvent &lhs, const CondEvent &rhs);

    /**
     * Less-than operator. Needed in order to to enable associative containers.
     *
     * @param lhs left-hand-side condition event
     * @param rhs right-hand-side condition event
     *
     * @return true if lhs less than rhs, false otherwise
     */
    friend bool operator<(const CondEvent &lhs, const CondEvent &rhs);

    /**
     * Generic ostream - &lh;&lt; operator for CondEvent.
     *
     * @param os the output stream
     * @param ce the condition event to stream
     *
     * @return the modified stream
     */
    friend std::ostream &operator<<(std::ostream &os, const CondEvent &ce);

    protected:
    private:
    EventCatenation eList_;     ///< List of Events interpreted as events.
    EventCatenation condList_;  ///< List of Events interpreted as conditions.
};

/**
 * Global operator that creates a Condition Event from two EventLists.
 * @param lhs left-hand-side event list
 * @param rhs right-hand-side event list
 * @return condition event ( P(lhs | rhs) )
 */
CondEvent operator||(const EventCatenation &lhs, const EventCatenation &rhs);

using VALUERANGES_TYPE = std::map<std::string, EventValueRange>;
class CSVAnalyzer;

/**
 * Abstract base class for floating point probability functions.
 */
struct ProbabilityFunction
{
    /**
     * Default construct.
     *
     * @param eventValueRanges ranges for the events
     * @param conditionValueRanges ranges for the conditions
     */
    ProbabilityFunction(VALUERANGES_TYPE eventValueRanges     = VALUERANGES_TYPE(),
                        VALUERANGES_TYPE conditionValueRanges = VALUERANGES_TYPE());

    virtual ~ProbabilityFunction() = default;

    /**
     * Clone a copy of this.
     *
     * @return a pointer to the clone
     */
    [[nodiscard]] virtual ProbabilityFunction *clone() const = 0;

    /**
     * Check whether a condition-event is compatible with this probability
     * function.
     *
     * @param ce the condition event to check
     * @param error a reference that will be populated by error-message if needed
     *
     * @return true, if compatible, false otherwise
     */
    bool possibleCondEvent(const CondEvent &ce, std::string &error) const;

    /**
     * Probability of a conditional event.
     *
     * @param ce the condition event for which to calculate the probability
     *
     * @return the probability
     */
    [[nodiscard]] virtual long double P(const CondEvent &ce) const = 0;

    /**
     * Delegate Probability from EventList to avoid excessive casting.
     *
     * @param el the event-list for which to calculate the probability
     *
     * @return the probability
     */
    [[nodiscard]] virtual long double P(const EventCatenation &el) const = 0;

    /**
     * Does the function satisfy probability requirements?
     * @return true, if so, false otherwise
     */
    [[nodiscard]] virtual bool isDistribution() const
    {
        return (true);
    }

    /**
     * Reset the parameters of the probability function.
     */
    virtual void clear() = 0;

    /**
     * Train (estimate) the parameters of of the probability function.
     *
     * @param csv a csv-analyzer to use as input
     * @param isAccumulativeCSV if true, then there is a column with
     *                          probabilities
     *
     * @return success
     */
    virtual bool train(CSVAnalyzer csv, bool isAccumulativeCSV = false) = 0;

    /**
     * Add a Variant-value to the range of possible event-values.
     *
     * @param name event name
     * @param val value to be added to the range
     *
     * @return success
     */
    bool addValueToEventRange(const std::string &name, const Var &val);

    /**
     * Add a Variant-value to the range of possible condition-values.
     *
     * @param name condition name
     * @param val value to be added to the range
     *
     * @return success
     */

    bool addValueToConditionRange(const std::string &name, const Var &val);

    protected:
    /**
     * Get the index up to which we interpret as event-Events (as opposed to
     * condition-Events). Needed when we have a list like we get it from a csv.
     *
     * @return the last event index
     */
    [[nodiscard]] size_t getLastEventIndex() const;

    /**
     * Number of Conditions. Use in combination with getLastEventIndex() to
     * split a yet un-discriminated list into event and condition parts.
     *
     * @return the number of conditions
     */
    [[nodiscard]] size_t getNumberOfConditions() const;

    /**
     * Add a value to a range (either event or condition). Checks validity
     * in order to guard against nonsensical inserts.
     * event-events and condition-events need must not be mutually exclusive.
     *
     * @param range a range of values
     * @param range_ortho a disjunct (orthogonal) range of values
     * @param name event name
     * @param value event value
     *
     * @return success
     */
    bool             addValidValueToRange(VALUERANGES_TYPE  &range,
                                          VALUERANGES_TYPE  &range_ortho,
                                          const std::string &name,
                                          const Var         &value);
    VALUERANGES_TYPE eventValueRanges_;      ///< Map of ranges for events.
    VALUERANGES_TYPE conditionValueRanges_;  ///< Map of ranges for conditions.
};

/**
 * Uniform probability function on real (floating point) domain.
 */
class UniformFloatFunction : public ProbabilityFunction
{
    public:
    /**
     * Storage for a set of parameters for a Uniform function (min/max).
     */

    struct UNIF_PARAM
    {
        UNIF_PARAM(VAR_FLOAT l = 0.0L, VAR_FLOAT h = 1.0L) : low(l), high(h), occurrences(0.0L)
        {
        }

        long double low;
        long double high;
        long double occurrences;
    };

    typedef std::map<EventCatenation, UNIF_PARAM> UNIF_PARAM_TABLE;

    /**
     * Default construct.
     *
     * @param minVal minimal value of the range
     * @param maxVal maximal value of the range
     * @param conditionValueRanges ranges for the conditional events
     */
    UniformFloatFunction(VAR_FLOAT               minVal               = 0.0L,
                         VAR_FLOAT               maxVal               = 1.0L,
                         const VALUERANGES_TYPE &conditionValueRanges = VALUERANGES_TYPE());

    /**
     * Clone a copy of this.
     * @return a pointer to the clone
     */
    [[nodiscard]] ProbabilityFunction *clone() const override;

    /**
     * Probability of an interval. Returns zero if the Event is not an
     * interval-event.
     *
     * @param ce the condition event to check
     *
     * @return the probability
     */
    [[nodiscard]] long double P(const CondEvent &ce) const override;

    /**
     * Delegate Probability from EventList to avoid excessive casting if we
     * are interested in an unconditional joint probability.
     *
     * @param ce the condition event to check
     *
     * @return the probability
     */
    [[nodiscard]] long double P(const EventCatenation &el) const override;

    /**
     * Reset the parameters of the probability function.
     */
    void clear() override;

    /**
     * Train (estimate) the parameters of of the probability function.
     *
     * @param csv csv-analyzer as input
     * @param isAccumulativeCSV if true then has an already accumulated
     *                           probability column
     *
     * @return success
     */
    bool train(CSVAnalyzer csv, bool isAccumulativeCSV = false) override;

    UNIF_PARAM_TABLE param_;  ///< Maps conditions to min-max-values.

    /**
     * Generic ostream - &lh;&lt; operator for UniformFloatFunction.
     *
     * @param os the output stream
     * @param uff the uniform float function to stream
     *
     * @return the modified out-stream
     */
    friend std::ostream &operator<<(std::ostream &os, const UniformFloatFunction &uff);
};

/**
 * Gaussian bell curve probability function.
 */
class GaussFunction : public ProbabilityFunction
{
    public:
    /**
     * Storage for a set of parameters for a Gaussian function.
     */
    struct GAUSS_PARAM
    {
        /**
         * Default construct.
         *
         * @param m mu
         * @param s sigma
         */
        GAUSS_PARAM(VAR_FLOAT m = 0.0L, VAR_FLOAT s = 0.0L) : mu(m), sigma(s), occurrences(0.0L)
        {
        }

        long double mu;
        long double sigma;
        long double occurrences;
    };
    using GAUSS_PARAM_TABLE = std::map<EventCatenation, GAUSS_PARAM>;

    /**
     * Default construct.
     *
     * @param mu expectation
     * @param sigma variance
     */
    GaussFunction(long double mu = 0.0L, long double sigma = 1.0L);

    /**
     * Clone a copy of this.
     *
     * @return a pointer to the clone
     */
    [[nodiscard]] ProbabilityFunction *clone() const override
    {
        return (new GaussFunction(*this));
    }

    /**
     * Reset the parameters mu and sigma to standard normal values.
     */
    void clear() override;

    /**
     * Probability of an interval. Returns zero if the Event is not an
     * interval-event.
     *
     * @param ce the condition event to check
     *
     * @return the probability
     */
    [[nodiscard]] long double P(const CondEvent &ce) const override;

    /**
     * Delegate Probability from EventList to avoid excessive casting if we
     * are interested in an unconditional joint probability.
     *
     * @param eventList the condition event to check
     *
     * @return the probability
     */
    [[nodiscard]] long double P(const EventCatenation &eventList) const override;

    /**
     * Estimate mu and sigma.
     * <ul>
     * <li> mu ~ sum(x)/numberOf(x)</li>
     * <li> sigma ~ sum((x-mu)^2\)/numberOf(x)</li>
     * </ul>
     *
     * @param csv csv-analyzer as input
     * @param isAccumulativeCSV if true then has an already accumulated
     *                           probability column
     *
     * @return success
     */
    bool train(CSVAnalyzer csv, bool isAccumulativeCSV) override;

    /**
     * Retrieve expectation mu.
     *
     * @param ce the condition event for which to get the expectation
     *
     * @return expectation
     */
    [[nodiscard]] long double mu(const CondEvent &ce = CondEvent()) const;

    /**
     * Retrieve variance.
     *
     * @param ce the condition event for which to get the variance
     *
     * @return variance
     */
    [[nodiscard]] long double sigma(const CondEvent &ce = CondEvent()) const;

    /**
     * Generic ostream - &lh;&lt; operator for GaussFunction.
     *
     * @param os the output stream
     * @param gf the gauss function to stream
     *
     * @return the modified out-stream
     */
    friend std::ostream &operator<<(std::ostream &os, const GaussFunction &gf);

    private:
    GAUSS_PARAM_TABLE param_;  ///< Maps conditions to Gauss-parameters.
};

/**
 * Exponential probability function.
 */
class ExponentialFunction : public ProbabilityFunction
{
    public:
    /**
     * Storage for a set of parameters for an exponential function.
     */
    struct EXP_PARAM
    {
        EXP_PARAM(long double l = 1.0L) : lambda(l), occurrences(0.0L)
        {
        }

        long double lambda;
        long double occurrences;
    };

    using EXP_PARAM_TABLE = std::map<EventCatenation, EXP_PARAM>;

    /**
     * Default construct.
     * @param lambda expectation
     */
    ExponentialFunction(VAR_FLOAT lambda = 1.0L);

    /**
     * Clone a copy of this.
     *
     * @return a pointer to the clone
     */
    [[nodiscard]] ProbabilityFunction *clone() const override
    {
        return (new ExponentialFunction(*this));
    }

    /**
     * Reset the expectation to 1.0.
     */
    void clear() override;

    /**
     * Probability of an interval. Returns zero if the Event is not an
     * interval-event.
     *
     * @param ce the condition event to check
     *
     * @return the probability
     */
    [[nodiscard]] long double P(const CondEvent &ce) const override;

    /**
     * Delegate Probability from EventList to avoid excessive casting if we
     * are interested in an unconditional joint probability.
     *
     * @param ce the condition event to check
     *
     * @return the probability
     */
    [[nodiscard]] long double P(const EventCatenation &el) const override;

    /**
     * Estimate lambda for first column (must be float). Last column might be
     * an accumulative column - all others are condition.
     * <ul>
     * <li> lambda ~ sum(x)/numberOf(x),/li>
     * </ul>
     * @param csv csv-analyzer as input
     * @param isAccumulativeCSV if true then has an already accumulated
     *                           probability column
     *
     * @return success
     */
    bool train(CSVAnalyzer csv, bool isAccumulativeCSV) override;

    /**
     * Retrieve the expectation lambda.
     */
    [[nodiscard]] long double lambda(const CondEvent &ce = CondEvent()) const;

    /**
     * Get the point where the cdf == 1/2.
     * P(0.0) &le; x &le; ln(2)/lambda_) = 0.5
     */
    [[nodiscard]] long double ln2ByLambda(const CondEvent &ce = CondEvent()) const;

    /**
     * Generic ostream - &lh;&lt; operator for ExponentialFunction.
     *
     * @param os the output stream
     * @param gf the exponential function to stream
     *
     * @return the modified out-stream
     */
    friend std::ostream &operator<<(std::ostream &os, const ExponentialFunction &d);

    private:
    EXP_PARAM_TABLE param_;  ///< Maps conditions to Exponential-parameters.
};

/**
 * Helper for accumulation of discrete probability table.
 * Keeps track of how many values n where used to create sum s.
 */
struct ACCUMULATION_DATA
{
    /**
     * Construct with initial sum = 0.0 and number = 0
     */
    ACCUMULATION_DATA(long double s = 0L, long double n = 0L) : sum(s), number(n)
    {
    }

    long double sum;
    long double number;
};

using ACCUMULATION_MAP = std::map<EventCatenation, ACCUMULATION_DATA>;

/**
 * Discrete probability function that enumerates value-probability-pairs.
 */
class DiscreteProbability : public ProbabilityFunction
{
    public:
    using PROB_TABLE = std::map<CondEvent, long double>;

    /**
     * Default construct.
     *
     * @param eventValueRanges ranges for the events
     * @param conditionValueRanges ranges for the conditions
     */
    DiscreteProbability(const VALUERANGES_TYPE &eventValueRanges     = VALUERANGES_TYPE(),
                        const VALUERANGES_TYPE &conditionValueRanges = VALUERANGES_TYPE());

    /**
     * Clone a copy of this.
     *
     * @return a pointer to the clone
     */
    ProbabilityFunction *clone() const override;

    /**
     * Set an equal probability for every event-value.
     *
     * @return success
     */
    bool makeUniform();

    /**
     * Make sure that probability values add up to 1.0.
     *
     * @return success
     */
    bool normalise();

    /**
     * Add probability values for all possible event-value combinations.
     *
     * @return success
     */
    bool canonise();

    /**
     * Check whether the probabilities add up to 1.0.
     *
     * @return true if so, false otherwise
     */
    bool isDistribution() const override;

    /**
     * Reset the distribution.
     */
    void clear() override;

    /**
     * Check whether the distribution is empty.
     *
     * @return true if so, false otherwise
     */
    bool empty() const;

    /**
     * Probability of an conditional event.
     *
     * @param ce the condition event to check
     *
     * @return the probability
     */
    long double P(const CondEvent &ce) const override;

    /**
     * Delegate Probability from EventList to avoid excessive casting if we
     * are interested in an unconditional joint probability.
     *
     * @param ce the condition event to check
     *
     * @return the probability
     */
    long double P(const EventCatenation &el) const override
    {
        return (P(CondEvent(el)));
    }

    /**
     * Estimate the probability function using a csv.
     * <ul>
     * <li> lastEventIndex is the last column that is not a condition</li>
     * <li> if lastEventIndex ==  x  columns x+1, x+2, x+3, ... are conditions</li>
     * </ul>
     *
     * @param csv csv-analyzer as input
     * @param isAccumulativeCSV if true then has an already accumulated
     *                           probability column
     *
     * @return success
     */
    bool train(CSVAnalyzer csv, bool isAccumulativeCSV = false) override;

    /**
     * Generic ostream - &lh;&lt; operator for DiscreteProbability.
     *
     * @param os the output stream
     * @param gf the discrete function to stream
     *
     * @return the modified out-stream
     */
    friend std::ostream &operator<<(std::ostream &os, const DiscreteProbability &d);

    /**
     * Reset the distribution.
     */
    void resetDistribution()
    {
        isUniform_       = false;
        hasBeenModified_ = false;
        clear();
    }

    /**
     * Return true if the distribution values have uniform probability.
     * @return true if so, false otherwise
     */
    bool isUniform() const
    {
        return (isUniform_);
    }

    /**
     * Check whether the distribution table has been modified.
     * @return true if so, false otherwise
     */
    bool isModified() const
    {
        return (hasBeenModified_);
    }

    protected:
    /**
     * Use the values (obtained by training) to update the ranges the
     * variables can assume. Don't remove any existing values.
     *
     * @param clearFirst if thro then reset range first
     */
    void updateValueRangesFromValues_(bool clearFirst = false);

    /**
     * Set the distribution to uniform. Don't update the table values.
     *
     * @param uni set to uniform (true) or not (false)
     */
    void setUniform(bool uni)
    {
        isUniform_ = uni;
    }

    /**
     * Set the state to modified to indicate to users that normalization
     * has to happen before probability calculations can take place.
     * @param mod set to modified (true) or not (false)
     */
    void setModified(bool mod) const
    {
        hasBeenModified_ = mod;
    }

    private:
    bool         isUniform_{false};
    mutable bool hasBeenModified_{false};
    PROB_TABLE   values_;
};
};  // namespace util

#endif  // NS_UTIL_STATUTIL_H_INCLUDED
