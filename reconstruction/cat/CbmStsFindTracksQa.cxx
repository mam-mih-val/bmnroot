// -------------------------------------------------------------------------
// -----                  CbmStsFindTracksQa source file               -----
// -----                  Created 11/01/06  by V. Friese               -----
// -------------------------------------------------------------------------

// Includes from sts
#include "CbmStsFindTracksQa.h"

#include "CbmStsHit.h"
#include "CbmStsPoint.h"
#include "CbmStsTrack.h"
#include "CbmTrackMatch.h"
#include "CbmGeoStsPar.h"

// Includes from base
#include "FairGeoNode.h"
#include "FairGeoPassivePar.h"
#include "FairGeoVector.h"
#include "CbmMCTrack.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFile.h"
#include "TH1F.h"
#include "TList.h"
#include "TVector3.h"

// Includes from C++
#include <iostream>
#include <iomanip>
using namespace std;

// -----   Default constructor   -------------------------------------------
CbmStsFindTracksQa::CbmStsFindTracksQa(Int_t iVerbose)
  : FairTask("STSFindTracksQA", iVerbose),
  fHitMap(),
  fMatchMap(),
  fQualiMap(),
  fMCTracks(NULL),
  fStsPoints(NULL),
  fStsHits(NULL),
  fStsTracks(NULL),
  fMatches(NULL),
  fPassGeo(NULL),
  fStsGeo(NULL),
  fTargetPos(0.,0.,0.),
  fNStations(0),
  fMinHits(4),
  fQuota(0.7),
  fhMomAccAll(new TH1F()),
  fhMomRecAll(new TH1F()),
  fhMomEffAll(new TH1F()),
  fhMomAccPrim(new TH1F()),
  fhMomRecPrim(new TH1F()),
  fhMomEffPrim(new TH1F()),
  fhMomAccSec(new TH1F()),
  fhMomRecSec(new TH1F()),
  fhMomEffSec(new TH1F()),
  fhNpAccAll(new TH1F()),
  fhNpRecAll(new TH1F()),
  fhNpEffAll(new TH1F()),
  fhNpAccPrim(new TH1F()),
  fhNpRecPrim(new TH1F()),
  fhNpEffPrim(new TH1F()),
  fhNpAccSec(new TH1F()),
  fhNpRecSec(new TH1F()),
  fhNpEffSec(new TH1F()),
  fhZAccSec(new TH1F()),
  fhZRecSec(new TH1F()),
  fhZEffSec(new TH1F()),
  fhNhClones(new TH1F()),
  fhNhGhosts(new TH1F()),
  fHistoList(new TList()),
  fNAccAll(0),
  fNAccPrim(0),
  fNAccRef(0),
  fNAccSec(0),
  fNRecAll(0),
  fNRecPrim(0),
  fNRecRef(0),
  fNRecSec(0),
  fNGhosts(0),
  fNClones(0),
  fNEvents(0),
  fNEventsFailed(0),
  fTime(0.),
  fTimer()
{}

// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsFindTracksQa::CbmStsFindTracksQa(Int_t minHits, Double_t quota,
				       Int_t iVerbose)
  : FairTask("STSFindTracksQA", iVerbose),
  fHitMap(),
  fMatchMap(),
  fQualiMap(),
  fMCTracks(NULL),
  fStsPoints(NULL),
  fStsHits(NULL),
  fStsTracks(NULL),
  fMatches(NULL),
  fPassGeo(NULL),
  fStsGeo(NULL),
  fTargetPos(0.,0.,0.),
  fNStations(0),
  fMinHits(minHits),
  fQuota(quota),
  fhMomAccAll(new TH1F()),
  fhMomRecAll(new TH1F()),
  fhMomEffAll(new TH1F()),
  fhMomAccPrim(new TH1F()),
  fhMomRecPrim(new TH1F()),
  fhMomEffPrim(new TH1F()),
  fhMomAccSec(new TH1F()),
  fhMomRecSec(new TH1F()),
  fhMomEffSec(new TH1F()),
  fhNpAccAll(new TH1F()),
  fhNpRecAll(new TH1F()),
  fhNpEffAll(new TH1F()),
  fhNpAccPrim(new TH1F()),
  fhNpRecPrim(new TH1F()),
  fhNpEffPrim(new TH1F()),
  fhNpAccSec(new TH1F()),
  fhNpRecSec(new TH1F()),
  fhNpEffSec(new TH1F()),
  fhZAccSec(new TH1F()),
  fhZRecSec(new TH1F()),
  fhZEffSec(new TH1F()),
  fhNhClones(new TH1F()),
  fhNhGhosts(new TH1F()),
  fHistoList(new TList()),
  fNAccAll(0),
  fNAccPrim(0),
  fNAccRef(0),
  fNAccSec(0),
  fNRecAll(0),
  fNRecPrim(0),
  fNRecRef(0),
  fNRecSec(0),
  fNGhosts(0),
  fNClones(0),
  fNEvents(0),
  fNEventsFailed(0),
  fTime(0.),
  fTimer()
{}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsFindTracksQa::~CbmStsFindTracksQa() {

  fHistoList->Delete();
  delete fHistoList;
}
// -------------------------------------------------------------------------



// -----   Public method SetParContainers   --------------------------------
void CbmStsFindTracksQa::SetParContainers() {

  // Get Run
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No FairRunAna!"
	 << endl;
    return;
  }

  // Get Runtime Database
  FairRuntimeDb* runDb = run->GetRuntimeDb();
  if ( ! run ) {
    cout << "-E- " << GetName() << "::SetParContainers: No runtime database!"
	 << endl;
    return;
  }

  // Get passive geometry parameters
  fPassGeo = (FairGeoPassivePar*) runDb->getContainer("FairGeoPassivePar");
  if ( ! fPassGeo ) {
    cout << "-E- " << GetName() << "::SetParContainers: "
	 << "No passive geometry parameters!" << endl;
    return;
  }

  // Get STS geometry parameters
  fStsGeo = (CbmGeoStsPar*) runDb->getContainer("CbmGeoStsPar");
  if ( ! fStsGeo ) {
    cout << "-E- " << GetName() << "::SetParContainers: "
	 << "No STS geometry parameters!" << endl;
    return;
  }

}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus CbmStsFindTracksQa::Init() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Initialising..." << endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if (! ioman) {
    cout << "-E- " << GetName() << "::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get MCTrack array
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack");
  if ( ! fMCTracks ) {
    cout << "-E- " << GetName() << "::Init: No MCTrack array!" << endl;
    return kFATAL;
  }

  // Get StsPoint array
  fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
  if ( ! fStsPoints ) {
    cout << "-E- " << GetName() << "::Init: No StsPoint array!" << endl;
    return kFATAL;
  }

  // Get StsHit array
  fStsHits = (TClonesArray*) ioman->GetObject("StsHit");
  if ( ! fStsHits ) {
    cout << "-E- " << GetName() << "::Init: No StsHit array!" << endl;
    return kFATAL;
  }

  // Get StsTrack array
  fStsTracks = (TClonesArray*) ioman->GetObject("StsTrack");
  if ( ! fStsTracks ) {
    cout << "-E- " << GetName() << "::Init: No StsTrack array!" << endl;
    return kERROR;
  }

  // Get StsTrackMatch array
  fMatches = (TClonesArray*) ioman->GetObject("StsTrackMatch");
  if ( ! fMatches ) {
    cout << "-E- " << GetName() << "::Init: No StsTrackMatch array!"
	 << endl;
    return kERROR;
  }

  // Get the geometry of target and STS
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::Init: Error in reading geometry!"
	 << endl;
    return geoStatus;
  }

  // Create histograms
  CreateHistos();
  Reset();

  // Output
  cout << "   Minimum number of STS hits   : " << fMinHits << endl;
  cout << "   Matching quota               : " << fQuota << endl;
  cout << "   Target position ( " << fTargetPos.X() << ", "
       << fTargetPos.Y() << ", " << fTargetPos.Z() << ") " << endl;
  cout << "   Number of STS stations : " << fNStations << endl;
  if (fActive) cout << "   *****   Task is ACTIVE   *****" << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;

}
// -------------------------------------------------------------------------



