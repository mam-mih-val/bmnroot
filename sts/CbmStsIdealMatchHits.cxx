//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsIdealMatchHits source file                -----
// -----                  Created 27/11/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include <iostream>
#include <iomanip>

// --- Includes from ROOT
#include "TClonesArray.h"

// --- Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// --- Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsHit.h"
#include "CbmStsIdealMatchHits.h"
#include "CbmStsPoint.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include "TMath.h"

using std::cout;
using std::endl;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;


// -----   Default constructor   -------------------------------------------
CbmStsIdealMatchHits::CbmStsIdealMatchHits() 
  : FairTask("STSMatchHits", 1), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fHits(NULL),
  fDigiScheme(NULL),
  fTimer(),
  fNEvents(0),
  fNEventsFailed(0),
  fTime(0.),
  fNHits(0.),
  fNMatched(0.),
  fNDistant(0.),
  fNBackgrd(0.),
  fCandMap(),
  fIter()
{
  fDigiScheme  = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsIdealMatchHits::CbmStsIdealMatchHits(Int_t iVerbose)
  : FairTask("STSMatchHits", iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fHits(NULL),
  fDigiScheme(NULL),
  fTimer(),
  fNEvents(0),
  fNEventsFailed(0),
  fTime(0.),
  fNHits(0.),
  fNMatched(0.),
  fNDistant(0.),
  fNBackgrd(0.),
  fCandMap(),
  fIter()
{
  fDigiScheme  = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsIdealMatchHits::CbmStsIdealMatchHits(const char* name, Int_t iVerbose)
  : FairTask(name, iVerbose),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fHits(NULL),
  fDigiScheme(NULL),
  fTimer(),
  fNEvents(0),
  fNEventsFailed(0),
  fTime(0.),
  fNHits(0.),
  fNMatched(0.),
  fNDistant(0.),
  fNBackgrd(0.),
  fCandMap(),
  fIter()
{
  fDigiScheme  = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsIdealMatchHits::~CbmStsIdealMatchHits() {
  if ( fGeoPar )     delete fGeoPar;
  if ( fDigiPar )    delete fDigiPar;
  if ( fDigiScheme ) delete fDigiScheme;
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsIdealMatchHits::Exec(Option_t* opt) {

  // Timer
  fTimer.Start();
  Bool_t warn = kFALSE;

  // Counters
  Int_t nHits    = fHits->GetEntriesFast();
  Int_t nNoDigi  = 0;
  Int_t nBackgrd = 0;
  Int_t nDistant = 0;
  Int_t nMatched = 0;

  // Loop over all StsHits
  for (Int_t iHit=0; iHit<nHits; iHit++) {
    CbmStsHit* hit = (CbmStsHit*) fHits->At(iHit);
    if ( ! hit ) {
      cout << "-W- " << GetName() << "::Exec: Empty hit at index "
	   << iHit << endl;
      warn = kTRUE;
      continue;
    }

    // Determine sector type and channel numbers
    Int_t iStation = hit->GetStationNr();
    Int_t iSector  = hit->GetSectorNr();
    CbmStsStation* station = fDigiScheme->GetStationByNr(iStation);
    CbmStsSector*  sector = fDigiScheme->GetSector(iStation, iSector);
    Int_t iType = sector->GetType();
    CbmStsDigiMatch* dMatchF = NULL;
    CbmStsDigiMatch* dMatchB = NULL;
    

    // Hit coordinates and errors
    Double_t xH = hit->GetX();
    Double_t yH = hit->GetY();
    Double_t dX = hit->GetDx();
    Double_t dY = hit->GetDy();

    // Get front side DigiMatch corresponding to hit
    Int_t  iMatchF  = hit->GetDigi(0);
    if ( iMatchF >= 0 ) 
      dMatchF = (CbmStsDigiMatch*) fDigiMatches->At(iMatchF);
    if ( ! dMatchF ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No DigiMatchF for hit " << iHit << endl;
      hit->SetRefIndex(-1);
      nNoDigi++;
      warn = kTRUE;
      continue;
    }

    // Get back side DigiMatch of hit (for strip sensors only)
    if ( iType != 1 ) {
      Int_t  iMatchB  = hit->GetDigi(1);    
      if ( iMatchB >= 0 ) 
	dMatchB = (CbmStsDigiMatch*) fDigiMatches->At(iMatchB);
      if ( ! dMatchB ) {
	cout << "-E- " << GetName() << "::Exec: "
	     << "No DigiMatchB for hit " << iHit << endl;
	hit->SetRefIndex(-1);
	nNoDigi++;
	warn = kTRUE;
	continue;
      }
    }

    // Map candidate points to distance to hit
    fCandMap.clear();
    Int_t nPointsF = 0;
    Int_t nPointsB = 0;

    if ( iType == 1 ) {
      // Case pixel: Candidates are all points of the digi.
      for (Int_t jMatchF=0; jMatchF<3; jMatchF++) {
	Int_t iPointF = dMatchF->GetRefIndex(jMatchF);
	if ( iPointF < 0 ) continue;
	nPointsF++;
	// Calculate distance to hit
	CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPointF);
	if ( ! point ) {
	  cout << "-E- " << GetName() << "::Exec: "
	       << "No StsPoint (" << iPointF << ") for pixel hit " 
	       << iHit << endl;
	  warn = kTRUE;
	  continue;
	}
	// point coordinates in station midplane
	Double_t xP = point->GetX(station->GetZ());
	Double_t yP = point->GetY(station->GetZ());
	Double_t dist = TMath::Sqrt( (xP-xH)*(xP-xH) + (yP-yH)*(yP-yH) );
	fCandMap[dist] = iPointF;
      }     // front digi loop
    }       // pixel sensor

    else if ( iType == 2 || iType == 3 ) {
      // Case strip: Candidates are points corresponding to front
      // and back side digis
      for ( Int_t jMatchF=0; jMatchF<3; jMatchF++) {
	Int_t iPointF = dMatchF->GetRefIndex(jMatchF);
	if ( iPointF < 0 ) continue; 
	nPointsF++;
	for ( Int_t jMatchB=0; jMatchB<3; jMatchB++) {
	  Int_t iPointB = dMatchB->GetRefIndex(jMatchB);
	  if ( iPointB < 0 ) continue; 
	  if ( jMatchF == 0 ) nPointsB++;
	  if ( iPointB != iPointF ) continue; // chance combination
	  // Calculate distance to hit
	  CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPointF);
	  if ( ! point ) {
	    cout << "-E- " << GetName() << "::Exec: "
		 << "No StsPoint (" << iPointF << ") for strip hit " 
		 << iHit << endl;
	    warn = kTRUE;
	    continue;
	  }
	  // point coordinates in station midplane
	  Double_t xP = point->GetX(station->GetZ());
	  Double_t yP = point->GetY(station->GetZ());
	  Double_t dist = TMath::Sqrt( (xP-xH)*(xP-xH) + 
				       (yP-yH)*(yP-yH) );
	  fCandMap[dist] = iPointF;
	}   // back digi loop
      }     // front digi loop
    }       // strip sensor

    else {
      // Unknown sensor type
      cout << "-E- " << GetName() << "::Exec: Unknwon sensor type "
	   << iType << endl;
      Fatal("Exec", "Unknwon sensor type");
    }

    if ( fVerbose > 1 ) cout << "-I- " << GetName() << ": Hit " 
			     << iHit << ", type " << iType 
			     << ", points " << nPointsF << " / " 
			     << nPointsB << ", candidates " 
			     << fCandMap.size();


    // Check for at least one candidate point. Else background hit.
    // This can happen for noise digis or for fake combinations
    // of strip digis.
    if ( fCandMap.empty() ) {
      hit->SetRefIndex(-1);
      if (fVerbose>1) cout << ", background " << endl;
      nBackgrd++;
      continue;
    }
      
    // Select closest point from candidates.
    Double_t distMin = 999999.;
    Int_t    iPoint  = -1;
    for (fIter=fCandMap.begin(); fIter!=fCandMap.end(); fIter++) {
      if ( (*fIter).first < distMin ) {
	distMin = (*fIter).first;
	iPoint  = (*fIter).second;
      }
    }
    if ( iPoint < 0 ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No closest point found in candidate map!" << endl;
      Fatal("Exec", "No closest point");
    }
    CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);
    if (fVerbose>1) cout << ", matched to " << iPoint << ", distance " 
			 << distMin << " cm";
 	    
    // Check whether closest point is more than 5 sigma away from hit.
    // This should not happen in case of pixel or strip OSU sensors,
    // but can happen for strip GSI sensors due to multiple
    // crossings of front and back side strips.
    Double_t xP = point->GetX(station->GetZ());
    Double_t yP = point->GetY(station->GetZ());
    if ( TMath::Abs(xP-xH) > 5. * dX || 
	 TMath::Abs(yP-yH) > 5. * dY ) {
      hit->SetRefIndex(-2);
      nDistant++;
      if (fVerbose>1) cout << ", distant" << endl;
      if ( iType == 1 || iType == 2) {
	cout << "-W- " << fName << "::Exec: "
	     << "Distant hit in pixel / strip MSU station" << endl;
	cout << "Hit " << iHit << " coordinates " << xH
	     << ", " << yH << endl;
	cout << "Matched point " << iPoint << ", distance "
	     << distMin << endl;
	cout << "Distance y " << TMath::Abs(xP-xH) << " Error " << dX << endl;
	cout << "Distance y " << TMath::Abs(yP-yH) << " Error " << dY << endl;
      }

      
      continue;
    }

    // Match closest StsPoint to hit
    hit->SetRefIndex(iPoint);
    nMatched++;
    if (fVerbose>1) cout << ", good match" << endl;

    // Check whether hit and point are in the same station.
    // Else something has really gone wrong.
    if ( TMath::Abs(hit->GetZ() - point->GetZ()) > 1. ) {
      cout << "-E- " << GetName() << "::Exec: Hit " << iHit
	   << " is at z = " << hit->GetZ() << " cm, but matched point "
	   << iPoint << " is at z = " << point->GetZ() << "cm " << endl;
      Fatal("Exec", "Different stations for hit and point");
    }

  }   // Loop over StsHits

  // Event statistics
  fTimer.Stop();
  if ( fVerbose ) {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) 
	 << fixed << right << fTimer.RealTime()
	 << " s, hits " << nHits << ", matched " << nMatched << ", distant " 
	 << nDistant << ", background " << setw(6) << nBackgrd << endl;
  }

  // Run statistics
  if ( warn) fNEventsFailed++;
  else {
    fNEvents++;
    fTime     += fTimer.RealTime();
    fNHits    += Double_t(nHits);
    fNMatched += Double_t(nMatched);
    fNDistant += Double_t(nDistant);
    fNBackgrd += Double_t(nBackgrd);
  }

}
// -------------------------------------------------------------------------



// -----   Private method SetParContainers   -------------------------------
void CbmStsIdealMatchHits::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");
  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get STS geometry and digitisation parameter container
  fGeoPar  = (CbmGeoStsPar*)  db->getContainer("CbmGeoStsPar");
  fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");

}
// -------------------------------------------------------------------------



