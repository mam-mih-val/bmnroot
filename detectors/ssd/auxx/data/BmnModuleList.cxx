/** @file BmnModuleList.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 07.12.2018
 **/

#include "BmnModuleList.h"

#include "FairLogger.h"

#include <boost/algorithm/string.hpp>

using std::map;

// -----   Initialisation of the list of modules   -------------------------
map<Int_t, TString> BmnModuleList::DefineModules() {
  map<Int_t, TString> data;

  data[kREF]      = "ref";
  data[kMVD]      = "mvd";
  data[kSSD]      = "ssd";
  data[kTOF]      = "tof";
  data[kECAL]     = "ecal";

  return data;
}
// -------------------------------------------------------------------------



// -----   Initialise static map -------------------------- ----------------
// This is done by using the copy constructor of std::map, calling the
// method DefineModules, which actually fills the static map.
map<Int_t, TString> BmnModuleList::fModules(BmnModuleList::DefineModules());
// -------------------------------------------------------------------------



// ------  Get module Id from module name  ---------------------------------
Int_t BmnModuleList::GetModuleId(const char* moduleName) {

  map<Int_t, TString>::iterator it = fModules.begin();
  while ( it != fModules.end() ) {
    if ( ! (it->second).CompareTo(moduleName, TString::kIgnoreCase) )
      return it->first;
  it++;
  }
  return -1;

}
// -------------------------------------------------------------------------



// ------   Get module name from module Id   --------------------------------
TString BmnModuleList::GetModuleName(Int_t moduleId) {
  if ( fModules.find(moduleId) == fModules.end() ) {
    LOG(error) << "Module List: Illegal module Id " << moduleId;
    return "";
  }
  return fModules.find(moduleId)->second;
}
// -------------------------------------------------------------------------



// ------   Get module name in capitals from module Id   -------------------
TString BmnModuleList::GetModuleNameCaps(Int_t moduleId) {
	TString name = GetModuleName(moduleId);
	name.ToUpper();
	return name;
}
// -------------------------------------------------------------------------


