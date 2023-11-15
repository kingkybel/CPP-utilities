/*
 * File Name:   csvutil.h
 * Description: comma separated value utility functions
 *
 * Copyright (C) 2023 Dieter J Kybelksties
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

#ifndef NS_UTIL_CSVUTIL_H_INCLUDED
#define NS_UTIL_CSVUTIL_H_INCLUDED

#include "anyutil.h"
#include "to_string.h"

#include <boost/config/warning_disable.hpp>
#include <boost/date_time.hpp>
#include <ctime>
#include <deque>
#include <exception>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <map>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace util
{
/**
 * @brief Error handling for CSV index errors.
 */
struct index_error : public std::logic_error
{
    enum class idx_type
    {
        col,
        row
    };

    index_error(const std::string &header) : std::logic_error("No column named '" + header + "' in csv")
    {
    }

    index_error(idx_type tp, size_t idx, size_t high)
    : std::logic_error(std::string(tp == idx_type::col ? "Column" : "Row") + " index " + toString(idx)
                       + " out of range [0.." + toString(high) + "] in csv")
    {
    }
};

/**
 * @brief Error handling for column-type mismatches in CSVs.
 */
struct column_type_error : public std::logic_error
{
    column_type_error(size_t col, const std::string &expected, const std::string &got)
    : std::logic_error("Wrong type in column " + toString(col) + ": expected '" + expected + "' but got '" + got + "'")
    {
    }
};

/**
 * @brief Error handling for errors when opening a CSV-file.
 */
struct fileopen_error : public std::logic_error
{
    fileopen_error(const std::string &filename) : std::logic_error("cannot open file '" + filename + "'")
    {
    }
};

extern const std::string CSV_COLUMN_TYPE_BOOL;
extern const std::string CSV_COLUMN_TYPE_CHAR;
extern const std::string CSV_COLUMN_TYPE_INT;
extern const std::string CSV_COLUMN_TYPE_UINT;
extern const std::string CSV_COLUMN_TYPE_FLOAT;
extern const std::string CSV_COLUMN_TYPE_STRING;
extern const std::string CSV_COLUMN_TYPE_DATE;
extern const std::string CSV_COLUMN_TYPE_GAUSSIAN;
extern const std::string CSV_COLUMN_TYPE_EXPONENTIAL;

/**
 * @brief Read/write comma-separated files and hold an in-homogenous table
 * with typed columns.
 * Types can be
 * <table>
 * <tr> <td>Type</td> <td> Synonyms </td> </tr>
 * <tr> <td>bool</td> <td> b, bool, boolean, truefalse </td> </tr>
 * <tr> <td>char</td> <td> c, char, character, letter </td> </tr>
 * <tr> <td>int</td> <td> i, int, integer </td> </tr>
 * <tr> <td>uint</td> <td> u, uint, unsigned, unsigned integer, ordinal </td>
 * </tr> <tr> <td>float</td> <td> f, float, floating point, real </td> </tr>
 * <tr> <td>string</td> <td> s, string, text </td> </tr>
 * <tr> <td>date</td> <td> d, date </td> </tr>
 * </table>
 * Additionally the following (aggregate) types are possible. They should be
 * used if it is the intention to aggregate the data into an
 * exponential/gaussian distribution respectively rather than recording the
 * individual values. <table> <tr> <td>Type</td> <td> Synonyms </td> </tr> <tr>
 * <td>exponential</td> <td> e, exp, exponential </td> </tr> <tr>
 * <td>gaussian</td> <td> g, gauss, gaussian, bell, normal </td> </tr>
 * </table>
 */
class CSVAnalyzer
{
    public:
    using COLUMN_TYPE      = std::deque<Var>;
    using COLUMN_TYPE_ITER = COLUMN_TYPE::iterator;
    using CSV_TYPE         = std::deque<COLUMN_TYPE>;
    using HEADER_INDEX     = std::map<std::string, size_t>;
    using COLUMN_RANGE     = std::set<Var>;

    /**
     * @brief Output configuration for CSVs
     */
    enum verbosityType : char
    {
        displayNone   = 0x00,                                      ///< don't display anything
        displayType   = 0x01,                                      ///< display the type row
        displayHeader = 0x02,                                      ///< display the header
        displayData   = 0x04,                                      ///< display the data rows
        displayAll    = displayType | displayHeader | displayData  ///< display the lot
    };

