/*
 * File Name:   csvutil.cc
 * Description: comma separated value utility functions
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

#include "csvutil.h"

#include "ci_string.h"
#include "to_string.h"

#include <utility>

using namespace std;
using namespace boost;
using namespace posix_time;
using namespace gregorian;

namespace util
{
const int CSVAnalyzer::xalloc_index = ios_base::xalloc();

const std::string CSV_COLUMN_TYPE_BOOL        = "bool";
const std::string CSV_COLUMN_TYPE_CHAR        = "char";
const std::string CSV_COLUMN_TYPE_INT         = "int";
const std::string CSV_COLUMN_TYPE_UINT        = "uint";
const std::string CSV_COLUMN_TYPE_FLOAT       = "float";
const std::string CSV_COLUMN_TYPE_STRING      = "string";
const std::string CSV_COLUMN_TYPE_DATE        = "date";
const std::string CSV_COLUMN_TYPE_GAUSSIAN    = "gaussian";
const std::string CSV_COLUMN_TYPE_EXPONENTIAL = "exponential";

unordered_map<ci_string, std::string> typeAliases()
{
    std::unordered_map<ci_string, string> reval;
    reval["b"] = reval["bool"] = reval["boolean"] = reval["truefalse"] = reval[typeid(VAR_BOOL).name()] = "bool";
    reval["c"] = reval["char"] = reval["character"] = reval["letter"] = reval[typeid(VAR_CHAR).name()] = "char";
    reval["i"] = reval["int"] = reval["integer"] = reval[typeid(VAR_INT).name()] = "int";
    reval["u"] = reval["uint"] = reval["unsigned"] = reval["unsigned int"] = reval["ordinal"] =
     reval[typeid(VAR_UINT).name()]                                        = "uint";
    reval["f"] = reval["float"] = reval["floating point"] = reval["real"] = reval[typeid(VAR_FLOAT).name()] = "float";
    reval["d"] = reval["date"] = reval[typeid(VAR_DATE).name()] = "date";
    reval["s"] = reval["string"] = reval["text"] = reval[typeid(VAR_STRING).name()] = "string";
    reval["e"] = reval["exp"] = reval["exponential"] = "exponential";
    reval["g"] = reval["gauss"] = reval["gaussian"] = reval["normal"] = reval["bell"] = "gaussian";

    return (reval);
}
unordered_map<ci_string, std::string> aliasMap = typeAliases();

CSVAnalyzer::CSVAnalyzer(const string &headerStr, const string &typeStr, string outSeparator)
: outSeparator_(std::move(outSeparator))
{
    if(!headerStr.empty())
        setHeaders(headerStr, false);

    if(!typeStr.empty())
        setTypes(typeStr);
}

CSVAnalyzer &CSVAnalyzer::operator<<(rowInputType rowType)
{
    inpType_ = rowType;

    return (*this);
}

bool CSVAnalyzer::splitLine(const string &str, vector<string> &result, const string &inSeparator)
{
    result = splitIntoVector(str, inSeparator);

    for(std::string &subStr: result)
        util::trim(subStr, " \t\n\r");

    return (result.size() > 0);
}

bool CSVAnalyzer::setHeaders(const string &headerString, bool replaceHeaders, const string &inSeparator)
{
    vector<string> headers;

    if(!splitLine(headerString, headers, inSeparator))
        cout << "error reading headers" << endl;

    if(replaceHeaders)
        data_.resize(0);

    if(data_.size() == 0 || data_[0].size() < 2)
    {
        data_.resize(0);

        COLUMN_TYPE newColumn(1);

        for(size_t i = 0; i < headers.size(); i++)
            data_.push_back(newColumn);
    }

    data_.resize(headers.size());

    for(size_t i = 0; i < headers.size(); i++)
    {
        data_[i][0]              = headers[i];
        headerIndex_[headers[i]] = i;
    }

    return (true);
}

bool CSVAnalyzer::resolveTypeAlias(string &type)
{
    auto found = aliasMap.find(ci_string(type.c_str()));
    bool reval = false;

    if(found != aliasMap.end())
    {
        type  = found->second;
        reval = true;
    }

    return (reval);
}

bool CSVAnalyzer::setTypes(const string &typeString, const string &inSeparator)
{
    vector<string> types;

    if(!splitLine(typeString, types, inSeparator))
        cout << "error reading headers" << endl;

    for(auto &type: types)
        resolveTypeAlias(type);

    if(!typesPresent())
        for(size_t i = 0; i < columns(); i++)
            data_[i].resize(2);

    if(types.size() > data_.size())
    {
        COLUMN_TYPE newColumn(min(data_[0].size(), 2UL));

        while(types.size() > data_.size())
        {
            newColumn[0] = scanAs<string>("Column" + toString(data_.size() + 1));
            data_.push_back(newColumn);
        }
    }

    for(size_t i = 0; i < types.size(); i++)
    {
        string originalType = toString(data_[i][1]);

        if(originalType != types[i])
        {
            data_[i][1] = types[i];

            for(size_t row = 0; row < lines(); row++)
            {
                convert(i, row, types[i]);
            }
        }
    }

    return (true);
}

string CSVAnalyzer::guessType(const string &stringVal)
{
    // treat 0 or 1 always as integers
    if(stringVal == "0" || stringVal == "1")
        return (CSV_COLUMN_TYPE_INT);

    if(stringVal.size() == 1 && stringVal[0] < '0' && stringVal[0] > '9')
        return (CSV_COLUMN_TYPE_CHAR);

    // default to string/char
    string reval = stringVal.size() == 1 ? CSV_COLUMN_TYPE_CHAR : CSV_COLUMN_TYPE_STRING;

    NumberClass numClass = classifyNumberString(stringVal);

    if(!stringVal.empty())
    {
        bool dummy;

        // start from the most complicated towards the easiest type
        reval = (numClass == util::NumberClass::NONE && valid(scanAs<VAR_DATE>(stringVal))) ? CSV_COLUMN_TYPE_DATE :
                (numClass == util::NumberClass::INT)                                        ? CSV_COLUMN_TYPE_INT :
                (numClass == util::NumberClass::UINT)                                       ? CSV_COLUMN_TYPE_UINT :
                (numClass == util::NumberClass::FLOAT)                                      ? CSV_COLUMN_TYPE_FLOAT :
                (scanBoolString(stringVal, dummy))                                          ? CSV_COLUMN_TYPE_BOOL :
                                                                                              CSV_COLUMN_TYPE_STRING;
    }

    return (reval);
}

bool CSVAnalyzer::createDefaultHeader(const vector<string> &values)
{
    string header = "";

    for(size_t i = 0; i < values.size(); i++)
        header += "Column" + toString(i) + (i == values.size() - 1 ? "" : ",");

    return (setHeaders(header));
}

bool CSVAnalyzer::createTypesFromValues(const vector<string> &values)
{
    string types = "";

    for(size_t i = 0; i < values.size(); i++)
        types += guessType(values[i]) + (i == values.size() - 1 ? "" : ",");

    return (setTypes(types));
}

bool CSVAnalyzer::setValues(const string &valueString, bool preserveRows, const string &inSeparator)
{
    vector<string> values;

    if(!splitLine(valueString, values, inSeparator))
    {
        cout << "error reading values" << endl;
    }

    // if we read in the first row without having a header or type row
    // create a default set
    if(!headerPresent())
    {
        createDefaultHeader(values);
    }

    if(!typesPresent())
    {
        createTypesFromValues(values);
    }

    if(!preserveRows)
    {
        // resize all columns to just the headers and types
        for(size_t i = 0; i < columns(); i++)
            data_[i].resize(2);
    }

    if(values.size() < data_.size())
    {
        COLUMN_TYPE newColumn(min(data_[0].size(), 2UL));

        newColumn[1] = scanAs<string>(scanAs<string>(CSV_COLUMN_TYPE_STRING));

        while(values.size() > data_.size())
        {
            newColumn[0] = scanAs<string>("Column" + toString(data_.size() + 1));
            data_.push_back(newColumn);
        }
    }

    for(size_t i = 0; i < values.size(); i++)
    {
        string tp = toString(data_[i][1]);
        if(tp == CSV_COLUMN_TYPE_BOOL)
            data_[i].push_back(scanAs<VAR_BOOL>(values[i]));
        else if(tp == CSV_COLUMN_TYPE_CHAR)
            data_[i].push_back(scanAs<VAR_CHAR>(values[i]));
        else if(tp == CSV_COLUMN_TYPE_INT)
            data_[i].push_back(scanAs<VAR_INT>(values[i]));
        else if(tp == CSV_COLUMN_TYPE_UINT)
            data_[i].push_back(scanAs<VAR_UINT>(values[i]));
        else if(tp == CSV_COLUMN_TYPE_FLOAT || tp == CSV_COLUMN_TYPE_GAUSSIAN || tp == CSV_COLUMN_TYPE_EXPONENTIAL)
            data_[i].push_back(scanAs<VAR_FLOAT>(values[i]));
        else if(tp == CSV_COLUMN_TYPE_STRING)
            data_[i].push_back(scanAs<VAR_STRING>(values[i]));
        else if(tp == CSV_COLUMN_TYPE_DATE)
            data_[i].push_back(scanAs<VAR_DATE>(values[i]));
    }

    return (true);
}

bool CSVAnalyzer::headerPresent() const
{
    return ((data_.size() > 0) && (data_[0].size() > 0));
}

bool CSVAnalyzer::typesPresent() const
{
    return ((data_.size() > 0) && (data_[0].size() > 1));
}

size_t CSVAnalyzer::columns() const
{
    return (data_.size());
}

size_t CSVAnalyzer::lines() const
{
    if(0 == columns())
        return 0;
    return (data_[0].size() > 2) ? data_[0].size() - 2 : 0;
}

string CSVAnalyzer::header(size_t col) const
{
    return (headerPresent() ? toString(data_[col][0]) : "");
}

string CSVAnalyzer::type(size_t col) const
{
    return (typesPresent() ? toString(data_[col][1]) : "");
}

void CSVAnalyzer::appendColumn(const string &header, const string &tp, const Var &defaultValue)
{
    COLUMN_TYPE col;

    col.resize(columns() > 0 ? data_[0].size() : 2, defaultValue);
    col[0] = header;

    string ltp = tp;

    resolveTypeAlias(ltp);
    col[1] = ltp;
    data_.push_back(col);
}

Var CSVAnalyzer::getVar(size_t column, size_t index) const
{
    if(column >= columns())
    {
        throw index_error(index_error::idx_type::col, column, columns() - 1);
    }

    if(index >= lines())
    {
        throw index_error(index_error::idx_type::row, index, lines() - 1);
    }

    return (data_[column][index + 2]);
}

Var CSVAnalyzer::getVar(const string &header, size_t index) const
{
    auto found = headerIndex_.find(header);

    if(found == headerIndex_.end())
    {
        throw index_error(header);
    }

    size_t column = found->second;

    return (getVar(column, index));
}

vector<VAR_FLOAT> CSVAnalyzer::getFloatVector(size_t column) const
{
    vector<VAR_FLOAT> reval;

    if(lines() == 0)
        return (reval);

    if(type(column) != CSV_COLUMN_TYPE_FLOAT)
        throw column_type_error(column, CSV_COLUMN_TYPE_FLOAT, type(column));

    for(size_t i = 2; i < data_[column].size(); i++)
        reval.push_back(toNative<VAR_FLOAT>(data_[column][i]));

    return (reval);
}

vector<VAR_FLOAT> CSVAnalyzer::getFloatVector(const std::string &header) const
{
    auto found = headerIndex_.find(header);

    if(found == headerIndex_.end())
    {
        throw index_error(header);
    }

    return (getFloatVector(found->second));
}

CSVAnalyzer::COLUMN_RANGE CSVAnalyzer::getRange(size_t column) const
{
    COLUMN_RANGE reval;

    if(column < columns())
    {
        for(size_t i = 0; i < lines(); i++)
            reval.insert(data_[column][i]);
    }

    return (reval);
}

CSVAnalyzer::COLUMN_TYPE_ITER CSVAnalyzer::begin(size_t column)
{
    if(column >= columns())
    {
        throw index_error(index_error::idx_type::col, column, columns() - 1);
    }

    return (data_[column].begin());
}

CSVAnalyzer::COLUMN_TYPE_ITER CSVAnalyzer::end(size_t column)
{
    if(column >= columns())
    {
        throw index_error(index_error::idx_type::col, column, columns() - 1);
    }

    return (data_[column].end());
}

CSVAnalyzer::COLUMN_TYPE_ITER CSVAnalyzer::begin(const string &header)
{
    auto found = headerIndex_.find(header);

    if(found == headerIndex_.end())
    {
        throw index_error(header);
    }

    return (begin(found->second));
}

CSVAnalyzer::COLUMN_TYPE_ITER CSVAnalyzer::end(const string &header)
{
    auto found = headerIndex_.find(header);

    if(found == headerIndex_.end())
    {
        throw index_error(header);
    }

    return (end(found->second));
}

CSVAnalyzer CSVAnalyzer::getSub(const vector<string> &headers) const
{
    CSVAnalyzer reval;
    size_t      index = 0;

    for(auto it = headers.begin(); it != headers.end(); it++)
    {
        auto found = headerIndex_.find(*it);

        if(found != headerIndex_.end())
        {
            reval.data_.push_back(data_[found->second]);
            reval.headerIndex_[*it] = index++;
        }
    }

    return (reval);
}

CSVAnalyzer CSVAnalyzer::getSub(std::initializer_list<string> iniList) const
{
    vector<string> v;

    for(auto header: iniList)
        v.push_back(header);

    return (getSub(v));
}

CSVAnalyzer CSVAnalyzer::getSub(const vector<size_t> &columns) const
{
    CSVAnalyzer reval;
    size_t      index = 0;
    for(auto it = columns.begin(); it != columns.end(); it++)
    {
        reval.data_.push_back(data_[*it]);
        reval.headerIndex_[header(*it)] = index++;
    }

    return (reval);
}

CSVAnalyzer CSVAnalyzer::getSub(std::initializer_list<size_t> iniList) const
{
    vector<size_t> v;

    for(auto col: iniList)
        v.push_back(col);

    return (getSub(v));
}

bool CSVAnalyzer::write(const string &filename, const string &outDelimiter, fileFormatType tp)
{
    bool     reval = false;
    ofstream ofs(filename.c_str());

    if(!ofs.is_open())
        throw fileopen_error(filename);

    string str;

    if((tp & fileFormatType::hasHeader) == fileFormatType::hasHeader)
    {
        for(size_t i = 0; i < columns(); i++)
        {
            ofs << toString(data_[i][0]) << (i == columns() - 1 ? "" : outDelimiter);
        }
        ofs << endl;
    }

    if((tp & fileFormatType::hasType) == fileFormatType::hasType)
    {
        for(size_t i = 0; i < columns(); i++)
        {
            ofs << toString(data_[i][1]) << (i == columns() - 1 ? "" : outDelimiter);
        }
        ofs << endl;
    }

    for(size_t row = 2; row < data_[0].size(); row++)
    {
        for(size_t i = 0; i < columns(); i++)
        {
            ofs << toString(data_[i][row]) << (i == columns() - 1 ? "" : outDelimiter);
        }
        ofs << endl;
    }

    return (reval);
}

bool CSVAnalyzer::read(const string &filename, const string &inDelimiter, fileFormatType tp)
{
    bool reval = false;

    data_.resize(0);

    ifstream ifs(filename.c_str());

    if(!ifs.is_open())
        throw fileopen_error(filename);

    string str;

    if((tp & fileFormatType::hasHeader) == fileFormatType::hasHeader)
    {
        getline(ifs, str);
        setHeaders(str, true, inDelimiter);
    }

    if((tp & fileFormatType::hasType) == fileFormatType::hasType)
    {
        getline(ifs, str);
        setTypes(str, inDelimiter);
    }

    while(getline(ifs, str) && !str.empty())
    {
        setValues(str, true, inDelimiter);
    }

    return (reval);
}

bool CSVAnalyzer::empty() const
{
    return (lines() == 0);
}

CSVAnalyzer &CSVAnalyzer::operator<<(const string &row)
{
    if(inpType_ == rowInputType(0))
        inpType_ = rowInputType::newData;

    if(inpType_ == rowInputType::newData)
    {
        setValues(row, false);
        inpType_ = rowInputType::appendData;
    }
    else if(inpType_ == rowInputType::appendData)
        setValues(row, true);
    else if(inpType_ == rowInputType::newHeader)
        setHeaders(row, true);
    else if(inpType_ == rowInputType::replaceHeader)
        setHeaders(row, false);
    else if(inpType_ == rowInputType::newType)
        setTypes(row);

    return (*this);
}

void CSVAnalyzer::convert(size_t col, size_t row, const string &tp)
{
    string value = toString(getVar(col, row));

    if(tp == CSV_COLUMN_TYPE_BOOL)
        data_[col][row + 2] = scanAs<VAR_BOOL>(value);
    else if(tp == CSV_COLUMN_TYPE_CHAR)
        data_[col][row + 2] = scanAs<VAR_CHAR>(value);
    else if(tp == CSV_COLUMN_TYPE_INT)
        data_[col][row + 2] = scanAs<VAR_INT>(value);
    else if(tp == CSV_COLUMN_TYPE_UINT)
        data_[col][row + 2] = scanAs<VAR_UINT>(value);
    else if(tp == CSV_COLUMN_TYPE_FLOAT || tp == CSV_COLUMN_TYPE_GAUSSIAN || tp == CSV_COLUMN_TYPE_EXPONENTIAL)
        data_[col][row + 2] = scanAs<VAR_FLOAT>(value);
    else if(tp == CSV_COLUMN_TYPE_STRING)
        data_[col][row + 2] = scanAs<VAR_STRING>(value);
    else if(tp == CSV_COLUMN_TYPE_DATE)
        data_[col][row + 2] = scanAs<VAR_DATE>(value);
}

bool CSVAnalyzer::eraseColumn(size_t col)
{
    if(col >= columns())
    {
        return (false);
    }

    data_.erase(data_.begin() + col);

    return (true);
}

bool CSVAnalyzer::eraseColumn(const string &header)
{
    auto found = headerIndex_.find(header);

    if(found == headerIndex_.end())
    {
        return (false);
    }

    return (eraseColumn(found->second));
}

ostream &operator<<(ostream &os, CSVAnalyzer::verbosityType vt)
{
    os.iword(CSVAnalyzer::xalloc_index) = vt;

    return (os);
}

ostream &operator<<(ostream &os, const CSVAnalyzer &csv)
{
    auto vt = static_cast<CSVAnalyzer::verbosityType>(os.iword(CSVAnalyzer::xalloc_index));

    if(vt == CSVAnalyzer::displayNone)
        vt = CSVAnalyzer::displayAll;

    if(vt & CSVAnalyzer::displayHeader)
    {
        for(size_t i = 0; i < csv.columns(); i++)
        {
            os << toString(csv.data_[i][0]) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
        }

        os << endl;
    }

    vector<string> types;

    if(vt & CSVAnalyzer::displayType)
    {
        for(size_t i = 0; i < csv.columns(); i++)
        {
            types.push_back(toString(csv.data_[i][1]));
            os << types[i] << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
        }

        os << endl;
    }

    if(vt & CSVAnalyzer::displayData)
    {
        for(size_t j = 0; j < csv.lines(); j++)
        {
            for(size_t i = 0; i < csv.columns(); i++)
            {
                if(types[i] == CSV_COLUMN_TYPE_BOOL)
                    os << toNative<VAR_BOOL>(csv.getVar(i, j)) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
                else if(types[i] == CSV_COLUMN_TYPE_CHAR)
                    os << toNative<VAR_CHAR>(csv.getVar(i, j)) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
                else if(types[i] == CSV_COLUMN_TYPE_INT)
                    os << toNative<VAR_INT>(csv.getVar(i, j)) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
                else if(types[i] == CSV_COLUMN_TYPE_UINT)
                    os << toNative<VAR_UINT>(csv.getVar(i, j)) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
                else if(types[i] == CSV_COLUMN_TYPE_FLOAT || types[i] == CSV_COLUMN_TYPE_GAUSSIAN
                        || types[i] == CSV_COLUMN_TYPE_EXPONENTIAL)
                    os << toNative<VAR_FLOAT>(csv.getVar(i, j)) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
                else if(types[i] == CSV_COLUMN_TYPE_DATE)
                    os << toNative<VAR_DATE>(csv.getVar(i, j)) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
                else if(types[i] == CSV_COLUMN_TYPE_STRING)
                    os << toNative<VAR_STRING>(csv.getVar(i, j)) << (i < csv.columns() - 1 ? csv.outSeparator_ : "");
            }

            os << endl;
        }
    }

    return (os);
}
};  // namespace util
