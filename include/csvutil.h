/*
 * File Name:   csvutil.h
 * Description: comma separated value utility functions
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
 * @date: 2014-01-28
 * @author: Dieter J Kybelksties
 */

#ifndef NS_UTIL_CSVUTIL_H_INCLUDED
#define NS_UTIL_CSVUTIL_H_INCLUDED

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <deque>
#include <ctime>                    // for struct tm
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/date_time.hpp>      // gregorian dates/posix time/...
#include <unordered_map>
#include <stringutil.h>
#include <anyutil.h>

namespace util
{

    /**
     * Error handling for CSV index errors.
     */
    struct index_error : public std::logic_error
    {

        enum class idx_type
        {
            col, row
        };

        index_error(const std::string& header)
        : std::logic_error("No column named '" + header + "' in csv")
        {
        }

        index_error(idx_type tp, size_t idx, size_t high)
        : std::logic_error(std::string(tp == idx_type::col ? "Column" : "Row") + " index " +
                           asString(idx) + " out of range [0.." +
                           asString(high) + "] in csv")
        {
        }
    };

    /**
     * Error handling for column-type mismatches in CSVs.
     */
    struct column_type_error : public std::logic_error
    {

        column_type_error(size_t col, const std::string& expected, const std::string& got)
        : std::logic_error("Wrong type in column "
                           + asString(col)
                           + ": expected '"
                           + expected
                           + "' but got '"
                           + got
                           + "'")
        {
        }

    };

    /**
     * Error handling for errors when opening a CSV-file.
     */
    struct fileopen_error : public std::logic_error
    {

        fileopen_error(const std::string& filename)
        : std::logic_error("cannot open file '" + filename + "'")
        {
        }
    };

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template <typename Iterator>
    bool parse_strings(Iterator first,
                       Iterator last,
                       std::vector<std::string>& v,
                       char separator = ',',
                       bool trimSpaces = true)
    {
        using qi::char_;
        using qi::phrase_parse;
        using qi::parse;
        using ascii::space;

        bool r = parse(first, last,

                       //  Begin grammar
                       (
                        *(char_ - separator) % separator
                        )
                       ,
                       //  End grammar

                       v);

        if (trimSpaces)
            for (size_t i = 0; i < v.size(); i++)
            {
                trim(v[i], "\n\t \r");
            }
        if (first != last) // fail if we did not get a full match
            return false;

        return r;
    }

    const static std::string CSV_COLUMN_TYPE_BOOL = "bool";
    const static std::string CSV_COLUMN_TYPE_CHAR = "char";
    const static std::string CSV_COLUMN_TYPE_INT = "int";
    const static std::string CSV_COLUMN_TYPE_UINT = "uint";
    const static std::string CSV_COLUMN_TYPE_FLOAT = "float";
    const static std::string CSV_COLUMN_TYPE_STRING = "string";
    const static std::string CSV_COLUMN_TYPE_DATE = "date";
    const static std::string CSV_COLUMN_TYPE_GAUSSIAN = "gaussian";
    const static std::string CSV_COLUMN_TYPE_EXPONENTIAL = "exponential";

    /**
     * Read/write comma-separated files and hold an in-homogenous table
     * with typed columns.
     * Types can be
     * <table>
     * <tr> <td>Type</td> <td> Synonyms </td> </tr>
     * <tr> <td>bool</td> <td> b, bool, boolean, truefalse </td> </tr>
     * <tr> <td>char</td> <td> c, char, character, letter </td> </tr>
     * <tr> <td>int</td> <td> i, int, integer </td> </tr>
     * <tr> <td>uint</td> <td> u, uint, unsigned, unsigned integer, ordinal </td> </tr>
     * <tr> <td>float</td> <td> f, float, floating point, real </td> </tr>
     * <tr> <td>string</td> <td> s, string, text </td> </tr>
     * <tr> <td>date</td> <td> d, date </td> </tr>
     * </table>
     * Additionally the following (aggregate) types are possible. They should be
     * used if it is the intention to aggregate the data into an exponential/gaussian
     * distribution respectively rather than recording the individual values.
     * <table>
     * <tr> <td>Type</td> <td> Synonyms </td> </tr>
     * <tr> <td>exponential</td> <td> e, exp, exponential </td> </tr>
     * <tr> <td>gaussian</td> <td> g, gauss, gaussian, bell, normal </td> </tr>
     * </table>
     */
    class CSVAnalyzer
    {
    public:
        typedef std::deque<Var> COLUMN_TYPE;
        typedef COLUMN_TYPE::iterator COLUMN_TYPE_ITER;
        typedef std::deque<COLUMN_TYPE> CSV_TYPE;
        typedef std::map<std::string, size_t> HEADER_INDEX;
        typedef std::set<Var> COLUMN_RANGE;

