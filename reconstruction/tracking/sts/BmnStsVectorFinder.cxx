// -------------------------------------------------------------------------
// -----                  BmnStsVectorFinder source file               -----
// -----  Created 3/12/21  by A.Zinchenko, D.Zinchenko, R.Zinchenko    -----
// -------------------------------------------------------------------------
#include "BmnStsVectorFinder.h"
#include "CbmKF.h"
#include "CbmKFTrack.h"
#include "CbmStsCluster.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsHit.h"
#include "CbmStsPoint.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "CbmStsTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"

#include <TClonesArray.h>
#include <TGraph.h>
#include <TMath.h>

#include <iostream>
#include <map>
#include <set>
#include <vector>

using std::cout;
using std::endl;
using std::map;
using std::multimap;
using std::pair;
using std::set;
using std::vector;

#include <TStopwatch.h>

static Double_t workTime = 0.0;

// -----   Default constructor   -------------------------------------------
BmnStsVectorFinder::BmnStsVectorFinder() 
  : FairTask("STS Vector Finder"),
    fHitArray(NULL),
    fTrackArray(NULL),
    fPass(0),
    fNsta(0),
    fNbranches(5),
    fExact(0),
    fExactSel(-9),
    fNhitsMin(NULL),
    discarded(0),
    fMatBudgetFileName("")
{
  for (int j = 0; j < 4; ++j) fClusArray[j] = nullptr;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
BmnStsVectorFinder::~BmnStsVectorFinder() { }
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus BmnStsVectorFinder::Init() {

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- BmnStsVectorFinder::Init: "
	 << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input arrays
  fClusArray[0] = (TClonesArray*) ioman->GetObject("StsCluster");
  if ( ! fClusArray[0] ) {
    cout << "-W- BmnStsVectorFinder::Init: "
	 << "No StsCluster array!" << endl;
    //return kERROR;
    fClusArray[0] = (TClonesArray*) ioman->GetObject("BmnSiliconUpperCluster");
    fClusArray[1] = (TClonesArray*) ioman->GetObject("BmnSiliconLowerCluster");
    fClusArray[2] = (TClonesArray*) ioman->GetObject("BmnGemUpperCluster");
    fClusArray[3] = (TClonesArray*) ioman->GetObject("BmnGemLowerCluster");
    for (int j = 0; j < 4; ++j) {
      if ( ! fClusArray[j] ) {
	cout << "-W- BmnStsVectorFinder::Init: "
	     << "No BmnCluster array!" << j << endl;
	return kERROR;
      }
    }
  }

  fHitArray = (TClonesArray*) ioman->GetObject("StsHit");
  if ( ! fHitArray ) {
    cout << "-W- BmnStsVectorFinder::Init: "
	 << "No StsHit array!" << endl;
    return kERROR;
  }

  fTrackArray = (TClonesArray*) ioman->GetObject("StsTrack");
  if ( ! fTrackArray ) {
    cout << "-W- BmnStsVectorFinder::Init: "
	 << "No StsTrack array!" << endl;
    //AZ-100722 return kERROR;
  }

  fDigiMatches = (TClonesArray*) ioman->GetObject("StsDigiMatch");
  if ( ! fDigiMatches ) {
    cout << "-W- BmnStsVectorFinder::Init: "
	 << "No StsDigiMatch array!" << endl;
    //return kERROR;
  }

  fStsPoints = (TClonesArray*) ioman->GetObject("StsPoint");
  if ( ! fStsPoints ) {
    cout << "-W- BmnStsVectorFinder::Init: "
	 << "No StsPoint array!" << endl;
    return kERROR;
  }

  fSilPoints = (TClonesArray*) ioman->GetObject("SiliconPoint");
  if ( ! fSilPoints ) {
    cout << "-E- BmnStsVectorFinder::Init: No SiliconPoint array!" << endl;
  }
  
  // Create and register output array
  fVectorArray = new TClonesArray("CbmStsTrack");
  ioman->Register("StsVector", "STS", fVectorArray, kTRUE);

  fXyzv[0] = 0.0;
  //fXyzv[1] = 0.86;
  //fXyzv[2] = -24.1;
  fXyzv[1] = 0.0;
  fXyzv[2] = 0.0;
  
  fitter.Init();

  // Define logic
  static Int_t nHitsMin[20] =   {7,    5,    4,    4}; // min number of hits per track vs iteration - Run 7
  //static Double_t dTanX[20] =   {0.001, 0.003, 0.002, 0.008}; // window size in TanX vs iteration
  static Double_t dTanX[20] =   {0.002, 0.003, 0.002, 0.008}; // window size in TanX vs iteration
  //static Double_t dTanY[20] =   {0.01, 0.02, 0.01, 0.02}; // window size in TanY vs iteration
  static Double_t dTanY[20] =   {0.01, 0.02, 0.02, 0.03}; // window size in TanY vs iteration
  static Double_t dTanY3[20] =  {0.01, 0.02, 0.02, 0.03}; // window size in TanY vs iteration
  //static Double_t dVarX[20] =   {0.004, 0.01, 0.006, 0.02}; // window size in VarX vs iteration - coherent with ptCut
  static Double_t ptCut[20] =   {1.0,  0.3,  0.7,  0.1}; // min pxz
  static Double_t curvSta[20] = {12.0, 12.0, 12.0, 7.5,  5.5,  4.5,   3.6,   3.0,  3.0,  3.0}; //AZ-310722 - max curvature in stations (for tracks starting in Si) 
  static Double_t phiXZ[20] =   {0.76, 0.69, 0.69, 0.95, 0.74, 0.60,  0.51,  0.44,  0.39,  0.35}; //Max phi_XZ in stations
  static Double_t tanXmax[20] = {0.95, 0.83, 0.83, 1.40, 0.91, 0.68,  0.56,  0.47,  0.41,  0.37}; //Max tan_X in stations
  static Double_t zMean[20] =   {18.5, 27.2, 35.8, 60.9, 91.7, 123.4, 154.0, 185.8, 216.5, 248.0}; //Mean Z of stations
  //*/

  fNhitsMin = nHitsMin;
  fdTanX = dTanX;
  fdTanY = dTanY;
  fdTanY3 = dTanY3;
  fPhiXZ = phiXZ;
  fTanXmax = tanXmax;
  fZmean = zMean;
  fPTcut = ptCut;
  //fVarx = dVarX;
  fCurvSta = curvSta;

  for (int j = 0; j < 20; ++j) if (fPTcut[j] > 0.001) fPTcut[j] = 1 / fPTcut[j]; //AZ-170722 - curvature 

  //fExact = 1; // for debug // removed DZ 13.10.2021
  //fExactSel = 13; // for debug

  if (fExact) {
    //if (1) {
    // Open windows
    //Double_t scale = 3;
    Double_t scale = 1;
    for (Int_t j = 0; j < 20; ++j) {
      dTanX[j] *= scale;
      dTanY[j] *= scale;
      //dX[j] *= scale;
    }
  }
  
  FairField *magField = FairRunAna::Instance()->GetField();
  fBy = TMath::Abs (magField->GetBy (0.0, 0.0, 135.0)) / 10; // max. field in Tesla
  for (int j = 0; j < 20; ++j) fCurvSta[j] *= (0.8 / fBy); //AZ-290822 - max curvature vs field 
  
  if (fMatBudgetFileName != "") fitter.ReadMatBudget(fMatBudgetFileName);

  cout << "-I- BmnStsVectorFinder: Intialisation successfull" << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void BmnStsVectorFinder::Exec(Option_t* opt) 
{
  
    TStopwatch sw;
    sw.Start();

  //*
  // Reset output array
  if ( ! fHitArray ) Fatal("Exec", "No StsHit array");

  fVectorArray->Delete();
  for (Int_t j = 0; j < 19; ++j) {
    fCandSet[j].clear();
    //fTripleCodes[j].clear();
    fCandCodes[j].clear();
  }
    
  //AZ-100722 Int_t nTracks = fTrackArray->GetEntriesFast();
  //AZ-100722 if (nTracks == 0) return;
  Int_t nTracks = 0;
  if (fTrackArray) {
     nTracks = fTrackArray->GetEntriesFast();
     if (nTracks == 0) return;     
  }

  const Int_t nPass = 4; //4; //5; //8;
  Int_t nsta = CbmKF::Instance()->GetNStsStations();
  for (Int_t j = 0; j < nPass; ++j) fNskips[j] = nsta - fNhitsMin[j];

  Int_t nHits0 = fHitArray->GetEntriesFast(), idmaxP = 0;

  // Fill cluster-to-hit maps
  fHit2id.clear();
  fClusMaps[0].clear();
  fClusMaps[1].clear();
  fmapHits.clear();
  
  for (Int_t ihit = 0; ihit < nHits0; ++ihit) {
    CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(ihit);
    Int_t iclusF = hit->GetDigi(0);
    Int_t iclusB = hit->GetDigi(1);
    fClusMaps[0].insert(pair<Int_t,Int_t>(iclusF,ihit));
    fClusMaps[1].insert(pair<Int_t,Int_t>(iclusB,ihit));
    Int_t ista = hit->GetStationNr() - 0;
    fNsta = TMath::Max(fNsta,ista);
    // Code below for debugging
    if (fVerbose > 0) {
      set<Int_t> idset = GetHitId(hit, idmaxP);
      for (set<Int_t>::iterator sit = idset.begin(); sit != idset.end(); ++sit)
        fHit2id.insert(pair<Int_t, Int_t>(ihit, *sit));
    }
    }
  
  Int_t nHitsOut = 0;
  discarded = 0;
  BuildTrackCand();

  for (Int_t ipass = 0; ipass < nPass; ++ipass) {
  //for (Int_t ipass = nPass-1; ipass < nPass; ++ipass) {
    fPass = ipass;
    //Int_t minHits = 5; //4; // minimum number of hits on tracks to accept
    //Int_t minHits = (ipass == 0) ? 15 : 5; // minimum number of hits on tracks to accept
    Int_t minHits = (ipass == 0) ? 15 : -fNhitsMin[ipass-1]; // minimum number of hits on tracks to accept
    TClonesArray *trArray = (ipass == 0) ? fTrackArray : fVectorArray;
    nHitsOut += ExcludeHits(minHits, trArray);

    if (fVerbose > 0) {
      cout << "-I- BmnStsVectorFinder: start - " << nHits0 << ", end - "
        << nHits0 - nHitsOut << endl;
    }

    Int_t ntr0 = fVectorArray->GetEntriesFast();
    //BuildTrackCand();
    /*
    //ExtendTracks(fNsta-1);
    FitTracks();
    RemoveFakes();
    RemoveDoubles();
    cout << "\n ***** Pass " << ipass << ": Number of found tracks = " 
	 << fVectorArray->GetEntriesFast() - ntr0 << " " << fVectorArray->GetEntriesFast() << "\n" << endl;
    //if (fNhitsMin[ipass] <= 4) RemoveFakes();
    */
    BuildDoublets();
    BuildTriplets();
    fTracks.clear(); //
    BuildTracks();
    FitTracks();
    RemoveDoubles();
    if (fVerbose > 0) {
      cout << "\n ***** Pass " << ipass << ": Number of found tracks = "
        << fVectorArray->GetEntriesFast() - ntr0 << " " << fVectorArray->GetEntriesFast() << "\n" << endl;
    }
  }
    
  if (fVerbose > 0) cout << "discarded " << discarded << " track candidates" << endl;
  
  // Post-processing - try to exclude fake tracks (with too many shared clusters)
  ExcludeFakes();
  
  sw.Stop();
  workTime += sw.RealTime();
}

// -------------------------------------------------------------------------

Int_t BmnStsVectorFinder::ExcludeHits(Int_t minHits, TClonesArray *trArray)
{
  // Exclude hits used for tracking
  
  //AZ-100722 Int_t nTracks = trArray->GetEntriesFast();
  Int_t nTracks = 0; //AZ-100722 
  if (trArray) nTracks = trArray->GetEntriesFast(); //AZ-100722

  if (nTracks == 0) return 0;
  Int_t nHitsOut = 0;
  //AZ-130722 multimap<Int_t,Int_t>::iterator mit;
  //AZ-130722 pair<multimap<Int_t,Int_t>::iterator,multimap<Int_t,Int_t>::iterator> ret;
  unordered_multimap<Int_t,Int_t>::iterator mit;
  pair<unordered_multimap<Int_t,Int_t>::iterator,unordered_multimap<Int_t,Int_t>::iterator> ret; //AZ-130722

  for (Int_t itra = 0; itra < nTracks; ++itra) {
    CbmStsTrack *track = (CbmStsTrack*) trArray->UncheckedAt(itra);
    if (track->GetNStsHits() < minHits) {
      trArray->Remove(track);
      continue;
    }

    Int_t nHitsTr = track->GetNStsHits();

    for (Int_t ihit = 0; ihit < nHitsTr; ++ihit) {
      int indx = track->GetStsHitIndex(ihit); //AZ-130722
      //AZ-130722 CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(track->GetStsHitIndex(ihit));
      CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(indx); //AZ-130722
      if ( ! hit ) continue;

      if (fNhitsMin[fPass] < -5) {
      //if (fNhitsMin[fPass] > 6) { //AZ-180722
	// Exclude all hits created from given clusters
	Int_t iclusF = hit->GetDigi(0);
	ret = fClusMaps[0].equal_range(iclusF);
	for (mit = ret.first; mit != ret.second; ++mit) {
	  CbmStsHit *hit1 = (CbmStsHit*) fHitArray->UncheckedAt(mit->second);
	  if (hit1->GetUniqueID() == 0) ++nHitsOut; // excluded hit - used in track
	  hit1->SetUniqueID(1); // exclude hit - used in track
	  //AZ-130722 fmapHits[ihit].used = 1;
	  fmapHits[mit->second].used = 1; //AZ-130722
	}
	Int_t iclusB = hit->GetDigi(1);
	ret = fClusMaps[1].equal_range(iclusB);
	for (mit = ret.first; mit != ret.second; ++mit) {
	  CbmStsHit *hit1 = (CbmStsHit*) fHitArray->UncheckedAt(mit->second);
	  if (hit1->GetUniqueID() == 0) ++nHitsOut; // excluded hit - used in track
	  hit1->SetUniqueID(1); // exclude hit - used in track
	  //AZ-130722 fmapHits[ihit].used = 1;
	  fmapHits[mit->second].used = 1; //AZ-130722
	}
      }
      if (hit->GetUniqueID() == 0) ++nHitsOut;
      hit->SetUniqueID(1);
      //AZ-130722 fmapHits[ihit].used = 1;
      fmapHits[indx].used = 1; //AZ-130722

      //AZ-130722 Remove used doublets
      //*
      int ista = hit->GetStationNr() - 1;
      ret = fMap2[ista].equal_range(indx);
      fMap2[ista].erase(ret.first,ret.second);
      //AZ-140722 Remove used triplets
      ret = fMap3[ista].equal_range(indx);
      for (mit = ret.first; mit != ret.second; ++mit) {
	 candvec cand = fCandVec3[ista][mit->second];
	 string code = MakeCode(cand);
	 fMapCode3[ista].erase(code);
      }
      fMap3[ista].erase(ret.first,ret.second);
      //*/
    }
    
  }

  trArray->Compress();
  return nHitsOut;
}

// -------------------------------------------------------------------------

set<Int_t> BmnStsVectorFinder::GetHitId(CbmStsHit *hit, Int_t& idmaxP)
{
  // Get IDs contributing to given hit (for debugging purposes)
  
  if (fClusArray[1]) return GetHitIdBmn (hit, idmaxP); // BmnRoot data structures

  set<Int_t> ids;
  //return ids; // FIXME

  Int_t iclusF = hit->GetDigi(0);
  Int_t iclusB = hit->GetDigi(1);
  CbmStsCluster* clusF = (CbmStsCluster*) fClusArray[0]->UncheckedAt(iclusF);
  CbmStsCluster* clusB = (CbmStsCluster*) fClusArray[0]->UncheckedAt(iclusB);

  map<Int_t,Double_t> indF, indB;
  //set<Int_t> ids;
  Int_t nDigis = clusF->GetNDigis();
  Double_t pmax = 0.0;
  TVector3 mom3;
  
  for (Int_t j = 0; j < nDigis; ++j) {
    CbmStsDigiMatch *digiMatch = (CbmStsDigiMatch*) fDigiMatches->UncheckedAt(clusF->GetDigi(j));
    for (Int_t j1 = 0; j1 < 3; ++j1) {
      Int_t ip = digiMatch->GetRefIndex(j1);
      if (ip < 0) break;
      CbmStsPoint *p = (CbmStsPoint*) fStsPoints->UncheckedAt(ip);
      p->Momentum(mom3);
      indF[p->GetTrackID()] = mom3.Mag();
    }
  }
      
  nDigis = clusB->GetNDigis();

  for (Int_t j = 0; j < nDigis; ++j) {
    CbmStsDigiMatch *digiMatch = (CbmStsDigiMatch*) fDigiMatches->UncheckedAt(clusB->GetDigi(j));
    for (Int_t j1 = 0; j1 < 3; ++j1) {
      Int_t ip = digiMatch->GetRefIndex(j1);
      if (ip < 0) break;
      CbmStsPoint *p = (CbmStsPoint*) fStsPoints->UncheckedAt(ip);
      //Not needed p->Momentum(mom3);
      indB[p->GetTrackID()] = 1.0; //mom3.Mag();
    }
  }

  idmaxP = -1;
  
  for (map<Int_t,Double_t>::iterator mit = indF.begin(); mit != indF.end(); ++mit) {
    Int_t id = (indB.count(mit->first) == 0) ? -mit->first : mit->first;
    ids.insert(id); // negative ID for ghost crossings
    if (id >= 0 && mit->second > pmax) {
      pmax = mit->second;
      idmaxP = id;
    }
  }
  // This is to be consistent with CBM hit matching procedure
  // (hit is matched with the closest point)
  if (hit->GetRefIndex() >= 0) {
    CbmStsPoint *p = (CbmStsPoint*) fStsPoints->UncheckedAt(hit->GetRefIndex());
    idmaxP = p->GetTrackID();
  } else idmaxP = -1;
  
  for (map<Int_t,Double_t>::iterator mit = indB.begin(); mit != indB.end(); ++mit) {
    if (ids.count(mit->first) > 0) continue;
    ids.insert(-mit->first); // negative ID for ghost crossings
  }

  return ids;
}

// -------------------------------------------------------------------------

set<Int_t> BmnStsVectorFinder::GetHitIdBmn(CbmStsHit *hit, Int_t& idmaxP)
{
  // Get IDs contributing to given hit (for debugging purposes) - for BmnRoot data structures

  set<Int_t> ids;
  //int ind = 0; // Si hit
  //if (hit->GetSystemId() == kGEM) ind = 2; // GEM hit

  if (hit->GetRefIndex() == -1 || fStsPoints == nullptr || fSilPoints == nullptr) ids.insert(-1); // fake hit
  else {
     //AZ-150722 CbmStsPoint *p = (CbmStsPoint*) fStsPoints->UncheckedAt(hit->GetRefIndex());
     FairMCPoint *p = nullptr;
     if (hit->GetSystemId() == kGEM) p = (FairMCPoint*) fStsPoints->UncheckedAt(hit->GetRefIndex());
     else p = (FairMCPoint*) fSilPoints->UncheckedAt(hit->GetRefIndex());
     ids.insert(p->GetTrackID());
  }
  return ids;
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::BuildTrackCand()
{
  // Build track candidates
  
  for (Int_t ist = 0; ist < fNsta; ++ist) {
    fmapPhx[ist].clear();
    fmapTy[ist].clear();
    fmapX[ist].clear();
    fmapY[ist].clear();
    fSeedVec[ist].clear();
    fCandVec[ist].clear();
    //fCandMap2[ist].clear();
    //fCandMap3[ist].clear();
    fCandVec2[ist].clear();
    fMap2[ist].clear();
    fCandVec3[ist].clear();
    fMap3[ist].clear();
    fMapCode3[ist].clear();
  }
  //fmapHits.clear();
  
  Int_t nHits = fHitArray->GetEntriesFast(), idmaxP = 0;
  if (fVerbose > 0) cout << "nHits " << nHits << endl;
  TVector3 pos;
  
  for (Int_t ih = 0; ih < nHits; ++ih) {
    CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(ih);
    if (hit->GetUniqueID()) continue; // used hit
    if (fExactSel >= 0) {
      // Accept only hits with given ID - for debug
      set<Int_t> ids = GetHitId(hit, idmaxP);
      if (ids.count(fExactSel) == 0) continue;
    }
    if (fExact) {
      // Accept only true hits
      set<Int_t> ids = GetHitId(hit, idmaxP);
      if (idmaxP < 0) continue;
    }
    Int_t ista = hit->GetStationNr() - 1;
    //!!! For debug
    //if (ista > 2) continue;
    //!!!
    hit->Position(pos);
    Double_t dx = hit->GetX() - fXyzv[0];
    Double_t dy = hit->GetY() - fXyzv[1];
    Double_t dz = hit->GetZ() - fXyzv[2];
    //AZ-250822 if (fPass >= 0) fmapHits[ih] = hitinfo(pos,dx/dz,dy/dz);
    if (fPass >= 0) fmapHits[ih] = hitinfo (pos, TMath::ATan2(dx,dz)/fPhiXZ[ista]/fZmean[ista]/fBy, dy/dz/fTanXmax[ista]); //AZ-280822
    fmapX[ista].insert(pair<Double_t,Int_t>(pos[0],ih));
    fmapY[ista].insert(pair<Double_t,Int_t>(pos[1],ih));
    fmapPhx[ista].insert(pair<Double_t,Int_t>(fmapHits[ih].phx,ih));
    fmapTy[ista].insert(pair<Double_t,Int_t>(fmapHits[ih].ty,ih));
  }

  //const Int_t stastop[9] = { fNsta-1, fNsta-2, fNsta-4}; // stop station vs iteration
  //const Int_t stastop[9] = { fNsta-3, fNsta-2, fNsta-3}; // stop station vs iteration
  Int_t stastop = fNsta - 1 - fNskips[fPass];
  stastop = 0;
  
  if (fVerbose > 0) cout << "fNskips fPass " << fNskips[fPass] << " " << fPass << " stastop " << stastop << " fNsta-1 " << fNsta-1 << endl;

  for (Int_t ista = fNsta-1; ista >= stastop; --ista) {
    if (fVerbose > 0) cout << "xmapsize " << fmapX[ista].size() << endl;
    for (multimap<Double_t,Int_t>::iterator mit = fmapX[ista].begin(); mit != fmapX[ista].end(); ++mit) {
      //map<Int_t,Int_t> aaa;
      candvec aaa;
      aaa.nskips = fNsta - 1 - ista;
      aaa.momxz = 0.0;
      aaa.stahit[ista] = mit->second;
      aaa.code = "-" + to_string(aaa.stahit[ista]) + "-"; // hit index coded
      //aaa.second[fNsta-1] = mit->second;
      //if (fExact) {
      
      set<Int_t> ids = GetHitId(mit->second, idmaxP);
      aaa.idmaxP = idmaxP;
	//}
      //cout << "aaa.z " <<((CbmStsHit*) fHitArray->UncheckedAt(aaa.stahit[ista]))->GetZ() << " ";
      fSeedVec[ista].push_back(aaa);
    } //for (multimap<Double_t,Int_t>::iterator mit
    if (fVerbose > 0) {
      Int_t ncand = fSeedVec[ista].size();
      cout << " Vector stat: " << ista << " " << ncand;// << endl;
      //AZ-130722 pair<multimap<Int_t, Int_t>::iterator, multimap<Int_t, Int_t>::iterator> ret;
      pair<unordered_multimap<Int_t, Int_t>::iterator, unordered_multimap<Int_t, Int_t>::iterator> ret; //AZ-130722 

      for (Int_t j = 0; j < ncand; ++j) {
        Int_t ih = fSeedVec[ista][j].stahit[ista];
        ret = fHit2id.equal_range(ih);
        cout << " (" << ih << "*";
        //AZ-130722 for (multimap<Int_t, Int_t>::iterator mit = ret.first; mit != ret.second; ++mit) {
        for (unordered_multimap<Int_t, Int_t>::iterator mit = ret.first; mit != ret.second; ++mit) { //AZ-130722
          if (mit != ret.first) cout << ":";
          cout << mit->second;
        }
        cout << ")";
      }
      cout << "\n";
    }

    // Extend track candidates
    //if (fCandVec[ista].size()) ExtendTracks(ista);
  } // for (Int_t ista = fNsta-1;
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::BuildDoublets()
{
  // Build doublets

  int rebuild2 = 1; //AZ-140722

  for (Int_t ist = 0; ist < fNsta; ++ist) {
    fCandVec[ist].clear();
    fCandVec2[ist].clear();
    fMap2[ist].clear();
    fCandVec3[ist].clear();
    fMap3[ist].clear();
    fMapCode3[ist].clear();
    //AZ-140722 - Does not seem to speed up
    /*
    if ( (fPass == 0) || (TMath::Abs(fdTanX[fPass]-fdTanX[fPass-1]) > 0.001) || //AZ-140722
	 (TMath::Abs(fdTanY[fPass]-fdTanY[fPass-1]) > 0.001) ) {
       fCandVec2[ist].clear();
       fMap2[ist].clear();
       rebuild2 = 1;
    }
    */
  }
  if (rebuild2 == 0) return; //AZ-140722 - do not rebuild doublets

  Int_t idmaxP = 0;

  for (Int_t ista = 0; ista < fNsta-1; ++ista) {
    Int_t nTra = fSeedVec[ista].size();
    Int_t istanext = ista + 1;
    if (nTra == 0 || fSeedVec[istanext].size() == 0) continue;
    Double_t dty = fdTanY[fPass], dphx = fdTanX[fPass];
    //dty *= 5; //AZ-040822
    //dtx *= 2; //AZ-040822

    for (Int_t itra = 0; itra < nTra; ++itra) {
      candvec &aaa = fSeedVec[ista][itra];
      Int_t ih = aaa.stahit[ista];
      if (fmapHits[ih].used) continue; // used hit
      //if (fMap2[ista].count
      Double_t phx = fmapHits[ih].phx;
      phx *= (fPhiXZ[ista] * fZmean[ista]); //AZ-280822
      phx /= (fPhiXZ[istanext] * fZmean[istanext]); //AZ-280822
      Double_t ty = fmapHits[ih].ty;
      ty *= (fTanXmax[ista] / fTanXmax[istanext]); //AZ-280822
      //AZ-250822 Double_t distxz = TMath::Sqrt (fmapHits[ih].tx*fmapHits[ih].tx + 1) * 
	   // (fmapHits[ih].xyz.Z() - fXyzv[2]); //AZ-310722 - XZ-distance to PV 
      // Get hits on the downstream station
      multimap<Double_t,Int_t>::iterator mityb = fmapTy[istanext].lower_bound(ty-dty);
      multimap<Double_t,Int_t>::iterator mitye = fmapTy[istanext].upper_bound(ty+dty);     
      multimap<Double_t,Int_t>::iterator mitxb = fmapPhx[istanext].lower_bound(phx-dphx);     
      multimap<Double_t,Int_t>::iterator mitxe = fmapPhx[istanext].upper_bound(phx+dphx);
      // Get hits from the acceptance window 
      set<Int_t> setTx, setTy, intersect;
      for (multimap<Double_t,Int_t>::iterator mit = mitxb; mit != mitxe; ++mit) 
	if (fmapHits[mit->second].used == 0) setTx.insert(mit->second);
      for (multimap<Double_t,Int_t>::iterator mit = mityb; mit != mitye; ++mit)
	if (fmapHits[mit->second].used == 0) setTy.insert(mit->second);
      set_intersection(setTx.begin(), setTx.end(), setTy.begin(), setTy.end(), std::inserter(intersect, intersect.begin()));
      //cout << " Intersect: " << ista << " " << setTx.size() << " " << setTy.size() << " " << intersect.size() << endl;
      Double_t xp = fmapHits[ih].xyz[0]; //AZ-280722
      Double_t zp = fmapHits[ih].xyz[2]; //AZ-280722

      for (set<Int_t>::iterator sit = intersect.begin(); sit != intersect.end(); ++sit) {
	CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(*sit);
	if (hit->GetUniqueID()) continue; // used hit
	if (fmapHits[*sit].used) continue;
	if (fExact) {
	  // Exact track ID match
	  set<Int_t> ids = GetHitId(*sit, idmaxP);
	  //if (ids.count(aaa.idmaxP) == 0) continue;
	  if (idmaxP != aaa.idmaxP) continue;
	}
	//map<Int_t,Int_t> aaa1 = aaa;
	candvec aaa1 = aaa;
	aaa1.stahit[istanext] = *sit; // second hit of the doublet
	//aaa1.nextind = *sit; // second hit of the doublet
	Double_t dx = xp - fmapHits[*sit].xyz[0]; //AZ-280722
	Double_t dz = zp - fmapHits[*sit].xyz[2]; //AZ-280722
	aaa1.lengxz = TMath::Sqrt (dx*dx + dz*dz); //AZ-280722 - doublet length in XZ-plane
	//AZ-250822 Double_t varx = 0.0;
	//AZ-250822 if (!CheckVarx(dx, dz, tx, distxz, varx)) continue; //AZ-310722 - apply cut on X-variable
	aaa1.momxz = Curv3(aaa1, aaa1, aaa1, 0); //AZ-310722 - pxz-estimate with the origin at the primary vertex
	if (TMath::Abs(aaa1.momxz) > fPTcut[fPass] || TMath::Abs(aaa1.momxz) > fCurvSta[ista]) continue; //AZ-310722
	aaa1.ty = fmapHits[*sit].ty; //AZ-290822
	//AZ-250822 aaa1.varx = varx;
	fCandVec2[ista].push_back(aaa1);
	fMap2[ista].insert(pair<int,int>(ih,fCandVec2[ista].size()-1));
	//if (aaa1.idmaxP == 78 || aaa1.idmaxP == 222 || aaa1.idmaxP == 127)
	//cout << " Doublet: " << aaa1.idmaxP << " " << aaa1.stahit.begin()->first << endl; // debug output
	//extendOK = kTRUE;
      }
      //if (extendOK) break; // track has been extended - do not jump over station
    } // for (Int_t itra = 0;
    
    Int_t ncand = fCandVec2[ista].size();
    if (fVerbose >= 1) cout << " Doublet stat: " << ista << " " << ncand << endl;
  } // for (Int_t ista = fNsta-1; 
  
}

// -------------------------------------------------------------------------

Bool_t BmnStsVectorFinder::CheckVarx(Double_t dx, Double_t dz, Double_t tx, Double_t distxz, Double_t& varx)
{
   //AZ-310722 - Check Varx

   varx = (dx / dz - tx) / distxz;
   //AZ-250822 if (TMath::Abs(varx) > fVarx[fPass]) return kFALSE;
   return kTRUE;
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::BuildTriplets()
{
  // Build triplets

  Int_t idmaxP = 0;
  
  //AZ-130722 pair<multimap<Int_t,int>::iterator,multimap<Int_t,int>::iterator> ret;
  //AZ-130722 multimap<Int_t,int>::iterator mit, mit1;
  pair<unordered_multimap<Int_t,int>::iterator,unordered_multimap<Int_t,int>::iterator> ret; //AZ-130722
  unordered_multimap<Int_t,int>::iterator mit, mit1; //AZ-130722
  
  for (Int_t ista = 0; ista < fNsta-2; ++ista) {
    discarded = 0;
    Int_t nTra = fCandVec2[ista].size();
    Int_t istanext = ista + 1;
    if (nTra == 0 || fCandVec2[istanext].size() == 0) continue;

    for (mit = fMap2[ista].begin(); mit != fMap2[ista].end(); ++mit) {
      candvec &aaa = fCandVec2[ista][mit->second]; // first doublet
      Double_t tany = fmapHits[aaa.stahit.begin()->second].ty; //AZ-2908822
      tany *= (fTanXmax[ista] / fTanXmax[istanext+1]); //AZ-290822
      
      ret = fMap2[istanext].equal_range(aaa.stahit.rbegin()->second);
      multimap<Double_t,candvec> mapDoublets; //AZ-110722 second doublets, attached to one first doublet  
      int nbr = 0, newtr = 0, newtr3 = 0;
      Double_t c2max = 999.0;

      for (mit1 = ret.first; mit1 != ret.second; ++mit1) {
	candvec aaa1;
	//AZ-150722 Int_t newtr = (mit1 == ret.first) ? 1 : 0;

	Int_t nhits = 3;
	aaa1 = aaa;
	candvec &aaa2 = fCandVec2[istanext][mit1->second]; // second doublet
	if (fExact) {
	  // Exact track ID match
	  if (aaa.idmaxP != aaa2.idmaxP) continue;
	}

	//AZ-310722 - Check doublet matching criteria
	//if (TMath::Abs(aaa.varx - aaa2.varx) > 0.002) continue; //AZ-050822
	//AZ-070822 if (TMath::Abs(aaa.ty - aaa2.ty) > 0.01) continue; //AZ-040822
	//if (TMath::Abs(aaa.ty - aaa2.ty) > fdTanY3[fPass]) continue; //AZ-250822
	if (TMath::Abs(tany - aaa2.ty) > fdTanY3[fPass]) continue; //AZ-290822
	if (TMath::Abs(aaa.momxz-aaa2.momxz) / (TMath::Abs(aaa.momxz)+TMath::Abs(aaa2.momxz)) 
	    * TMath::Sqrt(aaa.lengxz+aaa2.lengxz) / 10 > 0.2) continue; //AZ-250822
	
	aaa1.stahit[aaa2.stahit.rbegin()->first] = aaa2.stahit.rbegin()->second; // third hit of the triplet
	map<Int_t,Int_t> &hitMap = aaa1.stahit;
	
	CbmStsHit *hit = nullptr;
	CbmStsTrack track; 
	TArrayI &hits = *track.GetStsHits();
	hits.Set(3);
	Int_t indx = 0, iok = 1;
	
	/// hitMap ---- aaa1.stahit
	for (map<Int_t,Int_t>::iterator mit2 = hitMap.begin(); mit2 != hitMap.end(); ++mit2) {
	  hits[indx++] = mit2->second;
	  hit = (CbmStsHit*) fHitArray->UncheckedAt(mit2->second);
	  //cout << " hit: " << mit2->first << " " << mit2->second << endl;
	  //hit->SetDx(0.08/TMath::Sqrt(12.0));
	  hit->SetDx(0.08/TMath::Sqrt(12.0)*1.2);
	  //hit->SetDx(0.015);
	  hit->SetDy(0.1234);
	  if (hit->GetStationNr() <= 3) hit->SetDx(0.02/TMath::Sqrt(12.0));
	  //if (hit->GetStationNr() % 2 != 0) hit->SetCovXY(1.968e-3); // this is for 2-D hits
	  //else hit->SetCovXY(-1.968e-3);
	  //hit->SetDy(0.03); // for test
	  if (indx == 3) break;
	}
	
	//fitter.DoFit(&track);
	//float chi2 = track.GetChi2();
	//AZ-310722 Double_t ty = 0.0;
	Double_t ty = aaa.ty; //AZ-310722
	//AZ-280722 float chi2 = LinearFit (&track, newtr, ty) / hit->GetDy() / hit->GetDy();
	float chi2 = LinearFit (aaa, aaa2, &track, newtr, ty) / hit->GetDy() / hit->GetDy(); //AZ-280722
	++newtr; //AZ-150722
	//float chi2 = LinearFit (&track, newtr, ty) / 0.09 / 0.09;
	//cout << "3hit track Chi2 " << track.GetChi2() << " " << nhits << endl;
	// if Chi2 > 10.0 exclude track cand
	// TODO DZ add index so I know how many candidates were discarded
	//if (aaa1.idmaxP == 78 || aaa1.idmaxP == 222 || aaa1.idmaxP == 127)
	//cout << " Triplet: " << aaa1.idmaxP << " " << aaa1.stahit.begin()->first << " " << chi2 << endl; // debug output
	
	if (chi2 > 10.0) {
	  discarded++;
	  continue;
	}
	// Triplet curvature
	aaa1.momxz = Curv3(aaa, aaa2, aaa1, newtr3);
	newtr3 = 1;
	//AZ-150722 if (TMath::Abs(aaa1.momxz) < 0.2) {
	//Double_t ptcut = (fPass < 2) ? 0.3 : 0.2; //AZ-150722
	//if (TMath::Abs(aaa1.momxz) < ptcut) { //AZ-150722
	if (TMath::Abs(aaa1.momxz) > fPTcut[fPass]) { //AZ-170722 - curvature
	//if (TMath::Abs(aaa1.momxz) > fPTcut[fPass] || TMath::Abs(aaa1.momxz) > fCurvSta[ista]) { //AZ-310722 - curvature
	  discarded++;
	  continue;
	}
	
	aaa1.ty = ty;
	/*AZ-110722
	fCandVec3[ista].push_back(aaa1);
	fMap3[ista].insert(pair<Int_t,int>(mit->first,fCandVec3[ista].size()-1));
	string code = "-";
	for (map<Int_t,Int_t>::iterator mitr = hitMap.begin(); mitr != hitMap.end(); ++mitr)
	  code += (to_string(mitr->second) + "-");
	fMapCode3[ista][code] = fCandVec3[ista].size() - 1;
	*/
	if (chi2 > c2max && nbr >= fNbranches) continue;
	aaa1.chi2 = chi2; //AZ-140722
	mapDoublets.insert(pair<Double_t,candvec>(chi2,aaa1)); //AZ-110722
	//mapDoublets.insert(pair<Double_t,candvec>(TMath::Abs(aaa1.momxz),aaa1)); //AZ-280722
	nbr = mapDoublets.size();
	//AZ-310722 if (nbr > fNbranches) mapDoublets.erase(mapDoublets.rbegin()->first);
	if (nbr > fNbranches) {
	   multimap<Double_t,candvec>::iterator mit0 = mapDoublets.end();
	   --mit0;
	   mapDoublets.erase(mit0);
	}
	c2max = mapDoublets.rbegin()->first;
      } // for (mit1 = ret.first; 

      // Take only first fNbranches with lowest chi2
      int nok = 0; 
      for (multimap<Double_t,candvec>::iterator it = mapDoublets.begin(); it != mapDoublets.end(); ++it) {
	 if (nok >= fNbranches) break;
	 ++nok;
	 fCandVec3[ista].push_back(it->second);
	 fMap3[ista].insert(pair<Int_t,int>(mit->first,fCandVec3[ista].size()-1));
	 map<Int_t,Int_t> &hitMap = it->second.stahit;
	 string code = "-";
	 for (map<Int_t,Int_t>::iterator mitr = hitMap.begin(); mitr != hitMap.end(); ++mitr)
	    code += (to_string(mitr->second) + "-");
	 fMapCode3[ista][code] = fCandVec3[ista].size() - 1;
      }
    } // for (mit = fCandMap2[ista].begin();

    Int_t ncand = fCandVec3[ista].size();
    if (fVerbose > 0) cout << " Triplet stat: " << ista << " " << ncand << " " << discarded << endl;
  } // for (Int_t ista = fNsta-1; 
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::BuildTracks()
{
  // Build tracks starting from triplets

  int istaEnd = fNsta - fNhitsMin[fPass] + 1;
  istaEnd = TMath::Min (istaEnd, fNsta - 3); // do not allow simple triplets on last 3 stations
  //std::multimap<Int_t,int> &candMap = fMap3[ista];
  //vector<candvec> &candVec = fCandVec3[ista];
  //std::multimap<Int_t,int> *candMap = &fMap2[ista];
  //vector<candvec> *candVec = &fCandVec2[ista];
  
  for (Int_t ista = 0; ista < istaEnd; ++ista) {
    //std::multimap<Int_t,int> *candMap = &fMap2[ista];
    //vector<candvec> *candVec = &fCandVec2[ista];
    //AZ-130722 std::multimap<Int_t,int> *candMap = &fMap3[ista];
     std::unordered_multimap<Int_t,int> *candMap = &fMap3[ista]; //AZ-130722
    vector<candvec> *candVec = &fCandVec3[ista];
    Int_t nTra = candMap->size();
    //cout << " BuildTracks: " << ista << " " << nTra << endl;
    if (nTra == 0) continue;
    //Int_t istaup = ista - 2;
    
    //AZ-130722 for (multimap<Int_t,int>::iterator mit = candMap->begin(); mit != candMap->end(); ++mit) {
    for (unordered_multimap<Int_t,int>::iterator mit = candMap->begin(); mit != candMap->end(); ++mit) {
      candvec &cand = (*candVec)[mit->second];
      ExtendTrack (cand);
    }
  }
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::ExtendTrack(candvec cand)
{
  // Extend the track by triplets or doublets

  const Double_t c2ndfMax = 30.0; // cut on chi2/NDF

  Int_t nTra = 0, ista = cand.stahit.rbegin()->first;
  //cout << ista << " " << cand.stahit.size() << endl;
  /*
  //!!! Check - step back
  if (ista == fNsta-2) {
    cand.stahit.erase(cand.stahit.rbegin()->first);
    ista = cand.stahit.rbegin()->first;
  }
  */
  //std::multimap<Int_t,int> &candMap = (ista == fNsta-2) ? fMap2[ista] : fMap3[ista]; // doublets or triplets
  //vector<candvec> &candVec = (ista == fNsta-2) ? fCandVec2[ista] : fCandVec3[ista]; // doublets or triplets
  //std::multimap<Int_t,int> *pCandMap = (ista == fNsta-2) ? &fMap2[ista] : &fMap2[ista]; // doublets or triplets
  //vector<candvec> *pCandVec = (ista == fNsta-2) ? &fCandVec2[ista] : &fCandVec2[ista]; // doublets or triplets
  //AZ-130722 std::multimap<Int_t,int> *pCandMap = (ista == fNsta-2) ? &fMap2[ista] : &fMap2[ista]; // doublets or triplets
  std::unordered_multimap<Int_t,int> *pCandMap = (ista == fNsta-2) ? &fMap2[ista] : &fMap2[ista]; //AZ-130722 doublets or triplets
  vector<candvec> *pCandVec = (ista == fNsta-2) ? &fCandVec2[ista] : &fCandVec2[ista]; // doublets or triplets
  //AZ-130722 multimap<Int_t,int> &candMap = *pCandMap;
  unordered_multimap<Int_t,int> &candMap = *pCandMap; //AZ-130722
  vector<candvec> &candVec = *pCandVec;
  
  if (ista < fNsta-1) {
    /*
    if (ista == fNsta-2) {
      // Doublets
      candMap = fMap2[ista];
      candVec = fCandVec2[ista];
    }
    */
    nTra = candMap.count(cand.stahit.rbegin()->second);
  }

  //cout << " ExtendTrack " << ista << " " << nTra << endl;
  if (nTra == 0) {
    // No extension found
    if (cand.stahit.size() < fNhitsMin[fPass]) return; // too short track
    //cout << " ExtendTrack - store " << cand.stahit.size() << " " << cand.idmaxP << endl;
    //FitTrack(cand);
    string code("-");
    for (map<int,int>::iterator it = cand.stahit.begin(); it != cand.stahit.end(); ++it)
      code += (to_string(it->second) + "-");
    //cout << cand.stahit.rbegin()->first << " " << ista << endl;
    if (fCandCodes[cand.stahit.size()].find(code) == fCandCodes[cand.stahit.size()].end()) {
      fCandCodes[cand.stahit.size()].insert(code);
      //cout << " 1 " << endl;
      //AZ-170122 if (cand.stahit.size() >= 4 && FitTrack(cand) > c2ndfMax) return;
      if (cand.stahit.size() >= 4 && cand.stahit.rbegin()->first != ista && FitTrack(cand) > c2ndfMax) return;
      //cout << " ExtendTrack - store " << cand.stahit.size() << " " << cand.idmaxP << endl;
      fCandVec[cand.stahit.begin()->first].push_back(cand);
    } 
    return;
  }

  //AZ-130722 multimap<Int_t,int>::iterator mit;
  //AZ-130722 pair<multimap<Int_t,int>::iterator,multimap<Int_t,int>::iterator> ret;
  unordered_multimap<Int_t,int>::iterator mit; //AZ-130722 
  pair<unordered_multimap<Int_t,int>::iterator,unordered_multimap<Int_t,int>::iterator> ret; //AZ-130722
  ret = candMap.equal_range(cand.stahit.rbegin()->second);
  string code = "-";
  int istaMid = 0;
  
  // Use Kalman track constraints around extrapolated position
  /*
  set<int> kalmWind;
  if (nTra > 5)
    if (cand.stahit.size() >= 4) kalmWind = KalmanWindow(cand, candVec[ret.first->second].stahit.begin()->second);
  */

  for (mit = ret.first; mit != ret.second; ++mit) {
    //candvec &aaa = candVec[mit->second];
    candvec aaa = candVec[mit->second];
    if (fExact && aaa.idmaxP != cand.idmaxP) continue;
    
    //if (nTra > 5 && cand.stahit.size() >= 4 &&
    //	kalmWind.find(aaa.stahit.begin()->second) == kalmWind.end()) continue; // outside Kalman window
    
    // Check if there is a triplet started from the last but one point of the current one (cand)
    map<Int_t,Int_t>::iterator mitr = aaa.stahit.begin();
    ++mitr;
    if (code.size() == 1) {
      map<Int_t,Int_t>::reverse_iterator mit1 = cand.stahit.rbegin();
      ++mit1;
      istaMid = mit1->first;
      code += (to_string(mit1->second) + "-");
      --mit1;
      code += (to_string(mit1->second) + "-");
      code += (to_string(mitr->second) + "-");
    } else {
      // Replace last index
      int pos = code.rfind("-",code.size()-2);
      code.erase(code.begin()+pos+1,code.end());
      code += (to_string(mitr->second) + "-");
    }
    //cout << " Check code: " << code << " " << istaMid << endl;
    // Check extension possibility
    int ok = 1;
    if (fMapCode3[istaMid].find(code) == fMapCode3[istaMid].end()) ok = 0; // no triplet found
    //!!! Check triplet slopes and curvatures
    //*
    candvec *bbb = nullptr;
    if (ok) {
      bbb = &fCandVec3[istaMid][fMapCode3[istaMid][code]];
      //Double_t dty = TMath::Abs (cand.ty - bbb.ty);
      //if (dty * 2 / (TMath::Abs(cand.ty) + TMath::Abs(bbb.ty)) > 0.1) continue; // different slopes
      //if (dty > 0.005) continue; // different slopes
      // Curvatures
      //cout << cand.momxz << " " << bbb.momxz << endl;
      if (cand.stahit.size() > 2) {
	Double_t dp = TMath::Abs (cand.momxz - bbb->momxz);
	//if (dp * 2 / (TMath::Abs (cand.momxz) + TMath::Abs(bbb->momxz)) > 0.3) {
	//AZ-060822 if (dp * 2 / (TMath::Abs (cand.momxz) + TMath::Abs(bbb->momxz)) > 0.2) {
	if (dp / (TMath::Abs (cand.momxz) + TMath::Abs(bbb->momxz)) > 0.3) { //AZ-060822
	  // Different curvatures
	  ok = 0;
	}
      }
    }
    if (ok) {
      if (aaa.stahit.size() == 3) {
	// Extension by a triplet
	Double_t dp = TMath::Abs (cand.momxz - aaa.momxz);
	//if (dp * 2 / (TMath::Abs (cand.momxz) + TMath::Abs(aaa.momxz)) > 0.3) {
	//AZ-070822 if (dp * 2 / (TMath::Abs (cand.momxz) + TMath::Abs(aaa.momxz)) > 0.2) {
	if (dp / (TMath::Abs (cand.momxz) + TMath::Abs(aaa.momxz)) > 0.3) { //AZ-070822
	  // different curvatures
	  ok = 0;
	}
      }
    }
    if (!ok) {
      // No extension found
      if (cand.stahit.size() < fNhitsMin[fPass]) continue; // too short track
      //cout << " ExtendTrack - store " << cand.stahit.size() << " " << cand.idmaxP << endl;
      string codeVec("-");
      for (map<int,int>::iterator it = cand.stahit.begin(); it != cand.stahit.end(); ++it)
	codeVec += (to_string(it->second) + "-");
      if (fCandCodes[cand.stahit.size()].find(codeVec) == fCandCodes[cand.stahit.size()].end()) {
	fCandCodes[cand.stahit.size()].insert(codeVec);
	if (cand.stahit.size() >= 4) {
	  //cout << " 2 " << endl;
	  if (cand.stahit.rbegin()->first == ista) continue; //AZ-171121
	  Double_t chi2 = FitTrack(cand);
	  //cout << " --- Chi2: " << chi2 << endl;
	  if (chi2 > c2ndfMax) continue;
	}
	//cout << " ExtendTrack - store " << cand.stahit.size() << " " << cand.idmaxP << endl;
	fCandVec[cand.stahit.begin()->first].push_back(cand);
      } 
    }
    //*/
    //!!! Extension found
    candvec candext = cand;
    if (cand.stahit.size() < 3) candext.momxz = bbb->momxz; // update momentum
    //candext.ty = bbb.ty;
    // Add triplet (or doublet)
    for (mitr = aaa.stahit.begin(); mitr != aaa.stahit.end(); ++mitr)
      candext.stahit[mitr->first] = mitr->second;
    //cout << " ids: " << aaa.idmaxP << " " << cand.idmaxP << " " << candext.stahit.size() << endl;
    //AZ-120722 if (aaa.idmaxP != cand.idmaxP) cand.idmaxP = -1;  
    if (aaa.idmaxP != cand.idmaxP) candext.idmaxP = -1; //AZ-120722  
    // Fit track to fight with combinatorics
    if (candext.stahit.size() >= 4) {
      //cout << " 3 " << endl;
      Double_t chi2ndf = FitTrack(candext);
      if (chi2ndf > c2ndfMax) continue;
      /* This slows down
      if (chi2ndf > c2ndfMax) {
	 string codext = MakeCode(candext);
	 fCandCodes[candext.stahit.size()].insert(codext);
	 continue;
      }
      */
    }
    ExtendTrack (candext);
  } // for (mit = ret.first; 
}

// -------------------------------------------------------------------------

Double_t BmnStsVectorFinder::FitTrack(candvec &cand)
{
  // Fit track candidate
  
  const Int_t gkChi2Cut = 5.0; //10.0;

  CbmStsTrack track; 
  string hitcode;

  MakeStsTrack(cand, hitcode, track);
  int nhits = track.GetNStsHits();
  if (fCandSet[nhits].find(hitcode) != fCandSet[nhits].end()) return 0.0; // track has been saved already

  //AZ-160122 fitter.DoFit(&track);
  if (nhits <= 4) fitter.DoFit(&track);
  else FilterHit (cand, track);
  /*
  else {
    // Debug
    CbmStsTrack track1(track);
    FilterHit (cand, track);
    fitter.DoFit(&track1);
    cout << " debug " << track.GetChi2() << " " << track1.GetChi2() << endl;
  }
  */
  //track.GetParamFirst()->Print();
  //cout << " aaaaaa " << track.GetChi2() << " " << track.GetNStsHits() << endl;
  int ndf = TMath::Max (track.GetNDF(), 1);
  Double_t chi2ndf = track.GetChi2() / ndf;
  cand.param = *(track.GetParamLast());
  cand.chi2 = track.GetChi2();
  if (chi2ndf < gkChi2Cut && nhits >= fNhitsMin[fPass]) {
    // Refit with material budget
    //fitter.DoFit(&track);
    fitter.Fit(&track); //AZ
    if (track.GetChi2() / ndf > gkChi2Cut) {
      //cout << " yyyyyy " << " " << track.GetChi2() / ndf << endl;
      //fitter.Fit(&track); //AZ - for debugging
      return chi2ndf;
    }
    // Save good tracks
    Double_t qual = nhits;
    qual += (100.0 - TMath::Min(TMath::Abs(track.GetChi2()),100.0)) / 101.0;
    fTracks.insert(pair<Double_t,CbmStsTrack>(-qual,track));
    fCandSet[nhits].insert(hitcode);
    //cout << " FitTrack: " << hitcode << " " << track.GetChi2() << endl;
  }
  //fCandSet[nhits].insert(hitcode); - don't use this !!!
  return chi2ndf;
}

// -------------------------------------------------------------------------
//*
Double_t BmnStsVectorFinder::FilterHit(candvec &cand, CbmStsTrack &track)
{
  // Filter last hit on track candidate
  
  const Int_t gkChi2Cut = 10.0;
  /*
  CbmStsTrack track; 
  string hitcode;

  MakeStsTrack(cand, hitcode, track);
  */
  Int_t nhits = cand.stahit.size();
  CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(track.GetStsHitIndex(nhits-1));
  /*cout << " !!! parameters: " << cand.stahit.size() << " " << cand.param.GetX() << " " << cand.param.GetY()
       << " " << cand.param.GetZ() << " "
       << hit->GetZ() << " " << cand.chi2 << " " << cand.code << endl;*/
  //AZ-050222 fitter.Extrapolate(&track,hit->GetZ(),track.GetParamLast());
  //cout << hit->GetZ() << " " << track.GetParamLast()->GetZ() << endl;

  CbmKFTrack kftr;
  kftr.SetStsTrack(track, kFALSE); // last parameter
  fitter.SetKFHits(kftr,&track);
  Bool_t downstream = kTRUE;
  //Double_t qp0 = 0.0;
  Double_t qp0 = track.GetParamLast()->GetQp();
  Double_t *cov = kftr.GetCovMatrix();
  //for (Int_t j = 0; j < 15; ++j) cov[j] = 0.0;
  //cov[0] = cov[2] = cov[5] = cov[9] = cov[14] = 10000.0; 
  //kftr.GetRefChi2() = 0.0;
  //Double_t chi20 = 0.0, dchi2 = 0.0;
  Double_t chi20 = kftr.GetRefChi2(), dchi2 = 0.0;

  CbmKF *KF = CbmKF::Instance();
  CbmKFHit *h = NULL;
  Int_t istold = 0, imax = 0;
  Bool_t err = kFALSE;

  //for (Int_t i = 0; i < nhits; i++) {
  h = kftr.GetHit(nhits-1);
  err = err || h->Filter( kftr, downstream, qp0 );
  dchi2 = kftr.GetRefChi2() - chi20;
  //cout << err << " " << dchi2 << " " << kftr.GetRefChi2() << " " << cand.chi2+dchi2 << endl;
  kftr.GetTrackParam( *track.GetParamLast() );
  track.SetChi2(kftr.GetRefChi2()+err*1000);
  track.SetNDF(2*nhits-5);
  /*
  if (fCandSet[track.GetNStsHits()].find(hitcode) != fCandSet[track.GetNStsHits()].end()) return 0.0; // track has been saved already

  fitter.DoFit(&track);
  //track.GetParamFirst()->Print();
  //cout << " aaaaaa " << track.GetChi2() << " " << track.GetNStsHits() << endl;
  int ndf = TMath::Max (track.GetNDF(), 1);
  Double_t chi2ndf = track.GetChi2() / ndf;
  if (chi2ndf < gkChi2Cut && track.GetNStsHits() >= fNhitsMin[fPass]) {
    // Save good tracks
    Double_t qual = track.GetNStsHits();
    qual += (100.0 - TMath::Min(TMath::Abs(track.GetChi2()),100.0)) / 101.0;
    fTracks.insert(pair<Double_t,CbmStsTrack>(-qual,track));
    fCandSet[track.GetNStsHits()].insert(hitcode);
    //cout << " FitTrack: " << hitcode << " " << track.GetChi2() << endl;
  }
  return chi2ndf;
  */
 return 0.0;
}
//*/
// -------------------------------------------------------------------------

void BmnStsVectorFinder::MakeStsTrack(candvec &cand, string &hitcode, CbmStsTrack &track)
{
  // Fill STS track information

  map<Int_t,Int_t> &hitMap = cand.stahit;
  CbmStsHit *hit = nullptr;
  TArrayI &hits = *track.GetStsHits();
  hits.Set(hitMap.size());
  Int_t indx = 0, iok = 1;
  hitcode = "-";

  /// hitMap ---- cand.stahit
  for (map<Int_t,Int_t>::iterator mit2 = hitMap.begin(); mit2 != hitMap.end(); ++mit2) {
    hits[indx++] = mit2->second;
    hit = (CbmStsHit*) fHitArray->UncheckedAt(mit2->second);
    //cout << " hit: " << mit2->first << " " << mit2->second << endl;
    hit->SetDx(0.08/TMath::Sqrt(12.0));
    //hit->SetDx(0.015);
    hit->SetDy(0.1234);
    if (hit->GetStationNr() <= 3) hit->SetDx(0.02/TMath::Sqrt(12.0));
    //if (hit->GetStationNr() % 2 != 0) hit->SetCovXY(1.968e-3); // this is for 2-D hits
    //else hit->SetCovXY(-1.968e-3);
    //hit->SetDy(0.03); // for test
    CbmStsStation *stat = CbmStsDigiScheme::Instance()->GetStationByNr(hit->GetStationNr());
    Double_t dx = stat->GetSector(0)->GetDx();
    //if (dx < 0.02) dx *= 2; // scale up for Si
    //AZ-120722 if (dx > 0.01) dx *= 1.2; // for GEMs
    if (dx > 0.02) dx *= 1.2; //AZ-120722 for GEMs
    hit->SetDx(dx/TMath::Sqrt(12.0));
    hitcode += (to_string(mit2->second) + "-");
  }
  track.SetParamFirst(cand.param);
  track.SetParamLast(cand.param);
  track.SetChi2(cand.chi2);
  cand.code = hitcode;
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::ExtendTracks(Int_t ista)
{
  // Extend tracks by going upstream

  //const Double_t zsta[19] = {1.132, 32.750, 64.150, 96.750, 129.300, 160.800, 192.100}; // average hit Z-position - Run 6 !!!
  //const Double_t zsta[19] = {16.21, 22.79, 29.22, 42.56, 67.63, 116.10, 137.80, 164.10, 186.20}; // average hit Z-position - Run 7 !!!
  const Double_t zsta[19] = {18.55, 27.16, 35.78, 61.05, 91.50, 123.50, 153.90, 185.90, 216.40, 248.20}; // average hit Z-position - Run 8 !!!
  const Double_t pars[19][4] = { {1.30358, 20.8953, 0.857369, 0.56339}, {1.64202, 7.18813, 0.596002, 0.694601},
				 {3.10765, 3.21564, 0.291252, 1.08062}, {2.25484, 3.81810, 0.430724, 0.840519},
				 {3.38309, 4.33504, 0.349704, 1.07882}, {3.99252, 3.35587, 0.302402, 1.65050},
				 {10.3321, 4.24742, 0.168995, 1.71113} };
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::FinishEvent()
{
  // Finish event
  /*
  FairRootManager* ioman = FairRootManager::Instance();
  */
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::Finish() {
  printf("Work time of BmnStsVectorFinder: %4.2f sec.\n", workTime);
}

//__________________________________________________________________________

TVector3 BmnStsVectorFinder::Parabola(TVector3& pos0, TVector3& pos1, TVector3& pos2)
{
  // Get parabolic track approximation from 3 points (X vs Z)
  // y = a*x^2 + bx + c

  Double_t x[3] = {pos0[2]-fXyzv[2], pos1[2]-fXyzv[2], pos2[2]-fXyzv[2]};
  Double_t y[3] = {pos0[0]-fXyzv[0], pos1[0]-fXyzv[0], pos2[0]-fXyzv[0]};
  
  Double_t denom = (x[0] - x[1]) * (x[0] - x[2]) * (x[1] - x[2]);
  Double_t dy10 = y[1] - y[0];
  Double_t dy02 = y[0] - y[2];
  Double_t dy21 = y[2] - y[1];
  //Double_t a = x[2] * (y[1] - y[0]) + x[1] * (y[0] - y[2]) + x[0] * (y[2] - y[1]);
  Double_t a = x[2] * dy10 + x[1] * dy02 + x[0] * dy21;
  a /= denom;
  //Double_t b = x[0]*x[0] * (y[1] - y[2]) + x[2]*x[2] * (y[0] - y[1]) +
  //x[1]*x[1] * (y[2] - y[0]);
  Double_t b = -x[0]*x[0] * dy21 - x[2]*x[2] * dy10 - x[1]*x[1] * dy02;
  b /= denom;
  Double_t c = x[1]*x[1] * (x[2]*y[0] - x[0]*y[2]) + 
    x[1] * (x[0]*x[0]*y[2] - x[2]*x[2]*y[0]) + x[0]*x[2] * (x[2] - x[0]) * y[1];
  c /= denom;
  return TVector3(c,b,a);
  //𝐴=𝑥3(𝑦2−𝑦1)+𝑥2(𝑦1−𝑦3)+𝑥1(𝑦3−𝑦2) / (𝑥1−𝑥2)(𝑥1−𝑥3)(𝑥2−𝑥3)
  //𝐵=𝑥21(𝑦2−𝑦3)+𝑥23(𝑦1−𝑦2)+𝑥22(𝑦3−𝑦1) / (𝑥1−𝑥2)(𝑥1−𝑥3)(𝑥2−𝑥3)
  //𝐶=𝑥22(𝑥3𝑦1−𝑥1𝑦3)+𝑥2(𝑥21𝑦3−𝑥23𝑦1)+𝑥1𝑥3(𝑥3−𝑥1)𝑦2 / (𝑥1−𝑥2)(𝑥1−𝑥3)(𝑥2−𝑥3)
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::FitTracks()
{
  // Fit tracks

  const Int_t gkChi2Cut = 5.0; //10.0;
  //fTracks.clear();

  //for (Int_t ista = 0; ista < fNsta-3; ++ista) {
  for (Int_t ista = 0; ista < fNsta; ++ista) {
    Int_t ntra = fCandVec[ista].size();
    //cout << " FitTracks: " << ista << " " << ntra << endl;

    for (Int_t itra = 0; itra < ntra; ++itra) {
      candvec &cand = fCandVec[ista][itra];
      //if (cand.nextind >= 0) continue; // track has an extension already processed

      map<Int_t,Int_t> &hitMap = cand.stahit;
      Int_t nhits = hitMap.size();
      //if (nhits < 3) continue; // too short track
      if (nhits < fNhitsMin[fPass]) continue; // too short track
      CbmStsTrack track;
      TArrayI &hits = *track.GetStsHits();
      hits.Set(nhits);
      Int_t indx = 0, iok = 1;

      // Check if the track candidate has been checked already
      string hitcode("-");
      for (map<Int_t,Int_t>::iterator mit = hitMap.begin(); mit != hitMap.end(); ++mit) 
	hitcode += (to_string(mit->second) + "-");
      //cout << itra << " " << hitcode << endl;

      Int_t sta0 = hitMap.begin()->first, same = 0;

      //same = fCandSet[sta0].count(hitcode);
      same = fCandSet[nhits].count(hitcode);
      //if (same) cout << " same: " << sta0 << " " << same << " " << endl;

      /////AZ if (same) continue; // used combination - skip it ???
      //fCandSet[sta0].insert(hitcode);
      fCandSet[nhits].insert(hitcode);
     
      for (map<Int_t,Int_t>::iterator mit = hitMap.begin(); mit != hitMap.end(); ++mit) {
	hits[indx++] = mit->second;
	// Debug
	CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(mit->second);
	//cout << " hit: " << mit->first << " " << hit->GetX() << " " << hit->GetY() << " " << hit->GetZ() << " "
	//   << hit->GetDx() << " " << hit->GetDy() << endl;
	//cout << " hit: " << mit->first << " " << mit->second << endl;
	hit->SetDx(0.08/TMath::Sqrt(12.0));
	//hit->SetDx(0.015);
	hit->SetDy(0.1234);
	if (hit->GetStationNr() <= 3) hit->SetDx(0.02/TMath::Sqrt(12.0));
	//if (hit->GetStationNr() <= 3) hit->SetDx(0.0025);
	//if (hit->GetStationNr() % 2 != 0) hit->SetCovXY(1.968e-3); // this is for 2-D hits (CbmFitter)
	//else hit->SetCovXY(-1.968e-3);
	//hit->SetDy(0.03); // for test
	CbmStsStation *stat = CbmStsDigiScheme::Instance()->GetStationByNr(hit->GetStationNr());
	Double_t dx = stat->GetSector(0)->GetDx();
	//if (dx < 0.02) dx *= 2; // scale up for Si
	//AZ-120722 if (dx > 0.01) dx *= 1.2; // scale up for GEMs
	if (dx > 0.02) dx *= 1.2; //AZ-120722 scale up for GEMs
	hit->SetDx(dx/TMath::Sqrt(12.0));
	/*
	if (fMatBudgetFileName != "" && hit->GetDz() < 1.e-7) {
	  // Store material budget for the hit (along the beam)
	  Double_t zshift = (dx < 0.02) ? 0.5 : 1.5; // Si or GEM 
	  TProfile2D *prof = fMatHistos.lower_bound(hit->GetZ()-zshift)->second;
	  int ix = prof->FindBin(hit->GetX());
	  int iy = prof->FindBin(hit->GetY());
	  Double_t radThick = prof->GetBinContent(ix,iy);
	  hit->SetDz(radThick/100);
	}
	*/
      }

      //cout << " before " << track.GetParamFirst()->GetZ() << endl;
      //fitter.DoFit(&track);
      fitter.Fit(&track); //AZ
      //cout << " after " << track.GetParamFirst()->GetZ() << " " << track.GetNStsHits() << " " << track.GetChi2() << endl;
      /*
      CbmKFTrack kftr = CbmKFTrack(track);
      fitter.SetKFHits(kftr, &track);
      kftr.Fit(kTRUE); // downstream
      kftr.Fit(kFALSE); // upstream
      */
      if (fVerbose > 0) {
	 cout << " aaaaaaa " << endl;
	 //track.GetParamFirst()->Print();
	 cout << track.GetChi2() << " " << nhits << " " << endl;
      }
      //AZ-111221 if (track.GetChi2() / track.GetNDF() > gkChi2Cut) {
      if (track.GetChi2() / track.GetNDF() > gkChi2Cut && track.GetNStsHits() > 3) {
	/*
	// Too high chi2/NDF - try to remove one outlier with the largest dChi2
	// Based on the code from CbmKFTrackInterface::Fit()
	CbmKFTrack kftr;
	//kftr.SetStsTrack(track, kFALSE); // last parameter
	//Bool_t downstream = kFALSE;
	kftr.SetStsTrack(track, kTRUE); // first parameter
	fitter.SetKFHits(kftr,&track);
	Bool_t downstream = kTRUE;
	Double_t qp0 = 0.0;
	Double_t *cov = kftr.GetCovMatrix();
	for (Int_t j = 0; j < 15; ++j) cov[j] = 0.0;
	cov[0] = cov[2] = cov[5] = cov[9] = cov[14] = 10000.0; 
	kftr.GetRefChi2() = 0.0;
	Double_t chi20 = 0.0, dchi2 = 0.0, dchi2max = 0.0;

	CbmKF *KF = CbmKF::Instance();
	CbmKFHit *h = NULL;
	Int_t istold = 0, imax = 0;
	Bool_t err = kFALSE;
	set<Int_t> indok;

	//for (Int_t i = nhits - 1; i >= 0; i--) {
	 // h = kftr.GetHit( i );
	 // Int_t ist = h->MaterialIndex;
	 // if (i < nhits - 1) {
	  //  for (Int_t j = istold - 1; j > ist; j--)
	 //     err = err || KF->vMaterial[j]->Pass( kftr, downstream, qp0 );
	 // }

	for (Int_t i = 0; i < nhits; i++) {
	  h = kftr.GetHit( i );
	  Int_t ist = h->MaterialIndex;
	  if (i > 0) {
	    for (Int_t j = istold + 1; j < ist; j++)
	      err = err || KF->vMaterial[j]->Pass( kftr, downstream, qp0 );
	  }
	  err = err || h->Filter( kftr, downstream, qp0 );
	  dchi2 = kftr.GetRefChi2() - chi20;
	  cout << dchi2 << " " << kftr.GetRefChi2() << endl;
	  //if (dchi2 <= gkChi2Cut) indok.insert(i);
	  //if (dchi2 <= 2*gkChi2Cut) indok.insert(i);
	  indok.insert(i);
	  if (dchi2 > dchi2max) {
	    dchi2max = dchi2;
	    imax = i;
	  }
	  chi20 = kftr.GetRefChi2();
	  istold = ist;
	}
	*/
	// Remove last hit and refit
	set<Int_t> indok;
	for (Int_t i = 0; i < nhits; i++) indok.insert(i);
	int imax = nhits - 1;

	if (indok.size() > 3) {
	//if (indok.size() >= fNhitsMin[fPass]) {
	//if (indok.size()-1 >= fNhitsMin[fPass]) {
	  indok.erase(imax);
	  // Good short track - refit
	  indx = 0;
	  hitcode = "-";
	  for (set<Int_t>::iterator sit = indok.begin(); sit != indok.end(); ++sit) {
	    hits[indx++] = hits[*sit];
	    //hitcode += hits[*sit];
	    hitcode += (to_string(hits[*sit]) + "-");
	  }
	  //if (fCandSet[sta0].count(hitcode)) continue;
	  //fCandSet[sta0].insert(hitcode);
	  //cout << " failed " << hitcode << endl;
	  if (fCandSet[indok.size()].find(hitcode) != fCandSet[indok.size()].end()) continue;
	  fCandSet[indok.size()].insert(hitcode);
 
	  hits.Set(indok.size());
	  //fitter.DoFit(&track);
	  fitter.Fit(&track); //AZ
	  int ndf = (track.GetFlag() == 0) ? track.GetNDF() : 1;
	  //cout << " bbbbbb " << endl;
	  //track.GetParamFirst()->Print();
	  if (fVerbose > 0) cout << " bbbbbb " << track.GetChi2() << " " << hits.GetSize() << " " << track.GetChi2()/ndf << endl;
	  if (track.GetChi2() / track.GetNDF() > gkChi2Cut) iok = 0;
	} else iok = 0;
	
      } // if (track.GetChi2() / track.GetNDF() > gkChi2Cut)

      // Good track - extra printout
      if (iok) {
	Double_t qual = track.GetNStsHits();
	//qual += (100.0 - TMath::Min(TMath::Abs(track.GetChi2()),100.0)) / 101.0;
	//if (track.GetNStsHits() <= 4) qual += TMath::Min(1/TMath::Abs(track.GetParamFirst()->GetQp()),10.0) / 11.0;
	if (track.GetNStsHits() < 4) {
	  // Vertex constraint
	  /*
	  TVector3 mom3, pos3;
	  track.GetParamFirst()->Momentum(mom3);
	  track.GetParamFirst()->Position(pos3);
	  //Double_t ang = mom3.Angle(pos3);
	  Double_t phi1 = TMath::ATan2 (track.GetParamFirst()->GetY(), track.GetParamFirst()->GetX());
	  Double_t phi2 = TMath::ATan2 (track.GetParamFirst()->GetTy(), track.GetParamFirst()->GetTx());
	  Double_t dphi = phi1 - Proxim(phi1,phi2);
	  //qual += (2.1 - TMath::Min(TMath::Abs(dphi),2.1)) / 2.2;
	  //Double_t www = TMath::Exp(-TMath::Abs(dphi)/2.1);
	  Double_t www = TMath::Min (TMath::Exp(-TMath::Abs(dphi)/0.2), 0.999); // 0.2 - rms
	  if (TMath::Abs(dphi) > 2.1) www = 0.0;
	  */
	  // Linear extrapolation to Zvert
	  Double_t xextr; // = pos3.X() - pos3.Z() * track.GetParamFirst()->GetTx() + 0.04;
	  Double_t yextr; // = pos3.Y() - pos3.Z() * track.GetParamFirst()->GetTy();
	  // Extrapolation to Zvert
	  FairTrackParam param = *track.GetParamFirst();
	  fitter.Extrapolate (&param, fXyzv[2], &param);
	  xextr = param.GetX() - fXyzv[0];
	  yextr = param.GetY() - fXyzv[1];
	  Double_t rad = TMath::Sqrt (xextr * xextr / 0.035 / 0.035 + yextr * yextr / 0.2 / 0.2); // 0.035, 0.2 - sigmas
	  Double_t www = TMath::Min (TMath::Exp(-rad), 0.999);
	  qual += TMath::Exp(-TMath::Abs(track.GetChi2())) * www;
	}
	else qual += (100.0 - TMath::Min(TMath::Abs(track.GetChi2()),100.0)) / 101.0;
	fTracks.insert(pair<Double_t,CbmStsTrack>(-qual,track));
	//AZ-130722 pair<multimap<Int_t,Int_t>::iterator,multimap<Int_t,Int_t>::iterator> ret;
	pair<unordered_multimap<Int_t,Int_t>::iterator,unordered_multimap<Int_t,Int_t>::iterator> ret; //AZ-130722
	nhits = hits.GetSize();
	if (fVerbose > 0) {
	   cout << " Good track: " << endl;
	   //track.GetParamFirst()->Print();
	   cout << track.GetChi2() << " " << nhits << " " << track.GetParamFirst()->GetZ() << endl;
  
	   for (Int_t j = nhits-1; j >= 0; --j) {
	      ret = fHit2id.equal_range(hits[j]);
	      cout << " (";
	      //AZ-130722 for (multimap<Int_t,Int_t>::iterator mit = ret.first; mit != ret.second; ++mit) {
	      for (unordered_multimap<Int_t,Int_t>::iterator mit = ret.first; mit != ret.second; ++mit) {
		 if (mit == ret.first) cout << mit->first << "*";
		 else cout << ":";
		 cout << mit->second;
	      }
	      cout << ")";
	   }
	   cout << "\n";
	}
      }

    } // for (Int_t itra = 0; 

  }

}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::RemoveDoubles()
{
  // Remove double tracks (keep the ones with better quality)

  multimap<Double_t,CbmStsTrack>::iterator mit = fTracks.begin(), mit1;
  Int_t nOK = fTracks.size(), nOut = fVectorArray->GetEntriesFast();
  
  for ( ; mit != fTracks.end(); ++mit) {
    if (mit->second.GetFlag()) continue; // discarded track
    mit1 = mit;
    ++mit1;

    for ( ; mit1 != fTracks.end(); ++mit1) {
      if (mit1->second.GetFlag()) continue; // discarded track
      if (!AreTracksDoubles(mit1->second,mit->second)) continue;
      mit1->second.SetFlag(1); // double - discard it
      --nOK;
      /*
      // Debug printout
      TArrayI &hits = *mit1->second.GetStsHits();
      Int_t nhits = mit1->second.GetNStsHits();
      pair<multimap<Int_t,Int_t>::iterator,multimap<Int_t,Int_t>::iterator> ret;
      cout << " Double: " << mit->first << " " << mit1->first << endl;
      for (Int_t j = nhits-1; j >= 0; --j) {
	ret = fHit2id.equal_range(hits[j]);
	cout << " (";
	for (multimap<Int_t,Int_t>::iterator mit2 = ret.first; mit2 != ret.second; ++mit2) {
	  if (mit2 == ret.first) cout << mit2->first << "*";
	  else cout << ":";
	  cout << mit2->second;
	}
	cout << ")";
      }
      cout << "\n";
      */
    }

    new ((*fVectorArray)[nOut++]) CbmStsTrack(mit->second);
    //cout << " yyyyy " << mit->second.GetParamFirst()->GetZ() << " " << mit->second.GetNStsHits() << endl;
  }
  if (fVerbose > 0)cout << " RemoveDoubles: " << fTracks.size() << " " << nOK << endl;
}

// -------------------------------------------------------------------------

Bool_t BmnStsVectorFinder::AreTracksDoubles(CbmStsTrack &tr1, CbmStsTrack &tr2)
{
  /// Searching common hits in 2 tracks to determine doubles                    
  /// tr1 contains fewer hits than tr2                                     

  Int_t limCommonPoint = (tr1.GetNStsHits()+1) / 2; // at least so many common hits should be found                                                         
  //limCommonPoint = TMath::Min (limCommonPoint, 2); // 2 common hits at max    
  limCommonPoint = TMath::Min (limCommonPoint, 1); // 1 common hit at max
  TArrayI &hits1 = *tr1.GetStsHits(), &hits2 = *tr2.GetStsHits();
  Int_t nh1 = hits1.GetSize(), nh2 = hits2.GetSize(), nHitsCommon = 0, j = nh2 - 1;
  if (nh1 == 3) limCommonPoint = 0;

  for (Int_t i = nh1 - 1; i >= 0; i--){
  //for (Int_t i = 0; i <= nh1 - 1; ++i){
    CbmStsHit *hit1 = (CbmStsHit*) fHitArray->UncheckedAt(tr1.GetStsHitIndex(i));

    for ( ; j >= 0; j--){
    //for ( ; j <= nh2 - 1; ++j){
      CbmStsHit *hit2 = (CbmStsHit*) fHitArray->UncheckedAt(tr2.GetStsHitIndex(j));
      
      // Is the hit common for two tracks?                                      
      if (hit1 == hit2) {
        nHitsCommon++;
        //AZ-281121 if (nHitsCommon == limCommonPoint) return kTRUE; // already enough common hits
        if (nHitsCommon > limCommonPoint) return kTRUE; // already enough common hits
	break;
      }

      if (hit2->GetStationNr() < hit1->GetStationNr()) break; // already closer to target
    }
    
    //AZ-281121 if ((nh1 - i - 1) + limCommonPoint - nHitsCommon > nh1) return kFALSE; // there'll be not enough common hits already
    if (i + nHitsCommon <= limCommonPoint) return kFALSE; // there'll be not enough common hits already
  }

  //AZ-281121 if (nHitsCommon < limCommonPoint) return kFALSE; // not too many common hits  
  if (nHitsCommon <= limCommonPoint) return kFALSE; // not too many common hits  

  return kTRUE;
}

// -------------------------------------------------------------------------
/*
void BmnStsVectorFinder::RemoveFakes()
{
  // Remove fake tracks (according to some empirical rules)

  Int_t ncand = fVectorArray->GetEntriesFast();

  for (Int_t j = 0; j < ncand; ++j) {
    CbmStsTrack *tr = (CbmStsTrack*) fVectorArray->UncheckedAt(j);
    Int_t nhits = tr->GetNStsHits();
    if (1/TMath::Abs(tr->GetParamFirst()->GetQp()) < 0.2) { fVectorArray->Remove(tr); continue; }
    if (nhits < 4) {
      CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(tr->GetStsHitIndex(0));
      if (hit->GetStationNr() <= 3) continue;
      fVectorArray->Remove(tr); // short track starts in GEM
    }
  }
  fVectorArray->Compress();
    
  cout << " Remove fakes: " << ncand << " " << fVectorArray->GetEntriesFast() << endl;
}
*/
// -------------------------------------------------------------------------
void BmnStsVectorFinder::RemoveFakes()
{
  // Remove fake tracks (according to some empirical rules)

  multimap<Double_t,CbmStsTrack>::iterator mit = fTracks.begin();
  Int_t nOK = fTracks.size(), nOut = nOK;
  
  for ( ; mit != fTracks.end(); ++mit) {
    if (mit->second.GetFlag()) continue; // discarded track
    CbmStsTrack *tr = &mit->second;
    Int_t nhits = tr->GetNStsHits();
    if (1/TMath::Abs(tr->GetParamFirst()->GetQp()) < 0.2) { tr->SetFlag(1); --nOut; continue; }
    if (nhits < 4) {
      CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(tr->GetStsHitIndex(0));
      if (hit->GetStationNr() <= 3) continue;
      tr->SetFlag(1); // short track starts in GEM
      --nOut;
    }
  }
    
  if (fVerbose > 0) cout << " Remove fakes: " << nOK << " " << nOut << endl;
}

// -------------------------------------------------------------------------

void BmnStsVectorFinder::ExcludeFakes()
{
  // Exclude fake tracks (with too many shared clusters)

  Int_t ncand = fVectorArray->GetEntriesFast();

  map<Int_t,set<Int_t> > mClusTr;
  map<Int_t,Double_t> mWeight;

  for (Int_t j = 0; j < ncand; ++j) {
    CbmStsTrack *tr = (CbmStsTrack*) fVectorArray->UncheckedAt(j);
    Int_t nhits = tr->GetNStsHits();
      
    for (Int_t jh = 0; jh < nhits; ++jh) {
      Int_t indx = tr->GetStsHitIndex(jh);
      CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(indx);

      for (Int_t side = 0; side < 2; ++side) {
	Int_t iclus = hit->GetDigi(side);
	if (mClusTr.count(iclus) == 0) {
	  set<Int_t> aaa;
	  mClusTr[iclus] = aaa;
	}
	mClusTr[iclus].insert(j);
      }
    }

    Double_t www = TMath::Exp (-tr->GetChi2() / tr->GetNDF());
    // Extrapolation to Zvert
    if (nhits == 3) {
      FairTrackParam param = *tr->GetParamFirst();
      fitter.Extrapolate (&param, fXyzv[2], &param);
      Double_t xextr = param.GetX() - fXyzv[0];
      Double_t yextr = param.GetY() - fXyzv[1];
      Double_t rad = TMath::Sqrt (xextr * xextr / 0.035 / 0.035 + yextr * yextr / 0.2 / 0.2); // 0.035, 0.2 - sigmas
      www *= TMath::Exp(-rad);
    }
    mWeight[j] = www;
  }

  while (1) {
    multimap<Double_t,pair<Int_t,Int_t> > mapQual;
    set<Int_t> set2kill, set2kill1;
    
    for (Int_t j = 0; j < ncand; ++j) {
      CbmStsTrack *tr = (CbmStsTrack*) fVectorArray->UncheckedAt(j);
      if (tr == NULL) continue; // removed track
      Int_t nhits = tr->GetNStsHits();
      Int_t nover = 0;
      
      for (Int_t jh = 0; jh < nhits; ++jh) {
	Int_t indx = tr->GetStsHitIndex(jh);
	CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(indx);
	//if (mClusTr[hit->GetDigi(0)].size() > 1 || mClusTr[hit->GetDigi(1)].size() > 1) ++nover;

	for (Int_t side = 0; side < 2; ++side) {
	  Int_t iclus = hit->GetDigi(side);
	  if (mClusTr[iclus].size() > 1) ++nover;
	}
      }
      //Double_t quality = nhits * 1000 + (1 - Double_t(nover) / nhits) * 100
      //+ (1 - TMath::Min(tr->GetChi2(),100.0) / 100);
      Double_t quality = nhits * 1000;
      Double_t www = TMath::Exp(-Double_t(nover) / nhits / 2) * mWeight[j];
      mapQual.insert(pair<Double_t,pair<Int_t,Int_t> >(quality+www,pair<Int_t,Int_t>(j,nover)));
      //if ((nover == nhits) || (nhits == 3 && nover == 2)) set2kill.insert(j);
      //if ((nhits <= 4 && nover == nhits) || (nhits == 3 && nover == 2)) set2kill.insert(j);
      //if (nhits <= 4 && Double_t(nover)/nhits/2 > 0.81) set2kill.insert(j);
      if (nhits <= 5 && Double_t(nover)/nhits/2 > 0.6) set2kill.insert(j);
      else if (!fExact && nhits <= 3 && www < 1.e-8) set2kill1.insert(j); // 1e-8 - empirical value
    }

    if (set2kill.size()) set2kill1.clear(); // first kill in set2kill
    if (set2kill.size() || set2kill1.size()) {
      // Remove track and update cluster-to-track info

      for (multimap<Double_t,pair<Int_t,Int_t> >::iterator mit = mapQual.begin(); mit != mapQual.end(); ++mit) {
	Int_t itr = mit->second.first;
	CbmStsTrack *tr = (CbmStsTrack*) fVectorArray->UncheckedAt(itr);
	Int_t nhits = tr->GetNStsHits(), nover = mit->second.second;
	if (set2kill.count(itr) == 0 && set2kill1.count(itr) == 0) continue;
	
	for (Int_t jh = 0; jh < nhits; ++jh) {
	  Int_t indx = tr->GetStsHitIndex(jh);
	  CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(indx);
	  if (mClusTr[hit->GetDigi(0)].count(itr) == 0 || mClusTr[hit->GetDigi(1)].count(itr) == 0)
	     //AZ-300822 { cout << " !!! No track found: " << itr << endl; exit(0); }
	     { cout << " !!! No track found: " << itr << endl; /*exit(0);*/ } //AZ-300822
	  mClusTr[hit->GetDigi(0)].erase(itr);
	  mClusTr[hit->GetDigi(1)].erase(itr);
	}
	fVectorArray->Remove(tr);
	break;
      }
    } else break;
  } // while (1)
  
  fVectorArray->Compress();
  
  if (fVerbose > 0) cout << " Exclude fakes: " << ncand << " " << fVectorArray->GetEntriesFast() << endl;
}

// -------------------------------------------------------------------------

Double_t BmnStsVectorFinder::DxVsMom(Int_t ista, candvec &vec)
{
  // Compute momentum correction for parabolic extrapolation

  Double_t pars[19][4] = { {0,0,0,0}, {0,0,0,0}, {8.28698e-06,13.3515,0,0},
			   {0.031189,4.56933,0.031189,4.56933}, {0.395468,3.77954,0.190848,3.41027},
			   {0.128467,4.26652,0.128467,4.26652}, {0.160979,4.95095,0.160979,4.95095} };
  
  Double_t pxz = vec.momxz;
  Int_t endsta = vec.stahit.rbegin()->first;
  Double_t p0 = pars[ista][0];
  Double_t p1 = pars[ista][1];
  if (endsta > ista + 2) { p0 = pars[ista][2]; p1 = pars[ista][3]; }

  Double_t dx = p0 / TMath::Power(TMath::Log10(10*pxz),p1);
  return dx;
}

// -------------------------------------------------------------------------

Double_t BmnStsVectorFinder::Proxim(Double_t phi0, Double_t phi)
{
  /// Adjust angle phi to be "around" phi0 - to avoid discontinuity around +- Pi

  Double_t dPhi = phi0 - phi;
  if (TMath::Abs(dPhi) > TMath::Pi()) phi += TMath::Pi() * 2 * TMath::Sign(1.,dPhi);
  return phi;
}

// -------------------------------------------------------------------------
//AZ-280722 Double_t BmnStsVectorFinder::LinearFit(CbmStsTrack *tr, Int_t newtr, Double_t& ty)
Double_t BmnStsVectorFinder::LinearFit(candvec &cand, candvec &cand2, CbmStsTrack *tr, Int_t newtr, Double_t& ty) //AZ-280722
{
  /// Linear LS fit of 3 points (in R-Y plane): y = a + b * l, l is a track projection length on X-Z plane

  //static Double_t x[3], y[3], z[3], xs[3], ys[3], xys[3], x2s[3];
  static Double_t x[3], y[3], z[3], l[3] = {0}, lens[3] = {0}, ys[3], lys[3], l2s[3];

  //AZ-150722 Int_t i0 = (newtr == 1) ? 0 : 2;
  Int_t i0 = (newtr == 0) ? 0 : 2;

  for (Int_t i = i0; i < 3; ++i) {
    CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(tr->GetStsHitIndex(i));
    //AZ-280722 x[i] = hit->GetX();
    //AZ-280722 z[i] = hit->GetZ();
    if (i) {
       /*AZ-280722
      Double_t dx = x[i] - x[i-1];
      Double_t dz = z[i] - z[i-1];
      l[i] = lens[i] = TMath::Sqrt (dx * dx + dz * dz) + l[i-1];
      */
       if (i == 1) l[i] = lens[i] = cand.lengxz; //AZ-280722
       else l[i] = lens[i] = cand2.lengxz + l[i-1]; //AZ-280722
    }
    y[i] = ys[i] = hit->GetY();
    //cout << " ily " << i << " " << l[i] << " " << y[i] << endl;
  }
  
  for (Int_t i = i0; i < 3; ++i) {
    lys[i] = l[i] * y[i];
    l2s[i] = l[i] * l[i];
    if (i) {
      lens[i] += lens[i-1];
      ys[i] += ys[i-1];
      lys[i] += lys[i-1];
      l2s[i] += l2s[i-1];
    }
  }
  Double_t b = 3 * lys[2] - lens[2] * ys[2];
  b /= (3 * l2s[2] - lens[2] * lens[2]);
  Double_t a = (ys[2] - b * lens[2]) / 3;

  Double_t chi2 = 0.0;

  for (int i = 0; i < 3; ++i) {
    Double_t dy = a + b * l[i] - y[i];
    chi2 += (dy * dy);
  }
  // Check
  //TGraph gr(3,l,y);
  //gr.Fit("pol1");
  //cout << " LS fit: " << a << " " << b << " " << chi2 << endl;
  ty = b;
  return chi2;
}

// -------------------------------------------------------------------------

Double_t BmnStsVectorFinder::Curv3(candvec &cand1, candvec &cand2, candvec &cand3, int newtr)
{
  // Compute triplet curvature in ZX plane

  static TVector3 points3[3], midPoint;
  int indx = 1;
  static Double_t by = 0.0;
  static int first = 99;

  //if (first != fPass) {
  // first = fPass;
  if (first) {
     first = 0;
     points3[0].SetXYZ (fXyzv[0], fXyzv[1], fXyzv[2]);
  }
  
  map<int,int>::iterator mit = cand3.stahit.begin();
  if (0) { //fPass > 1) { //0 - assuming tracks coming from (0,0,0)
  //if (newtr == 0 && cand3.stahit.size() > 2) {
     points3[0] = fmapHits[mit->second].xyz; //AZ-280722
     points3[0].SetY(0.0); //AZ-280722
  }
  //AZ-310722 ++mit;
  if (cand3.stahit.size() > 2) ++mit; //AZ-310722
  
  for ( ; mit != cand3.stahit.end(); ++mit) {
     //if (newtr > 0 && indx < 2) { ++indx; continue; }
     points3[indx] = fmapHits[mit->second].xyz;
     if (indx == 1) midPoint = points3[indx];
     points3[indx] -= points3[0];
     points3[indx++].SetY(0.0);
  }

  TVector3 vec21 = points3[2] - points3[1];
  //AZ-280722 Double_t cosAlpha = points3[2] * vec21 / points3[2].Mag() / vec21.Mag();
  //Double_t cosAlpha = points3[1] * vec21 / points3[1].Mag() / vec21.Mag(); //AZ-280722
  Double_t cosAlpha = points3[1] * vec21 / points3[1].Mag() / cand2.lengxz; //AZ-280722
  //AZ-280722 Double_t rad = points3[1].Mag() / 2. / TMath::Sin(TMath::ACos(cosAlpha));
  Double_t rad = points3[2].Mag() / 2. / TMath::Sin(TMath::ACos(cosAlpha)); //AZ-280722
  //Double_t bz = FairRunAna::Instance()->GetField()->GetBz(0.,0.,0.);
  //Double_t factor = 0.003 * bz / 10.; // 0.3 * 0.01 * 5kG / 10                  
  //AZ-310722 Double_t sign = TMath::Sign (1.0, points3[1].Cross(points3[2]).Y());
  Double_t sign = TMath::Sign (3e-4, points3[1].Cross(points3[2]).Y()); //AZ-310722
  if (newtr == 0) {
     FairField *magField = FairRunAna::Instance()->GetField();
     by = magField->GetBy(midPoint[0],midPoint[1],midPoint[2]);
  }
  Double_t pxz = by * rad * sign;
  //cout << " pt " << pxz << endl; 
  //AZ-170722 return pxz;
  return 1/pxz; //AZ-170722
}
// -------------------------------------------------------------------------

set<int> BmnStsVectorFinder::KalmanWindow(candvec &cand, int hitIndx)
{
  // Find hits around extrapolated track

  CbmStsTrack track; 
  string hitcode;
  MakeStsTrack(cand, hitcode, track);
  fitter.DoFit(&track);
  FairTrackParam param;
  CbmStsHit *hit = (CbmStsHit*) fHitArray->UncheckedAt(hitIndx);
  Double_t z = hit->GetZ();
  fitter.Extrapolate(&track, z, &param);
  Double_t sigx = 5 * TMath::Sqrt (param.GetCovariance(0,0));
  Double_t sigy = 5 * TMath::Sqrt (param.GetCovariance(1,1));
  if (fVerbose > 0) cout << " Kalman: " << z << " " << hit->GetStationNr() << " " << sigx << " " << sigy << endl;

  // Get hits on the downstream station
  int istanext = hit->GetStationNr() - 1;
  multimap<Double_t,Int_t>::iterator mityb = fmapY[istanext].lower_bound(param.GetY()-sigy);
  multimap<Double_t,Int_t>::iterator mitye = fmapY[istanext].upper_bound(param.GetY()+sigy);     
  multimap<Double_t,Int_t>::iterator mitxb = fmapX[istanext].lower_bound(param.GetX()-sigx);     
  multimap<Double_t,Int_t>::iterator mitxe = fmapX[istanext].upper_bound(param.GetX()+sigx);
  // Get hits from the acceptance window 
  set<Int_t> setX, setY, intersect;
  for (multimap<Double_t,Int_t>::iterator mit = mitxb; mit != mitxe; ++mit) 
    if (fmapHits[mit->second].used == 0) setX.insert(mit->second);
  for (multimap<Double_t,Int_t>::iterator mit = mityb; mit != mitye; ++mit)
    if (fmapHits[mit->second].used == 0) setY.insert(mit->second);
  set_intersection(setX.begin(), setX.end(), setY.begin(), setY.end(), std::inserter(intersect, intersect.begin()));
  return intersect;
}

// -------------------------------------------------------------------------

std::string BmnStsVectorFinder::MakeCode(candvec &cand)
{
   // Make track code

   string code("-");
   for (map<int,int>::iterator it = cand.stahit.begin(); it != cand.stahit.end(); ++it)
      code += (to_string(it->second) + "-");
   return code;
}

// -------------------------------------------------------------------------

ClassImp(BmnStsVectorFinder)
