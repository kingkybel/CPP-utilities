////////////////////////////////////////////////////////////////////////////////
//
// File Name:   main.cc
// Description: Unit test cases for utility functions
// Created on January 24, 2014, 1:09 PM
// Author: Dieter Kybelksties
//
////////////////////////////////////////////////////////////////////////////////

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/bind.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <iostream>
#include <iostream>
#include <string>
#include <cmath>
#include <stringutil.h>
#include <dateutil.h>
#include <anyutil.h>
#include <csvutil.h>
#include <bayesutil.h>
#include <statutil.h>
#include <graphutil.h>

using namespace std;
using namespace std::tr1;
using namespace boost::unit_test;
using namespace util;
using namespace util::datescan;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::assign;
using namespace boost::filesystem;

const string filename = "/tmp/test.csv";
template<typename T_>
struct SR
{
    SR(const T_& source,
       const T_& tp,
       StripTrimMode m,
       const T_& modChars,
       char c,
       const T_& result,
       size_t line,
       bool differentInsensitive = false,
       const T_& resultInsensitive = "")
    : source_(source)
    , tp_(tp)
    , m_(m)
    , modChars_(modChars)
    , c_(c)
    , result_(result)
    , line_(line)
    , differentInsensitive_(differentInsensitive)
    , resultInsensitive_(differentInsensitive ? resultInsensitive : result_)
    {
    }
    bool correctResult()
    {
        T_ actualResult = source_;
        if (tp_ == "trim")
        {
            trim(actualResult, modChars_, m_);
        }
        else if (tp_ == "strip")
        {
            strip(actualResult, modChars_, m_);
        }
        else if (tp_ == "replace")
        {
            replaceChar(actualResult, modChars_, c_, m_);
        }
        T_ expectedResult =
                (typeid (T_) == typeid (ci_string) && differentInsensitive_) ?
                resultInsensitive_ : result_;
        bool reval = (expectedResult == actualResult);
        if (!reval)
            cout << " Line " << line_ << ": "
                << quoted(expectedResult) << " != " << quoted(actualResult)
            << " " << ends;
        return reval;
    }
    T_ source_;
    T_ tp_;
    StripTrimMode m_;
    T_ modChars_;
    char c_;
    T_ result_;
    bool differentInsensitive_;
    T_ resultInsensitive_;
    size_t line_;
};
template<typename T_>
void util_string_mod_test(int i, int j)
{
    typedef SR<T_> SR;
    SR modResults[] = {
                       // trivial
                       SR("", "trim", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR("", "trim", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR("", "trim", RIGHT, "\n\t \r", char(0), "", __LINE__),
                       SR(" ", "trim", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR(" ", "trim", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR(" ", "trim", RIGHT, "\n\t \r", char(0), "", __LINE__),
                       SR("\t", "trim", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR("\t", "trim", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR("\t", "trim", RIGHT, "\n\t \r", char(0), "", __LINE__),
                       SR("\n", "trim", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR("\n", "trim", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR("\n", "trim", RIGHT, "\n\t \r", char(0), "", __LINE__),

                       SR("", "strip", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR("", "strip", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR("", "strip", RIGHT, "\n\t \r", char(0), "", __LINE__),
                       SR(" ", "strip", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR(" ", "strip", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR(" ", "strip", RIGHT, "\n\t \r", char(0), "", __LINE__),
                       SR("\t", "strip", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR("\t", "strip", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR("\t", "strip", RIGHT, "\n\t \r", char(0), "", __LINE__),
                       SR("\n", "strip", ALL, "\n\t \r", char(0), "", __LINE__),
                       SR("\n", "strip", LEFT, "\n\t \r", char(0), "", __LINE__),
                       SR("\n", "strip", RIGHT, "\n\t \r", char(0), "", __LINE__),

                       SR("", "replace", ALL, "\n\t \r", '#', "", __LINE__),
                       SR("", "replace", LEFT, "\n\t \r", '#', "", __LINE__),
                       SR("", "replace", RIGHT, "\n\t \r", '#', "", __LINE__),
                       SR(" ", "replace", ALL, "\n\t \r", '#', "#", __LINE__, false),
                       SR(" ", "replace", LEFT, "\n\t \r", '#', "#", __LINE__),
                       SR(" ", "replace", RIGHT, "\n\t \r", '#', "#", __LINE__),
                       SR("\t", "replace", ALL, "\n\t \r", '#', "#", __LINE__),
                       SR("\t", "replace", LEFT, "\n\t \r", '#', "#", __LINE__),
                       SR("\t", "replace", RIGHT, "\n\t \r", '#', "#", __LINE__),
                       SR("\n", "replace", ALL, "\n\t \r", '#', "#", __LINE__),
                       SR("\n", "replace", LEFT, "\n\t \r", '#', "#", __LINE__),
                       SR("\n", "replace", RIGHT, "\n\t \r", '#', "#", __LINE__),

                       // trivial case-dependent
                       SR("", "trim", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("", "trim", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("", "trim", RIGHT, "abc", char(0), "", __LINE__, false, ""),
                       SR("a", "trim", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("a", "trim", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("a", "trim", RIGHT, "abc", char(0), "", __LINE__, false, ""),
                       SR("b", "trim", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("b", "trim", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("b", "trim", RIGHT, "abc", char(0), "", __LINE__, false, ""),
                       SR("c", "trim", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("c", "trim", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("c", "trim", RIGHT, "abc", char(0), "", __LINE__, false, ""),
                       SR("A", "trim", ALL, "abc", char(0), "A", __LINE__, true, ""),
                       SR("A", "trim", LEFT, "abc", char(0), "A", __LINE__, true, ""),
                       SR("A", "trim", RIGHT, "abc", char(0), "A", __LINE__, true, ""),
                       SR("B", "trim", ALL, "abc", char(0), "B", __LINE__, true, ""),
                       SR("B", "trim", LEFT, "abc", char(0), "B", __LINE__, true, ""),
                       SR("B", "trim", RIGHT, "abc", char(0), "B", __LINE__, true, ""),
                       SR("C", "trim", ALL, "abc", char(0), "C", __LINE__, true, ""),
                       SR("C", "trim", LEFT, "abc", char(0), "C", __LINE__, true, ""),
                       SR("C", "trim", RIGHT, "abc", char(0), "C", __LINE__, true, ""),

                       SR("", "strip", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("", "strip", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("", "strip", RIGHT, "abc", char(0), "", __LINE__, false, ""),
                       SR("a", "strip", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("a", "strip", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("a", "strip", RIGHT, "abc", char(0), "", __LINE__, ""),
                       SR("b", "strip", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("b", "strip", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("b", "strip", RIGHT, "abc", char(0), "", __LINE__, false, ""),
                       SR("c", "strip", ALL, "abc", char(0), "", __LINE__, false, ""),
                       SR("c", "strip", LEFT, "abc", char(0), "", __LINE__, false, ""),
                       SR("c", "strip", RIGHT, "abc", char(0), "", __LINE__, false, ""),
                       SR("A", "strip", ALL, "abc", char(0), "A", __LINE__, true, ""),
                       SR("A", "strip", LEFT, "abc", char(0), "A", __LINE__, true, ""),
                       SR("A", "strip", RIGHT, "abc", char(0), "A", __LINE__, true, ""),
                       SR("B", "strip", ALL, "abc", char(0), "B", __LINE__, true, ""),
                       SR("B", "strip", LEFT, "abc", char(0), "B", __LINE__, true, ""),
                       SR("B", "strip", RIGHT, "abc", char(0), "B", __LINE__, true, ""),
                       SR("C", "strip", ALL, "abc", char(0), "C", __LINE__, true, ""),
                       SR("C", "strip", LEFT, "abc", char(0), "C", __LINE__, true, ""),
                       SR("C", "strip", RIGHT, "abc", char(0), "C", __LINE__, true, ""),

                       SR("", "replace", ALL, "abc", '#', "", __LINE__, false, ""),
                       SR("", "replace", LEFT, "abc", '#', "", __LINE__, false, ""),
                       SR("", "replace", RIGHT, "abc", '#', "", __LINE__, false, ""),
                       SR("a", "replace", ALL, "abc", '#', "#", __LINE__, false, "#"),
                       SR("a", "replace", LEFT, "abc", '#', "#", __LINE__, false, "#"),
                       SR("a", "replace", RIGHT, "abc", '#', "#", __LINE__, false, "#"),
                       SR("b", "replace", ALL, "abc", '#', "#", __LINE__, false, "#"),
                       SR("b", "replace", LEFT, "abc", '#', "#", __LINE__, false, "#"),
                       SR("b", "replace", RIGHT, "abc", '#', "#", __LINE__, false, "#"),
                       SR("c", "replace", ALL, "abc", '#', "#", __LINE__, false, "#"),
                       SR("c", "replace", LEFT, "abc", '#', "#", __LINE__, false, "#"),
                       SR("c", "replace", RIGHT, "abc", '#', "#", __LINE__, false, "#"),
                       SR("A", "replace", ALL, "abc", '#', "A", __LINE__, true, "#"),
                       SR("A", "replace", LEFT, "abc", '#', "A", __LINE__, true, "#"),
                       SR("A", "replace", RIGHT, "abc", '#', "A", __LINE__, true, "#"),
                       SR("B", "replace", ALL, "abc", '#', "B", __LINE__, true, "#"),
                       SR("B", "replace", LEFT, "abc", '#', "B", __LINE__, true, "#"),
                       SR("B", "replace", RIGHT, "abc", '#', "B", __LINE__, true, "#"),
                       SR("C", "replace", ALL, "abc", '#', "C", __LINE__, true, "#"),
                       SR("C", "replace", LEFT, "abc", '#', "C", __LINE__, true, "#"),
                       SR("C", "replace", RIGHT, "abc", '#', "C", __LINE__, true, "#"),

                       // not-so-trivial case-dependent
                       SR("aABbCc", "trim", ALL, "abc", char(0), "ABbC", __LINE__, true, ""),
                       SR("aABbCc", "trim", LEFT, "abc", char(0), "ABbCc", __LINE__, true, ""),
                       SR("aABbCc", "trim", RIGHT, "abc", char(0), "aABbC", __LINE__, true, ""),

                       SR("aABbCc", "strip", ALL, "abc", char(0), "ABC", __LINE__, true, ""),
                       SR("aABbCc", "strip", LEFT, "abc", char(0), "ABbCc", __LINE__, true, ""),
                       SR("aABbCc", "strip", RIGHT, "abc", char(0), "aABbC", __LINE__, true, ""),

                       SR("aABbCc", "replace", ALL, "abc", '#', "#AB#C#", __LINE__, true, "######"),
                       SR("aABbCc", "replace", LEFT, "abc", '#', "#ABbCc", __LINE__, true, "######"),
                       SR("aABbCc", "replace", RIGHT, "abc", '#', "aABbC#", __LINE__, true, "######"),

                       SR("a-A-B-b-c-C", "trim", ALL, "abc", char(0), "-A-B-b-c-C", __LINE__, true, "-A-B-b-c-"),
                       SR("a-A-B-b-c-C", "trim", LEFT, "abc", char(0), "-A-B-b-c-C", __LINE__, true, "-A-B-b-c-C"),
                       SR("a-A-B-b-c-C", "trim", RIGHT, "abc", char(0), "a-A-B-b-c-C", __LINE__, true, "a-A-B-b-c-"),

                       SR("a-A-B-b-c-C", "strip", ALL, "abc", char(0), "-A-B---C", __LINE__, true, "-----"),
                       SR("a-A-B-b-c-C", "strip", LEFT, "abc", char(0), "-A-B-b-c-C", __LINE__, true, "-A-B-b-c-C"),
                       SR("a-A-B-b-c-C", "strip", RIGHT, "abc", char(0), "a-A-B-b-c-C", __LINE__, true, "a-A-B-b-c-"),

                       SR("a-A-B-b-c-C", "replace", ALL, "abc", '#', "#-A-B-#-#-C", __LINE__, true, "#-#-#-#-#-#"),
                       SR("a-A-B-b-c-C", "replace", LEFT, "abc", '#', "#-A-B-b-c-C", __LINE__, true, "#-A-B-b-c-C"),
                       SR("a-A-B-b-c-C", "replace", RIGHT, "abc", '#', "a-A-B-b-c-C", __LINE__, true, "a-A-B-b-c-#"),
    };
    for (size_t i = 0; i < sizeof (modResults) / sizeof (SR); i++)
    {
        BOOST_CHECK(modResults[i].correctResult());
    }
}
struct DR
{
    DR(const string& param, const string result, size_t line)
    : param_(param)
    , result_(result)
    , line_(line)
    {

    }
    bool correctResult()
    {
        bool reval = (result_ == asString(scanDate(param_)));
        if (!reval)
            cout << " Line " << line_ << ": "
                << quoted(result_) << " != " << quoted(asString(scanDate(param_)))
            << " " << ends;
        return reval;
    }
    string param_;
    string result_;
    size_t line_;
};
void util_ci_traits_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing util string functions ========");
    ci_char_traits tr;
    BOOST_CHECK(tr.eq('a', 'a'));
    BOOST_CHECK(tr.eq('a', 'A'));
    BOOST_CHECK(tr.ne('a', 'B'));
    BOOST_CHECK(tr.lt('a', 'B'));

    BOOST_CHECK_EQUAL(tr.compare(0, 0, 2), 0);
    BOOST_CHECK_EQUAL(tr.compare(0, "a", 2), -1);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "aA", 2), 0);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "bA", 2), -1);
    BOOST_CHECK_EQUAL(tr.compare("Ba", "aA", 2), 1);
    BOOST_CHECK_EQUAL(tr.compare("aaaBa", "AAAaA", 5), 4);
    BOOST_CHECK_EQUAL(tr.compare("aaaaa", "AAABA", 5), -4);

    BOOST_CHECK_EQUAL(tr.compare(0, 0, 0), 0);
    BOOST_CHECK_EQUAL(tr.compare(0, "a", 0), 0);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "aA", 0), 0);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "bA", 0), 0);
    BOOST_CHECK_EQUAL(tr.compare("Ba", "aA", 0), 0);
    BOOST_CHECK_EQUAL(tr.compare("aaaBa", "AAAaA", 0), 0);
    BOOST_CHECK_EQUAL(tr.compare("aaaaa", "AAABA", 0), 0);

    BOOST_CHECK_EQUAL(tr.compare(0, 0, 1), 0);
    BOOST_CHECK_EQUAL(tr.compare(0, "a", 1), -1);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "aA", 1), 0);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "bA", 1), -1);
    BOOST_CHECK_EQUAL(tr.compare("Ba", "aA", 1), 1);
    BOOST_CHECK_EQUAL(tr.compare("aaaBa", "AAAaA", 1), 0);
    BOOST_CHECK_EQUAL(tr.compare("aaaaa", "AAABA", 1), 0);

    BOOST_CHECK_EQUAL(tr.compare(0, 0, 10), 0);
    BOOST_CHECK_EQUAL(tr.compare(0, "a", 10), -1);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "aA", 10), 0);
    BOOST_CHECK_EQUAL(tr.compare("Aa", "bA", 10), -1);
    BOOST_CHECK_EQUAL(tr.compare("Ba", "aA", 10), 1);
    BOOST_CHECK_EQUAL(tr.compare("aaaBa", "AAAaA", 10), 4);
    BOOST_CHECK_EQUAL(tr.compare("aaaaa", "AAABA", 10), -4);
}
void util_container_conversion_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing conversion functions for containers ========");
    set<string> sSet;
    BOOST_CHECK(sSet.empty());
    vector<string> sVec = vectorFromSet(sSet);
    BOOST_CHECK(sVec.empty());
    sSet = setFromVector(sVec);
    BOOST_CHECK(sSet.empty());

    sSet.insert("1stString");
    sSet.insert("2ndString");
    BOOST_CHECK(!sSet.empty());
    BOOST_CHECK_EQUAL(sSet.size(), 2);
    sVec = vectorFromSet(sSet);
    BOOST_CHECK(!sVec.empty());
    BOOST_CHECK_EQUAL(sVec.size(), 2);
    sSet = setFromVector(sVec);
    BOOST_CHECK(!sSet.empty());
    BOOST_CHECK_EQUAL(sSet.size(), 2);

    unordered_set<string> uSet;
    uSet.insert("x");
    uSet.insert("abc");
    uSet.insert("ngb");

    sSet = setFromUnordered(uSet);
    BOOST_CHECK(!sSet.empty());
    BOOST_CHECK_EQUAL(sSet.size(), 3);
}
template<typename T_>
void util_string_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing util string functions ========");

    T_ trimstring = "";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\t";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\n";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\r";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " \r\n ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " \r\t\t \n ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "a";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\ta";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "a\t";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\n\t";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\n";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\r";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " \r\n ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " \r\t\t \n ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " \r\ta\t \n ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\na";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "a\t   ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\t\t\t\ta     ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");

    T_ source = "123/3456/7890a";
    BOOST_TEST_MESSAGE("split " << source << " into vector by '/'");
    vector<T_> result = splitIntoVector(source, '/');
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "123");
    BOOST_CHECK_EQUAL(result[1], "3456");
    BOOST_CHECK_EQUAL(result[2], "7890a");

    BOOST_TEST_MESSAGE("split " << source << " into vector by '.'");
    result = splitIntoVector(source, '.');
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], "123/3456/7890a");

    BOOST_TEST_MESSAGE("split " << source << " into vector by \"37\"");
    result = splitIntoVector(source, "37");
    BOOST_CHECK_EQUAL(result.size(), 4);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "/");
    BOOST_CHECK_EQUAL(result[2], "456/");
    BOOST_CHECK_EQUAL(result[3], "890a");

    source = "1/4/7/1/7";
    BOOST_TEST_MESSAGE("split " << source << " into vector by \"/\"");
    result = splitIntoVector(source, "/");
    BOOST_CHECK_EQUAL(result.size(), 5);
    BOOST_CHECK_EQUAL(result[0], "1");
    BOOST_CHECK_EQUAL(result[1], "4");
    BOOST_CHECK_EQUAL(result[2], "7");
    BOOST_CHECK_EQUAL(result[3], "1");
    BOOST_CHECK_EQUAL(result[4], "7");

    source = "123/456/789/123/789";
    BOOST_TEST_MESSAGE("split " + source + " into set by '/'");
    set<T_> resultSet = splitIntoSet(source, '/');
    BOOST_CHECK_EQUAL(resultSet.size(), 3);
    result = vectorFromSet(resultSet);
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "123");
    BOOST_CHECK_EQUAL(result[1], "456");
    BOOST_CHECK_EQUAL(result[2], "789");

    BOOST_TEST_MESSAGE("split " << source << " into set by '.'");
    resultSet = splitIntoSet(source, '.');
    BOOST_CHECK_EQUAL(resultSet.size(), 1);
    result = vectorFromSet(resultSet);
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], "123/456/789/123/789");

    BOOST_TEST_MESSAGE("split " << source << " into set by \"37\"");
    resultSet = splitIntoSet(source, "37");
    BOOST_CHECK_EQUAL(resultSet.size(), 5);
    result = vectorFromSet(resultSet);
    BOOST_CHECK_EQUAL(result.size(), 5);
    BOOST_CHECK_EQUAL(result[0], "/");
    BOOST_CHECK_EQUAL(result[1], "/456/");
    BOOST_CHECK_EQUAL(result[2], "12");
    BOOST_CHECK_EQUAL(result[3], "89");
    BOOST_CHECK_EQUAL(result[4], "89/12");

    const T_ stripStr = " _ 123.456/789-0ab/_ _";
    T_ stripable = stripStr;

    BOOST_TEST_MESSAGE("strip " << stripable << " of \".\"");
    strip(stripable, ".");
    BOOST_CHECK_EQUAL(stripable, " _ 123456/789-0ab/_ _");
    stripable = stripStr;
    BOOST_TEST_MESSAGE("strip " << stripable << " of \"/\"");
    strip(stripable, "/");
    BOOST_CHECK_EQUAL(stripable, " _ 123.456789-0ab_ _");
    stripable = stripStr;
    BOOST_TEST_MESSAGE("strip " << stripable << " of \"./\"");
    strip(stripable, "./");
    BOOST_CHECK_EQUAL(stripable, " _ 123456789-0ab_ _");

    stripable = stripStr;
    BOOST_TEST_MESSAGE("trim " << stripable << " of \" \"");
    trim(stripable, " ");
    BOOST_CHECK_EQUAL(stripable, "_ 123.456/789-0ab/_ _");
    stripable = stripStr;
    BOOST_TEST_MESSAGE("trim " << stripable << " of \"_\"");
    trim(stripable, "_");
    BOOST_CHECK_EQUAL(stripable, " _ 123.456/789-0ab/_ ");

    stripable = stripStr;
    BOOST_TEST_MESSAGE("trim " << stripable << " of \" _\"");
    trim(stripable, " _");
    BOOST_CHECK_EQUAL(stripable, "123.456/789-0ab/");

    stripable = stripStr; // " _ 123.456/789-0ab/_ _"
    BOOST_TEST_MESSAGE("replaceChar " << stripable << " chars \"_\" with '#'");
    replaceChar(stripable, "_", '#');
    BOOST_CHECK_EQUAL(stripable, " # 123.456/789-0ab/# #");
    stripable = stripStr; // " _ 123.456/789-0ab/_ _"
    BOOST_TEST_MESSAGE("replaceChar " << stripable << " chars \" _\" with '#'");
    replaceChar(stripable, "_ ", '#');
    BOOST_CHECK_EQUAL(stripable, "###123.456/789-0ab/###");

    BOOST_CHECK_EQUAL(toLower(T_("SoMeStRiNg")), string("somestring"));
    BOOST_CHECK_EQUAL(toUpper(T_("SoMeStRiNg")), string("SOMESTRING"));
}
template<typename T_>
void util_string_left_right_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing util left/right trim/strip/replace functions ========");

    T_ trimstring = "";
    trimLeft(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "";
    trimRight(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");

    trimstring = " ";
    trimLeft(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " ";
    trimRight(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");

    trimstring = "\t";
    trimLeft(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\t";
    trimRight(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");

    trimstring = "\t";
    trimLeft(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\t";
    trimRight(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");

    trimstring = "\t";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\n";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "\r";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " \r\n ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = " \r\t\t \n ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "a";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\ta";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "a\t";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\ta\n";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\na";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "a\t   ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");
    trimstring = "\t\t\t\ta     ";
    trim(trimstring, " \n\t\r");
    BOOST_CHECK_EQUAL(trimstring, "a");

    T_ source = "123/3456/7890a";
    BOOST_TEST_MESSAGE("split " << source << " into vector by '/'");
    vector<T_> result = splitIntoVector(source, '/');
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "123");
    BOOST_CHECK_EQUAL(result[1], "3456");
    BOOST_CHECK_EQUAL(result[2], "7890a");

    BOOST_TEST_MESSAGE("split " << source << " into vector by '.'");
    result = splitIntoVector(source, '.');
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], "123/3456/7890a");

    BOOST_TEST_MESSAGE("split " << source << " into vector by \"37\"");
    result = splitIntoVector(source, "37");
    BOOST_CHECK_EQUAL(result.size(), 4);
    BOOST_CHECK_EQUAL(result[0], "12");
    BOOST_CHECK_EQUAL(result[1], "/");
    BOOST_CHECK_EQUAL(result[2], "456/");
    BOOST_CHECK_EQUAL(result[3], "890a");

    source = "1/4/7/1/7";
    BOOST_TEST_MESSAGE("split " << source << " into vector by \"/\"");
    result = splitIntoVector(source, "/");
    BOOST_CHECK_EQUAL(result.size(), 5);
    BOOST_CHECK_EQUAL(result[0], "1");
    BOOST_CHECK_EQUAL(result[1], "4");
    BOOST_CHECK_EQUAL(result[2], "7");
    BOOST_CHECK_EQUAL(result[3], "1");
    BOOST_CHECK_EQUAL(result[4], "7");

    source = "123/456/789/123/789";
    BOOST_TEST_MESSAGE("split " + source + " into set by '/'");
    set<T_> resultSet = splitIntoSet(source, '/');
    BOOST_CHECK_EQUAL(resultSet.size(), 3);
    result = vectorFromSet(resultSet);
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "123");
    BOOST_CHECK_EQUAL(result[1], "456");
    BOOST_CHECK_EQUAL(result[2], "789");

    BOOST_TEST_MESSAGE("split " << source << " into set by '.'");
    resultSet = splitIntoSet(source, '.');
    BOOST_CHECK_EQUAL(resultSet.size(), 1);
    result = vectorFromSet(resultSet);
    BOOST_CHECK_EQUAL(result.size(), 1);
    BOOST_CHECK_EQUAL(result[0], "123/456/789/123/789");

    BOOST_TEST_MESSAGE("split " << source << " into set by \"37\"");
    resultSet = splitIntoSet(source, "37");
    BOOST_CHECK_EQUAL(resultSet.size(), 5);
    result = vectorFromSet(resultSet);
    BOOST_CHECK_EQUAL(result.size(), 5);
    BOOST_CHECK_EQUAL(result[0], "/");
    BOOST_CHECK_EQUAL(result[1], "/456/");
    BOOST_CHECK_EQUAL(result[2], "12");
    BOOST_CHECK_EQUAL(result[3], "89");
    BOOST_CHECK_EQUAL(result[4], "89/12");

    const T_ stripStr = " _ 123.456/789-0ab/_ _";
    T_ stripable = stripStr;

    BOOST_TEST_MESSAGE("strip " << stripable << " of \".\"");
    strip(stripable, ".");
    BOOST_CHECK_EQUAL(stripable, " _ 123456/789-0ab/_ _");
    stripable = stripStr;
    BOOST_TEST_MESSAGE("strip " << stripable << " of \"/\"");
    strip(stripable, "/");
    BOOST_CHECK_EQUAL(stripable, " _ 123.456789-0ab_ _");
    stripable = stripStr;
    BOOST_TEST_MESSAGE("strip " << stripable << " of \"./\"");
    strip(stripable, "./");
    BOOST_CHECK_EQUAL(stripable, " _ 123456789-0ab_ _");

    stripable = stripStr;
    BOOST_TEST_MESSAGE("trim " << stripable << " of \" \"");
    trim(stripable, " ");
    BOOST_CHECK_EQUAL(stripable, "_ 123.456/789-0ab/_ _");
    stripable = stripStr;
    BOOST_TEST_MESSAGE("trim " << stripable << " of \"_\"");
    trim(stripable, "_");
    BOOST_CHECK_EQUAL(stripable, " _ 123.456/789-0ab/_ ");

    stripable = stripStr;
    BOOST_TEST_MESSAGE("trim " << stripable << " of \" _\"");
    trim(stripable, " _");
    BOOST_CHECK_EQUAL(stripable, "123.456/789-0ab/");

    stripable = stripStr;
    BOOST_TEST_MESSAGE("replaceChar " << stripable << " chars \"_\" with '#'");
    replaceChar(stripable, "_", '#');
    BOOST_CHECK_EQUAL(stripable, " # 123.456/789-0ab/# #");
    stripable = stripStr;
    BOOST_TEST_MESSAGE("replaceChar " << stripable << " chars \" _\" with '#'");
    replaceChar(stripable, "_ ", '#');
    BOOST_CHECK_EQUAL(stripable, "###123.456/789-0ab/###");
}
void util_ci_string_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing util ci_string case insensitivity functions ========");

    ci_string trimstring = "";
    trim(trimstring, "aBZd");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "aA";
    trim(trimstring, "aBZd");
    BOOST_CHECK_EQUAL(trimstring, "");
    trimstring = "BaAb";
    trim(trimstring, "aBZd");
    BOOST_CHECK_EQUAL(trimstring, "");

    trimstring = "zBaAZb";
    trim(trimstring, "aBZd");
    BOOST_CHECK_EQUAL(trimstring, "");

    trimstring = "zBadDDdAZb";
    trim(trimstring, "aBZd");
    BOOST_CHECK_EQUAL(trimstring, "");

    trimstring = "zB<SOMETHING>adDD</SOMETHING>dAZb";
    trim(trimstring, "aBZd");
    BOOST_CHECK_EQUAL(trimstring, "<SOMETHING>adDD</SOMETHING>");

    ci_string source = "123a456B789c78A";
    BOOST_TEST_MESSAGE("split " << source << " into vector by 'a'");
    vector<ci_string> result = splitIntoVector(source, 'a');
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "123");
    BOOST_CHECK_EQUAL(result[1], "456B789c78");
    BOOST_CHECK_EQUAL(result[2], "");

    source = "123a456B789c78A";
    BOOST_TEST_MESSAGE("split " << source << " into vector by 'A'");
    result = splitIntoVector(source, 'A');
    BOOST_CHECK_EQUAL(result.size(), 3);
    BOOST_CHECK_EQUAL(result[0], "123");
    BOOST_CHECK_EQUAL(result[1], "456B789c78");
    BOOST_CHECK_EQUAL(result[2], "");

    source = "xxxAXxXbxXxC";
    BOOST_TEST_MESSAGE("split " + source + " into set by \"abc\"");
    set<ci_string> resultSet = splitIntoSet(source, "abc");
    BOOST_CHECK_EQUAL(resultSet.size(), 2);
    result = vectorFromSet(resultSet);
    BOOST_CHECK_EQUAL(result.size(), 2);
    BOOST_CHECK_EQUAL(result[0], "");
    BOOST_CHECK_EQUAL(result[1], "xxx");


    const ci_string stripStr = "abCaaAxxxabcxxxcBA";
    ci_string stripable = stripStr;

    BOOST_TEST_MESSAGE("strip " << stripable << " of \"abc\"");
    strip(stripable, "abc");
    BOOST_CHECK_EQUAL(stripable, "xxxxxx");

    stripable = stripStr;
    BOOST_TEST_MESSAGE("replaceChar " << stripable << " chars \"abc\" with '#'");
    replaceChar(stripable, "abc", '#');
    BOOST_CHECK_EQUAL(stripable, "######xxx###xxx###");
}
void util_date_european_test(int i, int j)
{
    resetDateFormats();
    initDateFormats(PREFER_EUROPEAN_DATE_FORMAT);
    DR scanResults[] = {
                        // test all declinated formats
                        DR("1967-November-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%Y-%B-%d %H:%M:%S"
                        DR("1967-Nov-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%Y-%b-%d %H:%M:%S"
                        DR("67-November-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%y-%B-%d %H:%M:%S"
                        DR("67-Nov-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%y-%b-%d %H:%M:%S"

                        DR("November 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // American Format"%B %d %Y %H:%M:%S"
                        DR("Nov 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // American Format"%b %d %Y %H:%M:%S"
                        DR("November 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // American Format"%B %d %y %H:%M:%S"
                        DR("Nov 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // American Format"%b %d %y %H:%M:%S"

                        DR("10 November 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%d %B %Y %H:%M:%S"
                        DR("10 Nov 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%d %b %Y %H:%M:%S"
                        DR("10 November 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%d %B %y %H:%M:%S"
                        DR("10 Nov 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%d %b %y %H:%M:%S"

                        DR("Friday 10 November, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%A %d %B, %Y %H:%M:%S"
                        DR("Fri 10 November, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%a %d %B, %Y %H:%M:%S"
                        DR("Friday 10 Nov, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%A %d %b, %Y %H:%M:%S"
                        DR("Fri 10 Nov, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%a %d %b, %Y %H:%M:%S"
                        DR("Friday 10 November, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%A %d %B, %y %H:%M:%S"
                        DR("Fri 10 November, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%a %d %B, %y %H:%M:%S"
                        DR("Friday 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%A %d %b, %y %H:%M:%S"
                        DR("Fri 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%a %d %b, %y %H:%M:%S"

                        DR("10/28/1967 12:34:56", "1967-Oct-28 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("10/28/67 12:34:56", "2067-Oct-28 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"

                        DR("28/11/1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("28/11/67 12:34:56", "2067-Nov-28 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"

                        DR("28.11.1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("28.11.67 12:34:56", "2067-Nov-28 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"

                        DR("28-11-1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("28-11-67 12:34:56", "2067-Nov-28 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"

                        DR("19671110_123456", "1967-Nov-10 12:34:56", __LINE__), // "%Y%m%d_%H%M%S"
                        DR("671110_123456", "2067-Nov-10 12:34:56", __LINE__), // "%y%m%d_%H%M%S"

                        //        // time-only (USES CURRENT-DATE! Need to test separately.)
                        //        DR("12:34:56", "2014-Jan-27 12:34:56",__LINE__),                       // "%H:%M:%S" time only
                        //        DR("12:34", "2014-Jan-27 12:34:00",__LINE__),                          // "%H:%M" time only

                        DR("November 10 1967", "1967-Nov-10 00:00:00", __LINE__), // "%B %d %Y" American Format
                        DR("Nov 10 1967", "1967-Nov-10 00:00:00", __LINE__), // "%b %d %Y" American Format
                        DR("November 10 67", "2067-Nov-10 00:00:00", __LINE__), // "%B %d %y" American Format
                        DR("Nov 10 67", "2067-Nov-10 00:00:00", __LINE__), // "%b %d %y" American Format

                        DR("10 November 1967", "1967-Nov-10 00:00:00", __LINE__), // "%d %B %Y"
                        DR("10 Nov 1967", "1967-Nov-10 00:00:00", __LINE__), // "%d %b %Y"
                        DR("10 November 67", "2067-Nov-10 00:00:00", __LINE__), // "%d %B %y"
                        DR("10 Nov 67", "2067-Nov-10 00:00:00", __LINE__), // "%d %b %y"

                        DR("Friday 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%A %d %B, %Y"
                        DR("Fri 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%a %d %B, %Y"
                        DR("Friday 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%A %d %b, %Y"
                        DR("Fri 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%a %d %b, %Y"
                        DR("Friday 10 November, 67", "2067-Nov-10 00:00:00", __LINE__), // "%A %d %B, %y"
                        DR("Fri 10 November, 67", "2067-Nov-10 00:00:00", __LINE__), // "%a %d %B, %y"
                        DR("Friday 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__), // "%A %d %b, %y"
                        DR("Fri 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__), // "%a %d %b, %y"

                        DR("10/28/1967", "1967-Oct-28 00:00:00", __LINE__), // "%m/%d/%Y" American Format
                        DR("10/28/19", "2019-Oct-28 00:00:00", __LINE__), // "%m/%d/%y" American Format
                        DR("28/11/1967", "1967-Nov-28 00:00:00", __LINE__), // "%d/%m/%Y"
                        DR("28/11/67", "2067-Nov-28 00:00:00", __LINE__), // "%d/%m/%y"
                        DR("28.11.1967", "1967-Nov-28 00:00:00", __LINE__), // "%d.%m.%Y"
                        DR("28.11.67", "2067-Nov-28 00:00:00", __LINE__), // "%d.%m.%y"
                        DR("28-11-1967", "1967-Nov-28 00:00:00", __LINE__), // "%d-%m-%Y"
                        DR("28-11-67", "2067-Nov-28 00:00:00", __LINE__), // "%d-%m-%y"
                        DR("19671110", "1967-Nov-10 00:00:00", __LINE__), // "%Y%m%d"
                        DR("671110", "6711-Oct-01 00:00:00", __LINE__), // "%y%m%d"

                        // test formats that might require leading zeros
                        DR("November 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // American Format"%B %d %Y %H:%M:%S"
                        DR("Nov 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // American Format"%b %d %Y %H:%M:%S"
                        DR("November 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // American Format"%B %d %y %H:%M:%S"
                        DR("Nov 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // American Format"%b %d %y %H:%M:%S"

                        DR("3 November 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%d %B %Y %H:%M:%S"
                        DR("3 Nov 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%d %b %Y %H:%M:%S"
                        DR("3 November 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%d %B %y %H:%M:%S"
                        DR("3 Nov 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%d %b %y %H:%M:%S"

                        DR("Friday 3 November, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%A %d %B, %Y %H:%M:%S"
                        DR("Fri 3 November, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%a %d %B, %Y %H:%M:%S"
                        DR("Friday 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%A %d %b, %Y %H:%M:%S"
                        DR("Fri 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%a %d %b, %Y %H:%M:%S"
                        DR("Friday 3 November, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%A %d %B, %y %H:%M:%S"
                        DR("Fri 3 November, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%a %d %B, %y %H:%M:%S"
                        DR("Friday 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%A %d %b, %y %H:%M:%S"
                        DR("Fri 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%a %d %b, %y %H:%M:%S"

                        DR("3/28/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("3/28/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"
                        DR("10/3/1967 12:34:56", "1967-Mar-10 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("10/3/67 12:34:56", "2067-Mar-10 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"
                        DR("9/3/1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("9/3/67 12:34:56", "2067-Mar-09 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"

                        DR("3/11/1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("3/11/67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"
                        DR("28/3/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("28/3/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"
                        DR("9/3/1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("9/3/67 12:34:56", "2067-Mar-09 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"

                        DR("3.11.1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("3.11.67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"
                        DR("28.3.1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("28.3.67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"
                        DR("9.3.1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("9.3.67 12:34:56", "2067-Mar-09 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"

                        DR("3-11-1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("3-11-67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"
                        DR("28-3-1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("28-3-67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"
                        DR("9-3-1967 12:34:56", "1967-Mar-09 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("9-3-67 12:34:56", "2067-Mar-09 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"
    };
    for (size_t i = 0; i < sizeof (scanResults) / sizeof (DR); i++)
    {
        BOOST_CHECK(scanResults[i].correctResult());
    }


    resetDateFormats();
    addDateFormat("%H:%M:%S");
    addDateFormat("%H:%M");
    ptime dt = scanDate("12:34:56");
    BOOST_CHECK_EQUAL("12:34:56", asString(dt.time_of_day()));
    dt = scanDate("12:34");
    BOOST_CHECK_EQUAL("12:34:00", asString(dt.time_of_day()));

    resetDateFormats();
    for (size_t i = 0; i < sizeof (scanResults) / sizeof (DR); i++)
    {
        BOOST_CHECK_EQUAL("not-a-date-time", asString(scanDate(scanResults[i].param_)));
    }
    addDateFormat("%H:%Y");
    BOOST_CHECK_EQUAL("2013-Jan-01 13:00:00", asString(scanDate("13:2013")));
}
void util_date_american_test(int i, int j)
{
    resetDateFormats();
    initDateFormats(PREFER_US_DATE_FORMAT);
    DR scanResults[] = {
                        // test all declinated formats
                        DR("1967-November-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%Y-%B-%d %H:%M:%S"
                        DR("1967-Nov-10 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%Y-%b-%d %H:%M:%S"
                        DR("67-November-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%y-%B-%d %H:%M:%S"
                        DR("67-Nov-10 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%y-%b-%d %H:%M:%S"

                        DR("November 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // American Format"%B %d %Y %H:%M:%S"
                        DR("Nov 10 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // American Format"%b %d %Y %H:%M:%S"
                        DR("November 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // American Format"%B %d %y %H:%M:%S"
                        DR("Nov 10 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // American Format"%b %d %y %H:%M:%S"

                        DR("10 November 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%d %B %Y %H:%M:%S"
                        DR("10 Nov 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%d %b %Y %H:%M:%S"
                        DR("10 November 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%d %B %y %H:%M:%S"
                        DR("10 Nov 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%d %b %y %H:%M:%S"

                        DR("Friday 10 November, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%A %d %B, %Y %H:%M:%S"
                        DR("Fri 10 November, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%a %d %B, %Y %H:%M:%S"
                        DR("Friday 10 Nov, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%A %d %b, %Y %H:%M:%S"
                        DR("Fri 10 Nov, 1967 12:34:56", "1967-Nov-10 12:34:56", __LINE__), // "%a %d %b, %Y %H:%M:%S"
                        DR("Friday 10 November, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%A %d %B, %y %H:%M:%S"
                        DR("Fri 10 November, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%a %d %B, %y %H:%M:%S"
                        DR("Friday 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%A %d %b, %y %H:%M:%S"
                        DR("Fri 10 Nov, 67 12:34:56", "2067-Nov-10 12:34:56", __LINE__), // "%a %d %b, %y %H:%M:%S"

                        DR("10/28/1967 12:34:56", "1967-Oct-28 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("10/28/67 12:34:56", "2067-Oct-28 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"

                        DR("28/11/1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("28/11/67 12:34:56", "2067-Nov-28 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"

                        DR("28.11.1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("28.11.67 12:34:56", "2067-Nov-28 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"

                        DR("28-11-1967 12:34:56", "1967-Nov-28 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("28-11-67 12:34:56", "2067-Nov-28 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"

                        DR("19671110_123456", "1967-Nov-10 12:34:56", __LINE__), // "%Y%m%d_%H%M%S"
                        DR("671110_123456", "2067-Nov-10 12:34:56", __LINE__), // "%y%m%d_%H%M%S"

                        //        // time-only (USES CURRENT-DATE! Need to test separately.)
                        //        DR("12:34:56", "2014-Jan-27 12:34:56",__LINE__),                       // "%H:%M:%S" time only
                        //        DR("12:34", "2014-Jan-27 12:34:00",__LINE__),                          // "%H:%M" time only

                        DR("November 10 1967", "1967-Nov-10 00:00:00", __LINE__), // "%B %d %Y" American Format
                        DR("Nov 10 1967", "1967-Nov-10 00:00:00", __LINE__), // "%b %d %Y" American Format
                        DR("November 10 67", "2067-Nov-10 00:00:00", __LINE__), // "%B %d %y" American Format
                        DR("Nov 10 67", "2067-Nov-10 00:00:00", __LINE__), // "%b %d %y" American Format

                        DR("10 November 1967", "1967-Nov-10 00:00:00", __LINE__), // "%d %B %Y"
                        DR("10 Nov 1967", "1967-Nov-10 00:00:00", __LINE__), // "%d %b %Y"
                        DR("10 November 67", "2067-Nov-10 00:00:00", __LINE__), // "%d %B %y"
                        DR("10 Nov 67", "2067-Nov-10 00:00:00", __LINE__), // "%d %b %y"

                        DR("Friday 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%A %d %B, %Y"
                        DR("Fri 10 November, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%a %d %B, %Y"
                        DR("Friday 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%A %d %b, %Y"
                        DR("Fri 10 Nov, 1967", "1967-Nov-10 00:00:00", __LINE__), // "%a %d %b, %Y"
                        DR("Friday 10 November, 67", "2067-Nov-10 00:00:00", __LINE__), // "%A %d %B, %y"
                        DR("Fri 10 November, 67", "2067-Nov-10 00:00:00", __LINE__), // "%a %d %B, %y"
                        DR("Friday 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__), // "%A %d %b, %y"
                        DR("Fri 10 Nov, 67", "2067-Nov-10 00:00:00", __LINE__), // "%a %d %b, %y"

                        DR("10/28/1967", "1967-Oct-28 00:00:00", __LINE__), // "%m/%d/%Y" American Format
                        DR("10/28/19", "2019-Oct-28 00:00:00", __LINE__), // "%m/%d/%y" American Format
                        DR("28/11/1967", "1967-Nov-28 00:00:00", __LINE__), // "%d/%m/%Y"
                        DR("28/11/67", "2067-Nov-28 00:00:00", __LINE__), // "%d/%m/%y"
                        DR("28.11.1967", "1967-Nov-28 00:00:00", __LINE__), // "%d.%m.%Y"
                        DR("28.11.67", "2067-Nov-28 00:00:00", __LINE__), // "%d.%m.%y"
                        DR("28-11-1967", "1967-Nov-28 00:00:00", __LINE__), // "%d-%m-%Y"
                        DR("28-11-67", "2067-Nov-28 00:00:00", __LINE__), // "%d-%m-%y"
                        DR("19671110", "1967-Nov-10 00:00:00", __LINE__), // "%Y%m%d"
                        DR("671110", "6711-Oct-01 00:00:00", __LINE__), // "%y%m%d"

                        // test formats that might require leading zeros
                        DR("November 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // American Format"%B %d %Y %H:%M:%S"
                        DR("Nov 3 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // American Format"%b %d %Y %H:%M:%S"
                        DR("November 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // American Format"%B %d %y %H:%M:%S"
                        DR("Nov 3 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // American Format"%b %d %y %H:%M:%S"

                        DR("3 November 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%d %B %Y %H:%M:%S"
                        DR("3 Nov 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%d %b %Y %H:%M:%S"
                        DR("3 November 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%d %B %y %H:%M:%S"
                        DR("3 Nov 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%d %b %y %H:%M:%S"

                        DR("Friday 3 November, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%A %d %B, %Y %H:%M:%S"
                        DR("Fri 3 November, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%a %d %B, %Y %H:%M:%S"
                        DR("Friday 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%A %d %b, %Y %H:%M:%S"
                        DR("Fri 3 Nov, 1967 12:34:56", "1967-Nov-03 12:34:56", __LINE__), // "%a %d %b, %Y %H:%M:%S"
                        DR("Friday 3 November, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%A %d %B, %y %H:%M:%S"
                        DR("Fri 3 November, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%a %d %B, %y %H:%M:%S"
                        DR("Friday 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%A %d %b, %y %H:%M:%S"
                        DR("Fri 3 Nov, 67 12:34:56", "2067-Nov-03 12:34:56", __LINE__), // "%a %d %b, %y %H:%M:%S"

                        DR("3/28/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("3/28/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"
                        DR("10/3/1967 12:34:56", "1967-Oct-03 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("10/3/67 12:34:56", "2067-Oct-03 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"
                        DR("9/3/1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__), //  American Format"%m/%d/%Y %H:%M:%S"
                        DR("9/3/67 12:34:56", "2067-Sep-03 12:34:56", __LINE__), //  American Format"%m/%d/%y %H:%M:%S"

                        DR("3/11/1967 12:34:56", "1967-Mar-11 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("3/11/67 12:34:56", "2067-Mar-11 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"
                        DR("28/3/1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("28/3/67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"
                        DR("9/3/1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__), // "%d/%m/%Y %H:%M:%S"
                        DR("9/3/67 12:34:56", "2067-Sep-03 12:34:56", __LINE__), // "%d/%m/%y %H:%M:%S"

                        DR("3.11.1967 12:34:56", "1967-Mar-11 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("3.11.67 12:34:56", "2067-Mar-11 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"
                        DR("28.3.1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("28.3.67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"
                        DR("9.3.1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__), // "%d.%m.%Y %H:%M:%S"
                        DR("9.3.67 12:34:56", "2067-Sep-03 12:34:56", __LINE__), // "%d.%m.%y %H:%M:%S"

                        DR("3-11-1967 12:34:56", "1967-Mar-11 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("3-11-67 12:34:56", "2067-Mar-11 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"
                        DR("28-3-1967 12:34:56", "1967-Mar-28 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("28-3-67 12:34:56", "2067-Mar-28 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"
                        DR("9-3-1967 12:34:56", "1967-Sep-03 12:34:56", __LINE__), // "%d-%m-%Y %H:%M:%S"
                        DR("9-3-67 12:34:56", "2067-Sep-03 12:34:56", __LINE__), // "%d-%m-%y %H:%M:%S"
    };
    for (size_t i = 0; i < sizeof (scanResults) / sizeof (DR); i++)
    {
        BOOST_CHECK(scanResults[i].correctResult());
    }
}
void util_any_test(int ii, int jj)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing any templates functions ========");

    BOOST_TEST_MESSAGE("boolean values...");
    bool bYes = scanAs<bool>("t");
    BOOST_CHECK_EQUAL(bYes, true);
    bYes = scanAs<bool>("TrUe");
    BOOST_CHECK_EQUAL(bYes, true);
    bYes = scanAs<bool>("yEs");
    BOOST_CHECK_EQUAL(bYes, true);
    bYes = scanAs<bool>("oN");
    BOOST_CHECK_EQUAL(bYes, true);
    BOOST_CHECK_EQUAL(bYes, true);

    bool bNo = scanAs<bool>("F");
    BOOST_CHECK_EQUAL(bNo, false);
    bNo = scanAs<bool>("F");
    BOOST_CHECK_EQUAL(bNo, false);
    bNo = scanAs<bool>("fAlSe");
    BOOST_CHECK_EQUAL(bNo, false);
    bNo = scanAs<bool>("ofF");
    BOOST_CHECK_EQUAL(bNo, false);
    bNo = scanAs<bool>("NO");
    BOOST_CHECK_EQUAL(bNo, false);

    BOOST_TEST_MESSAGE("int values...");
    VAR_INT i = scanAs<VAR_INT>("1234");
    BOOST_CHECK_EQUAL(i, 1234);
    i = scanAs<VAR_INT>("-666");
    BOOST_CHECK_EQUAL(i, -666);
    VAR_UINT ui = scanAs<VAR_UINT>("1234");
    BOOST_CHECK_EQUAL(ui, 1234);

    BOOST_TEST_MESSAGE("float values...");
    VAR_FLOAT f = scanAs<VAR_FLOAT>("-1234.3456");
    BOOST_CHECK_CLOSE(f, -1234.3456L, 1e-27);
    f = scanAs<VAR_FLOAT>("1234.34e-31");
    BOOST_CHECK_CLOSE(f, 1234.34e-31L, 1e-27);

    Var anAny(string("a T_"));
    BOOST_CHECK(isA<string>(anAny));
    anAny = (VAR_INT) 5L;
    BOOST_CHECK(isA<VAR_INT>(anAny));

    initDateFormats();
    anAny = scanAsVar<VAR_DATE>("1/11/2012 12:45:21");
    BOOST_CHECK_EQUAL(anAny, toDate(2012, 11, 1, 12, 45, 21));

    Var a1(VAR_UINT(1234UL));
    Var a2(VAR_UINT(1235UL));
    Var a3(string("A"));
    Var a4(string("B"));
    Var a5(false);
    Var a6(true);

    BOOST_CHECK(a1 < a2);
    BOOST_CHECK(a3 < a4);
    BOOST_CHECK(a5 < a6);
    BOOST_CHECK(a1 <= a2);
    BOOST_CHECK(a3 <= a4);
    BOOST_CHECK(a5 <= a6);
    BOOST_CHECK_LE(a1, a2);
    BOOST_CHECK_LE(a3, a4);
    BOOST_CHECK_LE(a5, a6);
    BOOST_CHECK(a2 >= a1);
    BOOST_CHECK(a4 >= a3);
    BOOST_CHECK(a6 >= a5);
    BOOST_CHECK_GE(a2, a1);
    BOOST_CHECK_GE(a4, a3);
    BOOST_CHECK_GE(a6, a5);

    Var v1 = VAR_UINT(1234UL);
    Var v2 = VAR_UINT(1235UL);
    BOOST_CHECK(v1 < v2);
    BOOST_CHECK_LT(v1, v2);

    Interval<VAR_FLOAT> interval(0.0L, 1.0L);
    BOOST_CHECK(interval.contains(0.5));
    Interval<VAR_FLOAT> interval2(-1.0L, 2.0L);
    BOOST_CHECK(interval.isSubIntervalOf(interval2));

    VAR_UINT_INTERVAL itvlVar0;
    BOOST_CHECK(itvlVar0.contains(VAR_UINT(8)));
    BOOST_CHECK(VAR_UINT_INTERVAL(2, true).isSubIntervalOf(itvlVar0));
    BOOST_CHECK(VAR_UINT_INTERVAL(2, false).isSubIntervalOf(itvlVar0));
    BOOST_CHECK(VAR_UINT_INTERVAL(4ULL, 12344ULL).isSubIntervalOf(itvlVar0));
    VAR_FLOAT_INTERVAL itvlVar1(5.0L, true);
    BOOST_CHECK(itvlVar1.contains(VAR_FLOAT(4.0)));
    BOOST_CHECK(VAR_FLOAT_INTERVAL(2.234, true).isSubIntervalOf(itvlVar1));
    BOOST_CHECK(!VAR_FLOAT_INTERVAL(2.234, false).isSubIntervalOf(itvlVar1));
    VAR_CHAR_INTERVAL itvlVar2 = VAR_CHAR_INTERVAL(VAR_CHAR('c'), false);
    BOOST_CHECK(itvlVar2.contains(VAR_CHAR('z')));
    BOOST_CHECK(!VAR_CHAR_INTERVAL('f', true).isSubIntervalOf(itvlVar2));
    BOOST_CHECK(!VAR_CHAR_INTERVAL('a', false).isSubIntervalOf(itvlVar2));
}
template<typename T_>
void util_any_interval_test(T_ i, T_ j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing any interval functions ========");
    typedef Interval<T_> INTERV;

    BOOST_CHECK(INTERV() == INTERV());
    BOOST_CHECK(!(INTERV() < INTERV()));
    set<INTERV> s;
    BOOST_CHECK(s.empty());
    s.insert(INTERV());
    BOOST_CHECK(!s.empty());
    BOOST_CHECK_EQUAL(s.size(), 1);
    BOOST_CHECK(s.find(INTERV()) != s.end());
    typename set<INTERV>::iterator found = s.find(INTERV());
    BOOST_CHECK_EQUAL(*found, INTERV());
    s.insert(INTERV(i, j));
    BOOST_CHECK(!s.empty());
    BOOST_CHECK_EQUAL(s.size(), 2);
    BOOST_CHECK(s.find(INTERV(i, j)) != s.end());
    found = s.find(INTERV(i, j));
    BOOST_CHECK_EQUAL(*found, INTERV(i, j));
}
void util_csv_test(int i, int j)
{
    initDateFormats();
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing csv functions ========");
    {
        BOOST_TEST_MESSAGE("Construct with header and type-rows");
        BOOST_TEST_MESSAGE("Also check whether the case insensitive type-strings work");
        CSVAnalyzer csv("Col1,Col2,Col3,Col4", "Text,d,real,ordInal");
        csv << string("abc, 10/11/67, 3.14159265, 5");

        BOOST_CHECK_EQUAL(csv.getString(0, 0), "abc");
        BOOST_CHECK_EQUAL(csv.getDate(1, 0), VAR_DATE(date(2067, 11, 10)));
        BOOST_CHECK_CLOSE(csv.getFloat(2, 0), 3.14159265L, 0.000000001);
        BOOST_CHECK_EQUAL(csv.getUint(3, 0), 5);
    }
    {
        BOOST_TEST_MESSAGE("Construct with header row only");
        CSVAnalyzer csv("Col1,Col2,Col3,Col4");
        BOOST_TEST_MESSAGE("create types (guessed from field-value)");
        csv << string("abc, 10/11/67, 3.14159265, 5");

        BOOST_CHECK_EQUAL(csv.getString(0, 0), "abc");
        BOOST_CHECK_EQUAL(csv.getDate(1, 0), VAR_DATE(date(2067, 11, 10)));
        BOOST_CHECK_CLOSE(csv.getFloat(2, 0), 3.14159265L, 0.000000001);
        BOOST_CHECK_EQUAL(csv.getInt(3, 0), 5);
    }
    {
        BOOST_TEST_MESSAGE("Default Construct (no header /types rows)");
        CSVAnalyzer csv;
        BOOST_TEST_MESSAGE("create default headers and types");
        csv << string("abc, 10/11/67, 3.14159265, -10,0,8,yes,Off,"
                      "999999999999999999,1000000000000000000,9223372036854775807,"
                      "9223372036854775808,10223372036854775807");

        BOOST_CHECK_EQUAL(csv.getString(0, 0), "abc");
        BOOST_CHECK_EQUAL(csv.getDate(1, 0), VAR_DATE(date(2067, 11, 10)));
        BOOST_CHECK_CLOSE(csv.getFloat(2, 0), 3.14159265L, 0.000000001);
        BOOST_CHECK_EQUAL(csv.getInt(3, 0), -10);
        BOOST_CHECK_EQUAL(csv.getInt(4, 0), 0);
        BOOST_CHECK_EQUAL(csv.getInt(5, 0), 8);
        BOOST_CHECK_EQUAL(csv.getBool(6, 0), true);
        BOOST_CHECK_EQUAL(csv.getBool(7, 0), false);
        BOOST_CHECK_EQUAL(csv.getInt(8, 0), 999999999999999999); // last one converted to  int
        BOOST_CHECK_EQUAL(csv.getUint(9, 0), 1000000000000000000); // first one converted to uint
        BOOST_CHECK_EQUAL(csv.getUint(10, 0), 9223372036854775807); // max long long
        BOOST_CHECK_EQUAL(csv.getUint(11, 0), 9223372036854775808UL); // one bigger
        BOOST_CHECK_EQUAL(csv.getUint(12, 0), 10223372036854775807UL); // a lot bigger
    }
    {
        BOOST_TEST_MESSAGE("Get sub-csv");
        CSVAnalyzer csv;
        BOOST_TEST_MESSAGE("create default headers and types");
        csv << string("abc, 10/11/67, 3.14159265, -10,0,8,yes,Off");

        BOOST_CHECK_EQUAL(csv.getString(0, 0), "abc");
        BOOST_CHECK_EQUAL(csv.getDate(1, 0), VAR_DATE(date(2067, 11, 10)));
        BOOST_CHECK_CLOSE(csv.getFloat(2, 0), 3.14159265L, 0.000000001);
        BOOST_CHECK_EQUAL(csv.getInt(3, 0), -10);
        BOOST_CHECK_EQUAL(csv.getInt(4, 0), 0);
        BOOST_CHECK_EQUAL(csv.getInt(5, 0), 8);
        BOOST_CHECK_EQUAL(csv.getBool(6, 0), true);
        BOOST_CHECK_EQUAL(csv.getBool(7, 0), false);

        csv = csv.getSub(1, 2, 5, 6);
        BOOST_CHECK_EQUAL(csv.getDate(0, 0), VAR_DATE(date(2067, 11, 10)));
        BOOST_CHECK_CLOSE(csv.getFloat(1, 0), 3.14159265L, 0.000000001);
        BOOST_CHECK_EQUAL(csv.getInt(2, 0), 8);
        BOOST_CHECK_EQUAL(csv.getBool(3, 0), true);

        csv = csv.getSub("Column2", "Column6");
        BOOST_CHECK_CLOSE(csv.getFloat(0, 0), 3.14159265L, 0.000000001);
        BOOST_CHECK_EQUAL(csv.getBool(1, 0), true);
    }
    {
        BOOST_TEST_MESSAGE("Read/Write csv to file");
        CSVAnalyzer csv("Col1,Col2,Col3,Col4", "Text,d,real,ordInal");
        csv << string("abc, 10/11/67, 3.14159265, 5");
        csv << string("def, 10/03/74, 1.41421356, 10");

        BOOST_TEST_MESSAGE("write in default format");
        csv.write(filename);
        {
            ifstream ifs(filename.c_str());
            vector<string> lines;
            string line;
            while (!ifs.eof())
            {
                getline(ifs, line);
                lines.push_back(line);
            }
            BOOST_CHECK_EQUAL(lines.size(), 5);
            BOOST_CHECK_EQUAL(lines[0], "Col1,Col2,Col3,Col4");
            BOOST_CHECK_EQUAL(lines[1], "string,date,float,uint");
            BOOST_CHECK_EQUAL(lines[2], "abc,2067-Nov-10 00:00:00,3.14159,5");
            BOOST_CHECK_EQUAL(lines[3], "def,2074-Mar-10 00:00:00,1.41421,10");
        }
        BOOST_TEST_MESSAGE("read in default format");
        csv.read(filename);
        BOOST_CHECK_EQUAL(csv.columns(), 4);
        BOOST_CHECK_EQUAL(csv.lines(), 2);
        BOOST_CHECK_EQUAL(csv.header(0), "Col1");
        BOOST_CHECK_EQUAL(csv.header(3), "Col4");
        BOOST_CHECK_EQUAL(csv.type(1), "date");
        BOOST_CHECK_EQUAL(csv.type(2), "float");
        BOOST_CHECK_EQUAL(csv.getString("Col1", 1), "def");
        BOOST_CHECK_CLOSE(csv.getFloat("Col3", 0), 3.14159, 0.000001);
        BOOST_CHECK_EQUAL(csv.getDate("Col2", 0), VAR_DATE(date(2067, 11, 10)));

        BOOST_TEST_MESSAGE("write using different delimiter");
        csv.write(filename, " | ");
        {
            ifstream ifs(filename.c_str());
            vector<string> lines;
            string line;
            while (!ifs.eof())
            {
                getline(ifs, line);
                lines.push_back(line);
            }
            BOOST_CHECK_EQUAL(lines.size(), 5);
            BOOST_CHECK_EQUAL(lines[0], "Col1 | Col2 | Col3 | Col4");
            BOOST_CHECK_EQUAL(lines[1], "string | date | float | uint");
            BOOST_CHECK_EQUAL(lines[2], "abc | 2067-Nov-10 00:00:00 | 3.14159 | 5");
            BOOST_CHECK_EQUAL(lines[3], "def | 2074-Mar-10 00:00:00 | 1.41421 | 10");
        }

        BOOST_TEST_MESSAGE("read using different delimiter");
        csv.read(filename, "|");
        BOOST_CHECK_EQUAL(csv.columns(), 4);
        BOOST_CHECK_EQUAL(csv.lines(), 2);
        BOOST_CHECK_EQUAL(csv.header(0), "Col1");
        BOOST_CHECK_EQUAL(csv.header(3), "Col4");
        BOOST_CHECK_EQUAL(csv.type(1), "date");
        BOOST_CHECK_EQUAL(csv.type(2), "float");
        BOOST_CHECK_EQUAL(csv.getString("Col1", 1), "def");
        BOOST_CHECK_CLOSE(csv.getFloat("Col3", 0), 3.14159, 0.000001);
        BOOST_CHECK_EQUAL(csv.getDate("Col2", 0), VAR_DATE(date(2067, 11, 10)));

        BOOST_TEST_MESSAGE("write using different delimiter and without header/types");
        csv.write(filename, " & ", CSVAnalyzer::hasValues);
        {
            ifstream ifs(filename.c_str());
            vector<string> lines;
            string line;
            while (!ifs.eof())
            {
                getline(ifs, line);
                lines.push_back(line);
            }
            BOOST_CHECK_EQUAL(lines.size(), 3);
            BOOST_CHECK_EQUAL(lines[0], "abc & 2067-Nov-10 00:00:00 & 3.14159 & 5");
            BOOST_CHECK_EQUAL(lines[1], "def & 2074-Mar-10 00:00:00 & 1.41421 & 10");
        }

        BOOST_TEST_MESSAGE("read using different delimiter and without header/types");
        csv.read(filename, "&", CSVAnalyzer::hasValues);
        BOOST_CHECK_EQUAL(csv.columns(), 4);
        BOOST_CHECK_EQUAL(csv.lines(), 2);
        BOOST_CHECK_EQUAL(csv.header(0), "Column0");
        BOOST_CHECK_EQUAL(csv.header(3), "Column3");
        BOOST_CHECK_EQUAL(csv.type(1), "date");
        BOOST_CHECK_EQUAL(csv.type(2), "float");
        BOOST_CHECK_EQUAL(csv.getString("Column0", 1), "def");
        BOOST_CHECK_CLOSE(csv.getFloat("Column2", 0), 3.14159, 0.000001);
        BOOST_CHECK_EQUAL(csv.getDate("Column1", 0), VAR_DATE(date(2067, 11, 10)));

        BOOST_TEST_MESSAGE("directly write csv using stream");
        {
            // write a sample data file
            ofstream ofs(filename.c_str());
            ofs << "Cloud , Rain         , Sprinkler , WetGrass" << endl;
            ofs << "bool  , string       , int       , bool" << endl;
            ofs << "yes   , none         , 5         , yes" << endl;
            ofs << "no    , none         , 1         , no" << endl;
            ofs << "yes   , heavy shower , 2         , yes" << endl;
            ofs << "yes   , light shower , 0         , yes" << endl;
        }
        CSVAnalyzer data;
        data.read(filename);
        BOOST_CHECK_EQUAL(data.columns(), 4);
        BOOST_CHECK_EQUAL(data.lines(), 4);
        BOOST_CHECK_EQUAL(data.get<string>("Rain", 2), "heavy shower");
        BOOST_CHECK_EQUAL(data.get<bool>(3, 2), true);
        BOOST_CHECK(data.begin("Rain") != data.end("Rain"));
        CSVAnalyzer::COLUMN_TYPE_ITER it = data.begin("Rain");
        size_t lineCount = 0;
        for (; it != data.end("Rain"); it++, lineCount++)
            BOOST_TEST_MESSAGE(*it);
        BOOST_CHECK(it == data.end("Rain"));
        BOOST_CHECK_EQUAL(lineCount, data.lines() + 2);

        vector<string> v;
        v.push_back("Rain");
        v.push_back("Cloud");

        CSVAnalyzer sub = data.getSub(v);
        BOOST_CHECK_EQUAL(sub.columns(), 2);
        BOOST_CHECK_EQUAL(sub.lines(), 4);
        BOOST_CHECK_THROW(sub.get<string>("Sprinkler", 2), index_error);
        BOOST_CHECK(sub.begin("Rain") != sub.end("Rain"));
        BOOST_CHECK_THROW(sub.begin("Sprinkler"), index_error);
        it = sub.begin("Rain");
        size_t lineCountSub = 0;
        for (; it != sub.end("Rain"); it++, lineCountSub++)
            BOOST_TEST_MESSAGE(*it);
        BOOST_CHECK(it == sub.end("Rain"));
        BOOST_CHECK_EQUAL(lineCountSub, sub.lines() + 2);
        BOOST_CHECK_EQUAL(lineCountSub, lineCount);

        BOOST_TEST_MESSAGE("make sure that columns are in the correct order");
        BOOST_CHECK_EQUAL(sub.header(0), "Rain");
        BOOST_CHECK_EQUAL(sub.header(1), "Cloud");
        BOOST_CHECK_EQUAL(sub.type(0), "string");
        BOOST_CHECK_EQUAL(sub.type(1), "bool");
        for (size_t line = 0; line < sub.lines(); line++)
        {
            BOOST_CHECK_EQUAL(sub.getString(0, line), sub.getString("Rain", line));
            BOOST_CHECK_EQUAL(sub.getBool(1, line), sub.getBool("Cloud", line));
        }
    }
    if (is_regular_file(filename))
    {
        remove(path(filename));
    }
}
void util_event_test(int i, int j)
{
    initDateFormats();
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing event/eventlist/condition event functions ========");
    {
        BOOST_TEST_MESSAGE("Two different EventLists of size 1");
        EventList el1 = Event("E1", true);
        EventList el2 = Event("E3", false);
        BOOST_CHECK_LT(el1, el2);

        BOOST_TEST_MESSAGE("Two *IDENTICAL* EventLists of size 1");
        el1 = Event("E1", true);
        el2 = Event("E1", true);
        BOOST_CHECK(!(el1 < el2));
        BOOST_CHECK(Event("E1", true).notConflicting(Event("E1", true)));
        BOOST_CHECK(!Event("E1", true).notConflicting(Event("E1", false)));
        BOOST_CHECK(Event("E1", true).notConflicting(Event("E2", true)));
        BOOST_CHECK(Event("E1", true).notConflicting(Event("E2", false)));

        BOOST_TEST_MESSAGE("Two EventLists of different size where one is front of other");
        el1 = Event("E1", true);
        el2 = Event("E1", true) && Event("E2", false);
        BOOST_CHECK_LT(el1, el2);
        BOOST_CHECK(!(el2 < el1));
        BOOST_CHECK(el1.notConflicting(el1));
        BOOST_CHECK(el2.notConflicting(el2));
        BOOST_CHECK(el2.notConflicting(el1));
        BOOST_CHECK(el1.notConflicting(el2));


        BOOST_TEST_MESSAGE("Two EventLists *IDENTICAL* to a certain size then different, but same length");
        el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        BOOST_CHECK_LT(el1, el2);
        BOOST_CHECK(!(el2 < el1));
        el1 = Event("E1", true) && Event("E2", false) && Event("E5", false) && Event("E4", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        BOOST_CHECK_LT(el2, el1);
        BOOST_CHECK(!(el1 < el2));

        BOOST_TEST_MESSAGE("Two EventLists *IDENTICAL* to a certain size then different, different length");
        el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false) && Event("E6", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        BOOST_CHECK_LT(el1, el2);
        BOOST_CHECK(!(el2 < el1));
        el1 = Event("E1", true) && Event("E2", false) && Event("E5", false) && Event("E4", false);
        el2 = Event("E1", true) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        BOOST_CHECK_LT(el2, el1);
        BOOST_CHECK(!(el1 < el2));
        BOOST_CHECK(el1.notConflicting(el1));
        BOOST_CHECK(el2.notConflicting(el2));
        BOOST_CHECK(el2.notConflicting(el1));
        BOOST_CHECK(el1.notConflicting(el2));

        el1 = Event("E1", true) && Event("E2", false) && Event("E3", false) && Event("E4", false) && Event("E6", false);
        el2 = Event("E1", false) && Event("E2", false) && Event("E3", true) && Event("E4", false);
        BOOST_CHECK(el1.notConflicting(el1));
        BOOST_CHECK(el2.notConflicting(el2));
        BOOST_CHECK(!el2.notConflicting(el1));
        BOOST_CHECK(!el1.notConflicting(el2));
    }
    {
        BOOST_TEST_MESSAGE("Check Events/EventLists creation");
        EventList el1 = Event("E1", true) && Event("E2", VAR_STRING("fdsa"));
        EventList el2 = Event("E3", false) && Event("E4", VAR_STRING("dfsg"));
        map<EventList, string> elMap;
        elMap[el1] = VAR_STRING("1st");
        elMap[el2] = VAR_STRING("2nd");

        BOOST_TEST_MESSAGE("Check Events/EventLists == an < operators");
        BOOST_CHECK_EQUAL(el1, Event("E1", true) && Event("E2", VAR_STRING("fdsa")));
        BOOST_CHECK_LT(el1, el2);
        BOOST_CHECK_EQUAL(el2, Event("E3", false) && Event("E4", VAR_STRING("dfsg")));
        BOOST_CHECK_EQUAL(el1, el1);
        BOOST_CHECK_EQUAL(el2, el2);

        BOOST_CHECK(elMap.find(el1) != elMap.end());
        BOOST_CHECK(elMap.find(el2) != elMap.end());
        BOOST_CHECK_EQUAL(elMap[el1], VAR_STRING("1st"));
        BOOST_CHECK_EQUAL(elMap[el2], VAR_STRING("2nd"));
    }
    {
        ACCUMULATION_MAP accMap;
        EventList el1 = Event("E1", true) && Event("E2", VAR_STRING("fdsa"));
        EventList el2 = Event("E3", false) && Event("E4", VAR_STRING("dfsg"));
        accMap[el1] = ACCUMULATION_DATA(0, 0);
    }
    {
        Event e;
        BOOST_CHECK(e.empty());
        e("SomeName", true);
        BOOST_CHECK(!e.empty());
    }
    {
        BOOST_TEST_MESSAGE("Check CondEvents");
        Event e;
        EventList el(e); // empty event is not added so results in empty list
        BOOST_CHECK(el.empty());

        el = Event("SomeName", true); // list is initialised with one element
        BOOST_CHECK(!el.empty());
        el && Event("SomeMore", (VAR_FLOAT) 3.14159365) && Event("EvenSomeMore", VAR_STRING("XXX"));
        BOOST_CHECK(!el.empty());

        CondEvent c = el;
        BOOST_CHECK(!c.empty());
        BOOST_CHECK_EQUAL(c.eventSize(), 3);
        BOOST_CHECK_EQUAL(c.event(), el);
        BOOST_CHECK_EQUAL(c.conditionSize(), 0);

        CondEvent c2(el, el);
        BOOST_CHECK(!c2.empty());

        BOOST_CHECK_EQUAL(c2.eventSize(), 3);
        BOOST_CHECK_EQUAL(c2.event(), el);
        BOOST_CHECK_EQUAL(c2.conditionSize(), 3);
        BOOST_CHECK_EQUAL(c2.condition(), el);

        BOOST_TEST_MESSAGE("Check CondEvents creation by csv");

        CSVAnalyzer csv("FEvent, BCond, CCond, Value", "f,b,c,f");
        csv << " 1.0, yes,  a, 0.5";
        csv << " 5.0, No,   B, 2.5";

        CondEvent c3(csv, 0, 0, true);
        BOOST_CHECK_EQUAL(c3.event(), (Event("FEvent", 1.0L)));
        BOOST_CHECK_EQUAL(c3.condition(), (Event("BCond", true) && Event("CCond", 'a')));
        CondEvent c4(csv, 1, 1, true);
        BOOST_CHECK_EQUAL(c4.event(), (Event("FEvent", 5.0L) && Event("BCond", false)));
        BOOST_CHECK_EQUAL(c4.condition(), (Event("CCond", 'B')));

        BOOST_CHECK_THROW(CondEvent(Event("E1", true) && Event("E1", false)), eventlist_conflict_error);
    }
    {
        BOOST_TEST_MESSAGE("Check CondEvent - manipulation");
        CondEvent ce(Event("E1", true) && Event("E2", true));
        CondEvent::CONDEVENT_LIST l;
        BOOST_CHECK(ce.chainRule(l, "E1"));

        ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);
        vector<string> order;
        order.push_back("E2");
        order.push_back("E1");
        order.push_back("E4");
        order.push_back("E3");
        ce.chainRule(l, order);
        for (CondEvent::CONDEVENT_LIST_CITER it = l.begin(); it != l.end(); it++)
        {
            BOOST_CHECK_EQUAL(it->eventSize(), 1);
        }

        set<string> strSet;
        strSet.insert("E5");
        strSet.insert("E6");
        ce = ce.filterConditions(strSet);
        BOOST_TEST_MESSAGE("Check needed events added as place-holders");
        BOOST_CHECK(ce.hasEvent("E1"));
        BOOST_CHECK(ce.hasEvent("E2"));
        BOOST_CHECK(ce.hasEvent("E3"));
        BOOST_CHECK(ce.hasEvent("E4"));
        BOOST_CHECK(ce.hasCondition("E5"));
        BOOST_CHECK(ce.hasCondition("E6"));

        strSet.insert("E1");
        strSet.insert("E4");
        BOOST_TEST_MESSAGE("Check that filtering does not create conflicts");
        BOOST_CHECK_THROW(ce.filterConditions(strSet), eventlist_conflict_error);

        BOOST_TEST_MESSAGE("Check that removes unneeded conditions");
        ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);

        strSet.clear();
        strSet.insert("E3");
        strSet.insert("E5");
        ce = Event("E1", true) || Event("E3", true) && Event("E4", true) && Event("E5", true);
        ce = ce.filterConditions(strSet);
        BOOST_CHECK(ce.hasEvent("E1"));
        BOOST_CHECK(ce.hasCondition("E3"));
        BOOST_CHECK(!ce.hasCondition("E4"));
        BOOST_CHECK(ce.hasCondition("E5"));

        ce = Event("E1", true) && Event("E2", true) && Event("E3", true) && Event("E4", true);
        ce.chainRule(l, order);
        for (CondEvent::CONDEVENT_LIST_CITER it = l.begin(); it != l.end(); it++)
        {
            BOOST_CHECK_EQUAL(it->eventSize(), 1);
        }
    }
}
void util_event_operation_test(int i, int j)
{
    initDateFormats();
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing event explicit operations ========");
    {
        BOOST_TEST_MESSAGE("Two EventLists of size 1");
        BOOST_TEST_MESSAGE("match to interval");

        Event comparator1 = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), toDate(2015, 2, 3)));
        BOOST_CHECK(Event("E1", toDate(2014, 2, 3)).matches(comparator1));
        BOOST_CHECK(Event("E1", toDate(2015, 2, 3)).matches(comparator1));
        BOOST_CHECK(Event("E1", toDate(2014, 3, 3)).matches(comparator1));
        BOOST_CHECK(!Event("E1", toDate(2014, 2, 2)).matches(comparator1));
        BOOST_CHECK(!Event("E1", toDate(2015, 2, 4)).matches(comparator1));

        comparator1 = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), ABOVE));
        BOOST_CHECK(Event("E1", toDate(2014, 2, 3)).matches(comparator1));
        BOOST_CHECK(Event("E1", toDate(2015, 2, 3)).matches(comparator1));
        BOOST_CHECK(Event("E1", toDate(2014, 3, 3)).matches(comparator1));
        BOOST_CHECK(!Event("E1", toDate(2014, 2, 2)).matches(comparator1));
        BOOST_CHECK(Event("E1", toDate(2015, 2, 4)).matches(comparator1));

        comparator1 = Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), BELOW));
        BOOST_CHECK(Event("E1", toDate(2014, 2, 3)).matches(comparator1));
        BOOST_CHECK(!Event("E1", toDate(2015, 2, 3)).matches(comparator1));
        BOOST_CHECK(!Event("E1", toDate(2014, 3, 3)).matches(comparator1));
        BOOST_CHECK(Event("E1", toDate(2014, 2, 2)).matches(comparator1));
        BOOST_CHECK(!Event("E1", toDate(2015, 2, 4)).matches(comparator1));


        BOOST_TEST_MESSAGE("match to less-operator");

        Event comparator2 = Event("E2", VAR_INT(11), &Event::less);
        BOOST_CHECK(Event("E2", VAR_INT(4)).matches(comparator2));
        BOOST_CHECK(Event("E2", VAR_INT(10)).matches(comparator2));
        BOOST_CHECK(!Event("E2", VAR_INT(11)).matches(comparator2));
        BOOST_CHECK(!Event("E2", VAR_INT(12)).matches(comparator2));

        BOOST_TEST_MESSAGE("match to lessEqual-operator");

        Event comparator3 = Event("E3", VAR_UINT(11), &Event::lessEqual);
        BOOST_CHECK(Event("E3", VAR_UINT(4)).matches(comparator3));
        BOOST_CHECK(Event("E3", VAR_UINT(11)).matches(comparator3));
        BOOST_CHECK(Event("E3", VAR_UINT(10)).matches(comparator3));
        BOOST_CHECK(!Event("E3", VAR_UINT(113)).matches(comparator3));
        BOOST_CHECK(!Event("E3", VAR_UINT(12)).matches(comparator3));

        BOOST_TEST_MESSAGE("match to greater-operator");

        Event comparator4 = Event("E4", VAR_CHAR('t'), &Event::greater);
        BOOST_CHECK(Event("E4", VAR_CHAR('u')).matches(comparator4));
        BOOST_CHECK(Event("E4", VAR_CHAR('z')).matches(comparator4));
        BOOST_CHECK(!Event("E4", VAR_CHAR('a')).matches(comparator4));
        BOOST_CHECK(!Event("E4", VAR_CHAR('t')).matches(comparator4));

        BOOST_TEST_MESSAGE("match to greaterEqual-operator");

        Event comparator5 = Event("E5", VAR_STRING("dieter"), &Event::greaterEqual);
        BOOST_CHECK(Event("E5", VAR_STRING("dieter")).matches(comparator5));
        BOOST_CHECK(Event("E5", VAR_STRING("freedom")).matches(comparator5));
        BOOST_CHECK(!Event("E5", VAR_STRING("diet")).matches(comparator5));
        BOOST_CHECK(!Event("E5", VAR_STRING("angry")).matches(comparator5));
    }
    {
        BOOST_TEST_MESSAGE("Two EventLists of equal size >1");
        BOOST_TEST_MESSAGE("match to interval");

        EventList el1; // E1 in [2014-02-03..2015-02-03], E2 < 11 , E3 >= "dieter"
        el1 && Event("E1", Interval<VAR_DATE>(toDate(2014, 2, 3), toDate(2015, 2, 3)));
        el1 && Event("E2", VAR_INT(11), &Event::less);
        el1 && Event("E3", VAR_STRING("dieter"), &Event::greaterEqual);

        // match only if *ALL* events are matching
        EventList el2;
        el2 = Event("E1", toDate(2014, 2, 3));
        el2 && Event("E2", VAR_INT(10));
        el2 && Event("E3", VAR_STRING("dieter"));
        BOOST_CHECK(el2.matches(el1));

        el2 = Event("E1", toDate(2015, 2, 3));
        el2 && Event("E2", VAR_INT(-5));
        el2 && Event("E3", VAR_STRING("freedom"));
        BOOST_CHECK(el2.matches(el1));

        el2 = Event("E1", toDate(2013, 2, 3));
        el2 && Event("E2", VAR_INT(-5));
        el2 && Event("E3", VAR_STRING("freedom"));
        BOOST_CHECK(!el2.matches(el1));

        el2 = Event("E1", toDate(2014, 2, 3));
        el2 && Event("E2", VAR_INT(23));
        el2 && Event("E3", VAR_STRING("freedom"));
        BOOST_CHECK(!el2.matches(el1));

        el2 = Event("E1", toDate(2014, 2, 3));
        el2 && Event("E2", VAR_INT(-5));
        el2 && Event("E3", VAR_STRING("angry"));
        BOOST_CHECK(!el2.matches(el1));
    }
}
void util_stat_test(int i, int j)
{
    initDateFormats();
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing statistical functions ========");
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
        BOOST_CHECK(!d.isModified());
        BOOST_CHECK(!d.isUniform());
        BOOST_CHECK(d.isDistribution());
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
        BOOST_CHECK(!d.isModified());
        BOOST_CHECK(!d.isUniform());
        BOOST_CHECK(d.isDistribution());
        BOOST_CHECK_LT(d.P(Event("FEvent", 5.0L) || Event("CCond", 'B') && Event("BCond", false)), 1.0L);
        BOOST_CHECK_GT(d.P(Event("FEvent", 5.0L) || Event("CCond", 'B') && Event("BCond", false)), 0.0L);
        BOOST_TEST_MESSAGE(d);
    }
    {
        CSVAnalyzer csv("FEvent, BCond, CCond", "f,b,c");
        csv << " 1.0, yes,  A";
        csv << " 2.0, No,   A";
        csv << " 3.0, yes,  B";
        DiscreteProbability d;
        d.clear();
        d.train(csv);
        BOOST_TEST_MESSAGE(d);
        d.canonise();
        d.normalise();
        BOOST_TEST_MESSAGE(d);
        BOOST_CHECK(!d.isModified());
        BOOST_CHECK(!d.isUniform());
        BOOST_CHECK(d.isDistribution());
    }
    {
        VALUERANGES_TYPE eventValRanges;
        VALUERANGES_TYPE condValRanges;
        eventValRanges["1stEventUint"] = EventValueRange(VAR_UINT(0), 5);
        eventValRanges["2ndEventInt"] = EventValueRange(VAR_INT(-3), 3);
        condValRanges["boolCond"] = EventValueRange(true);
        condValRanges["charCond"] = EventValueRange('a', 'h');

        DiscreteProbability d(eventValRanges, condValRanges);
        d.canonise();
        d.normalise();

        BOOST_TEST_MESSAGE(d);
        BOOST_CHECK(!d.isModified());
        BOOST_CHECK(!d.isUniform());
        BOOST_CHECK(d.isDistribution());
    }
}
void util_continuous_stat_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing continuous statistical functions ========");
    GaussFunction norm(0.0L, 1.0L);
    // probability P([mu-sigma, mu+sigma])
    const long double p_m_var_prob = 0.682689492137L;

    VAR_FLOAT prob = norm.P(Event("E", Interval<long double>()));
    BOOST_CHECK_CLOSE(prob, 1.0L, 1e-10L);
    prob = norm.P(Event("E", Interval<long double>(0.0L, true)));
    BOOST_CHECK_CLOSE(prob, 0.5L, 1e-10L);
    prob = norm.P(Event("E", Interval<long double>(0.0L, false)));
    BOOST_CHECK_CLOSE(prob, 0.5L, 1e-10L);
    prob = norm.P(Event("E", Interval<long double>(0.0L, 1.0L)));
    VAR_FLOAT prob2 = norm.P(Event("E", Interval<long double>(-1.0L, 0.0L)));
    BOOST_CHECK_CLOSE(prob, prob2, 1e-10L);

    prob = norm.P(Event("E", Interval<long double>(norm.mu() - norm.sigma(), norm.mu() + norm.sigma())));
    BOOST_CHECK_CLOSE(prob, p_m_var_prob, 1e-10L);

    CSVAnalyzer csv;
    vector<long double> sample;
    sample += -1.0L, -0.5L, -0.1L, -1.0L, -0.2L, -0.7L, 1.0L;
    csv.appendColumn("E", sample);
    norm.train(csv, false);
    BOOST_CHECK_CLOSE(norm.P(Event("E", Interval<long double>())), 1.0L, 1e-10L);
    BOOST_CHECK_CLOSE(norm.P(Event("E", Interval<long double>(norm.mu(), true))), 0.5L, 1e-10L);
    BOOST_CHECK_CLOSE(norm.P(Event("E", Interval<long double>(norm.mu(), false))), 0.5L, 1e-10L);

    BOOST_CHECK_CLOSE(norm.P(Event("E", Interval<long double>(norm.mu() - norm.sigma(),
                                                              norm.mu() + norm.sigma()))),
                      p_m_var_prob,
                      1e-10L);

    TRACE1(norm);
    ExponentialFunction ed(1.0L);
    BOOST_CHECK_CLOSE(ed.P(Event("E", Interval<long double>())), 1.0L, 1e-10L);
    BOOST_CHECK_CLOSE(ed.P(Event("E", Interval<long double>(0.0L, true))), 0.0L, 1e-10L);
    BOOST_CHECK_CLOSE(ed.P(Event("E", Interval<long double>(0.0L, false))), 1.0L, 1e-10L);

    BOOST_CHECK_CLOSE(ed.P(Event("E", Interval<long double>(ed.ln2ByLambda(), false))), 0.5L, 1e-10L);
    BOOST_CHECK_THROW(ed.train(csv, false), event_range_error);
    csv.clear();
    sample.clear();
    sample += 1.0L, 0.5L, 0.1L, 1.0L, 0.2L, 5.7L, 7.0L;
    csv.appendColumn("E", sample);
    ed.train(csv, false);
    TRACE1(ed);
    
    UniformFloatFunction uf;
    BOOST_CHECK_CLOSE(uf.P(Event("E", Interval<long double>())), 1.0L, 1e-10L);
    BOOST_CHECK_CLOSE(uf.P(Event("E", Interval<long double>(0.0L, true))), 0.0L, 1e-10L);
    BOOST_CHECK_CLOSE(uf.P(Event("E", Interval<long double>(0.0L, false))), 1.0L, 1e-10L);

    BOOST_CHECK_CLOSE(uf.P(Event("E", Interval<long double>(0.1234L, true))), 0.1234L, 1e-10L);
    BOOST_CHECK_CLOSE(uf.P(Event("E", Interval<long double>(0.1234L, false))), 1.0L - 0.1234L, 1e-10L);
    uf.train(csv, false);
    TRACE1(uf);
    
}
void util_graph_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing graph functions ========");
    {
        DirectedGraph<PODNode<string>, string> g(false,false);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        BOOST_CHECK_EQUAL(g.addNode(A).first, g.addNode(A).first);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "first"), true);
        BOOST_CHECK_THROW(g.addEdge(B, C, "first"), parallel_error);
        BOOST_CHECK_THROW(g.addEdge(B, C, "second"), parallel_error);
        BOOST_CHECK_THROW(g.addEdge(C, B, "third"), parallel_error);
        g.removeNode(B);
        BOOST_CHECK_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        BOOST_CHECK_THROW(g.addEdge(A, A, "zeroth"), circle_error);
        BOOST_CHECK_EQUAL(g.addEdge(A, B, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "second"), true);
        BOOST_CHECK_THROW(g.addEdge(C, A, "third"), circle_error);

        BOOST_CHECK_EQUAL(g.getEdge(A, A), (string*) 0);
        BOOST_CHECK_EQUAL(*(g.getEdge(A, B)), string("first"));
        BOOST_CHECK_EQUAL(*(g.getEdge(B, C)), string("second"));
        BOOST_CHECK_EQUAL(g.getEdge(C, A), (string*) 0);
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for (DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR_CITER it = ev.begin();
             it != ev.end();
             it++)
        {
            BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");
        TRACE1(g);
        g.removeEdge(B, C);
        BOOST_CHECK_EQUAL(g.getEdge(B, C), (string*) 0);

    //    TRACE1(g);
    }
    {
        DirectedGraph<PODNode<string>, string> g(false,true);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        BOOST_CHECK_EQUAL(g.addNode(A).first, g.addNode(A).first);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "second"), true);
        BOOST_CHECK_THROW(g.addEdge(C, B, "third"), circle_error);
        g.removeNode(B);
        BOOST_CHECK_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        BOOST_CHECK_THROW(g.addEdge(A, A, "zeroth"), circle_error);
        BOOST_CHECK_EQUAL(g.addEdge(A, B, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "second"), true);
        BOOST_CHECK_THROW(g.addEdge(C, A, "third"), circle_error);

        BOOST_CHECK_EQUAL(g.getEdge(A, A), (string*) 0);
        BOOST_CHECK_EQUAL(*(g.getEdge(A, B)), string("first"));
        BOOST_CHECK_EQUAL(*(g.getEdge(B, C)), string("second"));
        BOOST_CHECK_EQUAL(g.getEdge(C, A), (string*) 0);
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for (DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR_CITER it = ev.begin();
             it != ev.end();
             it++)
        {
            BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");
        TRACE1(g);
        g.removeEdge(B, C);
        BOOST_CHECK_EQUAL(g.getEdge(B, C), (string*) 0);

    //    TRACE1(g);
    }
    {
        DirectedGraph<PODNode<string>, string> g(true,false);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        BOOST_CHECK_EQUAL(g.addNode(A).first, g.addNode(A).first);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "first"), true);
        BOOST_CHECK_THROW(g.addEdge(B, C, "first"), parallel_error);
        BOOST_CHECK_THROW(g.addEdge(B, C, "second"), parallel_error);
        BOOST_CHECK_THROW(g.addEdge(C, B, "third"), parallel_error);
        g.removeNode(B);
        BOOST_CHECK_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        BOOST_CHECK_EQUAL(g.addEdge(A, A, "zeroth"), true);
        BOOST_CHECK_EQUAL(g.addEdge(A, B, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "second"), true);
        BOOST_CHECK_EQUAL(g.addEdge(C, A, "third"), true);

        BOOST_CHECK_EQUAL(*g.getEdge(A, A), string("zeroth"));
        BOOST_CHECK_EQUAL(*(g.getEdge(A, B)), string("first"));
        BOOST_CHECK_EQUAL(*(g.getEdge(B, C)), string("second"));
        BOOST_CHECK_EQUAL(*g.getEdge(C, A), string("third"));
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for (DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR_CITER it = ev.begin();
             it != ev.end();
             it++)
        {
            BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");
        TRACE1(g);
        g.removeEdge(B, C);
        BOOST_CHECK_EQUAL(g.getEdge(B, C), (string*) 0);

    //    TRACE1(g);
    }
    {
        DirectedGraph<PODNode<string>, string> g(true,true);

        string A("A");
        string B("B");
        string C("C");
        string D("D");
        string E("E");
        BOOST_CHECK_EQUAL(g.addNode(A).first, g.addNode(A).first);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "second"), true);
        BOOST_CHECK_EQUAL(g.addEdge(C, B, "third"), true);
        TRACE1(g);
        g.removeNode(B);
        BOOST_CHECK_EQUAL(g.addEdge(C, B, "xxxx"), true);
        g.clear();
        BOOST_CHECK_EQUAL(g.addEdge(A, A, "zeroth"), true);
        BOOST_CHECK_EQUAL(g.addEdge(A, B, "first"), true);
        BOOST_CHECK_EQUAL(g.addEdge(B, C, "second"), true);
        BOOST_CHECK_EQUAL(g.addEdge(C, A, "third"), true);

        BOOST_CHECK_EQUAL(*g.getEdge(A, A), string("zeroth"));
        BOOST_CHECK_EQUAL(*g.getEdge(A, B), string("first"));
        BOOST_CHECK_EQUAL(*g.getEdge(B, C), string("second"));
        BOOST_CHECK_EQUAL(*g.getEdge(C, A), string("third"));
        DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR ev = g.getEdges();
        for (DirectedGraph<PODNode<string>, string>::EDGE_PTR_VECTOR_CITER it = ev.begin();
             it != ev.end();
             it++)
        {
            BOOST_TEST_MESSAGE(*(*it));
        }

        g.clear();
        g.addEdge(A, B, "1");
        g.addEdge(A, C, "2");
        g.addEdge(A, D, "3");
        g.addEdge(A, E, "4");
        g.addEdge(B, C, "5");
        g.addEdge(B, E, "6");
        g.addEdge(C, E, "7");
        TRACE1(g);
        g.removeEdge(B, C);
        BOOST_CHECK_EQUAL(g.getEdge(B, C), (string*) 0);

        TRACE1(g);
    }
}
struct AlgoNode : public NodeBase
{
    AlgoNode(const string& name = "")
    : name_(name)
    {
    }
    size_t hash() const
    {
        std::tr1::hash<string> hasher;
        return hasher(name_);
    }
    friend bool operator<(const AlgoNode& lhs, const AlgoNode& rhs)
    {
        return lhs.name_ < rhs.name_;
    }
    friend bool operator ==(const AlgoNode& lhs, const AlgoNode& rhs)
    {
        return lhs.name_ == rhs.name_;
    }
    friend bool operator !=(const AlgoNode& lhs, const AlgoNode& rhs)
    {
        return lhs.name_ != rhs.name_;
    }
    friend ostream& operator <<(ostream& os, const AlgoNode& n)
    {
        os << n.name_;
        return os;
    }
    string name_;
};
struct dfVis : public boost::dfs_visitor<>
{
    dfVis(vector<AlgoNode>& v)
    : visitList(v)
    {
    }
    vector<AlgoNode>& visitList;
    template <class Vertex, class Graph>
    void discover_vertex(Vertex v, Graph& g)
    {
        visitList.push_back(g[v]);
    }
};
struct bfVis : public boost::bfs_visitor<>
{
    bfVis(vector<AlgoNode>& v)
    : visitList(v)
    {
    }
    vector<AlgoNode>& visitList;
    template <class Vertex, class Graph>
    void discover_vertex(Vertex v, Graph& g)
    {
        visitList.push_back(g[v]);
    }
};
void util_graph_algo_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing graph algorithm functions ========");

    DirectedGraph<AlgoNode> g;

    //          a   h
    //         /|\ / \ 
    //        / | c   \ 
    //       /  |/ \   \
    //      b   f   d  /
    //       \ /|  /  /
    //        g | /  /
    //         \|/  /
    //          e__/
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("a"), AlgoNode("b"), "01"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("a"), AlgoNode("c"), "02"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("a"), AlgoNode("f"), "03"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("h"), AlgoNode("c"), "04"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("h"), AlgoNode("e"), "05"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("b"), AlgoNode("g"), "06"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("c"), AlgoNode("d"), "07"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("c"), AlgoNode("f"), "08"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("d"), AlgoNode("e"), "09"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("f"), AlgoNode("e"), "10"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("f"), AlgoNode("g"), "11"), true);
    BOOST_CHECK_EQUAL(g.addEdge(AlgoNode("g"), AlgoNode("e"), "12"), true);

    //    TRACE1(g);
    vector<AlgoNode> result;
    dfVis vis(result);
    g.applyDepthFirst(vis);
    //    TRACE1(result);
}
void util_bayes_test(int i, int j)
{
    BOOST_TEST_MESSAGE("");
    BOOST_TEST_MESSAGE("====== Testing Bayes functions ========");
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
        BOOST_CHECK(!bn.fullyDefined());

        const char* shouldOrder[] = {"Cloud", "Rain", "Sprinkler", "WetGrass"};
        vector<string> breadthFirstNodes = bn.breadthFirstNodeNames();
        for (size_t i = 0; i != breadthFirstNodes.size(); i++)
        {
            BOOST_CHECK_EQUAL(breadthFirstNodes[i], string(shouldOrder[i]));
        }

        BayesNet::NODE_SET connTo_1 = bn.connectedNodes(Node("Sprinkler"));
        BOOST_CHECK_EQUAL(connTo_1.size(), 2);
        BOOST_CHECK(connTo_1.find(Node("Cloud")) != connTo_1.end());
        BOOST_CHECK(connTo_1.find(Node("WetGrass")) != connTo_1.end());

        BOOST_CHECK_THROW(bn.addCauseEffect("Rain", "Cloud"), parallel_error);
        BOOST_TEST_MESSAGE(bn);
        {
            // write a sample data file
            ofstream ofs(filename.c_str());
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
        BOOST_CHECK(bn.fullyDefined());

        BOOST_TEST_MESSAGE(bn);

        BOOST_CHECK_GE(bn.P(CondEvent(Event("Cloud", true))), 0.0L);
        long double p = bn.P(CondEvent(Event("Cloud", true)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);
        p = bn.P(CondEvent(Event("Cloud", false)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);
        p = bn.P(CondEvent(Event("Rain", "heavy"), Event("Cloud", true)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);
        p = bn.P(CondEvent(Event("Rain", "none"), Event("Cloud", false)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);

        bn.clear();
        connTo_1 = bn.connectedNodes(Node("Sprinkler"));
        BOOST_CHECK_EQUAL(connTo_1.size(), 0);
        BOOST_CHECK(connTo_1.find(Node("Cloud")) == connTo_1.end());
        BOOST_CHECK(connTo_1.find(Node("WetGrass")) == connTo_1.end());
    }
    BOOST_TEST_MESSAGE("====== Creating BayesNet without reading csv ========");
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

        BOOST_TEST_MESSAGE(bn);
        BayesNet::NODE_SET connNodes = bn.connectedNodes(Node("Sprinkler"));
        BOOST_CHECK_EQUAL(connNodes.size(), 2);
        BOOST_CHECK(connNodes.find(Node("Cloud")) != connNodes.end());
        BOOST_CHECK(connNodes.find(Node("WetGrass")) != connNodes.end());
        bn.canonise();
        bn.normalise();
        BOOST_CHECK(bn.fullyDefined());
        BOOST_TEST_MESSAGE(bn);
        long double p = bn.P(CondEvent(Event("Cloud", true)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);
        p = bn.P(CondEvent(Event("Cloud", false)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);
        p = bn.P(CondEvent(Event("Rain", VAR_UINT(4)), Event("Cloud", true)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);
        p = bn.P(CondEvent(Event("Rain", VAR_UINT(2)), Event("Cloud", false)));
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);

        p = bn.P(Event("Rain", VAR_UINT(2)) &&
                 Event("Cloud", false) &&
                 Event("Sprinkler", VAR_UINT(2)) &&
                 Event("WetGrass", true)
                 );
        BOOST_CHECK_GT(p, 0.0L);
        BOOST_CHECK_LE(p, 1.0L);

        EventList irrelevant;
        CondEvent
        e = bn.bayesBallAlgorithm(CondEvent(Event("Rain", VAR_UINT(4)),
                                            Event("Cloud", true)),
                                  irrelevant);
        e = bn.bayesBallAlgorithm(CondEvent(Event("Rain", VAR_UINT(4)),
                                            Event("Sprinkler", true)),
                                  irrelevant);
    }
    BOOST_TEST_MESSAGE("====== BayesNet check BayesBallAlgorithm works ========");
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
        BOOST_CHECK(ce.containsCondition("5"));
        e = bn.bayesBallAlgorithm(ce, irrelevant);
    }
}
bool init_function()
{
    resetDateFormats();
    initDateFormats(PREFER_US_DATE_FORMAT);
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_ci_traits_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_container_conversion_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_string_test<string>, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_string_test<ci_string>, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_string_mod_test<string>, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_string_mod_test<ci_string>, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_ci_string_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_date_european_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_date_american_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_any_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_any_interval_test<VAR_CHAR>, 'a', 'z')));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_any_interval_test<VAR_INT>, -5, 10)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_any_interval_test<VAR_UINT>, 5, 10)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_any_interval_test<VAR_FLOAT>, 5.0L, 10.0L)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_any_interval_test<VAR_DATE>, toDate(2014, 1, 24), toDate(2015, 12, 3))));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_csv_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_event_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_event_operation_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_stat_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_continuous_stat_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_graph_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_graph_algo_test, 1, 1)));
    framework::master_test_suite().add(BOOST_TEST_CASE(boost::bind(&util_bayes_test, 1, 1)));

    return true;
}
int main(int argc, char* argv[])
{
    return unit_test_main(&init_function, argc, argv);
}
