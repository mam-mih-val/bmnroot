// -------------------------------------------------------------------------
// -----                     CbmStsStation source file                 -----
// -----                  Created 31/03/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsStation.h"

#include "CbmStsSector.h"
#include "CbmStsSectorDigiPar.h"

#include "CbmStsSensor.h"

#include "TMath.h"

#include <iostream>
#include <map>

using std::cout;
using std::endl;
using std::map;


// -----   Default constructor   -------------------------------------------
CbmStsStation::CbmStsStation() 
: fDetectorId(0),
  fZ(0.),
  fD(0.),
  fRmin(0.),
  fRmax(0.),
  fRotation(0.),
  fSectors(),
  fName(),
  fRadLength(),
  fSectorMap(),
  fMcIdMap()
{
  fSectors = new TObjArray(100);
  fSectorMap.clear();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsStation::CbmStsStation(const char* name, Int_t iStation, Double_t z, 
			     Double_t d, Double_t rl, Double_t rmin, 
			     Double_t rmax, Double_t rotation) 
  : TNamed(name, "STS station"),
    fDetectorId(0),
    fZ(0.),
    fD(0.),
    fRmin(0.),
    fRmax(0.),
    fRotation(0.),
    fSectors(),
    fName(),
    fRadLength(),
    fSectorMap(),
    fMcIdMap()  
{
  fDetectorId = 2 << 24 | iStation << 16;
  fZ          = z;
  fD          = d;
  fRadLength  = rl;
  fRmin       = rmin;
  fRmax       = rmax;
  fRotation   = rotation;
  fSectors = new TObjArray(100);
  fSectorMap.clear();

  for ( Int_t isz = 0 ; isz < 10 ; isz++ )
    fSensorZ[isz] = -666.;
  fSensorZ[0] = z;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsStation::~CbmStsStation() {
  if ( fSectors ) {
    fSectors->Delete();
    delete fSectors;
  }
}
// -------------------------------------------------------------------------



// -----   Public method GetNChannels   ------------------------------------
Int_t CbmStsStation::GetNChannels() {
  Int_t nChan = 0;
  for (Int_t iSect=0; iSect<GetNSectors(); iSect++) 
    if ( GetSector(iSect)->GetNChannels() > 0 )
      nChan += GetSector(iSect)->GetNChannels();
  return nChan;
}
// -------------------------------------------------------------------------




// -----   Public method GetSectorByNr   -----------------------------------
CbmStsSector* CbmStsStation::GetSectorByNr(Int_t sectorNr) {
  if ( fSectorMap.find(sectorNr) != fSectorMap.end() ) {
    Int_t index = fSectorMap[sectorNr];
    return (CbmStsSector*) fSectors->At(index);
  }
  else {
    cout << "-W- CbmStsStation::GetSectorByNr: sector " << sectorNr
	 << " not found (station " << GetStationNr() << ")." << endl;
    return NULL;
  }
}
// -------------------------------------------------------------------------



// -----   Public method AddSector   ---------------------------------------
void CbmStsStation::AddSector(CbmStsSectorDigiPar* sectorPar) {

  // Get digitisation parameters
  Int_t iSector   = sectorPar->GetSectorNr();

  // Check for previous existence of sector
  if ( fSectorMap.find(iSector) != fSectorMap.end() ) {
    cout << "-W- " << fName << "::AddSector: Sector number " << iSector
	 << " already existing for station " << GetStationNr() << endl;
    return;
  }

  // Calculate detectorId and rotation in global c.s.
  Int_t    detId  = 2 << 24 | (GetStationNr() << 16) | (iSector << 4); 

  // Number of existing sectors
  Int_t nSectors = fSectors->GetEntries();

  // Create and add new sector
  fSectors->Add(new CbmStsSector(Form("%ssect%d",fName.Data(),iSector),detId));

  fSectorMap[iSector] = nSectors;
}
// -------------------------------------------------------------------------

// -----   Public method AddSector   ---------------------------------------
void CbmStsStation::AddSector(CbmStsSector* sector) {

  Int_t iSector  = sector->GetSectorNr();
  Int_t nSectors = fSectors->GetEntries();

  fSectors->Add(sector);

  fSectorMap[iSector] = nSectors;

}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Double_t CbmStsStation::GetZ(Int_t it)
{
  if ( fSensorZ[it] < -665. ) {
    Int_t knownZPos = 0;
    for ( knownZPos = 0 ; knownZPos < 10 ; knownZPos++ ) {
      if ( fSensorZ[knownZPos] < -665. ) break;
    }

    for (Int_t iSect=GetNSectors(); iSect > 0 ; iSect--) {
      CbmStsSector* sector = (CbmStsSector*)GetSector(iSect-1);
      for (Int_t iSens=sector->GetNSensors() ; iSens > 0 ; iSens--) {
	CbmStsSensor* sensor = (CbmStsSensor*)sector->GetSensor(iSens-1);
	Bool_t knownAlready = kFALSE;
	for ( Int_t isz = 0 ; isz < knownZPos ; isz++ )
	  if ( TMath::Abs(sensor->GetZ0()-fSensorZ[isz]) < 0.0001 ) {
	    knownAlready = kTRUE;
	    break;
	  }

	if ( knownAlready ) continue;

	fSensorZ[knownZPos] = sensor->GetZ0();
	knownZPos++;
      }
    }
  }
  return fSensorZ[it];
}
// -------------------------------------------------------------------------

// -------------------------------------------------------------------------
Int_t CbmStsStation::GetNofZ()
{
  Int_t knownZPos = 0;
  for ( knownZPos = 0 ; knownZPos < 10 ; knownZPos++ ) {
    if ( fSensorZ[knownZPos] < -665. ) break;
  }

  for (Int_t iSect=GetNSectors(); iSect > 0 ; iSect--) {
    CbmStsSector* sector = (CbmStsSector*)GetSector(iSect-1);
    for (Int_t iSens=sector->GetNSensors() ; iSens > 0 ; iSens--) {
      CbmStsSensor* sensor = (CbmStsSensor*)sector->GetSensor(iSens-1);
      Bool_t knownAlready = kFALSE;
      for ( Int_t isz = 0 ; isz < knownZPos ; isz++ )
	if ( TMath::Abs(sensor->GetZ0()-fSensorZ[isz]) < 0.0001 ) {
	  knownAlready = kTRUE;
	  break;
	}
      
      if ( knownAlready ) continue;
      
      fSensorZ[knownZPos] = sensor->GetZ0();
      knownZPos++;
    }
  }
  return knownZPos;
}
// -------------------------------------------------------------------------


// -----   Public method Reset   -------------------------------------------
void CbmStsStation::Reset() {
  for (Int_t iSector=0; iSector<GetNSectors(); iSector++) 
    GetSector(iSector)->Reset();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmStsStation::Print(Bool_t kLong) {
  cout << "Station Nr. ";
  cout.width(2);
  cout << GetStationNr() << ", z =  ";
  cout.width(3);
  cout << fZ << " cm, Rmin = ";
  cout.width(5);
  cout << fRmin << " cm, Rmax = " << fRmax << " cm, angle = ";
  cout.width(3);
  cout << fRotation * 180 / TMath::Pi() << " deg., sectors: ";
  cout.width(4);
  cout << GetNSectors() << ", channels: ";
  cout.width(8);
  cout << GetNChannels() << endl;
  if ( kLong ) 
    for (Int_t iSec=0; iSec<GetNSectors(); iSec++) 
      GetSector(iSec)->Print();
}
// -------------------------------------------------------------------------



ClassImp(CbmStsStation)
