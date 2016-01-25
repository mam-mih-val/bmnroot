/**
 * \file BmnLatexReportElement.cxx
 * \author Semen Lebedev <s.lebedev@gsi.de>
 * \date 2011
 */
#include "BmnLatexReportElement.h"
#include <sstream>
using std::stringstream;
using std::endl;

BmnLatexReportElement::BmnLatexReportElement()
{

}

BmnLatexReportElement::~BmnLatexReportElement()
{

}

string BmnLatexReportElement::TableBegin(
      const string& caption,
      const vector<string>& colNames) const
{
   string st = "\\begin{table}[h] \n";
   st += "\\centering";
   if (caption != "") st += "\\caption{" + caption + "} \n";
   st += "\\begin{tabular}{|";
   for (int i = 0; i < colNames.size(); i++) {
      st += "c|";
   }
   st += "} \\hline \n";
   // names start from the second column
   for (int i = 0; i < colNames.size(); i++) {
      st += (i < (colNames.size() - 1)) ? (colNames[i] + " & ") : (colNames[i]);
   }
   st += "\\\\ \\hline \n";
   return st;
}

string BmnLatexReportElement::TableEnd() const
{
   return "\\end{tabular} \\end{table}";
}

string BmnLatexReportElement::TableEmptyRow(
      int nofCols,
      const string& name) const
{
   stringstream ss;
   ss << "\\multicolumn{" << nofCols << "}{|c|}{" << name << "} \\\\ \\hline" << endl;
   return ss.str();
}

string BmnLatexReportElement::TableRow(
      const vector<string>& row) const
{
   string st;
   for (int i = 0; i < row.size(); i++) {
      st += (i < (row.size() - 1)) ? (row[i] + " & ") : (row[i]);
   }
   st += " \\\\ \\hline \n";
   return st;
}

string BmnLatexReportElement::Image(
      const string& title,
      const string& file) const
{
   stringstream ss;

   ss << "\\begin{figure}[h]" << endl;
   ss << "\\centering" << endl;
   ss << "\\includegraphics[width=7cm]{" << file << ".eps}" << endl;
   ss << "\\caption{" << title << "}" << endl;
   ss << "\\end{figure}" << endl;
   return ss.str();
}

string BmnLatexReportElement::DocumentBegin() const
{
   string str = "\\documentclass[a4paper,14pt]{article}";
   str += "\\usepackage{amssymb}";
   str += "\\setlength\\oddsidemargin{-2cm}";
   str += "\\setlength\\evensidemargin{-2cm}";
   str += "\\setlength\\textwidth{17cm}";
   str += "\\setlength\\topmargin{0cm}";
   str += "\\usepackage[dvips]{graphicx}";
   str += "\\begin{document}";

   return str;
}

string BmnLatexReportElement::DocumentEnd() const
{
   return "\\end{document}";
}

string BmnLatexReportElement::Title(
      int size,
      const string& title) const
{
   string st = "\\";
   for (int i = 0; i < size; i++) st += "sub";
   st += "title{" + title + "}";
   return st;
}

ClassImp(BmnLatexReportElement)
