/** @file BmnSsdAddress.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 07.12.2018
 **/

#include "BmnSsdAddress.h"

#include <cassert>
#include <sstream>
#include "BmnModuleList.h"


namespace BmnSsdAddress {

  // -----    Definition of address bit field   ------------------------------
  const Int_t kBits[kCurrentVersion+1][kSsdNofLevels] =
  {

   // Version 0 (until 23 August 2017)
   {  4,  // system
      4,  // unit / station
      4,  // ladder
      1,  // half-ladder
      3,  // module
      2,  // sensor
      1   // side
   },

   // Version 1 (current, since 23 August 2017)
   {  4,  // system
      6,  // unit
      5,  // ladder
      1,  // half-ladder
      5,  // module
      4,  // sensor
      1   // side
   }

  };
  // -------------------------------------------------------------------------


  // -----    Bit shifts -----------------------------------------------------
  const Int_t kShift[kCurrentVersion+1][kSsdNofLevels] =
  {
   {   0,
       kShift[0][0] + kBits[0][0],
       kShift[0][1] + kBits[0][1],
       kShift[0][2] + kBits[0][2],
       kShift[0][3] + kBits[0][3],
       kShift[0][4] + kBits[0][4],
       kShift[0][5] + kBits[0][5]
   },

   {   0,
       kShift[1][0] + kBits[1][0],
       kShift[1][1] + kBits[1][1],
       kShift[1][2] + kBits[1][2],
       kShift[1][3] + kBits[1][3],
       kShift[1][4] + kBits[1][4],
       kShift[1][5] + kBits[1][5]
   }
  };
  // -------------------------------------------------------------------------


  // -----    Bit masks  -----------------------------------------------------
  const Int_t kMask[kCurrentVersion+1][kSsdNofLevels] =
  {
   { ( 1 << kBits[0][0] ) -1,
     ( 1 << kBits[0][1] ) -1,
     ( 1 << kBits[0][2] ) -1,
     ( 1 << kBits[0][3] ) -1,
     ( 1 << kBits[0][4] ) -1,
     ( 1 << kBits[0][5] ) -1,
     ( 1 << kBits[0][6] ) -1
   },

   { ( 1 << kBits[1][0] ) -1,
     ( 1 << kBits[1][1] ) -1,
     ( 1 << kBits[1][2] ) -1,
     ( 1 << kBits[1][3] ) -1,
     ( 1 << kBits[1][4] ) -1,
     ( 1 << kBits[1][5] ) -1,
     ( 1 << kBits[1][6] ) -1
   }
 };
  // -------------------------------------------------------------------------


} // Namespace BmnSsdAddress


// -----   Construct address from element Ids   ------------------------------
Int_t BmnSsdAddress::GetAddress(UInt_t unit,
                                UInt_t ladder,
                                UInt_t halfladder,
                                UInt_t module,
                                UInt_t sensor,
                                UInt_t side,
                                UInt_t version) {

  assert ( version <= kCurrentVersion );

  // Catch overrun of allowed ranges
  if ( unit >= ( 1 << kBits[version][kSsdUnit] ) ) {
    LOG(error) << "Unit Id "  << unit << " exceeds maximum ("
        << ( 1 << kBits[version][kSsdUnit] ) - 1 << ")";
    return 0;
  }
  if ( ladder >= ( 1 << kBits[version][kSsdLadder]) ) {
    LOG(error) << "Ladder Id "  << ladder << " exceeds maximum ("
        << ( 1 << kBits[version][kSsdLadder] ) - 1 << ")";
    return 0;
  }
  if ( halfladder >= ( 1 << kBits[version][kSsdHalfLadder]) ) {
    LOG(error) << "HalfLadder Id "  << halfladder << " exceeds maximum ("
        << ( 1 << kBits[version][kSsdHalfLadder] ) - 1 << ")";
    return 0;
  }
  if ( module >= ( 1 << kBits[version][kSsdModule]) ) {
    LOG(error) << "Module Id "  << module << " exceeds maximum ("
        << ( 1 << kBits[version][kSsdModule] ) - 1 << ")";
    return 0;
  }
  if ( sensor >= ( 1 << kBits[version][kSsdSensor]) ) {
    LOG(error) << "Sensor Id "  << sensor << " exceeds maximum ("
        << ( 1 << kBits[version][kSsdSensor] ) - 1 << ")";
    return 0;
  }
  if ( side >= ( 1 << kBits[version][kSsdSide]) ) {
    LOG(error) << "Side Id "  << side << " exceeds maximum ("
        << ( 1 << kBits[version][kSsdSide] ) - 1 << ")";
    return 0;
  }

  return kSSD       << kShift[version][kSsdSystem]     |
         unit       << kShift[version][kSsdUnit   ]    |
         ladder     << kShift[version][kSsdLadder]     |
         halfladder << kShift[version][kSsdHalfLadder] |
         module     << kShift[version][kSsdModule]     |
         sensor     << kShift[version][kSsdSensor] |
         side       << kShift[version][kSsdSide]       |
         version    << kVersionShift;

}
// ---------------------------------------------------------------------------