        /**
         * Output configuration for CSVs
         */
        enum verbosityType : char
        {
            displayNone = 0x00, ///< don't display anything
            displayType = 0x01, ///< display the type row
            displayHeader = 0x02, ///< desplay the header
            displayData = 0x04, ///< display the data rows
            displayAll = displayType | displayHeader | displayData ///< display the lot
        };

        /**
         * Configuration for in-stream operator
         */
        enum rowInputType : char
        {
            initial = 0x00, ///< initial state
            newHeader = 0x01, ///< next read line will be a new header
            replaceHeader = 0x02, ///< next read line will replace the current header
            newType = 0x03, ///< next read line will be a new type row
            newData = 0x04, ///< next read line will be a new data row
            appendData = 0x05 ///< next read line will append a new data row
        };

        /**
         * Read-file configuration
         */
        enum fileFormatType : char
        {
            hasHeader = 0x01, ///< header present in file
            hasType = 0x02, ///< type-row present in file
            hasValues = 0x04, ///< values present in file
            allowsNull = 0x08, ///< null-values are allowed
            defaultFileFormat = hasHeader | hasType | hasValues | allowsNull ///< default combination
        };

        /**
         * Default construct the comma separated header-string, type-string
         * and out-separator.
         */
        explicit CSVAnalyzer(const std::string& headerStr = "",
                             const std::string& typeStr = "",
                             const std::string& outSeparator = ", ");

        /**
         * Reset the CSV.
         */
        bool clear()
        {
            data_.clear();
            headerIndex_.clear();
            outSeparator_ = ", ";
            return true;
        }

        /**
         * Check whether there are any data-rows other than header and type-row.
         */
        bool empty() const;

        /**
         * Read csv from file-system.
         */
        bool read(const std::string& filename = "",
                  const std::string& inDelimiter = ",",
                  fileFormatType = fileFormatType::defaultFileFormat);

        /**
         * Write to file-system.
         */
        bool write(const std::string& filename = "",
                   const std::string& outDelimiter = ",",
                   fileFormatType tp = fileFormatType::defaultFileFormat);

        /**
         * Split a delimited line-string into string tokens.
         */
        static bool splitLine(const std::string& str,
                              std::vector<std::string>& result,
                              const std::string& inSeparator = ",");

        /**
         * Use a delimited string to populate the headers of the csv.
         */
        bool setHeaders(const std::string& headerString,
                        bool replaceHeaders = true,
                        const std::string& inSeparator = ",");

        /**
         * Use a delimited string to configure the column-types of the csv.
         */
        bool setTypes(const std::string& typeString,
                      const std::string& inSeparator = ",");

        /**
         * Use a delimited string set a row of values in the csv.
         */
        bool setValues(const std::string& valueString,
                       bool preserveRows = true,
                       const std::string& inSeparator = ",");

        /**
         * Resolves an allowed type-alias in the correct type.
         */
        static bool resolveTypeAlias(std::string& type);

        /**
         * Guess the type of a value from the string representation.
         */
        static std::string guessType(const std::string& stringVal);

        /**
         * Set header-strings to unique default-headers per column.
         */
        bool createDefaultHeader(const std::vector<std::string>& values);

        /**
         * Use a vector of value strings to guess the types of corresponding columns.
         */
        bool createTypesFromValues(const std::vector<std::string>& values);

        /**
         * Retrieve the configured separator string used for output.
         */
        void setOutSeparator(const std::string& outSeparator)
        {
            outSeparator_ = outSeparator;
        }


        /**
         * Configure how the csv will be in-streamed.
         */
        CSVAnalyzer& operator<<(rowInputType rowType);

        /**
         * In-stream a string as -header/type/values.
         */
        CSVAnalyzer& operator<<(const std::string& row);

        /**
         * Check whether there is a header present in the csv.
         */
        bool headerPresent() const;

        /**
         * Check whether there is a column-types are all defined.
         */
        bool typesPresent() const;

