/*
 * File Name:   statutil.cc
 * Description: statistic utility functions
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

#include <statutil.h>
#include <stdexcept>

namespace util
{
    using namespace std;
    using namespace boost;

    /**
     *  Error handling for event lists with mutually exclusive events.
     */
    eventlist_conflict_error::eventlist_conflict_error(
                                                       eventlist_conflict_error::conflict_type tp,
                                                       const EventList& e1)
    : std::logic_error(string(tp == eventlist_conflict_error::evt ? "Event-list " : "Condition-list")
                       + asString(e1)
                       + " has conflicting events")
    {
    }

    eventlist_conflict_error::eventlist_conflict_error(
                                                       const EventList& e1,
                                                       const EventList& e2)
    : std::logic_error("Event-list "
                       + asString(e1)
                       + " and Condition-list "
                       + asString(e2)
                       + " have conflicting events")
    {
    }

    /**
     *  Construct a boolean range (or not if haveBoolRange == false).
     */
    EventValueRange::EventValueRange(bool haveBoolRange)
    : values_()
    , type_(discrete)
    {
        if (haveBoolRange)
        {
            insert(false);
            insert(true);
        }
    }

    /**
     *  Construct a range of enumerated character values.
     */
    EventValueRange::EventValueRange(const set<VAR_CHAR>& values)
    : values_()
    , type_(discrete)
    {
        for (auto it = values.begin();
             it != values.end();
             it++)
        {
            insert(*it);
        }
    }

    /**
     *  Construct a range of enumerated signed integer values.
     */
    EventValueRange::EventValueRange(const set<VAR_INT>& values)
    : values_()
    , type_(discrete)
    {
        for (auto it = values.begin(); it != values.end(); it++)
        {
            insert(*it);
        }
    }

    /**
     *  Construct a range of enumerated unsigned integer values.
     */
    EventValueRange::EventValueRange(const set<VAR_UINT>& values)
    : values_()
    , type_(discrete)
    {
        for (auto it = values.begin(); it != values.end(); it++)
        {
            insert(*it);
        }
    }

    /**
     *  Construct a range of enumerated date values.
     */
    EventValueRange::EventValueRange(const set<VAR_DATE>& values)
    : values_()
    , type_(discrete)
    {
        for (auto it = values.begin(); it != values.end(); it++)
        {
            insert(*it);
        }
    }

    /**
     *  Construct a range of enumerated string values.
     */
    EventValueRange::EventValueRange(const set<VAR_STRING>& values)
    : values_()
    , type_(discrete)
    {
        for (auto it = values.begin(); it != values.end(); it++)
        {
            insert(*it);
        }
    }

    /**
     *  Construct a range of enumerated character values as an interval from lowest to highest.
     */
    EventValueRange::EventValueRange(VAR_CHAR lowest, VAR_CHAR highest)
    : values_()
    , type_(discrete)
    {
        addRange(lowest, highest);
    }

    /**
     *  Construct a range of enumerated signed integer values as an interval from lowest to highest.
     */
    EventValueRange::EventValueRange(VAR_INT lowest, VAR_INT highest)
    : values_()
    , type_(discrete)
    {
        addRange(lowest, highest);
    }

    /**
     *  Construct a range of enumerated unsigned integer values as an interval from lowest to highest.
     */
    EventValueRange::EventValueRange(VAR_UINT lowest, VAR_UINT highest)
    : values_()
    , type_(discrete)
    {
        addRange(lowest, highest);
    }

    /**
     *  Construct a range of continuous uniform floating point values as an interval from lowest to highest.
     */
    EventValueRange::EventValueRange(VAR_FLOAT lowest, VAR_FLOAT highest)
    : values_()
    , type_(float_uniform)
    {
        addRange(lowest, highest);
    }

    /**
     *  Construct a range of continuous uniform floating point values as an interval.
     */
    EventValueRange::EventValueRange(VAR_FLOAT_INTERVAL interval)
    : values_()
    , type_(float_uniform)
    {
        addRange(interval.low_, interval.high_);
    }

    /**
     *  Construct a range of continuous for distribution-type.
     */
    EventValueRange::EventValueRange(DistributionType tp)
    : values_()
    , type_(tp)
    {
        if (tp == exponential)
        {
            VAR_FLOAT_INTERVAL interval(0.0L, ABOVE);
            addRange(interval.low_, interval.high_);
        }
        else if (tp == gaussian)
        {
            VAR_FLOAT_INTERVAL interval;
            addRange(interval.low_, interval.high_);
        }
    }

    /**
     *  Get the size of the range. Continuous ranges are always 0 or 2 (lowest,highest)
     */
    size_t EventValueRange::size() const
    {
        return values_.size();
    }

    /**
     *  Check whether the range is continuous
     */
    bool EventValueRange::isContinuous() const
    {
        return type_ != discrete;
    }

    /**
     *  Retrieve the type of distribution for this range.
     */

    EventValueRange::DistributionType EventValueRange::type() const
    {
        return type_;
    }

    /**
     *  check whether any particular value is valid, i.e. it has been added to
     *  the range previously or in case of float whether the value is in the
     *  interval
     */
    bool EventValueRange::validValue(const Var& value) const
    {
        if (isContinuous())
        {
            if (!isA<VAR_FLOAT>(value))
                return false;
            VAR_FLOAT f = toNative<VAR_FLOAT>(value);
            if (values_.size() == 0)
                return true;
            if (values_.size() != 2)
                throw event_range_error(f, values_.size());
            auto it = values_.begin();
            VAR_FLOAT v1 = toNative<VAR_FLOAT>(*it);
            it++;
            VAR_FLOAT v2 = toNative<VAR_FLOAT>(*it);
            return v1 <= f && f <= v2;
        }
        return values_.find(value) != values_.end();
    }

    /**
     *  Add a value if the type is valid.
     */
    bool EventValueRange::add(const Var& val)
    {
        if (validType(val))
            values_.insert(val);
        else
            return false;
        return true;
    }

    /**
     * if no value has been added to the range yet then any Var is valid
     * otherwise all have to be of the same type
     */
    bool EventValueRange::validType(const Var& value) const
    {
        return values_.empty() || (values_.begin()->type() == value.type());
    }

    /**
     * create a set of events by conjugating the name with all possible
     * values with the name. Helper for the creation of canonised (full)
     * probability table
     */
    EVENT_SET EventValueRange::makeEventSet(const string& name) const
    {
        EVENT_SET reval;
        for (auto it = values_.begin(); it != values_.end(); it++)
        {
            reval.insert(Event(name, *it, true));
        }
        return reval;
    }

    /**
     * Event value range streaming operator.
     */
    ostream& operator<<(ostream& os, const EventValueRange& evr)
    {
        os << evr.values_;
        return os;
    }

    // static member initialization
    Equals Event::equals;
    Less Event::less;
    LessEqual Event::lessEqual;
    Greater Event::greater;
    GreaterEqual Event::greaterEqual;
    IsElementOf Event::isElementOf;
    PlaceHolderOp Event::placeHolderOp;

    /**
     * Construct empty Event. Default-operation: Placeholder - returns false.
     */
    Event::Event(Operation* op)
    : name_("")
    , value_()
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    /**
     * Construct boolean Event. Default-operation: Equality.
     */
    Event::Event(const string& name, VAR_BOOL b, Operation* op)
    : name_(name)
    , value_(b)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    /**
     * Construct character Event. Default-operation: Equality.
     */
    Event::Event(const string& name, VAR_CHAR c, Operation* op)
    : name_(name)
    , value_(c)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_INT i, Operation* op)
    : name_(name)
    , value_(i)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_UINT u, Operation* op)
    : name_(name)
    , value_(u)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_FLOAT f, Operation* op)
    : name_(name)
    , value_(f)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_DATE d, Operation* op)
    : name_(name)
    , value_(d)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_STRING s, Operation* op)
    : name_(name)
    , value_(s)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_BOOL_INTERVAL itvl, Operation* op)
    : name_(name)
    , value_(itvl)
    , operation_(op ? op : &Event::isElementOf)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_CHAR_INTERVAL itvl, Operation* op)
    : name_(name)
    , value_(itvl)
    , operation_(op ? op : &Event::isElementOf)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_INT_INTERVAL itvl, Operation* op)
    : name_(name)
    , value_(itvl)
    , operation_(op ? op : &Event::isElementOf)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_UINT_INTERVAL itvl, Operation* op)
    : name_(name)
    , value_(itvl)
    , operation_(op ? op : &Event::isElementOf)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_FLOAT_INTERVAL itvl, Operation* op)
    : name_(name)
    , value_(itvl)
    , operation_(op ? op : &Event::isElementOf)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, VAR_DATE_INTERVAL itvl, Operation* op)
    : name_(name)
    , value_(itvl)
    , operation_(op ? op : &Event::isElementOf)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const string& name, const char* s, Operation* op)
    : name_(name)
    , value_(VAR_STRING(s ? s : ""))
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
    }

    Event::Event(const Event& rhs)
    : name_(rhs.name_)
    , value_(rhs.value_)
    , operation_(rhs.operation_)
    , isPlaceHolder_(rhs.isPlaceHolder_)
    {
    }

    /**
     * extra dummy parameter dummyConfirm to allow Var as parameter
     */
    Event::Event(const string& name, const Var& a, bool dummyConfirm, Operation* op)
    : name_(name)
    , value_(a)
    , operation_(op ? op : &Event::equals)
    , isPlaceHolder_(false)
    {
        if (!dummyConfirm)
            throw event_error("Event can only be created if dummy confirm "
                    "parameter is set to true!");
    }

    Event& Event::operator=(const Event& rhs)
    {
        if (&rhs != this)
        {
            name_ = rhs.name_;
            value_ = rhs.value_;
            operation_ = rhs.operation_;
            isPlaceHolder_ = rhs.isPlaceHolder_;
        }
        return *this;
    }

    Event& Event::operator()(const string& name, const Var& value, Operation* op)
    {
        name_ = name;
        value_ = value;
        operation_ = op ? op : &Event::equals;
        isPlaceHolder_ = false;
        return *this;
    }

    Event Event::placeholderEvent(const string& name)
    {
        Event reval(&Event::placeHolderOp);
        reval.name_ = name;
        reval.isPlaceHolder_ = true;
        return reval;
    }

    bool Event::isPlaceholder() const
    {
        return isPlaceHolder_;
    }

    bool Event::empty() const
    {
        return !isPlaceholder() &&
                (name_.empty() || value_.empty() || operation_ == 0);
    }

    bool Event::notConflicting(const Event& e) const
    {
        return name() != e.name() ||
                (value_ == e.value_ && typeid (operation_) == typeid (e.operation_)) ||
                (isPlaceholder() && e.isPlaceholder());
    }

    string Event::name() const
    {
        return name_;
    }

    Var Event::varValue() const
    {
        return value_;
    }

    string Event::opDesc() const
    {
        return operation_ ? operation_->desc(value_) : string("");
    }

    bool Event::matches(const Event& e) const
    {
        return name() == e.name() &&
                e.operation_->leftMatchesRight(varValue(), e.varValue());
    }

    bool operator==(const Event& lhs, const Event& rhs)
    {
        return lhs.name_ == rhs.name_ &&
                lhs.value_ == rhs.value_ &&
                typeid (lhs.operation_) == typeid (rhs.operation_);
    }

    bool operator<(const Event& lhs, const Event& rhs)
    {
        return lhs.name_ < rhs.name_ ||
                (lhs.name_ == rhs.name_ && lhs.value_ < rhs.value_) ||
                (lhs.name_ == rhs.name_ && lhs.value_ == rhs.value_ &&
                 string(typeid (lhs.operation_).name()) <
                 string(typeid (rhs.operation_).name()));
    }

    ostream& operator<<(ostream& os, const Event& e)
    {
        if (e.isPlaceholder())
            os << e.name_ << "=[_]";
        else
            os << e.name_ << e.opDesc();
        return os;
    }

    EventList::EventList()
    : evts_()
    {
    }

    EventList::EventList(const Event& e)
    : evts_()
    {
        if (!e.empty())
            evts_.insert(e);
    }

    EventList::EventList(const EventList& rhs)
    : evts_(rhs.evts_)
    {
    }

    EventList& EventList::operator=(const EventList& rhs)
    {
        if (&rhs != this)
        {
            evts_ = rhs.evts_;
        }
        return *this;
    }

    EventList& EventList::operator&&(const Event& e)
    {
        if (!e.empty())
            evts_.insert(e);
        return *this;
    }

    EventList& EventList::operator&&(const EventList& el)
    {
        for (auto it = el.cbegin(); it != el.cend(); it++)
            * this && *it;
        return *this;
    }

    bool EventList::empty() const
    {
        return evts_.empty();
    }

    size_t EventList::size() const
    {
        return evts_.size();
    }

    /**
     * two EventLists are notConflicting if their Events are pairwise notConflicting
     */
    bool EventList::notConflicting(const EventList& eList) const
    {
        bool reval = true;
        auto el1It = eList.cbegin();
        while (reval && el1It != eList.cend())
        {
            auto el2It = cbegin();
            while (reval && el2It != cend())
            {
                if (!el1It->notConflicting(*el2It))
                {
                    reval = false; // stop on the first conflict
                }
                el2It++;
            }
            el1It++;
        }
        return reval;
    }

    bool EventList::matches(const EventList& eList) const
    {
        bool reval = size() == eList.size();
        auto itThis = cbegin();
        auto itThat = eList.cbegin();
        for (; reval && itThis != cend(); itThis++, itThat++)
        {
            reval &= itThis->matches(*itThat);
        }
        return reval;
    }

    Event EventList::eventByName(const string& name) const
    {
        for (auto it = cbegin(); it != cend(); it++)
            if (it->name() == name)
                return *it;
        return Event();
    }

    bool EventList::moveEvent(const string& name, EventList& el)
    {
        bool hasBeenMoved = false;
        for (auto it = cbegin(); !hasBeenMoved && it != cend(); it++)
            if (it->name() == name)
            {
                el && *it;
                evts_.erase(it);
                hasBeenMoved = true;
            }

        return hasBeenMoved;
    }

    bool EventList::hasEvent(const string& e) const
    {
        for (auto it = evts_.begin(); it != evts_.end(); it++)
            if (it->name() == e)
                return true;
        return false;
    }

    EventList::EVENT_CONTAINER_ITER EventList::begin()
    {
        return evts_.begin();
    }

    EventList::EVENT_CONTAINER_CITER EventList::cbegin() const
    {
        return evts_.begin();
    }

    EventList::EVENT_CONTAINER_ITER EventList::end()
    {
        return evts_.end();
    }

    EventList::EVENT_CONTAINER_CITER EventList::cend() const
    {
        return evts_.end();
    }

    void EventList::erase(EVENT_CONTAINER_ITER it)
    {
        evts_.erase(it);
    }

    bool operator==(const EventList& lhs, const EventList& rhs)
    {
        return lhs.evts_ == rhs.evts_;
    }

    /**
     * check from left to right on the first difference we can decide &lt; or &gt;
     * if there is no difference then the lists are equal one of the lists is
     * wholly contained in the other. In that case the shorter list is "less"
     * than the longer list
     */
    bool operator<(const EventList& lhs, const EventList& rhs)
    {
        bool reval = true;
        auto itLhs = lhs.cbegin();
        auto itRhs = rhs.cbegin();
        bool foundDiff = false;
        for (; itLhs != lhs.cend() && itRhs != rhs.cend(); itLhs++, itRhs++)
        {
            // all left Events are equal because we break at first difference
            if (*itRhs < *itLhs) // rhs is smaller so we are *BIGGER* -> return false
            {
                foundDiff = true;
                reval = false;
                break;
            }
            if (*itLhs < *itRhs) // lhs is smaller so we are smaller -> return true
            {
                foundDiff = true;
                break;
            }
        }

        // lhs is wholly contained in rhs, if rhs is longer then lhs is smaller
        // otherwise rhs is smaller
        if (!foundDiff)
            reval = lhs.size() < rhs.size();

        return reval;
    }

    ostream& operator<<(ostream& os, const EventList& eList)
    {
        size_t i = 0;
        size_t maxIndex = eList.evts_.size() - 1;
        for (auto it = eList.evts_.begin(); it != eList.evts_.end(); it++, i++)
        {
            os << *it << (i < maxIndex ? ", " : "");
        }
        return os;
    }

    EventList operator&&(const Event& lhs, const Event& rhs)
    {
        EventList reval(lhs);
        reval && rhs;
        return reval;
    }

    CondEvent::CondEvent(const EventList& eList, const EventList& condList)
    : eList_(eList)
    , condList_(condList)
    {
        // The following errors prevent us from creating mis-formed CondEvents
        if (!eList_.notConflicting(eList_))
            throw eventlist_conflict_error(eventlist_conflict_error::evt, eList_);
        if (!condList_.notConflicting(condList_))
            throw eventlist_conflict_error(eventlist_conflict_error::cond, condList_);
        if (!eList_.notConflicting(condList_))
            throw eventlist_conflict_error(eList_, condList_);
    }

    CondEvent::CondEvent(const CSVAnalyzer& csv,
                         size_t row,
                         size_t lastEventIndex, // needs to at least 1 so index needs to be 0
                         bool isAccummulative)
    {
        // if isAccummulative id true, then the last column are prob-values
        size_t lastCondIndex = csv.columns() - ((isAccummulative) ? 2 : 1);
        for (size_t col = 0; col <= lastCondIndex; col++)
        {
            if (col <= lastEventIndex)
                eList_ && Event(csv.header(col), csv.getVar(col, row), true);
            else
                condList_ && Event(csv.header(col), csv.getVar(col, row), true);
        }
        if (!eList_.notConflicting(eList_))
            throw eventlist_conflict_error(eventlist_conflict_error::evt, eList_);
        if (!condList_.notConflicting(condList_))
            throw eventlist_conflict_error(eventlist_conflict_error::cond, condList_);
        if (!eList_.notConflicting(condList_))
            throw eventlist_conflict_error(eList_, condList_);
    }

    CondEvent::CondEvent(const CondEvent& rhs)
    : eList_(rhs.eList_)
    , condList_(rhs.condList_)
    {
    }

    CondEvent& CondEvent::operator=(const CondEvent& rhs)
    {
        if (&rhs != this)
        {
            eList_ = rhs.eList_;
            condList_ = rhs.condList_;
        }
        return *this;
    }

    bool CondEvent::empty() const
    {
        return eList_.empty();
    }

    size_t CondEvent::eventSize() const
    {
        return eList_.size();
    }

    size_t CondEvent::conditionSize() const
    {
        return condList_.size();
    }

    const EventList& CondEvent::event() const
    {
        return eList_;
    }

    const EventList& CondEvent::condition() const
    {
        return condList_;
    }

    /**
     * @TODO: this needs to change to cater for Intervals
     * conditions relevant are filtered out and missing ones are entered as
     * place-holders
     */
    CondEvent CondEvent::filterConditions(const set<string>& conds) const
    {
        EventList filteredConds;
        // first add all conditions that can be found in the filter-set (conds)
        // to the new filtered conditions
        for (auto it = condList_.cbegin(); it != condList_.cend(); it++)
        {
            if (conds.find(it->name()) != conds.end())
                filteredConds && *it;
        }

        // then add all conditions that are not present in the CondEvent
        // as place-holders
        for (auto nameIt = conds.begin(); nameIt != conds.end(); nameIt++)
        {
            if (!containsCondition(*nameIt))
                filteredConds && Event::placeholderEvent(*nameIt);
        }
        if (!eList_.notConflicting(filteredConds))
            throw eventlist_conflict_error(eList_, filteredConds);
        return CondEvent(eList_, filteredConds);
    }

    bool CondEvent::isMatch(const CondEvent& ce) const
    {
        return eList_.matches(ce.eList_) && condList_.matches(ce.condList_);
    }

    bool CondEvent::containsCondition(const string& name) const
    {
        for (auto it = condList_.cbegin(); it != condList_.cend(); it++)
        {
            if (it->name() == name)
                return true;
        }
        return false;
    }

    /**
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
     */
    bool CondEvent::chainRule(CondEvent::CONDEVENT_LIST& cel,
                              const string& name) const
    {
        if (cel.empty())
            cel.push_back(*this);
        bool reval = true;
        EventList newEl;
        EventList newCond = cel.back().condList_;
        for (auto it = cel.back().eList_.begin();
             cel.back().eList_.size() > 1 && it != cel.back().eList_.end();
             it++)
        {
            if (it->name() != name)
            {
                newCond && cel.back().eList_.eventByName(it->name());
                cel.back().eList_.moveEvent(it->name(), cel.back().condList_);
            }
            else
            {
                newEl && cel.back().eList_.eventByName(it->name());
            }
        }
        if (!newEl.empty())
            cel.push_back(CondEvent(newEl, newCond));

        return reval;
    }

    bool CondEvent::chainRule(CondEvent::CONDEVENT_LIST& cel,
                              const vector<string>& nameList) const
    {
        bool reval = true;
        cel.clear();
        for (auto it = nameList.rbegin(); it != nameList.rend(); it++)
        {
            chainRule(cel, *it);
        }

        return reval;
    }

    CondEvent& CondEvent::operator&&(const Event& el)
    {
        eList_ && el;
        return *this;
    }

    CondEvent& CondEvent::operator||(const Event& el)
    {
        condList_ && el;
        return *this;
    }

    bool CondEvent::hasEvent(const string& name) const
    {
        return eList_.hasEvent(name);
    }

    bool CondEvent::hasCondition(const string& name) const
    {
        return condList_.hasEvent(name);
    }

    bool operator==(const CondEvent& lhs, const CondEvent& rhs)
    {
        return lhs.eList_ == rhs.eList_ && lhs.condList_ == rhs.condList_;
    }

    bool operator<(const CondEvent& lhs, const CondEvent& rhs)
    {
        return lhs.condList_ < rhs.condList_ ||
                (lhs.condList_ == rhs.condList_ && lhs.eList_ < rhs.eList_);
    }

    ostream& operator<<(ostream& os, const CondEvent& ce)
    {
        os << ce.eList_;
        if (!ce.condList_.empty())
            os << " | " << ce.condList_;
        return os;
    }

    CondEvent operator||(const EventList& lhs, const EventList& rhs)
    {
        CondEvent reval(lhs, rhs);
        return reval;
    }

    ProbabilityFunction::ProbabilityFunction(const VALUERANGES_TYPE& eventValueRanges,
                                             const VALUERANGES_TYPE& conditionValueRanges)
    : eventValueRanges_(eventValueRanges)
    , conditionValueRanges_(conditionValueRanges)
    {
    }

    /**
     * @TODO: this needs to change to cater for Intervals
     */
    bool ProbabilityFunction::possibleCondEvent(const CondEvent& ce, string& error) const
    {
        // check type and name of event and condition
        for (auto evIt = ce.event().cbegin(); evIt != ce.event().cend(); evIt++)
        {
            auto found = eventValueRanges_.find(evIt->name());
            if (found == eventValueRanges_.end())
            {
                error = "Event '" + evIt->name() + "' not a valid probability variable.";
                return false;
            }
            if (!found->second.validValue(evIt->varValue()))
            {
                error = "p(" + asString(ce) + "): Event value '"
                        + asString(evIt->varValue())
                        + "' of Event '" + evIt->name()
                        + "' has incorrect type or invalid value: "
                        + asString(found->second) + ".";
                return false;
            }
        }
        for (auto condIt = ce.condition().cbegin();
             condIt != ce.condition().cend();
             condIt++)
        {
            auto found = conditionValueRanges_.find(condIt->name());
            if (found == conditionValueRanges_.end())
            {
                error = "p(" + asString(ce) + "): Condition '" + condIt->name()
                        + "' not a valid probability variable.";
                return false;
            }
            if (!found->second.validType(condIt->varValue()))
            {
                error = "Condition value '" + asString(condIt->varValue())
                        + "' of Condition '"
                        + condIt->name() + "' has incorrect type or invalid value: "
                        + asString(found->second) + ".";
                return false;
            }
        }

        return true;
    }

    bool ProbabilityFunction::addValueToEventRange(const string& name,
                                                   const Var& val)
    {
        return addValidValueToRange(eventValueRanges_,
                                    conditionValueRanges_,
                                    name,
                                    val);
    }

    bool ProbabilityFunction::addValueToConditionRange(const string& name,
                                                       const Var& val)
    {
        return addValidValueToRange(conditionValueRanges_,
                                    eventValueRanges_,
                                    name,
                                    val);
    }

    bool ProbabilityFunction::addValidValueToRange(VALUERANGES_TYPE& range,
                                                   VALUERANGES_TYPE& range_ortho,
                                                   const string& name,
                                                   const Var& value)
    {
        bool reval = true;

        // when adding to range we must make sure it's not already in range_ortho
        if (range_ortho.find(name) != range_ortho.end())
            throw distribution_error("Event '" + name +
                                     "' cannot be in events and conditions at the same time.");
        range[name].add(value);
        return reval;
    }

    size_t ProbabilityFunction::getLastEventIndex() const
    {
        return eventValueRanges_.empty() ? 0 : eventValueRanges_.size() - 1;
    }

    size_t ProbabilityFunction::getNumberOfConditions() const
    {
        return conditionValueRanges_.size();
    }

    /**
     * Default construct.
     */
    UniformFloatFunction::UniformFloatFunction(VAR_FLOAT low,
                                               VAR_FLOAT high,
                                               const VALUERANGES_TYPE& conditionValueRanges)
    {
        param_[EventList()] = UNIF_PARAM(low, high);
    }

    /**
     * Clone a copy of this.
     */
    ProbabilityFunction* UniformFloatFunction::clone() const
    {
        return new UniformFloatFunction(*this);
    }

    /**
     * Probability of a conditional event.
     */
    long double UniformFloatFunction::P(const CondEvent& ce) const
    {
        // ce.eventSize() has to be exactly 1, the event-type has to be float or
        // float-interval
        if (ce.eventSize() > 1)
            return 0.0L;
        auto foundParam = param_.find(ce.condition());

        if (foundParam != param_.end())
        {
            VAR_FLOAT low = foundParam->second.low;
            VAR_FLOAT high = foundParam->second.high;
            Interval<VAR_FLOAT> itvl = ce.event().cbegin()->interval<VAR_FLOAT>();
            long double lowP = itvl.isLeftOpen() ? foundParam->second.low : itvl.low_;
            long double highP = itvl.isRightOpen() ? foundParam->second.high : itvl.high_;

            return (highP - lowP) / (foundParam->second.high - foundParam->second.low);
        }
        return 0.0L;
    }

    long double UniformFloatFunction::P(const EventList& el) const
    {
        return P(CondEvent(el));
    }

    /**
     * Reset the parameters of the probability function.
     */
    void UniformFloatFunction::clear()
    {
        param_.clear();
    }

    /**
     * Train (estimate) the parameters of of the probability function.
     * Find minimal and maximal value of the sample and extend the interval
     * to +- (1/numValues).
     */
    bool UniformFloatFunction::train(CSVAnalyzer csv, bool isAccumulativeCSV)
    {
        bool reval = true;

        if (csv.columns() == 0)
            return false;

        size_t lastEventIndex = 1;

        if (!isAccumulativeCSV || csv.type(csv.columns() - 1) != CSV_COLUMN_TYPE_FLOAT)
        {
            // append a 1.0 - initialised value column
            csv.appendColumn("[[RESULT]]", "float", 1.0L);
            isAccumulativeCSV = true;
        }
        param_.clear();
        if (csv.lines() > 0)
        {
            size_t valueIndex = csv.columns() - 1;
            for (size_t row = 0; row < csv.lines(); row++)
            {
                CondEvent ce(csv, row, lastEventIndex, isAccumulativeCSV);
                VAR_FLOAT occurences = csv.getFloat(valueIndex, row);
                if (occurences > 0.0L)
                {
                    VAR_FLOAT val = csv.getFloat(0, row);
                    if (row > 0)
                    {
                        if (val < param_[ce.condition()].low)
                            param_[ce.condition()].low = val;
                        if (val > param_[ce.condition()].high)
                            param_[ce.condition()].high = val;
                    }
                    else
                    {
                        // initialise
                        param_[ce.condition()].low = val;
                        param_[ce.condition()].high = val;
                    }
                    param_[ce.condition()].occurences += occurences;
                }
            }
            for (auto parIt = param_.begin(); parIt != param_.end(); parIt++)
            {
                parIt->second.low -= (1.0L / parIt->second.occurences);
                parIt->second.high += (1.0L / parIt->second.occurences);
            }
        }

        return reval;
    }

    /**
     * Generic ostream - &lt;&lt; operator for UniformFloatFunction.
     */
    ostream& operator<<(ostream& os, const UniformFloatFunction& uff)
    {
        for (auto it = uff.param_.begin(); it != uff.param_.end(); it++)
        {
            if (!it->first.empty())
                os << "For condition " << it->first << " ";
            os << "Uniform on [" << it->second.low << "," << it->second.high << "]" << endl;
        }
        return os;
    }

    /**
     * Default construct with expectation mu and variance sigma
     */
    GaussFunction::GaussFunction(long double mu, long double sigma)
    {
        param_[EventList()] = GAUSS_PARAM(mu, sigma);
    }

    /**
     * Reset the parameters mu and sigma to standard normal values.
     */
    void GaussFunction::clear()
    {
        param_.clear();
    }

    /**
     * Probability of a conditional event.
     */
    long double GaussFunction::P(const CondEvent& ce) const
    {
        // ce.eventSize() has to be exactly 1, the event-type has to be float or
        // float-interval
        if (ce.eventSize() > 1)
            return 0.0L;
        auto foundParam = param_.find(ce.condition());

        if (foundParam != param_.end())
        {
            VAR_FLOAT mu = foundParam->second.mu;
            VAR_FLOAT sigma = foundParam->second.sigma;
            boost::math::normal_distribution<VAR_FLOAT> nd(mu, sigma);
            Interval<VAR_FLOAT> itvl = ce.event().cbegin()->interval<VAR_FLOAT>();
            long double lowP = itvl.isLeftOpen() ? 0.0L : boost::math::cdf(nd, itvl.low_);
            long double highP = itvl.isRightOpen() ? 1.0L : boost::math::cdf(nd, itvl.high_);

            return highP - lowP;
        }
        return 0.0L;
    }

    /**
     * Probability of an interval. Returns zero if the Event is not an interval-event.
     */
    long double GaussFunction::P(const EventList& el) const
    {
        return P(CondEvent(el));
    }

    /**
     * Estimate mu and sigma.
     * <ul>
     * <li>mu ~ sum(x)/numberOf(x)</li>
     * <li>sigma ~ sum((x-mu)^2)/numberOf(x)</li>
     * </ul>
     */
    bool GaussFunction::train(CSVAnalyzer csv, bool isAccumulativeCSV)
    {
        bool reval = true;

        if (csv.columns() == 0)
            return false;

        size_t lastEventIndex = 1;

        if (!isAccumulativeCSV || csv.type(csv.columns() - 1) != CSV_COLUMN_TYPE_FLOAT)
        {
            // append a 1.0 - initialised value column
            csv.appendColumn("[[RESULT]]", "float", 1.0L);
            isAccumulativeCSV = true;
        }
        param_.clear();
        if (csv.lines() > 0)
        {
            size_t valueIndex = csv.columns() - 1;
            for (size_t row = 0; row < csv.lines(); row++)
            {
                CondEvent ce(csv, row, lastEventIndex, isAccumulativeCSV);
                VAR_FLOAT occurences = csv.getFloat(valueIndex, row);
                if (occurences > 0.0L)
                {
                    param_[ce.condition()].mu += csv.getFloat(0, row) * occurences;
                    param_[ce.condition()].occurences += occurences;
                }
            }
            for (auto parIt = param_.begin(); parIt != param_.end(); parIt++)
            {
                parIt->second.mu /= parIt->second.occurences;
                parIt->second.sigma = 0.0L; // reset to 0 just to make sure
            }
            // now we got all the mu's we calculate the sigmas
            for (size_t row = 0; row < csv.lines(); row++)
            {
                CondEvent ce(csv, row, lastEventIndex, isAccumulativeCSV);
                VAR_FLOAT occurences = csv.getFloat(valueIndex, row);
                if (occurences > 0.0L)
                {
                    VAR_FLOAT mu = param_[ce.condition()].mu;
                    VAR_FLOAT val = csv.getFloat(0, row);
                    param_[ce.condition()].sigma += (val - mu)*(val - mu) * occurences;
                }
            }
            for (auto parIt = param_.begin();
                 parIt != param_.end();
                 parIt++)
            {
                parIt->second.sigma /= parIt->second.occurences;
            }
        }

        return reval;
    }

    /**
     * Retrieve mu.
     */
    long double GaussFunction::mu(const CondEvent& ce) const
    {
        auto found = param_.find(ce.condition());
        return found != param_.end() ? found->second.mu : 0.0L;
    }

    /**
     * Retrieve variance.
     */
    long double GaussFunction::sigma(const CondEvent& ce) const
    {
        auto found = param_.find(ce.condition());
        return found != param_.end() ? found->second.sigma : 0.0L;
    }

    /**
     * Generic ostream - &lt;&lt; operator for GaussFunction.
     */
    ostream& operator<<(ostream& os, const GaussFunction& d)
    {
        for (auto it = d.param_.begin(); it != d.param_.end(); it++)
        {
            if (!it->first.empty())
                os << "For condition " << it->first << " ";
            os << "Gauss-distributed with mu=" << it->second.mu << " and sigma=" << it->second.sigma << endl;
        }
        return os;
    }

    /**
     * Default create.
     */
    ExponentialFunction::ExponentialFunction(VAR_FLOAT lambda)
    {
        param_[EventList()] = lambda;
    }

    /**
     * Reset the expectation to 1.0.
     */
    void ExponentialFunction::clear()
    {
        param_.clear();
    }

    /**
     * Probability of an interval. Returns zero if the Event is not an interval-event.
     */
    long double ExponentialFunction::P(const CondEvent& ce) const
    {
        // ce.eventSize() has to be exactly 1, the event-type has to be float or
        // float-interval
        if (ce.eventSize() > 1)
            return 0.0L;
        Interval<VAR_FLOAT> itvl = ce.event().cbegin()->interval<VAR_FLOAT>();
        Interval<VAR_FLOAT> safeItvl(itvl);
        if (safeItvl.low_ < 0.0L)
            safeItvl.low_ = 0.0L;
        if (safeItvl.high_ < 0.0L)
            safeItvl.high_ = 0.0L;

        auto foundParam = param_.find(ce.condition());
        VAR_FLOAT lambda = foundParam != param_.end() ? foundParam->second.lambda : 1.0L;
        boost::math::exponential_distribution<VAR_FLOAT> ed(lambda);
        long double lowP = itvl.isLeftOpen() ? 0.0L : boost::math::cdf(ed, itvl.low_);
        long double highP = itvl.isRightOpen() ? 1.0L : boost::math::cdf(ed, itvl.high_);

        return highP - lowP;
    }

    /**
     * Delegate Probability from from EventList to avoid excessive casting.
     */
    long double ExponentialFunction::P(const EventList& el) const
    {
        return P(CondEvent(el));
    }

    /**
     * Estimate lambda for first column (must be float). Last column might be
     * an accumulative column - all others are condition.
     * - lambda ~ sum(x)/numberOf(x)
     */
    bool ExponentialFunction::train(CSVAnalyzer csv, bool isAccumulativeCSV)
    {
        if (csv.columns() == 0)
            return false;

        size_t lastEventIndex = 1;

        if (!isAccumulativeCSV || csv.type(csv.columns() - 1) != CSV_COLUMN_TYPE_FLOAT)
        {
            // append a 1.0 - initialised value column
            csv.appendColumn("[[RESULT]]", "float", 1.0L);
            isAccumulativeCSV = true;
        }
        param_.clear();
        bool reval = false;
        if (csv.lines() > 0)
        {
            size_t valueIndex = csv.columns() - 1;
            for (size_t row = 0; row < csv.lines(); row++)
            {
                CondEvent ce(csv, row, lastEventIndex, isAccumulativeCSV);
                VAR_FLOAT occurences = csv.getFloat(valueIndex, row);
                if (occurences > 0.0L)
                {
                    if (csv.getFloat(0, row) < 0.0L)
                        throw event_range_error(event_range_error::exponential_range, csv.getFloat(0, row));
                    param_[ce.condition()].lambda += csv.getFloat(0, row) * occurences;
                    param_[ce.condition()].occurences += occurences;
                }
            }
            for (auto parIt = param_.begin(); parIt != param_.end(); parIt++)
            {
                parIt->second.lambda /= parIt->second.occurences;
            }
        }

        return reval;
    }

    /**
     * Retrieve the expectation lambda.
     */
    long double ExponentialFunction::lambda(const CondEvent& ce) const
    {
        auto found = param_.find(ce.condition());
        return found != param_.end() ? found->second.lambda : 0.0L;
    }

    /**
     * Get the point where the cdf == 1/2.
     * P(0.0 &le; x &le; ln(2)/lambda_) = 0.5
     */
    long double ExponentialFunction::ln2ByLambda(const CondEvent& ce) const
    {
        return ln_2 / lambda(ce);
    }

    /**
     * Generic ostream - &lt;&lt; operator for ExponentialFunction.
     */
    ostream& operator<<(ostream& os, const ExponentialFunction& d)
    {
        for (auto it = d.param_.begin(); it != d.param_.end(); it++)
        {
            if (!it->first.empty())
                os << "For condition " << it->first << " ";
            os << "Gauss-distributed with lambda=" << it->second.lambda << endl;
        }
        return os;
    }

    DiscreteProbability::DiscreteProbability(const VALUERANGES_TYPE& eventValueRanges,
                                             const VALUERANGES_TYPE& conditionValueRanges)
    : ProbabilityFunction(eventValueRanges, conditionValueRanges)
    , isUniform_(false)
    , hasBeenModified_(false)
    {
    }

    ProbabilityFunction* DiscreteProbability::clone() const
    {
        return new DiscreteProbability(*this);
    }

    bool DiscreteProbability::makeUniform()
    {
        bool reval = true;

        size_t numberOfValues = values_.size();
        if (numberOfValues == 0)
            canonise();
        setUniform(true);
        long double prob = 1.0 / (long double) numberOfValues;
        for (auto it = values_.begin(); it != values_.end(); it++)
        {
            it->second = prob;
        }
        setModified(false);

        return reval;
    }

    /*
     * (1) Bayes formula
     * =================
     *
     *            P(L|W)P(W)            P(L|W)P(W)
     * P(W|L) = ------------ = -------------------------
     *              P(L)         P(L|W)P(W) + P(L|M)P(M)
     *
     * (2) Chain rule of probability
     * =============================
     *
     * P(A,B) = P(B|A)P(A)
     * P(An,An-1,...,A1)=P(An|An-1,...,A1)P(An-1,...,A1)
     * P(A4,A3,A2,A1)=P(A4|A3,A2,A1)P(A3|A2,A1)P(A2|A1)P(A1)
     *
     * (3) Total probability
     * =====================
     *
     * P(A) = sum(P(A,B_i)) = sum(P(A|B_i)P(B_i)
     */
    bool DiscreteProbability::normalise()
    {
        bool reval = true;

        if (isUniform())
        {
            return makeUniform();
        }

        if (values_.size() == 0)
            reval = canonise();

        ACCUMULATION_MAP sum;
        for (auto it = values_.begin(); it != values_.end(); it++)
        {
            if (it->second < 0.0L) // values *CAN* be > 1.0 until the probability is normalised
                throw distribution_error(it->second);

            auto acIt = sum.find(it->first.condition());
            if (acIt == sum.end()) // initialise
            {
                sum[it->first.condition()].number = 1.0L;
                sum[it->first.condition()].sum = it->second;
            }
            else
            {
                acIt->second.number++;
                acIt->second.sum += it->second;
            }
        }
        for (auto it = values_.begin(); it != values_.end(); it++)
        {
            if (sum[it->first.condition()].sum == 0.0L) // make uniform
                it->second = 1.0L / (long double) (sum[it->first.condition()].number);
            else // normalise
                it->second /= sum[it->first.condition()].sum;
        }
        setModified(false);

        return reval;
    }

    void spread(vector<CondEvent>& condEvents,
                const EVENT_SET& ev,
                bool isCond,
                size_t module)
    {
        auto evIt = ev.begin();
        size_t count = 0;
        for (auto vIt = condEvents.begin(); vIt != condEvents.end(); vIt++)
        {
            if (isCond)
                *vIt || *evIt;
            else
                *vIt && *evIt;
            count++;
            if (count % module == 0)
            {
                evIt++;
                if (evIt == ev.end())
                    evIt = ev.begin();
            }
        }
    }

    bool DiscreteProbability::canonise()
    {
        bool reval = true;

        vector<CondEvent> condEvents;
        size_t numCondEvents = 1;
        if (eventValueRanges_.size() == 0 ||
            eventValueRanges_.begin()->second.size() == 0)
            throw distribution_error("need at least one event with non-empty"
                                     " value-range to canonise discrete distribution.");

        for (auto eRangeIt = eventValueRanges_.begin();
             eRangeIt != eventValueRanges_.end();
             eRangeIt++)
        {
            numCondEvents *= eRangeIt->second.size();
        }

        for (auto cRangeIt = conditionValueRanges_.begin();
             cRangeIt != conditionValueRanges_.end();
             cRangeIt++)
        {
            numCondEvents *= cRangeIt->second.size();
        }
        condEvents.resize(numCondEvents);

        size_t module = 1;
        for (auto eRangeIt = eventValueRanges_.begin();
             eRangeIt != eventValueRanges_.end();
             eRangeIt++)
        {
            EVENT_SET eSet = eRangeIt->second.makeEventSet(eRangeIt->first);
            spread(condEvents, eSet, false, module);
            module *= eSet.size();
        }

        for (auto cRangeIt = conditionValueRanges_.begin();
             cRangeIt != conditionValueRanges_.end();
             cRangeIt++)
        {
            EVENT_SET eSet = cRangeIt->second.makeEventSet(cRangeIt->first);
            spread(condEvents, eSet, true, module);
            module *= eSet.size();
        }

        for (auto ceIt = condEvents.begin(); ceIt != condEvents.end(); ceIt++)
        {
            if (values_.find(*ceIt) == values_.end())
                values_[*ceIt] = 0.0L;
        }

        return reval;
    }

    bool DiscreteProbability::isDistribution() const
    {
        bool reval = !empty();
        ACCUMULATION_MAP sum;
        for (auto it = values_.begin(); reval && it != values_.end(); it++)
        {
            if (it->second < 0.0L) // values *CAN* be > 1.0 until the probability is normalised
                reval = false;

            sum[it->first.condition()].sum += it->second;
        }
        auto it = sum.begin();
        while (reval && it != sum.end())
        {
            if (!withinTolerance(it->second.sum, 1.0L))
                reval = false;
            it++;
        }

        return reval;
    }

    void DiscreteProbability::clear()
    {
        values_.clear();
        conditionValueRanges_.clear();
        eventValueRanges_.clear();
    }

    bool DiscreteProbability::empty() const
    {
        return values_.empty();
    }

    // TODO: this needs to change to cater for Intervals

    long double DiscreteProbability::P(const CondEvent& ce) const
    {
        if (!isDistribution())
            throw distribution_error();
        string error = "";
        if (!possibleCondEvent(ce, error))
            throw distribution_error(error);

        auto found = values_.find(ce);
        if (found == values_.end())
            return 0.0L;
        return found->second;
    }

    /**
     * lastEventIndex is the last column that is not a condition
     * if lastEventIndex ==  x  columns x+1, x+2, x+3, ... are conditions
     * if isAccumulativeCSV is true, then last column contains a float
     * probability value >= 0.0
     */
    bool DiscreteProbability::train(CSVAnalyzer csv, bool isAccumulativeCSV)
    {
        if (csv.columns() == 0)
            return false;

        size_t lastEventIndex = 0;

        if (!isAccumulativeCSV || csv.type(csv.columns() - 1) != CSV_COLUMN_TYPE_FLOAT)
        {
            // append a 1.0 - initialised value column
            csv.appendColumn("[[RESULT]]", "float", 1.0L);
            isAccumulativeCSV = true;
        }
        bool reval = false;
        if (csv.lines() > 0)
        {
            size_t valueIndex = csv.columns() - 1;
            for (size_t row = 0; row < csv.lines(); row++)
            {
                CondEvent ce(csv, row, lastEventIndex, isAccumulativeCSV);
                values_[ce] += csv.getFloat(valueIndex, row);
            }
            updateValueRangesFromValues_();
            reval = normalise();
        }

        return reval;
    }

    /**
     * Use the values (obtained by training) to update the ranges the variables
     * can assume. Don't remove any existing values.
     */
    void DiscreteProbability::updateValueRangesFromValues_(bool clearFirst)
    {
        if (clearFirst)
        {
            conditionValueRanges_.clear();
            eventValueRanges_.clear();
        }
        for (auto it = values_.begin(); it != values_.end(); it++)
        {
            auto condIt = it->first.condition().cbegin();
            while (condIt != it->first.condition().cend())
            {
                addValidValueToRange(conditionValueRanges_,
                                     eventValueRanges_,
                                     condIt->name(),
                                     condIt->varValue());
                condIt++;
            }

            auto evtIt = it->first.event().cbegin();
            while (evtIt != it->first.event().cend())
            {
                addValidValueToRange(eventValueRanges_,
                                     conditionValueRanges_,
                                     evtIt->name(),
                                     evtIt->varValue());
                evtIt++;
            }
        }
    }

    ostream& operator<<(ostream& os, const DiscreteProbability& d)
    {
        os << "Event value ranges:" << endl;
        for (auto evIt = d.eventValueRanges_.begin();
             evIt != d.eventValueRanges_.end();
             evIt++)
        {
            os << "\t" << evIt->first << ": " << evIt->second << endl;
        }
        os << "Condition value ranges:" << endl;
        for (auto cIt = d.conditionValueRanges_.begin();
             cIt != d.conditionValueRanges_.end();
             cIt++)
        {
            os << "\t" << cIt->first << ": " << cIt->second << endl;
        }
        for (auto it = d.values_.begin(); it != d.values_.end(); it++)
        {
            os << "P(" << it->first << ")=" << it->second << endl;
        }
        return os;
    }

}; // namespace util
