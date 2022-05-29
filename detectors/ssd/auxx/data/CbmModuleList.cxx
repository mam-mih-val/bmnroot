/** @file CbmModuleList.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 13.06.2013
 **/

#include "CbmModuleList.h"

#include "FairLogger.h"

#include <boost/algorithm/string.hpp>

using std::map;

// -----   Initialisation of the list of modules   -------------------------
map<Int_t, TString> CbmModuleList::DefineModules() {
  map<Int_t, TString> data;

  data[kRef]      = "ref";
  data[kMvd]      = "mvd";
  data[kSts]      = "sts";
  data[kRich]     = "rich";
  data[kMuch]     = "much";
  data[kTrd]      = "trd";
  data[kTof]      = "tof";
  data[kEcal]     = "ecal";
  data[kDummyDet] = "dummy";
  data[kMagnet]   = "magnet";
  data[kTarget]   = "target";
  data[kPipe]     = "pipe";
  data[kShield]   = "shield";
  data[kPlatform] = "platform";

  return data;
}
// -------------------------------------------------------------------------



// -----   Initialise static map -------------------------- ----------------
// This is done by using the copy constructor of std::map, calling the
// method DefineModules, which actually fills the static map.
map<Int_t, TString> CbmModuleList::fModules(CbmModuleList::DefineModules());
// -------------------------------------------------------------------------



// ------  Get module Id from module name  ---------------------------------
Int_t CbmModuleList::GetModuleId(const char* moduleName) {

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
TString CbmModuleList::GetModuleName(Int_t moduleId) {
  if ( fModules.find(moduleId) == fModules.end() ) {
    LOG(error) << "Module List: Illegal module Id " << moduleId;
    return "";
  }
  return fModules.find(moduleId)->second;
}
// -------------------------------------------------------------------------



// ------   Get module name in capitals from module Id   -------------------
TString CbmModuleList::GetModuleNameCaps(Int_t moduleId) {
	TString name = GetModuleName(moduleId);
	name.ToUpper();
	return name;
}
// -------------------------------------------------------------------------