        /**
         * Retrieve the number of columns of the csv.
         */
        size_t columns() const;

        /**
         * Retrieve the number of *data* lines.
         */
        size_t lines() const;

        /**
         * Retrieve the header string at column col.
         */
        std::string header(size_t col) const;

        /**
         * Retrieve the type of column col.
         */
        std::string type(size_t col) const;

        /**
         * Append a column with header, type and values to the csv.
         */
        void appendColumn(const std::string& header,
                          const std::string& tp,
                          const Var& defaultValue = Var());

        template<typename T_>
        void appendColumn(const std::string& header,
                          const std::vector<T_>& values)
        {
            COLUMN_TYPE col;
            col.resize(columns() > 0 ? data_[0].size() : values.size() + 2, Var());
            col[0] = header;
            std::string ltp = typeid (T_).name();
            resolveTypeAlias(ltp);
            col[1] = ltp;
            for (size_t i = 0; i < values.size(); i++)
                col[i + 2] = values[i];
            data_.push_back(col);
        }

        /**
         * Get the value at position [column, line] as template type if possible.
         */
        template <typename T_>
        T_ get(size_t column, size_t line) const
        {
            if (column >= columns())
            {
                throw index_error(index_error::idx_type::col, column, columns() - 1);
            }
            if (line >= lines())
            {
                throw index_error(index_error::idx_type::row, line, lines() - 1);
            }
            return (data_[column][line + 2]).get<T_>();
        }

        /**
         * Get the value at position [header, line] as template type if possible.
         */
        template <typename T_>
        T_ get(const std::string& header, size_t line) const
        {
            auto found = headerIndex_.find(header);
            if (found == headerIndex_.end())
            {
                throw index_error("No column named '" + header + "' in csv");
            }
            size_t column = headerIndex_[header];
            return get<T_>(column, line);
        }

        /**
         * Get the value at position [column, line] as boolean if possible.
         */
        inline VAR_BOOL getBool(size_t column, size_t line) const
        {
            return get<VAR_BOOL>(column, line);
        }

        /**
         * Get the value at position [header, line] as boolean if possible.
         */
        inline VAR_BOOL getBool(const std::string& header, size_t line) const
        {
            return get<VAR_BOOL>(header, line);
        }

        /**
         * Get the value at position [column, line] as character if possible.
         */
        inline VAR_CHAR getChar(size_t column, size_t line) const
        {
            return get<VAR_CHAR>(column, line);
        }

        /**
         * Get the value at position [header, line] as character if possible.
         */
        inline VAR_CHAR getChar(const std::string& header, size_t line) const
        {
            return get<VAR_CHAR>(header, line);
        }

        /**
         * Get the value at position [column, line] as integer if possible.
         */
        inline VAR_INT getInt(size_t column, size_t line) const
        {
            return get<VAR_INT>(column, line);
        }

        /**
         * Get the value at position [header, line] as integer if possible.
         */
        inline VAR_INT getInt(const std::string& header, size_t line) const
        {
            return get<VAR_INT>(header, line);
        }

        /**
         * Get the value at position [column, line] as unsigned integer if possible.
         */
        inline VAR_UINT getUint(size_t column, size_t line) const
        {
            return get<VAR_UINT>(column, line);
        }

        /**
         * Get the value at position [header, line] as unsigned integer if possible.
         */
        inline VAR_UINT getUint(const std::string& header, size_t line) const
        {
            return get<VAR_UINT>(header, line);
        }

        /**
         * Get the value at position [column, line] as real if possible.
         */
        inline VAR_FLOAT getFloat(size_t column, size_t line) const
        {
            return get<VAR_FLOAT>(column, line);
        }

        /**
         * Get the value at position [header, line] as real if possible.
         */
        inline VAR_FLOAT getFloat(const std::string& header, size_t line) const
        {
            return get<VAR_FLOAT>(header, line);
        }

        /**
         * Get the value at position [column, line] as date if possible.
         */
        inline VAR_DATE getDate(size_t column, size_t line) const
        {
            return get<VAR_DATE>(column, line);
        }

        /**
         * Get the value at position [header, line] as date if possible.
         */
        inline VAR_DATE getDate(const std::string& header, size_t line) const
        {
            return get<VAR_DATE>(header, line);
        }

        /**
         * Get the value at position [column, line] as string if possible.
         */
        inline VAR_STRING getString(size_t column, size_t line) const
        {
            return get<VAR_STRING>(column, line);
        }

