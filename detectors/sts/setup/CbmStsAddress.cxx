/** @file CbmStsAddress.h
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17.05.2013
 **/


#include "CbmDetectorList.h"
#include "setup/CbmStsAddress.h"


// -----    Definition of the address field   -------------------------------
const Int_t CbmStsAddress::fgkBits[] = { fgkSystemBits,   // system = kSTS
                                         4,   // station
                                         4,   // ladder
                                         1,   // halfladder
                                         3,   // module
                                         2,   // sensor
                                         1,   // side
                                        13 }; // channel
// -------------------------------------------------------------------------



// -----    Initialisation of bit shifts -----------------------------------
const Int_t CbmStsAddress::fgkShift[] =
   { 0,
     CbmStsAddress::fgkShift[0] + CbmStsAddress::fgkBits[0],
     CbmStsAddress::fgkShift[1] + CbmStsAddress::fgkBits[1],
     CbmStsAddress::fgkShift[2] + CbmStsAddress::fgkBits[2],
     CbmStsAddress::fgkShift[3] + CbmStsAddress::fgkBits[3],
     CbmStsAddress::fgkShift[4] + CbmStsAddress::fgkBits[4],
     CbmStsAddress::fgkShift[5] + CbmStsAddress::fgkBits[5],
     CbmStsAddress::fgkShift[6] + CbmStsAddress::fgkBits[6] };
// -------------------------------------------------------------------------



// -----    Initialisation of bit masks  -----------------------------------
const Int_t CbmStsAddress::fgkMask[] = { ( 1 << fgkBits[0] ) -1,
                                         ( 1 << fgkBits[1] ) -1,
                                         ( 1 << fgkBits[2] ) -1,
                                         ( 1 << fgkBits[3] ) -1,
                                         ( 1 << fgkBits[4] ) -1,
                                         ( 1 << fgkBits[5] ) -1,
                                         ( 1 << fgkBits[6] ) -1,
                                         ( 1 << fgkBits[7] ) -1  };
// -------------------------------------------------------------------------



// -----  Unique element address   -----------------------------------------
UInt_t CbmStsAddress::GetAddress(Int_t station,
                                 Int_t ladder,
                                 Int_t halfladder,
                                 Int_t module,
                                 Int_t sensor,
                                 Int_t side,
                                 Int_t channel) {

  // Catch overrunning of allowed ranges
  if ( station >= ( 1 << fgkBits[kStsStation] ) ) {
    LOG(error) << "Station Id "  << station << " exceeds maximum ("
               << ( 1 << fgkBits[kStsStation] ) - 1 << ")";
    return 0;
  }
  if ( ladder >= ( 1 << fgkBits[kStsLadder]) ) {
    LOG(error) << "Ladder Id "  << ladder << " exceeds maximum ("
               << ( 1 << fgkBits[kStsLadder] ) - 1 << ")";
    return 0;
  }
  if ( halfladder >= ( 1 << fgkBits[kStsHalfLadder]) ) {
    LOG(error) << "HalfLadder Id "  << halfladder << " exceeds maximum ("
               << ( 1 << fgkBits[kStsHalfLadder] ) - 1 << ")";
    return 0;
  }
  if ( module >= ( 1 << fgkBits[kStsModule]) ) {
    LOG(error) << "Module Id "  << module << " exceeds maximum ("
               << ( 1 << fgkBits[kStsModule] ) - 1 << ")";
    return 0;
  }
  if ( sensor >= ( 1 << fgkBits[kStsSensor]) ) {
    LOG(error) << "Sensor Id "  << sensor << " exceeds maximum ("
               << ( 1 << fgkBits[kStsSensor] ) - 1 << ")";
    return 0;
  }
  if ( side >= ( 1 << fgkBits[kStsSide]) ) {
    LOG(error) << "Side Id "  << side << " exceeds maximum ("
               << ( 1 << fgkBits[kStsSide] ) - 1 << ")";
    return 0;
  }
  if ( channel >= ( 1 << fgkBits[kStsChannel]) ) {
    LOG(error) << "Channel Id "  << channel << " exceeds maximum ("
               << ( 1 << fgkBits[kStsChannel] ) - 1 << ")";
    return 0;
  }

  return kSts       << fgkShift[kStsSystem]     |
         station    << fgkShift[kStsStation]    |
         ladder     << fgkShift[kStsLadder]     |
         halfladder << fgkShift[kStsHalfLadder] |
         module     << fgkShift[kStsModule]     |
         sensor     << fgkShift[kStsSensor]     |
         side       << fgkShift[kStsSide]       |
         channel    << fgkShift[kStsChannel];

}
// -------------------------------------------------------------------------



// -----  Unique element address   -----------------------------------------
UInt_t CbmStsAddress::GetAddress(Int_t* elementId) {

  UInt_t address = kSts << fgkShift[kStsSystem];
  for (Int_t level = 1; level < kStsNofLevels; level++) {
    if ( elementId[level] >= ( 1 << fgkBits[level] ) ) {
      LOG(error) << "Id " << elementId[level] << " for STS level " << level
                 << " exceeds maximum (" << (1 << fgkBits[level]) - 1
                 << ")";
      return 0;
    }
    address = address | ( elementId[level] << fgkShift[level] );
  }

  return address;
}
// -------------------------------------------------------------------------



// -----   Print info   ----------------------------------------------------
void CbmStsAddress::Print() {
  LOG(info) << "Number of STS levels is " << kStsNofLevels;
  for (Int_t level = 0; level < kStsNofLevels; level++)
    LOG(info) << "Level " << std::setw(2) << std::right << level
              << ": bits " << std::setw(2) << fgkBits[level]
              << ", max. range " << std::setw(6) << fgkMask[level];
}
// -------------------------------------------------------------------------


ClassImp(CbmStsAddress)

