// -------------------------------------------------------------------------
// -----                 CbmStsStationDigiPar source file              -----
// -----                  Created 28/06/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include "CbmStsStationDigiPar.h"

#include "TMath.h"

// -----   Default constructor   -------------------------------------------
CbmStsStationDigiPar::CbmStsStationDigiPar() 
:  fStationNr(-1),
   fRotation(0.),
   fSectors()
{
  fSectors = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsStationDigiPar::CbmStsStationDigiPar(Int_t stationNr, 
					   Double_t rotation) 
:  fStationNr(-1),
   fRotation(0.),
   fSectors()
{
  fStationNr = stationNr;
  fRotation  = rotation * TMath::Pi() / 180.;
  fSectors   = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsStationDigiPar::~CbmStsStationDigiPar() {
  if ( fSectors) {
    fSectors->Delete();
    delete fSectors;
  }
}
// -------------------------------------------------------------------------




ClassImp(CbmStsStationDigiPar)
