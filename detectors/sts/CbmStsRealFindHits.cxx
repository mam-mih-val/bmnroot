//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsRealFindHits source file             -----
// -----                  Created 26/06/2008 by R. Karabowicz          -----
// -------------------------------------------------------------------------

#include "CbmStsRealFindHits.h"

#include "CbmGeoStsPar.h"
#include "CbmStsCluster.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsHit.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "TClonesArray.h"
#include "TMath.h"

#include <iomanip>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::fixed;
using std::right;
using std::left;
using std::setw;
using std::setprecision;
using std::set;
using std::map;

// -----   Default constructor   ------------------------------------------
CbmStsRealFindHits::CbmStsRealFindHits() : FairTask("STS Hit Finder", 1) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fClusters   = NULL;
  fHits    = NULL;
  fNHits = 0;
  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsRealFindHits::CbmStsRealFindHits(Int_t iVerbose) 
  : FairTask("STSRealFindHits", iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fClusters   = NULL;
  fHits    = NULL;
  fNHits = 0;
  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsRealFindHits::CbmStsRealFindHits(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fClusters   = NULL;
  fHits    = NULL;
  fNHits = 0;
  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsRealFindHits::~CbmStsRealFindHits() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsRealFindHits::Exec(Option_t* opt) {

  fTimer.Start();
  Bool_t warn = kFALSE;

  // Check for digi scheme
  if ( ! fDigiScheme ) {
    cerr << "-E- " << fName << "::Exec: No digi scheme!" << endl;
    return;
  }

  // Clear output array
  //  fHits->Clear();
  fHits->Delete();

  // Sort STS digis with respect to sectors
  SortClusters();

  // Find hits in sectors
  Int_t nClustersF = 0;
  Int_t nClustersB = 0;
  Int_t nHits   = 0;
  Int_t nStations = fDigiScheme->GetNStations();
  CbmStsStation* station = NULL;
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    station = fDigiScheme->GetStation(iStation);

    Int_t nClustersFInStation = 0;
    Int_t nClustersBInStation = 0;
    Int_t nHitsInStation   = 0;
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      set <Int_t> fSet, bSet;
      if ( fClusterMapF.find(sector) == fClusterMapF.end() ) {
	cout << "-E- " << fName << "::Exec: sector " 
	     << sector->GetSectorNr() << " of station " 
	     << station->GetStationNr() << "not found in front map!" 
	     << endl;
	warn = kTRUE;
	continue;
      }
      fSet = fClusterMapF[sector];
      if ( sector->GetType() == 2 || sector->GetType() == 3 ) {
	if ( fClusterMapB.find(sector) == fClusterMapB.end() ) {
	  cout << "-E- " << fName << "::Exec: sector " 
	       << sector->GetSectorNr() << " of station " 
	       << station->GetStationNr() << "not found in back map!" 
	       << endl;
	  warn = kTRUE;
	  continue;
	}
      }
      bSet = fClusterMapB[sector];
      Int_t nClustersFInSector = fSet.size();
      Int_t nClustersBInSector = bSet.size();
      Int_t nHitsInSector   = FindHits(station, sector, fSet, bSet);
      if ( fVerbose > 2 ) 
	cout << "Sector " << sector->GetSectorNr() 
	     << ", Clusters front " << nClustersFInSector 
	     << ", Clusters Back " << nClustersBInSector
	     << ", Hits " << nHitsInSector << endl;
      nHitsInStation   += nHitsInSector;
      nClustersFInStation += nClustersFInSector;
      nClustersBInStation += nClustersBInSector;      
    }      // Sector loop

    if ( fVerbose > 1 ) cout << "Total for station " 
			     << station->GetStationNr() << ": Clusters front "
			     << nClustersFInStation << ", clusters back "
			     << nClustersBInStation << ", hits "
			     << nHitsInStation << endl;
    nClustersB += nClustersBInStation;
    nClustersF += nClustersFInStation;
    nHits   += nHitsInStation;
    
  }       // Station loop

  fTimer.Stop();  
  if ( fVerbose  ) {
    cout << endl;
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Clusters front side       : " << nClustersF << endl;
    cout << "    Clusters back side        : " << nClustersB << endl;
    cout << "    Hits created              : " << nHits   << endl;
    cout << "    Real time                 : " << fTimer.RealTime() 
	 << endl;
  }
  else {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) 
	 << fixed << right << fTimer.RealTime() 
	 << " s, clusters " << nClustersF << " / " << nClustersB << ", hits: " 
	 << nHits << endl;
  }

  fNHits += nHits;
}
// -------------------------------------------------------------------------

    


// -----   Private method SetParContainers   -------------------------------
void CbmStsRealFindHits::SetParContainers() {

  // Get run and runtime database
  FairRunAna* run = FairRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get STS geometry parameter container
  fGeoPar = (CbmGeoStsPar*) db->getContainer("CbmGeoStsPar");

  // Get STS digitisation parameter container
  fDigiPar = (CbmStsDigiPar*) db->getContainer("CbmStsDigiPar");

}
// -------------------------------------------------------------------------




