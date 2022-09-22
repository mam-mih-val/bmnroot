/** @file BmnDaqBuffer.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @author D. Baranov
 ** @date 19.12.2018
 **/


#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>

#include "TString.h"

#include "FairLogger.h"

#include "BmnDaqBuffer.h"
#include "BmnModuleList.h"

using std::setprecision;
using std::stringstream;
using std::fixed;
using std::string;
using std::pair;
using std::multimap;

// -----   Initialisation of static variables   ------------------------------
BmnDaqBuffer* BmnDaqBuffer::fgInstance = NULL;
// ---------------------------------------------------------------------------



// -----   Constructor   -----------------------------------------------------
BmnDaqBuffer::BmnDaqBuffer() {
}
// ---------------------------------------------------------------------------


// -----   Destructor   ------------------------------------------------------
BmnDaqBuffer::~BmnDaqBuffer() {
}
// ---------------------------------------------------------------------------



// -----   Time of first raw data   ------------------------------------------
Double_t BmnDaqBuffer::GetFirstTime() const {
  Double_t time = -1.;
  Bool_t firstDetector = kTRUE;
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++) {
    if ( GetSize(iDet) ) {
      if ( firstDetector ) {
        time = GetFirstTime(iDet);
        firstDetector = kFALSE;
      } //? first detector with data
      else time = ( time < GetFirstTime(iDet) ? time : GetFirstTime(iDet) );
    }  //? detector has data
  } //# detectors
  return time;
}
// ---------------------------------------------------------------------------



// -----   Time of first raw data for detector  ------------------------------
Double_t BmnDaqBuffer::GetFirstTime(Int_t iDet) const {
  if ( iDet < kREF || iDet >= kNOFDETS ) return -1.;
  if ( ! GetSize(iDet) ) return -1.;
  assert ( (fData[iDet].begin())->second );
  return (fData[iDet].begin())->second->GetTime();
}
// ---------------------------------------------------------------------------



// -----   Time of last raw data   -------------------------------------------
Double_t BmnDaqBuffer::GetLastTime() const {
  Double_t time = -1.;
  Bool_t firstDetector = kTRUE;
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++) {
    if ( GetSize(iDet) ) {
      if ( firstDetector ) {
        time = GetLastTime(iDet);
        firstDetector = kFALSE;
      } //? first detector
      else time = ( time > GetLastTime(iDet) ? time : GetLastTime(iDet) );
    } //? detector has data
  } //# detectors
  return time;
}
// ---------------------------------------------------------------------------



// -----   Time of last raw data for detector  -------------------------------
Double_t BmnDaqBuffer::GetLastTime(Int_t iDet) const {
  if ( iDet < kREF || iDet >= kNOFDETS ) return -1.;
  if ( ! GetSize(iDet) ) return -1.;
  assert ( (--fData[iDet].end())->second );
  return (--fData[iDet].end())->second->GetTime();
}
// ---------------------------------------------------------------------------



// -----   Access to next data   ---------------------------------------------
BmnDigi* BmnDaqBuffer::GetNextData(Int_t iDet) {

  // --- Check for system ID
  if ( iDet >= kNOFDETS ) {
    LOG(WARNING) << "DaqBuffer: Illegal system ID " << iDet;
    return NULL;
  }

  // --- Check for empty buffer
  if ( ! fData[iDet].size() ) return NULL;

  // --- Get data from buffer
  BmnDigi* digi = NULL;
  multimap<Double_t, BmnDigi*>::iterator it = fData[iDet].begin();
  BmnDigi* test = it->second;
  digi = test;
  fData[iDet].erase(it);

  return digi;
}
// ---------------------------------------------------------------------------



// -----   Access to next data with time limit  ------------------------------
BmnDigi* BmnDaqBuffer::GetNextData(Int_t iDet, Double_t time) {


  // --- Check for system ID
  if ( iDet >= kNOFDETS ) {
    LOG(WARNING) << "DaqBuffer: Illegal system ID " << iDet;
    return NULL;
  }

  // --- Check for empty buffer
  if ( ! fData[iDet].size() ) return NULL;

  // --- Get data from buffer
  BmnDigi* digi = NULL;
  multimap<Double_t, BmnDigi*>::iterator it = fData[iDet].begin();
  BmnDigi* test = it->second;
  if ( test->GetTime() < time ) {
    digi = test;
    fData[iDet].erase(it);
  }
  return digi;

}
// ---------------------------------------------------------------------------



// -----   Number of objects in buffer   -------------------------------------
Int_t BmnDaqBuffer::GetSize() const {

  Int_t size = 0;
  for (Int_t iDet = kREF; iDet < kNOFDETS; iDet++)
    size += fData[iDet].size();
  return size;

}
// ---------------------------------------------------------------------------



// -----   Number of objects in buffer for given detector   ------------------
Int_t BmnDaqBuffer::GetSize(Int_t det) const {
  if ( det < kREF || det > kNOFDETS) return 0;
  return fData[det].size();
}
// ---------------------------------------------------------------------------



// -----   Insert data into buffer   -----------------------------------------
void BmnDaqBuffer::InsertData(BmnDigi* digi) {

  if ( ! digi ) LOG(fatal) << "DaqBuffer: invalid digi pointer";

  Int_t iDet = digi->GetSystemId();
  if ( iDet >= kNOFDETS) {
    LOG(WARNING) << "DaqBuffer: Illegal system ID " << iDet;
    return;
  }

  pair<Double_t, BmnDigi*> value (digi->GetTime(), digi);
  fData[iDet].insert(value);

  LOG(debug2) << "DaqBuffer: Inserting digi, detectorID "
              << digi->GetAddress() << ", time " << digi->GetTime();

}
// ---------------------------------------------------------------------------



// -----   Instance   --------------------------------------------------------
BmnDaqBuffer* BmnDaqBuffer::Instance() {
  if ( ! fgInstance ) fgInstance = new BmnDaqBuffer();
  return fgInstance;
}
// ---------------------------------------------------------------------------



// -----   Print status   ----------------------------------------------------
void BmnDaqBuffer::PrintStatus() const {
  TString sysName;
  Int_t size = GetSize();
  LOG(info) << "DaqBuffer: Status ";
  if ( ! size ) {
    LOG(info) << "empty";
    return;
  }
  for (Int_t det = kREF; det < kNOFDETS; det++) {
    if ( GetSize(det) ) {
      sysName = BmnModuleList::GetModuleNameCaps(det);
      LOG(info) << sysName << " " << GetSize(det) << "  ";
    }
  }
  LOG(info) << "\t     " << "Total: " << GetSize() << " from "
            << fixed << setprecision(3) << GetFirstTime() << " ns to "
            << GetLastTime() << " ns";
}
// ---------------------------------------------------------------------------



// -----   Status to string   ------------------------------------------------
string BmnDaqBuffer::ToString() const {
  stringstream ss;
  ss << "DaqBuffer: ";
  Int_t size = GetSize();
  if ( ! size ) {
    ss << "empty";
    return ss.str();
  }
  TString sysName;
  for (Int_t det = kREF; det < kNOFDETS; det++) {
    if ( GetSize(det) ) {
      sysName = BmnModuleList::GetModuleNameCaps(det);
      ss << sysName << " " << GetSize(det) << "  ";
    }
  }
  ss << "Total: " << size << " from " << fixed << setprecision(3)
    << GetFirstTime() << " ns to " << GetLastTime() << " ns";

  return ss.str();
}
// ---------------------------------------------------------------------------