    /**
     * @brief Configuration for in-stream operator
     */
    enum rowInputType : char
    {
        initial       = 0x00,  ///< initial state
        newHeader     = 0x01,  ///< next read line will be a new header
        replaceHeader = 0x02,  ///< next read line will replace the current header
        newType       = 0x03,  ///< next read line will be a new type row
        newData       = 0x04,  ///< next read line will be a new data row
        appendData    = 0x05   ///< next read line will append a new data row
    };

    /**
     * @brief Read-file configuration
     */
    enum fileFormatType : char
    {
        hasHeader         = 0x01,                                         ///< header present in file
        hasType           = 0x02,                                         ///< type-row present in file
        hasValues         = 0x04,                                         ///< values present in file
        allowsNull        = 0x08,                                         ///< null-values are allowed
        defaultFileFormat = hasHeader | hasType | hasValues | allowsNull  ///< default combination
    };

    /**
     * @brief Default construct the comma separated header-string, type-string
     * and out-separator.
     *
     * @param headerStr string with comma separated headers
     * @param typeStr string with comma-separated types
     * @param outSeparator separator for output
     */
    explicit CSVAnalyzer(const std::string &headerStr    = "",
                         const std::string &typeStr      = "",
                         std::string        outSeparator = ", ");

    /**
     * @brief Reset the CSV.
     *
     * @return true always
     */
    bool clear()
    {
        data_.clear();
        headerIndex_.clear();
        outSeparator_ = ", ";

        return (true);
    }

    /**
     * @brief Check whether there are any data-rows other than header and type-row.
     *
     * @return true, if csv is empty, false otherwise
     */
    bool empty() const;

    /**
     * @brief Read csv from file-system.
     *
     * @param filename file from which to read the csv
     * @param inDelimiter delimiter of fields
     * @return true, if successful, false otherwise
     */
    bool read(const std::string &filename    = "",
              const std::string &inDelimiter = ",",
              fileFormatType                 = fileFormatType::defaultFileFormat);

    /**
     * @brief Write to file-system.
     *
     * @param filename file to which to write the csv
     * @param outDelimiter delimiter of fields
     * @param fileFormat format of the file
     * @return true, if successful, false otherwise
     */
    bool write(const std::string &filename     = "",
               const std::string &outDelimiter = ",",
               fileFormatType     fileFormat   = fileFormatType::defaultFileFormat);

    /**
     * @brief Split a delimited line-string into string tokens.
     *
     * @param str original string
     * @param result string-vector
     * @param inSeparator delimiter of fields
     * @return true, if successful, false otherwise
     */
    static bool
     splitLine(const std::string &str, std::vector<std::string> &result, const std::string &inSeparator = ",");

    /**
     * @brief Use a delimited string to populate the headers of the csv.
     *
     * @param headerString comma-separated headers
     * @param replaceHeaders if set to false, then append the headers, otherwise reset the headers with the given ones
     * @param inSeparator delimiter of fields
     * @return true, if successful, false otherwise
     */
    bool setHeaders(const std::string &headerString, bool replaceHeaders = true, const std::string &inSeparator = ",");

    /**
     * @brief Use a delimited string to configure the column-types of the csv.
     *
     * @param typeString comma-separated type
     * @param inSeparator delimiter of fields
     * @return true, if successful, false otherwise
     */
    bool setTypes(const std::string &typeString, const std::string &inSeparator = ",");

    /**
     * @brief Use a delimited string set a row of values in the csv.
     *
     * @param valueString comma-separated line of values
     * @param preserveRows if set to true, then append a new line, otherwise reset the rows with the given ones
     * @param inSeparator delimiter of fields
     * @return true, if successful, false otherwise
     */
    bool setValues(const std::string &valueString, bool preserveRows = true, const std::string &inSeparator = ",");

    /**
     * @brief Resolves an allowed type-alias in the correct type.
     *
     * @param type string to interpret
     * @return true, if successful, false otherwise
     */
    static bool resolveTypeAlias(std::string &type);

    /**
     * @brief Guess the type of a value from the string representation.
     *
     * @param stringVal the string to interpret
     * @return std::string a type string
     */
    static std::string guessType(const std::string &stringVal);

