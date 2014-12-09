// -------------------------------------------------------------------------
// -----                 CbmStsSectorDigiPar source file              -----
// -----                  Created 28/06/05  by V. Friese               -----
// -------------------------------------------------------------------------


#include "CbmStsSectorDigiPar.h"

#include "TMath.h"

// -----   Default constructor   -------------------------------------------
CbmStsSectorDigiPar::CbmStsSectorDigiPar() 
:  fSectorNr(-1),
   fSensors()
{
  fSensors = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsSectorDigiPar::CbmStsSectorDigiPar(Int_t sectorNr) 
:  fSectorNr(),
   fSensors()
{
  fSectorNr = sectorNr;
  fSensors   = new TObjArray();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSectorDigiPar::~CbmStsSectorDigiPar() {
  if ( fSensors) {
    fSensors->Delete();
    delete fSensors;
  }
}
// -------------------------------------------------------------------------




ClassImp(CbmStsSectorDigiPar)
