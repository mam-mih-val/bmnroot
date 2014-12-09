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
        case kDCH1: name = "dch1"; break;
        case kDCH2: name = "dch2"; break;
        case kTOF:  name = "tof";  break;
        case kZDC:  name = "zdc";  break;
        case kRECOIL:  name = "recoil";  break;
        case kMWPC1:  name = "mwpc1";  break;
        case kMWPC2:  name = "mwpc2";  break;
        case kMWPC3:  name = "mwpc3";  break;
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
        case kDCH1: name = "DCH1";  break;
        case kDCH2: name = "DCH2";  break;
        case kTOF:  name = "TOF";  break;
        case kZDC:  name = "ZDC";  break;
        case kRECOIL:  name = "RECOIL";  break;
        case kMWPC1:  name = "mwpc1";  break;
        case kMWPC2:  name = "mwpc2";  break;
        case kMWPC3:  name = "mwpc3";  break;
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
