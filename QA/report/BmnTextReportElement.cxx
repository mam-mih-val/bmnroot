/**
 * \file BmnTextReportElement.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnTextReportElement.h"
#include <sstream>
#include <iomanip>

using std::stringstream;
using std::endl;
using std::right;
using std::setfill;
using std::endl;
using std::setw;
using std::left;

BmnTextReportElement::BmnTextReportElement():
   fColW(25)
{
}

BmnTextReportElement::~BmnTextReportElement()
{

}

string BmnTextReportElement::TableBegin(
      const string& caption,
      const vector<string>& colNames) const
{
   stringstream ss;
   ss << caption << endl;
   ss << right;
   ss << setfill('_') << setw(colNames.size() * fColW) << "_"<< endl;
   ss << setfill(' ');
   for (Int_t i = 0; i < colNames.size(); i++) {
      ss << FormatCell(colNames[i]); //setw(fColW) << colNames[i];
   }
   ss << endl;
   ss << setfill('_') << setw(colNames.size() * fColW) << "_"<< endl;
   return ss.str();
}

string BmnTextReportElement::TableEnd() const
{
   stringstream ss;
   ss << endl;
   return ss.str();
}

string BmnTextReportElement::TableEmptyRow(
      Int_t nofCols,
      const string& name) const
{
   stringstream ss;
   ss << setfill('-') << left;
   ss << setw(nofCols * fColW) << name << endl;
   ss << setfill(' ') << left;
   return ss.str();
}

string BmnTextReportElement::TableRow(
      const vector<string>& row) const
{
   stringstream ss;
   ss << right;
   for (Int_t i = 0; i < row.size(); i++) {
      ss << FormatCell(row[i]); //setw(fColW) << row[i];
   }
   ss << endl;
   return ss.str();
}

string BmnTextReportElement::Image(
      const string& title,
      const string& file) const
{
   return "";
}

string BmnTextReportElement::DocumentBegin() const
{
   stringstream ss;
   ss << "------------------------------------------------" << endl;
   return ss.str();
}

string BmnTextReportElement::DocumentEnd() const
{
   return "";
}

string BmnTextReportElement::Title(
      Int_t size,
      const string& title) const
{
   return title;
}

string BmnTextReportElement::FormatCell(
      const string& cell) const
{
	if (cell.size() <= fColW) {
		stringstream ss;
		ss << setw(fColW) << cell;
		return ss.str();
	} else {
		string str = cell;
		str.resize(fColW - 3);
		str.insert(fColW - 3, "...");
		return str;
	}
}

ClassImp(BmnTextReportElement)
