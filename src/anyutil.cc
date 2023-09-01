/*
 * File Name:   anyutil.cc
 * Description: std::any utility functions
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

#include "anyutil.h"
#include "to_string.h"
#include "stringutil.h"
//#define DO_TRACE_
#include "traceutil.h"

using namespace util;
namespace util
{
using namespace std;
const int Var::xalloc_index = ios_base::xalloc();

Var::Var() : value_(std::any())
{
}

Var::Var(const VAR_BOOL &v) : value_(v)
{
}

Var::Var(const VAR_CHAR &v) : value_(v)
{
}

Var::Var(const VAR_INT &v) : value_(v)
{
}

Var::Var(const VAR_UINT &v) : value_(v)
{
}

Var::Var(const VAR_FLOAT &v) : value_(v)
{
}

Var::Var(const VAR_DATE &v) : value_(v)
{
}

Var::Var(const VAR_STRING &v) : value_(v)
{
}

const type_info &Var::type() const
{
    return (value_.type());
}

bool Var::empty() const
{
    return (!value_.has_value());
}

Var &Var::swap(Var &rhs)
{
    value_.swap(rhs.value_);

    return (*this);
}

std::any Var::value() const
{
    return (value_);
}

bool Var::contains(const Var &val) const
{
    bool reval = (containsT<VAR_CHAR>(*this, val) || containsT<VAR_INT>(*this, val) || containsT<VAR_UINT>(*this, val)
                  || containsT<VAR_FLOAT>(*this, val) || containsT<VAR_DATE>(*this, val) || (*this == val));

    return (reval);
}

bool operator==(const Var &lhs, const Var &rhs)
{
    return (equalT<VAR_BOOL>(lhs, rhs) || equalT<VAR_CHAR>(lhs, rhs) || equalT<VAR_INT>(lhs, rhs)
            || equalT<VAR_UINT>(lhs, rhs) || equalT<VAR_FLOAT>(lhs, rhs) || equalT<VAR_STRING>(lhs, rhs)
            || equalT<VAR_DATE>(lhs, rhs) || equalT<VAR_BOOL_INTERVAL>(lhs, rhs) || equalT<VAR_CHAR_INTERVAL>(lhs, rhs)
            || equalT<VAR_INT_INTERVAL>(lhs, rhs) || equalT<VAR_UINT_INTERVAL>(lhs, rhs)
            || equalT<VAR_DATE_INTERVAL>(lhs, rhs) || equalT<VAR_FLOAT_INTERVAL>(lhs, rhs));
}

bool operator<(const Var &lhs, const Var &rhs)
{
    if(isA<VAR_BOOL>(lhs))
        return lessT<VAR_BOOL>(lhs, rhs);
    if(isA<VAR_CHAR>(lhs))
        return lessT<VAR_CHAR>(lhs, rhs);
    if(isA<VAR_INT>(lhs))
        return lessT<VAR_INT>(lhs, rhs);
    if(isA<VAR_UINT>(lhs))
        return lessT<VAR_UINT>(lhs, rhs);
    if(isA<VAR_FLOAT>(lhs))
        return lessT<VAR_FLOAT>(lhs, rhs);
    if(isA<VAR_DATE>(lhs))
        return lessT<VAR_DATE>(lhs, rhs);
    if(isA<VAR_STRING>(lhs))
        return lessT<VAR_STRING>(lhs, rhs);

    if(isA<VAR_BOOL_INTERVAL>(lhs))
        return lessT<VAR_BOOL_INTERVAL>(lhs, rhs);
    if(isA<VAR_CHAR_INTERVAL>(lhs))
        return lessT<VAR_CHAR_INTERVAL>(lhs, rhs);
    if(isA<VAR_INT_INTERVAL>(lhs))
        return lessT<VAR_INT_INTERVAL>(lhs, rhs);
    if(isA<VAR_UINT_INTERVAL>(lhs))
        return lessT<VAR_UINT_INTERVAL>(lhs, rhs);
    if(isA<VAR_DATE_INTERVAL>(lhs))
        return lessT<VAR_DATE_INTERVAL>(lhs, rhs);
    if(isA<VAR_FLOAT_INTERVAL>(lhs))
        return lessT<VAR_FLOAT_INTERVAL>(lhs, rhs);
    
    return (toString(lhs) < toString(rhs));
}

bool operator<=(const Var &lhs, const Var &rhs)
{
    return (lessEqualT<VAR_BOOL>(lhs, rhs) || lessEqualT<VAR_CHAR>(lhs, rhs) || lessEqualT<VAR_INT>(lhs, rhs)
            || lessEqualT<VAR_UINT>(lhs, rhs) || lessEqualT<VAR_FLOAT>(lhs, rhs) || lessEqualT<VAR_DATE>(lhs, rhs)
            || lessEqualT<VAR_STRING>(lhs, rhs) || lessEqualT<VAR_BOOL_INTERVAL>(lhs, rhs)
            || lessEqualT<VAR_CHAR_INTERVAL>(lhs, rhs) || lessEqualT<VAR_INT_INTERVAL>(lhs, rhs)
            || lessEqualT<VAR_UINT_INTERVAL>(lhs, rhs) || lessEqualT<VAR_DATE_INTERVAL>(lhs, rhs)
            || lessEqualT<VAR_FLOAT_INTERVAL>(lhs, rhs) || (toString(lhs) <= toString(rhs)));
}

bool operator>(const Var &lhs, const Var &rhs)
{
    return (greaterT<VAR_BOOL>(lhs, rhs) || greaterT<VAR_CHAR>(lhs, rhs) || greaterT<VAR_INT>(lhs, rhs)
            || greaterT<VAR_UINT>(lhs, rhs) || greaterT<VAR_FLOAT>(lhs, rhs) || greaterT<VAR_DATE>(lhs, rhs)
            || greaterT<VAR_STRING>(lhs, rhs) || greaterT<VAR_BOOL_INTERVAL>(lhs, rhs)
            || greaterT<VAR_CHAR_INTERVAL>(lhs, rhs) || greaterT<VAR_INT_INTERVAL>(lhs, rhs)
            || greaterT<VAR_UINT_INTERVAL>(lhs, rhs) || greaterT<VAR_DATE_INTERVAL>(lhs, rhs)
            || greaterT<VAR_FLOAT_INTERVAL>(lhs, rhs) || (toString(lhs) > toString(rhs)));
}

bool operator>=(const Var &lhs, const Var &rhs)
{
    return (greaterEqualT<VAR_BOOL>(lhs, rhs) || greaterEqualT<VAR_CHAR>(lhs, rhs) || greaterEqualT<VAR_INT>(lhs, rhs)
            || greaterEqualT<VAR_UINT>(lhs, rhs) || greaterEqualT<VAR_FLOAT>(lhs, rhs)
            || greaterEqualT<VAR_DATE>(lhs, rhs) || greaterEqualT<VAR_STRING>(lhs, rhs)
            || greaterEqualT<VAR_BOOL_INTERVAL>(lhs, rhs) || greaterEqualT<VAR_CHAR_INTERVAL>(lhs, rhs)
            || greaterEqualT<VAR_INT_INTERVAL>(lhs, rhs) || greaterEqualT<VAR_UINT_INTERVAL>(lhs, rhs)
            || greaterEqualT<VAR_DATE_INTERVAL>(lhs, rhs) || greaterEqualT<VAR_FLOAT_INTERVAL>(lhs, rhs)
            || (toString(lhs) >= toString(rhs)));
}

bool Less::leftMatchesRight(const Var &lhs, const Var &rhs) const
{
    return (lhs < rhs);
}

string Less::desc(const Var &v) const
{
    stringstream ss;

    ss << "<" << v;

    return (ss.str());
}

bool Equals::leftMatchesRight(const Var &lhs, const Var &rhs) const
{
    return (lhs == rhs);
}

string Equals::desc(const Var &v) const
{
    stringstream ss;

    ss << "==" << v;

    return (ss.str());
}

bool LessEqual::leftMatchesRight(const Var &lhs, const Var &rhs) const
{
    return (lhs <= rhs);
}

string LessEqual::desc(const Var &v) const
{
    stringstream ss;

    ss << "<=" << v;

    return (ss.str());
}

bool Greater::leftMatchesRight(const Var &lhs, const Var &rhs) const
{
    return (lhs > rhs);
}

string Greater::desc(const Var &v) const
{
    stringstream ss;

    ss << ">" << v;

    return (ss.str());
}

bool GreaterEqual::leftMatchesRight(const Var &lhs, const Var &rhs) const
{
    return (lhs >= rhs);
}

string GreaterEqual::desc(const Var &v) const
{
    stringstream ss;

    ss << ">=" << v;

    return (ss.str());
}

bool IsElementOf::leftMatchesRight(const Var &elem, const Var &itvl) const
{
    return (itvl.contains(elem));
}

string IsElementOf::desc(const Var &itvl) const
{
    stringstream ss;

    ss << " in " << itvl;

    return (ss.str());
}

bool PlaceHolderOp::leftMatchesRight(const Var &elem, const Var &itvl) const
{

    return (false);
}

string PlaceHolderOp::desc(const Var &itvl) const
{
    stringstream ss;

    ss << " <<NO_MATCH>> " << itvl;

    return (ss.str());
}
};  // namespace util
