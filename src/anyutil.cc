////////////////////////////////////////////////////////////////////////////////
//
// File Name:   anyutil.cc
// Description: boost::any utility functions
// Created on January 28, 2014, 12:40 PM
// Author: Dieter Kybelksties
//
////////////////////////////////////////////////////////////////////////////////

#include <anyutil.h>

namespace util
{
    using namespace std;
    const int Var::xalloc_index = ios_base::xalloc();
    Var::Var()
    : value_(boost::any())
    {
    }
    Var::Var(const VAR_BOOL& v)
    : value_(v)
    {
    }
    Var::Var(const VAR_CHAR& v)
    : value_(v)
    {
    }
    Var::Var(const VAR_INT& v)
    : value_(v)
    {
    }
    Var::Var(const VAR_UINT& v)
    : value_(v)
    {
    }
    Var::Var(const VAR_FLOAT& v)
    : value_(v)
    {
    }
    Var::Var(const VAR_DATE& v)
    : value_(v)
    {
    }
    Var::Var(const VAR_STRING& v)
    : value_(v)
    {
    }
    Var::Var(const Var& rhs)
    : value_(rhs.value_)
    {
    }
    Var& Var::operator =(const Var& rhs)
    {
        if (&rhs != this)
            value_ = rhs.value_;
        return *this;
    }
    const type_info& Var::type() const
    {
        return value_.type();
    }
    bool Var::empty() const
    {
        return value_.empty();
    }
    Var& Var::swap(Var& rhs)
    {
        value_.swap(rhs.value_);
    }
    boost::any Var::value() const
    {
        return value_;
    }
    bool Var::contains(const Var& val) const
    {
        return containsT<VAR_CHAR>(*this, val) ||
                containsT<VAR_INT>(*this, val) ||
                containsT<VAR_UINT>(*this, val) ||
                containsT<VAR_FLOAT>(*this, val) ||
                containsT<VAR_DATE>(*this, val) ||
                *this == val;
    }
    bool operator ==(const Var& lhs, const Var& rhs)
    {
        return equalT<VAR_BOOL>(lhs, rhs) ||
                equalT<VAR_CHAR>(lhs, rhs) ||
                equalT<VAR_INT>(lhs, rhs) ||
                equalT<VAR_UINT>(lhs, rhs) ||
                equalT<VAR_FLOAT>(lhs, rhs) ||
                equalT<VAR_STRING>(lhs, rhs) ||
                equalT<VAR_DATE>(lhs, rhs) ||
                equalT<VAR_BOOL_INTERVAL>(lhs, rhs) ||
                equalT<VAR_CHAR_INTERVAL>(lhs, rhs) ||
                equalT<VAR_INT_INTERVAL>(lhs, rhs) ||
                equalT<VAR_UINT_INTERVAL>(lhs, rhs) ||
                equalT<VAR_DATE_INTERVAL>(lhs, rhs) ||
                equalT<VAR_FLOAT_INTERVAL>(lhs, rhs);
    }
    bool operator<(const Var& lhs, const Var& rhs)
    {
        return lessT<VAR_BOOL>(lhs, rhs) ||
                lessT<VAR_CHAR>(lhs, rhs) ||
                lessT<VAR_INT>(lhs, rhs) ||
                lessT<VAR_UINT>(lhs, rhs) ||
                lessT<VAR_FLOAT>(lhs, rhs) ||
                lessT<VAR_DATE>(lhs, rhs) ||
                lessT<VAR_STRING>(lhs, rhs) ||
                lessT<VAR_BOOL_INTERVAL>(lhs, rhs) ||
                lessT<VAR_CHAR_INTERVAL>(lhs, rhs) ||
                lessT<VAR_INT_INTERVAL>(lhs, rhs) ||
                lessT<VAR_UINT_INTERVAL>(lhs, rhs) ||
                lessT<VAR_DATE_INTERVAL>(lhs, rhs) ||
                lessT<VAR_FLOAT_INTERVAL>(lhs, rhs) ||
                asString(lhs) < asString(rhs);
    }
    bool operator <=(const Var& lhs, const Var& rhs)
    {
        return lessEqualT<VAR_BOOL>(lhs, rhs) ||
                lessEqualT<VAR_CHAR>(lhs, rhs) ||
                lessEqualT<VAR_INT>(lhs, rhs) ||
                lessEqualT<VAR_UINT>(lhs, rhs) ||
                lessEqualT<VAR_FLOAT>(lhs, rhs) ||
                lessEqualT<VAR_DATE>(lhs, rhs) ||
                lessEqualT<VAR_STRING>(lhs, rhs) ||
                lessEqualT<VAR_BOOL_INTERVAL>(lhs, rhs) ||
                lessEqualT<VAR_CHAR_INTERVAL>(lhs, rhs) ||
                lessEqualT<VAR_INT_INTERVAL>(lhs, rhs) ||
                lessEqualT<VAR_UINT_INTERVAL>(lhs, rhs) ||
                lessEqualT<VAR_DATE_INTERVAL>(lhs, rhs) ||
                lessEqualT<VAR_FLOAT_INTERVAL>(lhs, rhs) ||
                asString(lhs) <= asString(rhs);
    }
    bool operator>(const Var& lhs, const Var& rhs)
    {
        return greaterT<VAR_BOOL>(lhs, rhs) ||
                greaterT<VAR_CHAR>(lhs, rhs) ||
                greaterT<VAR_INT>(lhs, rhs) ||
                greaterT<VAR_UINT>(lhs, rhs) ||
                greaterT<VAR_FLOAT>(lhs, rhs) ||
                greaterT<VAR_DATE>(lhs, rhs) ||
                greaterT<VAR_STRING>(lhs, rhs) ||
                greaterT<VAR_BOOL_INTERVAL>(lhs, rhs) ||
                greaterT<VAR_CHAR_INTERVAL>(lhs, rhs) ||
                greaterT<VAR_INT_INTERVAL>(lhs, rhs) ||
                greaterT<VAR_UINT_INTERVAL>(lhs, rhs) ||
                greaterT<VAR_DATE_INTERVAL>(lhs, rhs) ||
                greaterT<VAR_FLOAT_INTERVAL>(lhs, rhs) ||
                asString(lhs) > asString(rhs);
    }
    bool operator >=(const Var& lhs, const Var& rhs)
    {
        return greaterEqualT<VAR_BOOL>(lhs, rhs) ||
                greaterEqualT<VAR_CHAR>(lhs, rhs) ||
                greaterEqualT<VAR_INT>(lhs, rhs) ||
                greaterEqualT<VAR_UINT>(lhs, rhs) ||
                greaterEqualT<VAR_FLOAT>(lhs, rhs) ||
                greaterEqualT<VAR_DATE>(lhs, rhs) ||
                greaterEqualT<VAR_STRING>(lhs, rhs) ||
                greaterEqualT<VAR_BOOL_INTERVAL>(lhs, rhs) ||
                greaterEqualT<VAR_CHAR_INTERVAL>(lhs, rhs) ||
                greaterEqualT<VAR_INT_INTERVAL>(lhs, rhs) ||
                greaterEqualT<VAR_UINT_INTERVAL>(lhs, rhs) ||
                greaterEqualT<VAR_DATE_INTERVAL>(lhs, rhs) ||
                greaterEqualT<VAR_FLOAT_INTERVAL>(lhs, rhs) ||
                asString(lhs) >= asString(rhs);
    }
    ostream& operator<<(ostream& os, Var::StreamMode sm)
    {
        os.iword(Var::xalloc_index) = sm;
        return os;
    }
    ostream& operator <<(ostream& os, const Var& v)
    {
        Var::StreamMode sm = (Var::StreamMode)os.iword(Var::xalloc_index);
        if (sm == 0)
            sm = Var::standard;

        if (isA<VAR_BOOL>(v))
        {
            if ((sm & Var::alpha_bool) == Var::alpha_bool)
                os << boolalpha;
            os << v.get<VAR_BOOL>();
        }
        else if (isA<VAR_CHAR>(v))
        {
            if ((sm & Var::hex_char) == Var::hex_char)
                os << hex;
            if ((sm & Var::quoted_char) == Var::quoted_char)
                os << squoted(v.get<VAR_CHAR>());
            else
                os << v.get<VAR_CHAR>();
        }
        else if (isA<VAR_INT>(v))
            os << v.get<VAR_INT>();
        else if (isA<VAR_UINT> (v))
            os << v.get<VAR_UINT> ();
        else if (isA<VAR_FLOAT>(v))
        {
            if ((sm & Var::short_float) == Var::short_float)
                os << fixed << setprecision(5);
            else if ((sm & Var::long_float) == Var::long_float)
                os << fixed << setprecision(20);
            if ((sm & Var::scientific_float) == Var::scientific_float)
                os << scientific << setprecision(20);
            os << v.get<VAR_FLOAT>();
        }
        else if (isA<VAR_STRING> (v))
        {
            if ((sm & Var::quoted_string) == Var::quoted_string)
                os << quoted(v.get<VAR_STRING>());
            else
                os << v.get<VAR_STRING>();
        }
        else if (isA<VAR_DATE>(v))
        {
            if ((sm & Var::quoted_date) == Var::quoted_date)
                os << quoted(v.get<VAR_DATE>());
            else
                os << v.get<VAR_DATE>();
        }
        else if (isA<VAR_BOOL_INTERVAL>(v))
            os << v.get<VAR_BOOL_INTERVAL>();
        else if (isA<VAR_CHAR_INTERVAL>(v))
            os << v.get<VAR_CHAR_INTERVAL>();
        else if (isA<VAR_INT_INTERVAL>(v))
            os << v.get<VAR_INT_INTERVAL>();
        else if (isA<VAR_UINT_INTERVAL>(v))
            os << v.get<VAR_UINT_INTERVAL>();
        else if (isA<VAR_DATE_INTERVAL>(v))
            os << v.get<VAR_DATE_INTERVAL>();
        else if (isA<VAR_FLOAT_INTERVAL>(v))
            os << v.get<VAR_FLOAT_INTERVAL>();
        return os;
    }
    bool Equals::leftMatchesRight(const Var& lhs, const Var& rhs) const
    {
        return lhs == rhs;
    }
    string Equals::desc(const Var& v) const
    {
        stringstream ss;
        ss << "=" << v;
        return ss.str();
    }
    bool Less::leftMatchesRight(const Var& lhs, const Var& rhs) const
    {
        return lhs < rhs;
    }
    string Less::desc(const Var& v) const
    {
        stringstream ss;
        ss << "<" << v;
        return ss.str();
    }
    bool LessEqual::leftMatchesRight(const Var& lhs, const Var& rhs) const
    {
        return lhs <= rhs;
    }
    string LessEqual::desc(const Var& v) const
    {
        stringstream ss;
        ss << "<=" << v;
        return ss.str();
    }
    bool Greater::leftMatchesRight(const Var& lhs, const Var& rhs) const
    {
        return lhs > rhs;
    }
    string Greater::desc(const Var& v) const
    {
        stringstream ss;
        ss << ">" << v;
        return ss.str();
    }
    bool GreaterEqual::leftMatchesRight(const Var& lhs, const Var& rhs) const
    {
        return lhs >= rhs;
    }
    string GreaterEqual::desc(const Var& v) const
    {
        stringstream ss;
        ss << ">=" << v;
        return ss.str();
    }
    bool IsElementOf::leftMatchesRight(const Var& elem, const Var& itvl) const
    {
        return itvl.contains(elem);
    }
    string IsElementOf::desc(const Var& itvl) const
    {
        stringstream ss;
        ss << " in " << itvl;
        return ss.str();
    }
    bool PlaceHolderOp::leftMatchesRight(const Var& elem, const Var& itvl) const
    {
        return false;
    }
    string PlaceHolderOp::desc(const Var& itvl) const
    {
        stringstream ss;
        ss << " <<NO_MATCH>> " << itvl;
        return ss.str();
    }
}; // namespace util