// -----   Public method ReInit   ------------------------------------------
InitStatus CbmStsFindTracksQa::ReInit() {

  cout << "==========================================================="
       << endl;;
  cout << GetName() << ": Reinitialising..." << endl;

  // Get the geometry of target and STS
  InitStatus geoStatus = GetGeometry();
  if ( geoStatus != kSUCCESS ) {
    cout << "-E- " << GetName() << "::ReInit: Error in reading geometry!"
	 << endl;
    return geoStatus;
  }

  // Output
  cout << "   Target position ( " << fTargetPos.X() << ", "
       << fTargetPos.Y() << ", " << fTargetPos.Z() << ") " << endl;
  cout << "   Number of STS stations : " << fNStations << endl;
  if (fActive) cout << "   *****   Task is ACTIVE   *****" << endl;
  cout << "==========================================================="
       << endl << endl;

  return geoStatus;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsFindTracksQa::Exec(Option_t* opt) {

  // Timer
  fTimer.Start();

  // Eventwise counters
  Int_t nRec     = 0;
  Int_t nGhosts  = 0;
  Int_t nClones  = 0;
  Int_t nAll     = 0;
  Int_t nAcc     = 0;
  Int_t nRecAll  = 0;
  Int_t nPrim    = 0;
  Int_t nRecPrim = 0;
  Int_t nRef     = 0;
  Int_t nRecRef  = 0;
  Int_t nSec     = 0;
  Int_t nRecSec  = 0;
  TVector3 momentum;
  TVector3 vertex;

  // Fill hit and track maps
  FillHitMap();
  FillMatchMap(nRec, nGhosts, nClones);

  // Loop over MCTracks
  Int_t nMC = fMCTracks->GetEntriesFast();
  for (Int_t iMC=0; iMC<nMC; iMC++) {
    CbmMCTrack* mcTrack = (CbmMCTrack*) fMCTracks->At(iMC);
    if ( ! mcTrack ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No MCTrack at index " << iMC
	   << endl;
      Fatal("Exec", "No MCTrack in array");
    }

    // Check reconstructability; continue only for reconstructable tracks
    nAll++;
    if ( fHitMap.find(iMC) == fHitMap.end() ) continue; // No hits
    Int_t nHits = fHitMap[iMC];
    if ( nHits < fMinHits ) continue;                   // Too few hits
    nAcc++;

    // Check origin of MCTrack
    mcTrack->GetStartVertex(vertex);
    Bool_t isPrim = kFALSE;
    if ( (vertex-fTargetPos).Mag() < 1. ) {
      isPrim = kTRUE;
      nPrim++;
    }
    else nSec++;

    // Get momentum
    mcTrack->GetMomentum(momentum);
    Double_t mom = momentum.Mag();
    Bool_t isRef = kFALSE;
    if ( mom > 1. && isPrim) {
      isRef = kTRUE;
      nRef++;
    }

    // Fill histograms for reconstructable tracks
    fhMomAccAll->Fill(mom);
    fhNpAccAll->Fill(Double_t(nHits));
    if ( isPrim) {
      fhMomAccPrim->Fill(mom);
      fhNpAccPrim->Fill(Double_t(nHits));
    }
    else {
      fhMomAccSec->Fill(mom);
      fhNpAccSec->Fill(Double_t(nHits));
      fhZAccSec->Fill(vertex.Z());
    }

    // Get matched StsTrack
    Int_t    iRec  = -1;
    Double_t quali =  0.;
    //Bool_t   isRec = kFALSE;
    if (fMatchMap.find(iMC) != fMatchMap.end() ) {
      iRec  = fMatchMap[iMC];
      //isRec = kTRUE;
      CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(iRec);
      if ( ! stsTrack ) {
	cout << "-E- " << GetName() << "::Exec: "
	     << "No StsTrack for matched MCTrack " << iMC << endl;
	Fatal("Exec", "No StsTrack for matched MCTrack");
      }
      quali = fQualiMap[iMC];
      if ( quali < fQuota ) {
	cout << "-E- " << GetName() << "::Exec: "
	     << "Matched StsTrack " << iRec << " is below matching "
	     << "criterion ( " << quali << ")" << endl;
	Fatal("Exec", "Match below matching quota");
      }
      CbmTrackMatch* match = (CbmTrackMatch*) fMatches->At(iRec);
      if ( ! match ) {
	cout << "-E- " << GetName() << "::Exec: "
	     << "No StsTrackMatch for matched MCTrack " << iMC << endl;
	Fatal("Exec", "No StsTrackMatch for matched MCTrack");
      }
      Int_t nTrue  = match->GetNofTrueHits();
      Int_t nWrong = match->GetNofWrongHits();
      Int_t nFake  = match->GetNofFakeHits();
      Int_t nAllHits  = stsTrack->GetNStsHits();
      if ( nTrue + nWrong + nFake != nAllHits ) {
	cout << "True " << nTrue << " wrong " << nWrong << " Fake "
	     << nFake << " Hits " << nAllHits << endl;
	Fatal("Exec", "Wrong number of hits");
      }

      // Verbose output
      if ( fVerbose > 1 )
	cout << "-I- " << GetName() << ": "
	     << "MCTrack " << iMC << ", hits "
	     << nHits << ", StsTrack " << iRec << ", hits " << nAllHits
	     << ", true hits " << nTrue << endl;

      // Fill histograms for reconstructed tracks
      nRecAll++;
      fhMomRecAll->Fill(mom);
      fhNpRecAll->Fill(Double_t(nAllHits));
      if ( isPrim ) {
	nRecPrim++;
	fhMomRecPrim->Fill(mom);
	fhNpRecPrim->Fill(Double_t(nAllHits));
	if ( isRef ) nRecRef++;
      }
      else {
	nRecSec++;
	fhMomRecSec->Fill(mom);
	fhNpRecSec->Fill(Double_t(nAllHits));
	fhZRecSec->Fill(vertex.Z());
      }

    }  // Match found in map?

  } // Loop over MCTracks


  // Calculate efficiencies
  Double_t effAll  = Double_t(nRecAll)  / Double_t(nAcc);
  Double_t effPrim = Double_t(nRecPrim) / Double_t(nPrim);
  Double_t effRef  = Double_t(nRecRef)  / Double_t(nRef);
  Double_t effSec  = Double_t(nRecSec)  / Double_t(nSec);


  // Event summary
  if ( fVerbose > 0 ) {
    cout << "----------   StsFindTracksQa : Event summary   ------------"
	 << endl;
    cout << "MCTracks   : " << nAll << ", reconstructable: " << nAcc
	 << ", reconstructed: " << nRecAll << endl;
    cout << "Vertex     : reconstructable: " << nPrim << ", reconstructed: "
	 << nRecPrim << ", efficiency " << effPrim*100. << "%" << endl;
    cout << "Reference  : reconstructable: " << nRef  << ", reconstructed: "
	 << nRecRef  << ", efficiency " << effRef*100. << "%" << endl;
    cout << "Non-vertex : reconstructable: " << nSec << ", reconstructed: "
	 << nRecSec << ", efficiency " << effSec*100. << "%" << endl;
    cout << "STSTracks " << nRec << ", ghosts " << nGhosts
	 << ", clones " << nClones << endl;
    cout << "-----------------------------------------------------------"
	 << endl;
    cout << endl;
  }
  else  cout << "+ " << setw(15) << left << fName << ": " << setprecision(4)
       << setw(8) << fixed << right << fTimer.RealTime()
       << " s, efficiency all " << effAll*100. << " %, vertex "
       << effPrim*100. << " %, reference " << effRef*100. << " %" << endl;

  // Increase counters
  fNAccAll  += nAcc;
  fNAccPrim += nPrim;
  fNAccRef  += nRef;
  fNAccSec  += nSec;
  fNRecAll  += nRecAll;
  fNRecPrim += nRecPrim;
  fNRecRef  += nRecRef;
  fNRecSec  += nRecSec;
  fNGhosts  += nGhosts;
  fNClones  += nClones;
  fNEvents++;
  fTime += fTimer.RealTime();

}
// -------------------------------------------------------------------------



// -----   Private method Finish   -----------------------------------------
void CbmStsFindTracksQa::Finish() {

  // Divide histograms for efficiency calculation
  DivideHistos(fhMomRecAll,  fhMomAccAll,  fhMomEffAll);
  DivideHistos(fhMomRecPrim, fhMomAccPrim, fhMomEffPrim);
  DivideHistos(fhMomRecSec,  fhMomAccSec,  fhMomEffSec);
  DivideHistos(fhNpRecAll,   fhNpAccAll,   fhNpEffAll);
  DivideHistos(fhNpRecPrim,  fhNpAccPrim,  fhNpEffPrim);
  DivideHistos(fhNpRecSec,   fhNpAccSec,   fhNpEffSec);
  DivideHistos(fhZRecSec,    fhZAccSec,    fhZEffSec);

  // Normalise histos for clones and ghosts to one event
  if ( fNEvents ) {
    fhNhClones->Scale(1./Double_t(fNEvents));
    fhNhGhosts->Scale(1./Double_t(fNEvents));
  }

  // Calculate integrated efficiencies and rates
  Double_t effAll  = Double_t(fNRecAll)  / Double_t(fNAccAll);
  Double_t effPrim = Double_t(fNRecPrim) / Double_t(fNAccPrim);
  Double_t effRef  = Double_t(fNRecRef)  / Double_t(fNAccRef);
  Double_t effSec  = Double_t(fNRecSec)  / Double_t(fNAccSec);
  Double_t rateGhosts = Double_t(fNGhosts) / Double_t(fNEvents);
  Double_t rateClones = Double_t(fNClones) / Double_t(fNEvents);

  // Run summary to screen
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "=====   " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Good events   : " << setw(6) << fNEvents << endl;
  cout << "===== Failed events : " << setw(6) << fNEventsFailed << endl;
  cout << "===== Average time  : " << setprecision(4) << setw(8) << right
       << fTime / Double_t(fNEvents)  << " s" << endl;
  cout << "===== " << endl;
  cout << "===== Efficiency all tracks       : " << effAll*100 << " % ("
       << fNRecAll << "/" << fNAccAll <<")" << endl;
  cout << "===== Efficiency vertex tracks    : " << effPrim*100 << " % ("
       << fNRecPrim << "/" << fNAccPrim <<")" << endl;
  cout << "===== Efficiency reference tracks : " << effRef*100 << " % ("
       << fNRecRef << "/" << fNAccRef <<")" << endl;
  cout << "===== Efficiency secondary tracks : " << effSec*100 << " % ("
       << fNRecSec << "/" << fNAccSec <<")" << endl;
  cout << "===== Ghost rate " << rateGhosts << " per event" << endl;
  cout << "===== Clone rate " << rateClones << " per event" << endl;
  cout << "============================================================"
       << endl;

  // Write histos to output
  gDirectory->mkdir("STSFindTracksQA");
  gDirectory->cd("STSFindTracksQA");
  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Write();
  gDirectory->cd("..");
}
// -------------------------------------------------------------------------



// -----   Private method GetGeometry   ------------------------------------
InitStatus CbmStsFindTracksQa::GetGeometry() {

  // Get target geometry
  if ( ! fPassGeo ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!"
	 <<endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  TObjArray* passNodes = fPassGeo->GetGeoPassiveNodes();
  if ( ! passNodes ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive node array"
	 << endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  FairGeoNode* target = (FairGeoNode*) passNodes->FindObject("targ");
  if ( ! target ) {
    cout << "-E- " << GetName() << "::GetGeometry: No target node"
	 << endl;
    fTargetPos.SetXYZ(0., 0., 0.);
    return kERROR;
  }
  FairGeoVector targetPos = target->getLabTransform()->getTranslation();
  FairGeoVector centerPos = target->getCenterPosition().getTranslation();
  Double_t targetX = targetPos.X() + centerPos.X();
  Double_t targetY = targetPos.Y() + centerPos.Y();
  Double_t targetZ = targetPos.Z() + centerPos.Z();
  fTargetPos.SetXYZ(targetX, targetY, targetZ);

  // Get STS geometry
  if ( ! fStsGeo ) {
    cout << "-W- " << GetName() << "::GetGeometry: No passive geometry!"
	 <<endl;
    fNStations = 0;
    return kERROR;
  }
  TObjArray* stsNodes = fStsGeo->GetGeoSensitiveNodes();
  if ( ! stsNodes ) {
    cout << "-E- " << GetName() << "::GetGeometry: No STS node array"
	 << endl;
    fNStations = 0;
    return kERROR;
  }
  fNStations = stsNodes->GetEntries();

  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Private method CreateHistos   -----------------------------------
void CbmStsFindTracksQa::CreateHistos() {

  // Histogram list
  fHistoList = new TList();

  // Momentum distributions
  Double_t minMom   =  0.;
  Double_t maxMom   = 10.;
  Int_t    nBinsMom = 40;
  fhMomAccAll  = new TH1F("hMomAccAll", "all reconstructable tracks",
			 nBinsMom, minMom, maxMom);
  fhMomRecAll  = new TH1F("hMomRecAll", "all reconstructed tracks",
			 nBinsMom, minMom, maxMom);
  fhMomEffAll  = new TH1F("hMomEffAll", "efficiency all tracks",
			 nBinsMom, minMom, maxMom);
  fhMomAccPrim = new TH1F("hMomAccPrim", "reconstructable vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomRecPrim = new TH1F("hMomRecPrim", "reconstructed vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomEffPrim = new TH1F("hMomEffPrim", "efficiency vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomAccSec  = new TH1F("hMomAccSec", "reconstructable non-vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomRecSec  = new TH1F("hMomRecSec", "reconstructed non-vertex tracks",
			 nBinsMom, minMom, maxMom);
  fhMomEffSec  = new TH1F("hMomEffSec", "efficiency non-vertex tracks",
			 nBinsMom, minMom, maxMom);
  fHistoList->Add(fhMomAccAll);
  fHistoList->Add(fhMomRecAll);
  fHistoList->Add(fhMomEffAll);
  fHistoList->Add(fhMomAccPrim);
  fHistoList->Add(fhMomRecPrim);
  fHistoList->Add(fhMomEffPrim);
  fHistoList->Add(fhMomAccSec);
  fHistoList->Add(fhMomRecSec);
  fHistoList->Add(fhMomEffSec);

  // Number-of-points distributions
  Double_t minNp   = -0.5;
  Double_t maxNp   = 15.5;
  Int_t    nBinsNp = 16;
  fhNpAccAll  = new TH1F("hNpAccAll", "all reconstructable tracks",
			 nBinsNp, minNp, maxNp);
  fhNpRecAll  = new TH1F("hNpRecAll", "all reconstructed tracks",
			 nBinsNp, minNp, maxNp);
  fhNpEffAll  = new TH1F("hNpEffAll", "efficiency all tracks",
			 nBinsNp, minNp, maxNp);
  fhNpAccPrim = new TH1F("hNpAccPrim", "reconstructable vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpRecPrim = new TH1F("hNpRecPrim", "reconstructed vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpEffPrim = new TH1F("hNpEffPrim", "efficiency vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpAccSec  = new TH1F("hNpAccSec", "reconstructable non-vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpRecSec  = new TH1F("hNpRecSec", "reconstructed non-vertex tracks",
			 nBinsNp, minNp, maxNp);
  fhNpEffSec  = new TH1F("hNpEffSec", "efficiency non-vertex tracks",
			 nBinsNp, minNp, maxNp);
  fHistoList->Add(fhNpAccAll);
  fHistoList->Add(fhNpRecAll);
  fHistoList->Add(fhNpEffAll);
  fHistoList->Add(fhNpAccPrim);
  fHistoList->Add(fhNpRecPrim);
  fHistoList->Add(fhNpEffPrim);
  fHistoList->Add(fhNpAccSec);
  fHistoList->Add(fhNpRecSec);
  fHistoList->Add(fhNpEffSec);

  // z(vertex) distributions
  Double_t minZ    =  0.;
  Double_t maxZ    = 50.;
  Int_t    nBinsZ  = 50;
  fhZAccSec = new TH1F("hZAccSec", "reconstructable non-vertex tracks",
			 nBinsZ, minZ, maxZ);
  fhZRecSec = new TH1F("hZRecSecl", "reconstructed non-vertex tracks",
			 nBinsZ, minZ, maxZ);
  fhZEffSec = new TH1F("hZEffRec", "efficiency non-vertex tracks",
			 nBinsZ, minZ, maxZ);
  fHistoList->Add(fhZAccSec);
  fHistoList->Add(fhZRecSec);
  fHistoList->Add(fhZEffSec);

  // Number-of-hit distributions
  fhNhClones  = new TH1F("hNhClones", "number of hits for clones",
			 nBinsNp, minNp, maxNp);
  fhNhGhosts  = new TH1F("hNhGhosts", "number of hits for ghosts",
			 nBinsNp, minNp, maxNp);
  fHistoList->Add(fhNhClones);
  fHistoList->Add(fhNhGhosts);

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsFindTracksQa::Reset() {

  TIter next(fHistoList);
  while ( TH1* histo = ((TH1*)next()) ) histo->Reset();

  fNAccAll = fNAccPrim = fNAccRef = fNAccSec = 0;
  fNRecAll = fNRecPrim = fNRecRef = fNRecSec = 0;
  fNGhosts = fNClones = fNEvents = 0;

}
// -------------------------------------------------------------------------



// -----   Private method FillHitMap   -------------------------------------
void CbmStsFindTracksQa::FillHitMap() {
  fHitMap.clear();
  Int_t nHits = fStsHits->GetEntriesFast();
  for (Int_t iHit=0; iHit<nHits; iHit++) {
    CbmStsHit* hit = (CbmStsHit*) fStsHits->At(iHit);
    Int_t iMc = hit->GetRefIndex();
    if ( iMc < 0 ) continue;
    CbmStsPoint* stsPoint = (CbmStsPoint*) fStsPoints->At(iMc);
    Int_t iTrack = stsPoint->GetTrackID();
    fHitMap[iTrack]++;
  }
}
// -------------------------------------------------------------------------



// ------   Private method FillMatchMap   ----------------------------------
void CbmStsFindTracksQa::FillMatchMap(Int_t& nRec, Int_t& nGhosts,
				      Int_t& nClones) {

  // Clear matching maps
  fMatchMap.clear();
  fQualiMap.clear();

  // Loop over StsTracks. Check matched MCtrack and fill maps.
  nGhosts = 0;
  nClones = 0;
  nRec    = fStsTracks->GetEntriesFast();
  Int_t nMtc = fMatches->GetEntriesFast();
  if ( nMtc != nRec ) {
    cout << "-E- " << GetName() << "::Exec: Number of StsMatches ("
	 << nMtc << ") does not equal number of StsTracks ("
	 << nRec << ")" << endl;
    Fatal("Exec", "Inequal number of StsTrack and StsTrackMatch");
  }
  for (Int_t iRec=0; iRec<nRec; iRec++) {

    CbmStsTrack* stsTrack = (CbmStsTrack*) fStsTracks->At(iRec);
    if ( ! stsTrack ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No StsTrack at index " << iRec << endl;
      Fatal("Exec", "No StsTrack in array");
      }
    Int_t nHits = stsTrack->GetNStsHits();

    CbmTrackMatch* match = (CbmTrackMatch*) fMatches->At(iRec);
    if ( ! match ) {
      cout << "-E- " << GetName() << "::Exec: "
	   << "No StsTrackMatch at index " << iRec << endl;
      Fatal("Exec", "No StsTrackMatch in array");
    }
    Int_t nTrue = match->GetNofTrueHits();

    Int_t iMC = match->GetMCTrackId();
    if (iMC == -1 ) {       // no common point with MC, really ghastly!
      if ( fVerbose > 1 )
	cout << "-I- " << GetName() << ":"
	     << "No MC match for StsTrack " << iRec << endl;
      fhNhGhosts->Fill(nHits);
      nGhosts++;
      continue;
    }

    // Check matching criterion (quota)
    Double_t quali = Double_t(nTrue) / Double_t(nHits);
    if ( quali >= fQuota ) {

      // No previous match for this MCTrack
      if ( fMatchMap.find(iMC) == fMatchMap.end() ) {
	fMatchMap[iMC] = iRec;
	fQualiMap[iMC] = quali;
      }

      // Previous match; take the better one
      else {
	if ( fVerbose > 1 )
	  cout << "-I- " << GetName() << ": "
	       << "MCTrack " << iMC << " doubly matched."
	       << "Current match " << iRec
	       << ", previous match " << fMatchMap[iMC]
	       << endl;
	if ( fQualiMap[iMC] < quali ) {
	  CbmStsTrack* oldTrack
	    = (CbmStsTrack*) fStsTracks->At(fMatchMap[iMC]);
	  fhNhClones->Fill(Double_t(oldTrack->GetNStsHits()));
	  fMatchMap[iMC] = iRec;
	  fQualiMap[iMC] = quali;
	}
	else fhNhClones->Fill(nHits);
	nClones++;
      }

    }

    // If not matched, it's a ghost
    else {
      if ( fVerbose > 1 )
	cout << "-I- " << GetName() << ":"
	     << "StsTrack " << iRec << " below matching criterion "
	     << "(" << quali << ")" << endl;
      fhNhGhosts->Fill(nHits);
      nGhosts++;
    }

  }   // Loop over StsTracks

}
// -------------------------------------------------------------------------



// -----   Private method DivideHistos   -----------------------------------
void CbmStsFindTracksQa::DivideHistos(TH1* histo1, TH1* histo2,
				      TH1* histo3) {

  if ( !histo1 || !histo2 || !histo3 ) {
    cout << "-E- " << GetName() << "::DivideHistos: "
	 << "NULL histogram pointer" << endl;
    Fatal("DivideHistos", "Null histo pointer");
  }

  Int_t nBins = histo1->GetNbinsX();
  if ( histo2->GetNbinsX() != nBins || histo3->GetNbinsX() != nBins ) {
    cout << "-E- " << GetName() << "::DivideHistos: "
	 << "Different bin numbers in histos" << endl;
    cout << histo1->GetName() << " " << histo1->GetNbinsX() << endl;
    cout << histo2->GetName() << " " << histo2->GetNbinsX() << endl;
    cout << histo3->GetName() << " " << histo3->GetNbinsX() << endl;
   return;
  }

  Double_t c1, c2, c3, ce;
  for (Int_t iBin=0; iBin<nBins; iBin++) {
    c1 = histo1->GetBinContent(iBin);
    c2 = histo2->GetBinContent(iBin);
    if ( c2 != 0. ) {
      c3 = c1 / c2;
      Double_t c4=(c3 * ( 1. - c3 ) / c2);
      if ( c4 >= 0.) {
        ce = TMath::Sqrt( c3 * ( 1. - c3 ) / c2 );
      } else {
       ce=0;
      }
    }
    else {
      c3 = 0.;
      ce = 0.;
    }
    histo3->SetBinContent(iBin, c3);
    histo3->SetBinError(iBin, ce);
  }

}
// -------------------------------------------------------------------------



ClassImp(CbmStsFindTracksQa)
