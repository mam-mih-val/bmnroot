/** @file CbmMCBuffer.cxx
 ** @author Volker Friese <v.friese@gsi.de>
 ** @date 8 February 2012
 **/

#include <iomanip>

#include "TMath.h"

#include "FairLogger.h"

#include "CbmMCBuffer.h"

using namespace std;

CbmMCBuffer* CbmMCBuffer::fgInstance = 0;


// -----   Default constructor   ---------------------------------------------
CbmMCBuffer::CbmMCBuffer() 
  : fMvdBuffer("MVD"),
    fGemBuffer("GEM"),
    fTofBuffer("TOF"),
    fZdcBuffer("ZDC"),
    fTime(0.),
    fEventId(0),
    fEndOfRun(kFALSE)
{ 
}
// ---------------------------------------------------------------------------


// -----   Destructor   ------------------------------------------------------
CbmMCBuffer::~CbmMCBuffer() { }
// ---------------------------------------------------------------------------


// -----   Finish   ----------------------------------------------------------
void CbmMCBuffer::Clear() {
  fMvdBuffer.Clear();
  fGemBuffer.Clear();
  fTofBuffer.Clear();
  fZdcBuffer.Clear();
}
// ---------------------------------------------------------------------------
    

// -----   Fill buffer   -----------------------------------------------------
Int_t CbmMCBuffer::Fill(TClonesArray* points, DetectorId det,
                        Int_t eventId, Double_t eventTime) {
  Int_t iDet = det;
  return Fill(points, iDet, eventId, eventTime);
}
// ---------------------------------------------------------------------------


// -----   Fill buffer   -----------------------------------------------------
Int_t CbmMCBuffer::Fill(TClonesArray* points, Int_t det,
                        Int_t eventId, Double_t eventTime) {

  fTime    = eventTime;
  fEventId = eventId;

  Int_t nPoints = 0;
  if ( points ) {
    switch (det) {
    case kMVD:  nPoints = fMvdBuffer.Fill(points, eventTime, eventId);  break;
    case kGEM:  nPoints = fGemBuffer.Fill(points, eventTime, eventId);  break;
    case kTOF:  nPoints = fTofBuffer.Fill(points, eventTime, eventId);  break;
    case kZDC:  nPoints = fZdcBuffer.Fill(points, eventTime, eventId);  break;
    default:    nPoints = 0; break;
    }
  }

  return nPoints;
}
// ---------------------------------------------------------------------------


// -----   Get time of last data   ------------------------------------------
Double_t CbmMCBuffer::GetMaxTime() const {
  Double_t tMax = fMvdBuffer.GetMaxTime();
  tMax = TMath::Max( tMax, fGemBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fTofBuffer.GetMaxTime() );
  tMax = TMath::Max( tMax, fZdcBuffer.GetMaxTime() );

  return tMax;
}
// ---------------------------------------------------------------------------


// -----   Get time of first data   ------------------------------------------
Double_t CbmMCBuffer::GetMinTime() const {
  Double_t tMin = GetMaxTime();
  if ( fMvdBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fMvdBuffer.GetMinTime() );
  if ( fGemBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fGemBuffer.GetMinTime() );
  if ( fTofBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fTofBuffer.GetMinTime() );
  if ( fZdcBuffer.GetMinTime() > 0.)
    tMin = TMath::Min( tMin, fZdcBuffer.GetMinTime() );

  return tMin;
}
// ---------------------------------------------------------------------------


// -----   Get next point   --------------------------------------------------
const FairMCPoint* CbmMCBuffer::GetNextPoint(DetectorId det) {

  const FairMCPoint* nextPoint = NULL;

  if ( ! fEndOfRun ) {
    switch (det) {
      case kMVD:  nextPoint = fMvdBuffer.GetNextPoint(fTime);  break;
      case kGEM:  nextPoint = fGemBuffer.GetNextPoint(fTime);  break;
      case kTOF:  nextPoint = fTofBuffer.GetNextPoint(fTime);  break;
      case kZDC:  nextPoint = fZdcBuffer.GetNextPoint(fTime);  break;
      default:    nextPoint = NULL; break;
    }
  }
  else {
    switch (det) {
      case kMVD:  nextPoint = fMvdBuffer.GetNextPoint();  break;
      case kGEM:  nextPoint = fGemBuffer.GetNextPoint();  break;
      case kTOF:  nextPoint = fTofBuffer.GetNextPoint();  break;
      case kZDC:  nextPoint = fZdcBuffer.GetNextPoint();  break;
      default:    nextPoint = NULL; break;
    }
  }

  return nextPoint;
}
// ---------------------------------------------------------------------------


// -----   Get number of points   ---------------------------------------------
Int_t CbmMCBuffer::GetNofEntries() const
{
  Int_t nEntries = fMvdBuffer.GetNofEntries();
  nEntries += fGemBuffer.GetNofEntries();
  nEntries += fTofBuffer.GetNofEntries();
  nEntries += fZdcBuffer.GetNofEntries();

  return nEntries;
}
// ---------------------------------------------------------------------------


// -----   Get buffer size   -------------------------------------------------
Double_t CbmMCBuffer::GetSize() const
{
  Double_t size = 0.;
  size += fMvdBuffer.GetSize();
  size += fGemBuffer.GetSize();
  size += fTofBuffer.GetSize();
  size += fZdcBuffer.GetSize();

  return size;
}
// ---------------------------------------------------------------------------


// -----   Instance   --------------------------------------------------------
CbmMCBuffer* CbmMCBuffer::Instance()
{
  if ( ! fgInstance ) fgInstance = new CbmMCBuffer();
  return fgInstance;
}
// ---------------------------------------------------------------------------
    

// -----   Print   -----------------------------------------------------------
void CbmMCBuffer::Print(const char* option) const {

  LOG(INFO) << "MCBuffer: Last event " << fEventId << " at "
            << fixed << setprecision(3) << fTime << " ns, "
            << GetNofEntries() << " points from " << GetMinTime()
            << " ns to " << GetMaxTime() << " ns, size " << GetSize()
            << " MB" << FairLogger::endl;

  if ( fMvdBuffer.GetSize() )  fMvdBuffer.Print();
  if ( fGemBuffer.GetSize() )  fGemBuffer.Print();
  if ( fTofBuffer.GetSize() )  fTofBuffer.Print();
  if ( fZdcBuffer.GetSize() )  fZdcBuffer.Print();
}
// ---------------------------------------------------------------------------
