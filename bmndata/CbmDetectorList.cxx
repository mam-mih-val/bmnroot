/** @file CbmDetectorList.cxx
 ** @author V.Friese  <V.Friese@Gsi.De>
 ** @date 29.04.2010
 **/

#include "CbmDetectorList.h"

#include "TString.h"


// -----   Constructor   ---------------------------------------------------
CbmDetectorList::CbmDetectorList()
{
}
// -------------------------------------------------------------------------


// -----   GetSystemName   -------------------------------------------------
void CbmDetectorList::GetSystemName(DetectorId det, TString& name)
{
    switch (det)
    {
        case kREF:  name = "ref";  break;
        case kMVD:  name = "mvd";  break;
        case kGEM:  name = "gem";  break;
        case kTOF1: name = "tof1"; break;
        case kDCH: name = "dch"; break;
        case kTOF:  name = "tof";  break;
        case kZDC:  name = "zdc";  break;
        case kRECOIL:  name = "recoil";  break;
        case kBD:  name = "bd";  break;
        case kFD:  name = "fd";  break;
        case kMWPC:  name = "MWPC";  break;
        case kSILICON: name = "silicon"; break;
        case kCSC: name = "csc"; break;
        case kARMTRIG: name = "armtrig"; break;
        case kBC: name = "bc"; break;
        case kSCWALL: name = "scwall"; break;
        case kHODO: name = "hodo"; break;
        default:    name = "unknown"; break;
    }
}
// -------------------------------------------------------------------------


// -----   GetSystemName   -------------------------------------------------
void CbmDetectorList::GetSystemName(Int_t det, TString& name)
{
  if ( det < kNOFDETS ) GetSystemName(DetectorId(det), name);
  else name = "unknown";
}
// -------------------------------------------------------------------------


// -----   GetSystemNameCaps   ---------------------------------------------
void CbmDetectorList::GetSystemNameCaps(DetectorId det, TString& name)
{
    switch (det)
    {
        case kREF:  name = "REF";  break;
        case kMVD:  name = "MVD";  break;
        case kGEM:  name = "GEM";  break;
        case kTOF1: name = "TOF1";  break;
        case kDCH: name = "DCH";  break;
        case kTOF:  name = "TOF";  break;
        case kZDC:  name = "ZDC";  break;
        case kRECOIL:  name = "RECOIL";  break;
        case kBD:  name = "BD";  break;
        case kFD:  name = "FD";  break;
        case kMWPC:  name = "MWPC";  break;
        case kSILICON: name = "SILICON"; break;
        case kBC: name = "BC"; break;
        case kCSC: name = "CSC"; break;
        case kARMTRIG: name = "ARMTRIG"; break;
        case kSCWALL: name = "SCWALL"; break;
        case kHODO: name = "HODO"; break;
        default:    name = "UNKNOWN"; break;
  }
}
// -------------------------------------------------------------------------


// -----   GetSystemNameCaps   ---------------------------------------------
void CbmDetectorList::GetSystemNameCaps(Int_t det, TString& name)
{
  if ( det < kNOFDETS ) GetSystemNameCaps(DetectorId(det), name);
  else name = "unknown";
}
// -------------------------------------------------------------------------


ClassImp(CbmDetectorList)
