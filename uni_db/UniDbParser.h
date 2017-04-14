// -----------------------------------------------------------------------------------------
// -----  Functions to parse TXT, XML, CSV files and write the values to the database  -----
// -----                     Created 18/08/15  by K. Gertsenberger                     -----
// -----------------------------------------------------------------------------------------
//
// It uses *.xslt scheme with rules to parse given format:
// <skip line_count=""/> - skip given count of text lines in file
// <search name="tag name"/> - search tag with given name
// <move down="recursively child number"/> - move to child tag recursively
// <cycle child="parent name" table_name="database table to write parsing values" delimiter="delimeters between elements" skip="number of skipping text lines">
//   - cycle for elements separated by delimiters and write them to the database (childs of parent with given name if it was set)
// <element action=["skip", "update", "write", "multi", "parse"], where:
// action="skip" - skip current element
// action="update" column_name="database column name" type="type name"/> - update database table row with database column name being equal the cuurent element
// action="write" column_name="database column name" type="type name"/> - convert current element to the given type (int, double, string, datetime, hex, binary) and write to the given database table column
// action="multi"> <subelement action...> <subelement action...> ... </element>
// action="parse" start_index="parse from start symbol" parse_type=["counter" - current element number from 1, "value:fixed_value", "parsed type name + 'int_array' or 'double_array'"] delimiter="delimeter symbol if parsed array" column_name="database column name" type="C++ type"/>
//   - converted value from start symbol (default, 0) OR special values to write to the database table column
// action="parse"  parse_type=["counter" - current element number from 1, "value:fixed_value"] column_name="database column name" type="type name"/> - special values to write to the database table column
// </cycle>

#ifndef UNIDBPARSER_H
#define UNIDBPARSER_H 1

#include "TString.h"

#include <vector>
using namespace std;

struct structParseRow
{
    TString strColumnName;
    TString strStatementType;
    bool isParse;
    int iStartIndex;
    TString strParseType;
    TString strDelimiter;

    structParseRow(TString column_name, TString statement_type, bool is_parse = false, int start_index = 0, TString parse_type = "", TString delimiter = "")
    {
        strColumnName = column_name;
        strStatementType = statement_type;
        isParse = is_parse;
        iStartIndex = start_index;
        strParseType = parse_type;
        strDelimiter = delimiter;
    }
};

struct structParseSchema
{
    // skip element
    bool isSkip;
    bool isUpdate;
    vector<structParseRow> vecRows;

    structParseSchema()
    {
        isUpdate = false;
    }
    structParseSchema(bool is_skip, TString column_name="", TString statement_type="")
    {
        isUpdate = false;
        isSkip = is_skip;
        if (is_skip == false)
        {
            structParseRow row(column_name, statement_type);
            vecRows.push_back(row);
        }
    }
    ~structParseSchema()
    {
        if (isSkip == false)
            vecRows.clear();
    }
};

class UniDbParser
{
 public:
    UniDbParser();
    ~UniDbParser();

    int ParseXml2Db(TString xmlName, TString schemaPath, bool isUpdate = false);
    int ParseCsv2Db(TString csvName, TString schemaPath, bool isUpdate = false);
    int ParseTxt2Db(TString txtName, TString schemaPath, bool isUpdate = false);
    int ParseTxtNoise2Db(int period_number, TString txtName, TString schemaPath);

    // parse functions with specific features for BM@N
    int ParseElogCsv(int period_number, TString csvName, char separate_symbol = ';');

 ClassDef(UniDbParser,1)
};

#endif