    /**
     * @brief Create a Default Headers for the csv
     *
     * @param values a vector of header-strings
     * @return true, if successful, false otherwise
     */
    bool createDefaultHeader(const std::vector<std::string> &values);

    /**
     * @brief Use a vector of value strings to guess the types of corresponding
     * columns.
     *
     * @param values a vector of value-strings
     * @return true, if successful, false otherwise
     */
    bool createTypesFromValues(const std::vector<std::string> &values);

    /**
     * @brief Set the configured separator string used for output.
     *
     * @param outSeparator the ne separator
     */
    void setOutSeparator(const std::string &outSeparator)
    {
        outSeparator_ = outSeparator;
    }

    /**
     * @brief Configure how the csv will be in-streamed.
     *
     * @param rowType header/type/value
     * @return CSVAnalyzer& self
     */
    CSVAnalyzer &operator<<(rowInputType rowType);

    /**
     * @brief In-stream a string as -header/type/values.
     *
     * @param row a row to add
     * @return CSVAnalyzer& self
     */
    CSVAnalyzer &operator<<(const std::string &row);

    /**
     * @brief Check whether there is a header present in the csv.
     *
     * @return true, if so, false otherwise
     */
    bool headerPresent() const;

    /**
     * @brief Check whether there is a column-types are all defined.
     *
     * @return true, if so, false otherwise
     */
    bool typesPresent() const;

    /**
     * @brief Retrieve the number of columns of the csv.
     *
     * @return size_t number of columns
     */
    size_t columns() const;

    /**
     * @brief Retrieve the number of *data* lines.
     *
     * @return size_t number of lines (excluding header and type lines)
     */
    size_t lines() const;

    /**
     * @brief Retrieve the header string at column col.
     *
     * @param col column index
     * @return std::string the header
     */
    std::string header(size_t col) const;

    /**
     * @brief Retrieve the type of column col.
     *
     * @param col column index
     * @return std::string the type
     */
    std::string type(size_t col) const;

    /**
     * @brief Append a column with header, type and default-values to the csv.
     *
     * @param header header string
     * @param tp type-string
     * @param defaultValue default value
     */
    void appendColumn(const std::string &header, const std::string &tp, const Var &defaultValue = Var());

    /**
     * @brief Append a column with header, type and values to the csv.
     *
     * @tparam T_ type of the values
     * @param header header string
     * @param values vector of values
     */
    template<typename T_>
    void appendColumn(const std::string &header, const std::vector<T_> &values)
    {
        COLUMN_TYPE col;

        col.resize(columns() > 0 ? data_[0].size() : values.size() + 2, Var());
        col[0] = header;

        std::string ltp = typeid(T_).name();

        resolveTypeAlias(ltp);
        col[1] = ltp;

        for(size_t i = 0; i < values.size(); i++)
            col[i + 2] = values[i];

        data_.push_back(col);
    }

    /**
     * @brief Get the value at position [column, line] as template type if possible.
     *
     * @tparam T_ type of the value
     * @param column column of the
     * @param line line of the value
     * @return T_ the value
     */
    template<typename T_>
    T_ get(size_t column, size_t line) const
    {
        if(column >= columns())
        {
            throw index_error(index_error::idx_type::col, column, columns() - 1);
        }

        if(line >= lines())
        {
            throw index_error(index_error::idx_type::row, line, lines() - 1);
        }

        return ((data_[column][line + 2]).get<T_>());
    }

    /**
     * @brief Get the value at position [header, line] as template type if possible.
     *
     * @tparam T_ type of the value
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return T_ the value
     */
    template<typename T_>
    T_ get(const std::string &header, size_t line) const
    {
        auto found = headerIndex_.find(header);

        if(found == headerIndex_.end())
        {
            throw index_error("No column named '" + header + "' in csv");
        }

        size_t column = headerIndex_[header];

        return (get<T_>(column, line));
    }

    /**
     * @brief Get the value at position [column, line] as boolean if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return VAR_BOOL the value as bool
     */
    inline VAR_BOOL getBool(size_t column, size_t line) const
    {
        return (get<VAR_BOOL>(column, line));
    }

    /**
     * @brief Get the value at position [column, line] as boolean if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return VAR_BOOL the value as bool
     */
    inline VAR_BOOL getBool(const std::string &header, size_t line) const
    {
        return (get<VAR_BOOL>(header, line));
    }

