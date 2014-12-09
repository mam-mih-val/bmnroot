/** @file CbmMCBuffer.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 17 July 2012
 **/


#include <iostream>
#include <iomanip>

#include "TString.h"

#include "FairLogger.h"

#include "CbmDaqBuffer.h"
#include "CbmDetectorList.h"



// -----   Initialisation of static variables   ------------------------------
CbmDaqBuffer* CbmDaqBuffer::fgInstance = NULL;
// ---------------------------------------------------------------------------



// -----   Constructor   -----------------------------------------------------
CbmDaqBuffer::CbmDaqBuffer() {
}
// ---------------------------------------------------------------------------


// -----   Destructor   ------------------------------------------------------
CbmDaqBuffer::~CbmDaqBuffer() {
}
// ---------------------------------------------------------------------------



// -----   Time of first raw data   ------------------------------------------
Double_t CbmDaqBuffer::GetFirstTime() const {
  Double_t time = -1.;
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++) {
    if ( GetSize(iDet) ) {
        if ( time < 0. ) time = GetFirstTime(iDet);
        else time = ( time < GetFirstTime(iDet) ? time : GetFirstTime(iDet) );
    }
  }
  return time;
}
// ---------------------------------------------------------------------------



// -----   Time of last raw data   -------------------------------------------
Double_t CbmDaqBuffer::GetLastTime() const {
  Double_t time = -1.;
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++)
    time = ( time > GetLastTime(iDet) ? time : GetLastTime(iDet) );
  return time;
}
// ---------------------------------------------------------------------------



// -----   Access to next data   ---------------------------------------------
CbmDigi* CbmDaqBuffer::GetNextData(Int_t iDet, Double_t time) {


  // --- Check for system ID
  if ( iDet >= kNOFDETS ) {
    LOG(WARNING) << "DaqBuffer: Illegal system ID " << iDet
                 << FairLogger::endl;
    return NULL;
  }

  // --- Check for empty buffer
  if ( ! fData[iDet].size() ) return NULL;

  // --- Get data from buffer
  CbmDigi* digi = NULL;
  multimap<Double_t, CbmDigi*>::iterator it = fData[iDet].begin();
  CbmDigi* test = it->second;
  if ( test->GetTime() < time ) {
    digi = test;
    fData[iDet].erase(it);
  }
  return digi;

}
// ---------------------------------------------------------------------------



// -----   Number of objects in buffer   -------------------------------------
Int_t CbmDaqBuffer::GetSize() const {

  Int_t size = 0;
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++)
    size += fData[iDet].size();
  return size;

}
// ---------------------------------------------------------------------------



// -----   Number of objects in buffer for given detector   ------------------
Int_t CbmDaqBuffer::GetSize(Int_t det) const {
  if ( det < kREF || det > kNOFDETS) return 0;
  return fData[det].size();
}
// ---------------------------------------------------------------------------



// -----   Insert data into buffer   -----------------------------------------
void CbmDaqBuffer::InsertData(CbmDigi* digi) {

  if ( ! digi ) LOG(FATAL) << "DaqBuffer: invalid digi pointer"
                           << FairLogger::endl;

  Int_t iDet = digi->GetSystemId();
  if ( iDet >= kNOFDETS) {
    LOG(WARNING) << "DaqBuffer: Illegal system ID " << iDet
                 << FairLogger::endl;
    return;
  }

  pair<Double_t, CbmDigi*> value (digi->GetTime(), digi);
  fData[iDet].insert(value);

  LOG(DEBUG2) << "DaqBuffer: Inserting digi, detectorID "
              << digi->GetAddress() << ", time " << digi->GetTime()
              << FairLogger::endl;

}
// ---------------------------------------------------------------------------



// -----   Instance   --------------------------------------------------------
CbmDaqBuffer* CbmDaqBuffer::Instance() {
  if ( ! fgInstance ) fgInstance = new CbmDaqBuffer();
  return fgInstance;
}
// ---------------------------------------------------------------------------



// -----   Print status   ----------------------------------------------------
void CbmDaqBuffer::PrintStatus() const {
  TString sysName;
  Int_t size = GetSize();
  LOG(INFO) << "DaqBuffer: Status ";
  if ( ! size ) {
    LOG(INFO) << "empty" << FairLogger::endl;
    return;
  }
  for (Int_t det = kREF; det < kNOFDETS; det++) {
    if ( GetSize(det) ) {
      CbmDetectorList::GetSystemNameCaps(det, sysName);
      LOG(INFO) << sysName << " " << GetSize(det) << "  ";
    }
  }
  LOG(INFO) << FairLogger::endl;
  LOG(INFO) << "\t     " << "Total: " << GetSize() << " from "
            << fixed << setprecision(3) << GetFirstTime() << " ns to "
            << GetLastTime() << " ns" << FairLogger::endl;
}
// ---------------------------------------------------------------------------