// -----   Construct address from array of element Ids   ----------------------
Int_t BmnSsdAddress::GetAddress(UInt_t* elementId, UInt_t version) {

  assert ( version <= kCurrentVersion );

  Int_t address = kSSD << kShift[version][kSsdSystem];
  for (Int_t level = 1; level < kSsdNofLevels; level++) {
    if ( elementId[level] >= ( 1 << kBits[version][level] ) ) {
      LOG(error) << "Id " << elementId[level] << " for SSD level " << level
          << " exceeds maximum (" << (1 << kBits[version][level]) - 1
          << ")";
      return 0;
    }
    address = address | ( elementId[level] << kShift[version][level] );
  }
  address = address | ( version << kVersionShift );

  return address;
}
// ---------------------------------------------------------------------------



// -----   Construct address from address of descendant element   ------------
Int_t BmnSsdAddress::GetMotherAddress(Int_t address, Int_t level) {
  assert ( level >= kSsdSystem && level < kSsdNofLevels );
  if ( level == kSsdNofLevels - 1 ) return address;
  UInt_t version = GetVersion(address);
  Int_t motherAdd = ( address & ( ( 1 << kShift[version][level+1] ) - 1 ) ) ;
  motherAdd = motherAdd | ( version << kVersionShift );
  return motherAdd;
}
// ---------------------------------------------------------------------------



// -----   Get the index of an element   -------------------------------------
UInt_t BmnSsdAddress::GetElementId(Int_t address, Int_t level) {
  assert ( level >= kSsdSystem && level < kSsdNofLevels );
  UInt_t version = GetVersion(address);
  return ( address & ( kMask[version][level] << kShift[version][level] ) )
      >> kShift[version][level];
}
// ---------------------------------------------------------------------------



// -----   Get System ID   -------------------------------------------------
UInt_t BmnSsdAddress::GetSystemId(Int_t address) {
  return GetElementId(address, kSsdSystem);
}
// -------------------------------------------------------------------------



// -----   Get the version number from the address   -------------------------
UInt_t BmnSsdAddress::GetVersion(Int_t address) {
  return UInt_t( ( address & ( kVersionMask << kVersionShift ) )
                 >> kVersionShift );
}
// ---------------------------------------------------------------------------



// -----  Construct address by changing the index of an element   ------------
Int_t BmnSsdAddress::SetElementId(Int_t address, Int_t level,
                                  UInt_t newId) {
  assert ( level >= kSsdSystem && level < kSsdNofLevels );
  UInt_t version = GetVersion(address);
  if ( newId >= ( 1 << kBits[version][level]) ) {
    LOG(fatal) << "Id " << newId << " for SSD level " << level
        << " exceeds maximum (" << (1 << kBits[version][level]) - 1
        << ")";
    return 0;
  }
  return ( address & (~ (kMask[version][level] << kShift[version][level]) ) )
      | ( newId << kShift[version][level]);
}
// -------------------------------------------------------------------------



// -----   String output   -------------------------------------------------
std::string BmnSsdAddress::ToString(Int_t address) {
  std::stringstream ss;

  ss << "SsdAddress: address " << address
      << " (version " << GetVersion(address) << ")"
      << ": system " << GetElementId(address, kSsdSystem)
      << ", unit " << GetElementId(address, kSsdUnit)
      << ", ladder " << GetElementId(address, kSsdLadder)
      << ", half-ladder " << GetElementId(address, kSsdHalfLadder)
      << ", module " << GetElementId(address, kSsdModule)
      << ", sensor " << GetElementId(address, kSsdSensor)
      << ", side " << GetElementId(address, kSsdSide);
  return ss.str();
}
// -------------------------------------------------------------------------