    /**
     * @brief Get the value at position [column, line] as character if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return VAR_CHAR the value as char
     */
    inline VAR_CHAR getChar(size_t column, size_t line) const
    {
        return (get<VAR_CHAR>(column, line));
    }

    /**
     * @brief Get the value at position [header, line] as character if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return VAR_CHAR the value as char
     */
    inline VAR_CHAR getChar(const std::string &header, size_t line) const
    {
        return (get<VAR_CHAR>(header, line));
    }

    /**
     * @brief Get the value at position [column, line] as integer if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return VAR_INT the value as int
     */
    inline VAR_INT getInt(size_t column, size_t line) const
    {
        return (get<VAR_INT>(column, line));
    }

    /**
     * @brief Get the value at position [header, line] as integer if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return VAR_INT the value as int
     */
    inline VAR_INT getInt(const std::string &header, size_t line) const
    {
        return (get<VAR_INT>(header, line));
    }

    /**
     * @brief Get the value at position [column, line] as unsigned integer if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return VAR_UINT the value as unsined int
     */
    inline VAR_UINT getUint(size_t column, size_t line) const
    {
        return (get<VAR_UINT>(column, line));
    }

    /**
     * @brief Get the value at position [header, line] as unsigned integer if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return VAR_UINT the value as unsined int
     */
    inline VAR_UINT getUint(const std::string &header, size_t line) const
    {
        return (get<VAR_UINT>(header, line));
    }

    /**
     * @brief Get the value at position [column, line] as real if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return VAR_FLOAT the value as floating point
     */
    inline VAR_FLOAT getFloat(size_t column, size_t line) const
    {
        return (get<VAR_FLOAT>(column, line));
    }

    /**
     * @brief Get the value at position [header, line] as real if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return VAR_FLOAT the value as floating point
     */
    inline VAR_FLOAT getFloat(const std::string &header, size_t line) const
    {
        return (get<VAR_FLOAT>(header, line));
    }

    /**
     * @brief Get the value at position [column, line] as date if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return VAR_DATE the value as date
     */
    inline VAR_DATE getDate(size_t column, size_t line) const
    {
        return (get<VAR_DATE>(column, line));
    }

    /**
     * @brief Get the value at position [header, line] as date if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return VAR_DATE the value as date
     */
    inline VAR_DATE getDate(const std::string &header, size_t line) const
    {
        return (get<VAR_DATE>(header, line));
    }

    /**
     * @brief Get the value at position [column, line] as string if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return VAR_STRING the value as string
     */
    inline VAR_STRING getString(size_t column, size_t line) const
    {
        return (get<VAR_STRING>(column, line));
    }

    /**
     * @brief Get the value at position [header, line] as string if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return VAR_STRING the value as string
     */
    inline VAR_STRING getString(const std::string &header, size_t line) const
    {
        return (get<VAR_STRING>(header, line));
    }

    /**
     * @brief Get the value at position [column, line] as variant if possible.
     *
     * @param column column of the value
     * @param line line of the value
     * @return Var the value as variant
     */
    Var getVar(size_t column, size_t line) const;

    /**
     * Get the value at position [header, line] as variant if possible.
     */

    /**
     * @brief  Get the value at position [header, line] as variant if possible.
     *
     * @param header column of the value identified by the header-string
     * @param line line of the value
     * @return Var the value as variant
     */
    Var getVar(const std::string &header, size_t line) const;

    /**
     * @brief Get all data-values of column as float-values in a vector if possible.
     *
     * @param column column to get
     * @return std::vector<VAR_FLOAT>
     */
    std::vector<VAR_FLOAT> getFloatVector(size_t column) const;

    /**
     * @brief Get all data-values of column as float-values in a vector if possible.
     *
     * @param header column to get identified by the header-string
     * @return std::vector<VAR_FLOAT> all floats of the column
     */
    std::vector<VAR_FLOAT> getFloatVector(const std::string &header) const;

    /**
     * @brief Retrieve the range of values in column.
     *
     * @param column column to extract values from
     * @return COLUMN_RANGE a set of values occurrung in the column
     */
    COLUMN_RANGE getRange(size_t column) const;

    /**
     * @brief Begin-iterator for column.
     *
     * @param column the column to iterate
     * @return COLUMN_TYPE_ITER iterator pointing to the begin of the values of the columns
     */
    COLUMN_TYPE_ITER begin(size_t column);