// -----   Private method Init   -------------------------------------------
InitStatus CbmStsRealFindHits::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fClusters = (TClonesArray*) ioman->GetObject("StsCluster");

  // Register output array
  fHits = new TClonesArray("CbmStsHit", 1000);
  ioman->Register("StsHit", "Hit in STS", fHits, kTRUE);

  // Build digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;

  // Create sectorwise cluster sets
  MakeSets();

  // Control output
  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully initialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsRealFindHits::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;

  // Create sectorwise digi sets
  MakeSets();

  // Control output
  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully reinitialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method MakeSets   ---------------------------------------
void CbmStsRealFindHits::MakeSets() {

  fClusterMapF.clear();
  fClusterMapB.clear();
  Int_t nStations = fDigiScheme->GetNStations();
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    CbmStsStation* station = fDigiScheme->GetStation(iStation);
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      set<Int_t> a;
      fClusterMapF[sector] = a;
      if ( sector->GetType() == 2 || sector->GetType() ==3 ) {
	set<Int_t> b;
	fClusterMapB[sector] = b;
      }
    }
  }

}
// -------------------------------------------------------------------------




// -----   Private method SortClusters   --------------------------------------
void CbmStsRealFindHits::SortClusters() {

  // Check input array
  if ( ! fClusters ) {
    cout << "-E- " << fName << "::SortClusters: No input array!" << endl;
    return;
  }

  // Clear sector cluster sets
  map<CbmStsSector*, set<Int_t> >::iterator mapIt;
  for (mapIt=fClusterMapF.begin(); mapIt!=fClusterMapF.end(); mapIt++)
    ((*mapIt).second).clear();
  for (mapIt=fClusterMapB.begin(); mapIt!=fClusterMapB.end(); mapIt++)
    ((*mapIt).second).clear();

  // Fill clusters into sets
  CbmStsCluster* cluster = NULL;
  CbmStsSector* sector = NULL;
  Int_t stationNr = -1;
  Int_t sectorNr  = -1;
  Int_t iSide     = -1;
  Int_t nClusters = fClusters->GetEntriesFast();
  for (Int_t iClus=0; iClus<nClusters ; iClus++) {
    cluster = (CbmStsCluster*) fClusters->At(iClus);
    stationNr = cluster->GetStationNr();
    sectorNr  = cluster->GetSectorNr();
    iSide     = cluster->GetSide();
    sector = fDigiScheme->GetSector(stationNr, sectorNr);
    if (iSide == 0 ) {
      if ( fClusterMapF.find(sector) == fClusterMapF.end() ) {
	cerr << "-E- " << fName << "::SortClusters:: sector " << sectorNr
	     << " of station " << stationNr 
	     << " not found in digi scheme (F)!" << endl;
	continue;
      }
      fClusterMapF[sector].insert(iClus);      
    }
    else if (iSide == 1 ) {
      if ( fClusterMapB.find(sector) == fClusterMapB.end() ) {
	cerr << "-E- " << fName << "::SortClusters:: sector " << sectorNr
	     << " of station " << stationNr 
	     << " not found in digi scheme (B)!" << endl;
	continue;
      }
      fClusterMapB[sector].insert(iClus);      
    }
  }

}
// -------------------------------------------------------------------------


    