// -----   Private method Init   -------------------------------------------
InitStatus CbmStsIdealMatchHits::Init() {

  // Reset counters
  fNEvents = fNEventsFailed = 0;
  fTime = fNHits = fNMatched = fNDistant = fNBackgrd = 0.;

  // Get input arrays
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("StsPoint");
  if ( ! fPoints ) {
    cout << "-E- " << GetName() << "::Init: No STSPoint array!" << endl;
    return kERROR;
  }
  fDigis  = (TClonesArray*) ioman->GetObject("StsDigi");
  if ( ! fDigis ) {
    cout << "-E- " << GetName() << "::Init: No StsDigi array!" << endl;
    return kERROR;
  }
  fDigiMatches = (TClonesArray*) ioman->GetObject("StsDigiMatch");
  if ( ! fDigiMatches ) {
    cout << "-E- " << GetName() << "::Init: No StsDigiMatch array!" 
	 << endl;
    return kERROR;
  }
  fHits = (TClonesArray*) ioman->GetObject("StsHit");
  if ( ! fHits ) {
    cout << "-E- " << GetName() << "::Init: No StsHit array!" << endl;
    return kERROR;
  }

  // Build digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
    else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
    cout << "-I- " << fName << "::Init: "
	 << "STS digitisation scheme succesfully initialised" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations() 
	 << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
	 << fDigiScheme->GetNChannels() << endl;
    return kSUCCESS;
  }

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsIdealMatchHits::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) return kSUCCESS;

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Finish   -----------------------------------------
void CbmStsIdealMatchHits::Finish() {

  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "=====   " << GetName() << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Good events   : " << setw(6) << fNEvents << endl;
  cout << "===== Failed events : " << setw(6) << fNEventsFailed << endl;
  cout << "===== Average time  : " << setprecision(4) << setw(8) << right
       << fTime / Double_t(fNEvents)  << " s" << endl;
  cout << "===== " << endl;
  cout << "===== Hits per event  : " << fixed << setprecision(0)
       << fNHits / Double_t(fNEvents) << endl;
  cout << setprecision(2);
  cout << "===== Matched hits    : " << fixed << setw(6) << right
       << fNMatched / fNHits * 100. << " %" << endl;
  cout << "===== Distant hits    : " << fixed << setw(6) << right
       << fNDistant / fNHits * 100. << " %" << endl;
  cout << "===== Background hits : " << fixed << setw(6) << right
       << fNBackgrd / fNHits * 100. << " % " << endl;
  cout << "============================================================"
       << endl;

}
// -------------------------------------------------------------------------



ClassImp(CbmStsIdealMatchHits)

