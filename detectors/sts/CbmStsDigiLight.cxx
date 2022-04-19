/** CbmStsDigiLight.cxx
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.2006
 **@version 2.0
 **/


#include "CbmStsDigiLight.h"


// -----   Bit fields for charge and time stamp   --------------------------
const Int_t CbmStsDigiLight::fgkCharBits = 12;   // in data0
const Int_t CbmStsDigiLight::fgkTimeBits = 14;   // in data1
const Int_t CbmStsDigiLight::fgkAddrBits = 
  CbmStsDigiLight::fgkStatBits +
  CbmStsDigiLight::fgkSectBits +
  CbmStsDigiLight::fgkSideBits + 
  CbmStsDigiLight::fgkChanBits;
// -------------------------------------------------------------------------



// -----   Bit shift for charge and time stamp   ---------------------------
const Int_t CbmStsDigiLight::fgkCharShift = CbmStsDigiLight::fgkAddrBits;
const Int_t CbmStsDigiLight::fgkTimeShift = CbmStsDigiLight::fgkCharShift + CbmStsDigiLight::fgkCharBits;
// -------------------------------------------------------------------------



// -----   Bit masks for charge and time stamp   ---------------------------
const Long64_t CbmStsDigiLight::fgkAddrMask = (1 << CbmStsDigiLight::fgkAddrBits) - 1;
const Long64_t CbmStsDigiLight::fgkCharMask = (1 << CbmStsDigiLight::fgkCharBits) - 1;
const Long64_t CbmStsDigiLight::fgkTimeMask = (1 << CbmStsDigiLight::fgkTimeBits) - 1;
// -------------------------------------------------------------------------




// -----   Default constructor   -------------------------------------------
CbmStsDigiLight::CbmStsDigiLight() 
  : CbmDigi(),
    CbmStsDetectorId(),
    fData(0) 
{
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
// CbmStsDigiLight::CbmStsDigiLight(Int_t station, Int_t sector, Int_t side, 
// 		       Int_t channel, Int_t adc, Int_t time) 
//   : FairMultiLinkedData(),
//     CbmStsDetectorId(), 
//     fData(0) 
// {
//   AddIndex(index);
//   fData = ( (DetectorId(station, sector, side, channel) >> 4) & fgkAddrMask )
//     | ( (adc  & fgkCharMask) << fgkCharShift )
//     | ( (time & fgkTimeMask) << fgkTimeShift ) ;
//  
// }
// -------------------------------------------------------------------------
CbmStsDigiLight::CbmStsDigiLight(Int_t station, Int_t sector, Int_t side, 
		       Int_t channel, Int_t adc, Int_t time) 
  : CbmDigi(),
    CbmStsDetectorId(),
    fData(0) 
{
//   AddIndex(index,adc);
  fData = ( (DetectorId(station, sector, side, channel) >> 4) & fgkAddrMask )
    | ( (adc  & fgkCharMask) << fgkCharShift )
    | ( (time & fgkTimeMask) << fgkTimeShift ) ; 
}



// -----   Destructor   ----------------------------------------------------
CbmStsDigiLight::~CbmStsDigiLight() { }
// -------------------------------------------------------------------------




// -----   Add charge   ----------------------------------------------------
void CbmStsDigiLight::AddAdc(Int_t adc) {
  Int_t maxAdc = (1 << fgkCharBits) - 1;
  Int_t newAdc = GetAdc() + adc;
  if ( newAdc > maxAdc ) newAdc = maxAdc;
  SetAdc(newAdc);
}
// -------------------------------------------------------------------------




// -----   Set new charge   ------------------------------------------------
void CbmStsDigiLight::SetAdc(Int_t adc) {

  // First set old charge to zero
  fData = fData & ~(fgkCharMask << fgkCharShift);

  // Now set new charge
  fData = fData | ( (adc & fgkCharMask) << fgkCharShift );

}
// -------------------------------------------------------------------------



ClassImp(CbmStsDigiLight)