// -----   Private method FindHits   ---------------------------------------
Int_t CbmStsRealFindHits::FindHits(CbmStsStation* station,
				   CbmStsSector* sector, 
				   set<Int_t>& fSet, set<Int_t>& bSet) {

  // Counter
  Int_t nNew = 0;
  
  // Get sector parameters
  Int_t    detId  = sector->GetDetectorId();
  Int_t    iType  = sector->GetType();

  Double_t rot    = sector->GetRotation();
  Double_t dx     = sector->GetDx();
  Double_t dy     = sector->GetDy();
  Double_t stereoB = sector->GetStereoB();

  //  Double_t z      = station->GetZ();
  Int_t stationNr = station->GetStationNr();
  Int_t sectorNr  = sector->GetSectorNr();

  // Some auxiliary values
  Double_t sinrot = TMath::Sin(rot);
  Double_t cosrot = TMath::Cos(rot);
  Double_t tanstr = TMath::Tan(stereoB);

  // Calculate error matrix in sector system
  Double_t vX, vY, vXY;
  if ( iType == 1 ) {
    vX  = dx / TMath::Sqrt(12.);
    vY  = dy / TMath::Sqrt(12.);
    vXY = 0.;
  }
  else if ( iType == 2 || iType == 3 ) {
    vX  = dx / TMath::Sqrt(12.);
    vY  = dx / TMath::Sqrt(6.) / TMath::Abs(tanstr);
    vXY = -1. * dx * dx / 12. / tanstr;
  }
  else {
    cerr << "-E- " << fName << "::FindHits: Illegal sector type "
	 << iType << endl;
    return 0;
  }
    
  // Transform variances into global c.s.
  Double_t wX  = vX * vX  * cosrot * cosrot 
               - 2. * vXY * cosrot * sinrot
               + vY * vY  * sinrot * sinrot;
  Double_t wY  = vX * vX  * sinrot * sinrot
               + 2. * vXY * cosrot * sinrot
	       + vY * vY  * cosrot * cosrot; 
  Double_t wXY = (vX*vX - vY*vY) * cosrot * sinrot
               + vXY * ( cosrot*cosrot - sinrot*sinrot );
  Double_t sigmaX = TMath::Sqrt(wX);
  Double_t sigmaY = TMath::Sqrt(wY);

  // Now perform the loop over active channels
  set<Int_t>::iterator it1;
  set<Int_t>::iterator it2;

  // ----- Type 1 : Pixel sector   ---------------------------------------
  if ( iType == 1 ) {
    Fatal("FindHits","Sorry, not implemented yet");
  }     // Pixel sensor
  // ---------------------------------------------------------------------

  // -----  Type 2: Strip sector OSU   -----------------------------------
  else if ( iType == 2 ) {
    Fatal("FindHits","Sorry, not implemented yet");
  }         // Strip OSU
  // ---------------------------------------------------------------------
      
  // -----  Type 3: Strip sector GSI   -----------------------------------
  else if (iType == 3 ) {
    Int_t iClusF = -1;
    Int_t iClusB = -1;
    Double_t chanF = -1;
    Double_t chanB = -1;
    Int_t nHits = fHits->GetEntriesFast();
    Double_t xHit;
    Double_t yHit;
    Double_t zHit;
    TVector3 pos, dpos;
    CbmStsCluster* clusterF = NULL;
    CbmStsCluster* clusterB = NULL;
    for (it1=fSet.begin(); it1!=fSet.end(); it1++) {
      iClusF = (*it1);
      clusterF  = (CbmStsCluster*) fClusters->At(iClusF);
      if ( ! clusterF ) {
	cout << "-W- " << GetName() << "::FindHits: Invalid cluster index " 
	     << iClusF << " in front set of sector " 
	     << sector->GetSectorNr() << ", station " 
	     << station->GetStationNr() << endl;
	continue;
      }
      chanF = clusterF->GetMean();
      for (it2=bSet.begin(); it2!=bSet.end(); it2++ ) {
	iClusB = (*it2);
	clusterB = (CbmStsCluster*) fClusters->At(iClusB);
	if ( ! clusterB ) {
	  cout << "-W- " << GetName() << "::FindHits: Invalid cluster index " 
	       << iClusB << " in back set of sector " 
	       << sector->GetSectorNr() << ", station " 
	       << station->GetStationNr() << endl;
	  continue;
	}
	chanB = clusterB->GetMean();
	
	Int_t sensorDetId = sector->IntersectClusters(chanF,chanB,xHit,yHit,zHit);

	if ( sensorDetId == -1 ) continue;

	pos.SetXYZ(xHit, yHit, zHit);
	dpos.SetXYZ(sigmaX*clusterF->GetMeanError(), sigmaY*clusterB->GetMeanError(), 0.);
	
	Int_t statLayer = -1;
	for ( Int_t istatL = station->GetNofZ() ; istatL > 0 ; istatL-- ) 
	  if ( TMath::Abs(zHit-station->GetZ(istatL-1)) < 0.00001 ) {
	    statLayer = istatL-1;
	    break;
	  }
	
	if ( statLayer == -1 ) 
	  cout << "unknown layer for hit at z = " << zHit << endl;

	new ((*fHits)[nHits++]) CbmStsHit(sensorDetId, pos, dpos, wXY, 
					  iClusF, iClusB, (Int_t)chanF, (Int_t)chanB, statLayer);

	nNew++;
	if ( fVerbose > 3 ) cout << "New StsHit at (" << xHit << ", " << yHit
				 << ", " << zHit << "), station " 
				 << stationNr << ", sector " << sectorNr 
				 << ", channel " << chanF << " / " 
				 << chanB 
				 << endl;
	
      }      // back side strip loop
    }        // front side strip loop
    
  }          // strip GSI
  // ---------------------------------------------------------------------
  
  
  return nNew;
}
// -------------------------------------------------------------------------

// -----   Virtual method Finish   -----------------------------------------
void CbmStsRealFindHits::Finish() {
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Number of hits                 : " 
       << setw(8) << setprecision(2) 
       << fNHits << endl;
  cout << "============================================================"
       << endl;
	
}					       
// -------------------------------------------------------------------------

ClassImp(CbmStsRealFindHits)


	
	
	
  
		      





 
