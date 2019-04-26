// -------------------------------------------------------------------------
// -----                   BmnBdHitProducer source file                -----
// -----                  Created 12/07/18  by A. Zinchenko            -----
// -------------------------------------------------------------------------

#include "BmnBdHit.h"
#include "BmnBdHitProducer.h"
#include "BmnBdPoint.h"

#include "FairRootManager.h"

#include "TClonesArray.h"

#include <iostream>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
BmnBdHitProducer::BmnBdHitProducer() 
  : FairTask("Bmn Bd Hit Producer"),
    fPointArray(NULL),
    fHitArray()
{ 
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnBdHitProducer::~BmnBdHitProducer() { }
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus BmnBdHitProducer::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- BmnBdHitProducer::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("BdPoint");
  if ( ! fPointArray ) {
    cout << "-W- BmnBdHitProducer::Init: "
	 << "No BdPoint array!" << endl;
    return kERROR;
  }

  // Create and register output array
  fHitArray = new TClonesArray("BmnBdHit");
  ioman->Register("BdHit", "BD", fHitArray, kTRUE);

  cout << "-I- BmnBdHitProducer: Intialization successfull" << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void BmnBdHitProducer::Exec(Option_t* opt) {

  // Reset output array
  if ( ! fHitArray ) Fatal("Exec", "No BdHit array");

  fHitArray->Delete();
  fHitMap.clear();

  // Declare some variables
  BmnBdPoint* point = NULL;
  Int_t detID   = 0;        // Detector ID
  Int_t trackID = 0;        // Track index
  Double_t x, y, z;         // Position
  Double_t dx = 0.0001;     // Position error
  TVector3 pos, dpos;       // Position and error vectors

  // Loop over BdPoints
  Int_t nPoints = fPointArray->GetEntriesFast();

  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point = (BmnBdPoint*) fPointArray->UncheckedAt(iPoint);
    if ( ! point) continue;

    // Detector ID
    detID = point->GetCopy();

    // MCTrack ID
    trackID = point->GetTrackID();

    // Hit position 
    x  = point->GetX();
    y  = point->GetY();
    z  = point->GetZ();

    // Create new hit
    pos.SetXYZ(x,y,z);
    dpos.SetXYZ(dx, dx, 0.);
    //new ((*fHitArray)[iPoint]) BmnBdHit(detID, pos, dpos, iPoint);
    if (fHitMap.find(detID) == fHitMap.end()) fHitMap[detID] = BmnBdHit(detID, pos, dpos, iPoint);
    // Add point to the hit links
    fHitMap[detID].AddLink(FairLink(0,iPoint));

  }   // Loop over MCPoints

  // Store hits
  Int_t ihit = 0;
  for (map<Int_t,BmnBdHit>::iterator mit = fHitMap.begin(); mit != fHitMap.end(); ++mit) 
    new ((*fHitArray)[ihit++]) BmnBdHit(mit->second);

  // Event summary
  cout << "-I- BmnBdHitProducer: " << nPoints << " BdPoints, "
       << fHitMap.size() << " Hits created." << endl;

}
// -------------------------------------------------------------------------

ClassImp(BmnBdHitProducer)