        /**
         * Get the value at position [header, line] as string if possible.
         */
        inline VAR_STRING getString(const std::string& header, size_t line) const
        {
            return get<VAR_STRING>(header, line);
        }

        /**
         * Get the value at position [column, line] as variant if possible.
         */
        Var getVar(size_t column, size_t line) const;

        /**
         * Get the value at position [header, line] as variant if possible.
         */
        Var getVar(const std::string& header, size_t line) const;

        /**
         * Get all data-values of column as float-values in a vector if possible.
         */
        std::vector<VAR_FLOAT> getFloatVector(size_t column) const;

        /**
         * Get all data-values of column as float-values in a vector if possible.
         */
        std::vector<VAR_FLOAT> getFloatVector(const std::string& header) const;

        /**
         * Retrieve the range of values in column.
         */
        COLUMN_RANGE getRange(size_t column) const;

        /**
         * Begin-iterator for column.
         */
        COLUMN_TYPE_ITER begin(size_t column);

        /**
         * End-iterator for column.
         */
        COLUMN_TYPE_ITER end(size_t column);

        /**
         * Begin-iterator for header.
         */
        COLUMN_TYPE_ITER begin(const std::string& header);

        /**
         * End-iterator for header.
         */
        COLUMN_TYPE_ITER end(const std::string& header);

        /**
         * Convert a value to a different type if possible.
         */
        void convert(size_t i, size_t row, const std::string& tp);

        /**
         * Extract a sub-csv given a list of header-names.
         */
        CSVAnalyzer getSub(const std::vector<std::string>& headers) const;

        /**
         * Extract a sub-csv convenience function.
         */
        CSVAnalyzer getSub(const std::string& header0,
                           const std::string& header1 = "",
                           const std::string& header2 = "",
                           const std::string& header3 = "",
                           const std::string& header4 = "",
                           const std::string& header5 = "",
                           const std::string& header6 = "",
                           const std::string& header7 = "",
                           const std::string& header8 = "",
                           const std::string& header9 = ""
                           ) const;

        /**
         * Extract a sub-csv given a list of column indices.
         */
        CSVAnalyzer getSub(const std::vector<size_t>& columns) const;

        /**
         * Extract a sub-csv convenience function.
         */
        CSVAnalyzer getSub(const size_t column0,
                           const size_t column1 = (size_t) - 1,
                           const size_t column2 = (size_t) - 1,
                           const size_t column3 = (size_t) - 1,
                           const size_t column4 = (size_t) - 1,
                           const size_t column5 = (size_t) - 1,
                           const size_t column6 = (size_t) - 1,
                           const size_t column7 = (size_t) - 1,
                           const size_t column8 = (size_t) - 1,
                           const size_t column9 = (size_t) - 1
                           ) const;


        /**
         * Remove column col from the csv.
         */
        bool eraseColumn(size_t col);

        /**
         *  Remove column header from the csv.
         */
        bool eraseColumn(const std::string& header);

        /**
         * configure how the csv is displayed.
         */
        friend std::ostream& operator<<(std::ostream& os, CSVAnalyzer::verbosityType vt);

        /**
         * Generic ostream - \<\< operator for CSVAnalyzer.
         */
        friend std::ostream& operator<<(std::ostream& os, const CSVAnalyzer& err);

    private:
        CSV_TYPE data_; ///< Rectangular variant data container.
        mutable HEADER_INDEX headerIndex_; ///< map header names to column-indices.
        static const int xalloc_index; ///< unique index for outstream configuration.
        rowInputType inpType_; ///< what will the next row-input-type be.
        std::string outSeparator_; ///< output separator string.
    };

    /**
     * Configure the output of a CSV.
     */
    std::ostream& operator<<(std::ostream& os, CSVAnalyzer::verbosityType vt);

    /**
     * Generic ostream - \<\< operator for CSVAnalyzer.
     */
    std::ostream& operator<<(std::ostream& os, const CSVAnalyzer& csv);

    /**
     * template specialisation of string conversion for CSVs.
     */
    template <>
    inline std::string asString(const CSVAnalyzer& csv)
    {
        std::stringstream ss;
        ss << CSVAnalyzer::verbosityType::displayAll << csv;
        return ss.str();
    }

}; // namespace util

#endif // NS_UTIL_CSVUTIL_H_INCLUDED