    /**
     * @brief End-iterator for column.
     *
     * @param column the column to iterate
     * @return COLUMN_TYPE_ITER  iterator pointing to the end of the values of the columns
     */
    COLUMN_TYPE_ITER end(size_t column);

    /**
     * @brief Begin-iterator for column.
     *
     * @param header the column to iterate, identified by its header
     * @return COLUMN_TYPE_ITER iterator pointing to the begin of the values of the columns
     */
    COLUMN_TYPE_ITER begin(const std::string &header);

    /**
     * @brief End-iterator for header.
     *
     * @param header the column to iterate, identified by its header
     * @return COLUMN_TYPE_ITER iterator pointing to the end of the values of the columns
     */
    COLUMN_TYPE_ITER end(const std::string &header);

    /**
     * @brief Convert a value to a different type if possible.
     *
     * @param col column of the value
     * @param row row of the value
     * @param tp the new type
     */
    void convert(size_t col, size_t row, const std::string &tp);

    /**
     * @brief Extract a sub-csv given a list of column indices.
     *
     * @param columns vector of column indices
     * @return CSVAnalyzer a new csv with only the columns in columns
     */
    CSVAnalyzer getSub(const std::vector<size_t> &columns) const;

    /**
     * @brief Extract a sub-csv given a list of header-names.
     *
     * @param headers a vector of header-strings of the columns to extract
     * @return CSVAnalyzer a new csv with only the columns in headers
     */
    CSVAnalyzer getSub(const std::vector<std::string> &headers) const;

    /**
     * @brief Extract a sub-csv convenience function.
     *
     * @param iniList columns defined by initializer_list
     * @return CSVAnalyzer  a new csv with only the columns in colulmns
     */
    CSVAnalyzer getSub(std::initializer_list<size_t> iniList) const;

    /**
     * @brief Extract a sub-csv convenience function.
     *
     * @param iniList columns defined by initializer_list
     * @return CSVAnalyzer a new csv with only the columns in headers
     */
    CSVAnalyzer getSub(std::initializer_list<std::string> iniList) const;

    /**
     * @brief Remove column col from the csv.
     *
     * @param col column to erase
     * @return true, if successful, false otherwise
     */
    bool eraseColumn(size_t col);

    /**
     * @brief
     *
     * @param header column to erase, identified by its header
     * @return true, if successful, false otherwise
     */
    bool eraseColumn(const std::string &header);

    /**
     * @brief configure how the csv is displayed.
     *
     * @param os output stream
     * @param vt verbosity configuration
     * @return std::ostream&
     */
    friend std::ostream &operator<<(std::ostream &os, CSVAnalyzer::verbosityType vt);

    /**
     * @brief Generic ostream - &lt;&lt; operator for CSVAnalyzer.
     *
     * @param os output stream
     * @param csv the csv object to stream
     * @return std::ostream&
     */
    friend std::ostream &operator<<(std::ostream &os, const CSVAnalyzer &csv);

    private:
    CSV_TYPE             data_;                            ///< Rectangular variant data container.
    mutable HEADER_INDEX headerIndex_;                     ///< map header names to column-indices.
    static const int     xalloc_index;                     ///< unique index for outstream configuration.
    rowInputType         inpType_{rowInputType::initial};  ///< what will the next row-input-type be.
    std::string          outSeparator_;                    ///< output separator string.
};

/**
 * @brief configure how the csv is displayed.
 *
 * @param os output stream
 * @param vt verbosity configuration
 * @return std::ostream&
 */
std::ostream &operator<<(std::ostream &os, CSVAnalyzer::verbosityType vt);

/**
 * @brief Generic ostream - &lt;&lt; operator for CSVAnalyzer.
 *
 * @param os output stream
 * @param csv the csv object to stream
 * @return std::ostream&
 */
std::ostream &operator<<(std::ostream &os, const CSVAnalyzer &csv);

/**
 * template specialisation of string conversion for CSVs.
 */
template<>
inline std::string toString(const CSVAnalyzer &csv)
{
    std::stringstream ss;
    ss << CSVAnalyzer::verbosityType::displayAll << csv;
    return (ss.str());
}

};  // namespace util

#endif  // NS_UTIL_CSVUTIL_H_INCLUDED
