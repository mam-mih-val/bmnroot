// -------------------------------------------------------------------------
// -----                   CbmStsTrackFinder source file               -----
// -----                  Created 02/02/05  by V. Friese               -----
// -------------------------------------------------------------------------


// Empty file, just there to please CINT

#include "CbmStsTrackFinder.h"

//#include "CbmStsDigiScheme.h"

//#include "FairField.h"

//#include "TClonesArray.h"

CbmStsTrackFinder::CbmStsTrackFinder()
  : TNamed(),
    fDigiScheme(NULL), 
    fField(NULL),     
    fMvdHits(NULL),  
    fStsHits(NULL),    
    fTracks(NULL), 
    fVerbose(0)
{
}

ClassImp(CbmStsTrackFinder)
