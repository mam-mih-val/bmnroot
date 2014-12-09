/**
 * \file CbmLitResultChecker.cxx
 * \author Andrey Lebedev <andrey.lebedev@gsi.de>
 * \date 2011
 */
#include "CbmLitResultChecker.h"
#include <iostream>
#include <boost/property_tree/json_parser.hpp>
using boost::property_tree::json_parser_error;
using std::cout;
using std::pair;

CbmLitResultChecker::CbmLitResultChecker()
{

}

CbmLitResultChecker::~CbmLitResultChecker()
{

}

void CbmLitResultChecker::DoCheck(
      const string& qaFile,
      const string& idealFile,
      const string& checkFile)
{
   ptree qa, ideal, check;

   try {
      read_json(qaFile.c_str(), qa);
   } catch (json_parser_error& error) {
      cout << error.what();
   }

   try {
      read_json(idealFile.c_str(), ideal);
   } catch (json_parser_error& error) {
      cout << error.what();
   }

   DoCheck(qa, ideal, check);

   try {
      write_json(checkFile.c_str(), check);
   } catch (json_parser_error& error) {
      cout << error.what();
   }
}

void CbmLitResultChecker::DoCheck(
      const ptree& qa,
      const ptree& ideal,
      ptree& out)
{
   // Build map out of property tree for convenience.
   map<string, Double_t> mymap;
   PropertyTreeToMap("", qa, mymap);

   // Iterate over the map, get each property and compare it to ideal.
   for (map<string, Double_t>::const_iterator it = mymap.begin(); it != mymap.end(); it++) {
      map<string, Double_t>::value_type v = *it;

      boost::optional<Double_t> vmin = ideal.get_optional<Double_t>(v.first + ".min");
      boost::optional<Double_t> vmax = ideal.get_optional<Double_t>(v.first + ".max");

      // Check if value exists in ideal
      if (!vmin || !vmax) {
         // -1 in the output tree indicates that value was not checked because
         // it was not found in ideal property tree
         out.put(v.first, -1.f);
         continue;
      } else {
         // Check that qa value lays within (min, max) limits
         if (v.second >= vmin && v.second <= vmax) {
            // Qa value is within limits
            out.put(v.first, 1.f);
         } else {
            // Qa value is out of range
            out.put(v.first, 0.f);
         }
      }
   }
}

void CbmLitResultChecker::PropertyTreeToMap(
      const string& path,
      const ptree& pt,
      map<string, Double_t>& mymap) const
{
   if (pt.size() == 0) {
      mymap.insert(pair<string, Double_t>(path, pt.get_value(-1.f)));
      return;
   }
   for (ptree::const_iterator it = pt.begin(); it != pt.end(); it++) {
      ptree::value_type v = *it;
      string path1 = (path != "") ? (path + "." + v.first) : v.first;
      PropertyTreeToMap(path1, v.second, mymap);
   }
}
