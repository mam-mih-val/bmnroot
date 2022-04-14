//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsFindHits source file             -----
// -----                  Created 26/06/2008 by R. Karabowicz          -----
// -------------------------------------------------------------------------

#include "CbmStsFindHits.h"

#include "CbmGeoStsPar.h"
#include "CbmStsCluster.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsHit.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include "FairField.h" //AZ
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairEventHeader.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom.h> //AZ

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
CbmStsFindHits::CbmStsFindHits() 
  : FairTask("STS Hit Finder", 1), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fClusters(NULL),
  fHits(NULL),
  fNHits(0),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fClusterMapF(),
  fClusterMapB(),
  fTimer()
{}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsFindHits::CbmStsFindHits(Int_t iVerbose) 
  : FairTask("STSRealFindHits", iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fClusters(NULL),
  fHits(NULL),
  fNHits(0),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fClusterMapF(),
  fClusterMapB(),
  fTimer()
{}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsFindHits::CbmStsFindHits(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fClusters(NULL),
  fHits(NULL),
  fNHits(0),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fClusterMapF(),
  fClusterMapB(),
  fTimer()
{}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsFindHits::~CbmStsFindHits() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsFindHits::Exec(Option_t* opt) {

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
    //if (station->GetStationNr() > 4) continue; //AZ-220322 - only 4 stations

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
    //AZ cout << "-I- " << fName << ":Event summary" << endl;
    cout << "-I- " << fName << ":Event " << FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber() << " summary" << endl;
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
void CbmStsFindHits::SetParContainers() {

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
InitStatus CbmStsFindHits::Init() {

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
InitStatus CbmStsFindHits::ReInit() {

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
void CbmStsFindHits::MakeSets() {

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
void CbmStsFindHits::SortClusters() {

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
    if (fDigiScheme->GetStationByNr(stationNr) == NULL) continue; //AZ - reduced setup
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
Int_t CbmStsFindHits::FindHits(CbmStsStation* station,
				   CbmStsSector* sector, 
				   set<Int_t>& fSet, set<Int_t>& bSet) {

  //AZ - get magnetic field (for Lorentz shifts in GEMs)
  static Int_t ifield = -1;
  if (ifield < 0) {
    FairRunAna* run = FairRunAna::Instance();
    FairField *magField = run->GetField();
    ifield = TMath::Nint(TMath::Abs(magField->GetBy(0,0,135.0)));
    //ifield = 8; //AZ-281021 Dirty trick to use fast digitizer
  }

  // Counter
  Int_t nNew = 0;
  
  // Get sector parameters
  Int_t    detId  = sector->GetDetectorId();
  Int_t    iType  = sector->GetType();

  Double_t rot    = sector->GetRotation();
  Double_t dx     = sector->GetDx();
  Double_t dy     = sector->GetDy();
  Double_t stereoB = sector->GetStereoB();
  Double_t stereoF = sector->GetStereoF();

  //  Double_t z      = station->GetZ();
  Int_t stationNr = station->GetStationNr();
  Int_t sectorNr  = sector->GetSectorNr();

  // Some auxiliary values
  Double_t sinrot = TMath::Sin(rot);
  Double_t cosrot = TMath::Cos(rot);
  Double_t tanstr = TMath::Tan(stereoB);

  // Calculate error matrix in sector system
//   Double_t vX, vY, vXY;
//   if ( iType == 1 ) {
//     vX  = dx / TMath::Sqrt(12.);
//     vY  = dy / TMath::Sqrt(12.);
//     vXY = 0.;
//   }
//   else if ( iType == 2 || iType == 3 ) {
// 
//     if (stereoF==0.&&stereoB*180/TMath::Pi()<80) {
//       vX  = dx / TMath::Sqrt(12.);
//       vY  = dx / TMath::Sqrt(6.) / TMath::Abs(TMath::Tan(stereoB));
//       vXY = -1. * dx * dx / 12. / TMath::Tan(stereoB);
//     }
//     else if (stereoF==0.&&stereoB*180/TMath::Pi()>80) {
//       vX  = dx / TMath::Sqrt(12.);
//       vY  = dx / TMath::Sqrt(12.);
//       vXY = 0.;
//     }
//     else {
//       vX  = dx / TMath::Sqrt(24.);
//       vY  = dx / TMath::Sqrt(24.) / TMath::Abs(TMath::Tan(stereoB));
//       vXY = 0.;
//     }
//   }
//   else {
//     cerr << "-E- " << fName << "::FindHits: Illegal sector type "
// 	 << iType << endl;
//     return 0;
//   }
    
  // Transform variances into global c.s.
//   Double_t wX  = vX * vX  * cosrot * cosrot 
//                - 2. * vXY * cosrot * sinrot
//                + vY * vY  * sinrot * sinrot;
//   Double_t wY  = vX * vX  * sinrot * sinrot
//                + 2. * vXY * cosrot * sinrot
// 	       + vY * vY  * cosrot * cosrot; 
//   Double_t wXY = (vX*vX - vY*vY) * cosrot * sinrot
//                + vXY * ( cosrot*cosrot - sinrot*sinrot );
//   Double_t sigmaX = TMath::Sqrt(wX);
//   Double_t sigmaY = TMath::Sqrt(wY);

  // Now perform the loop over active channels
  set<Int_t>::iterator it1;
  set<Int_t>::iterator it2;

  // ----- Type 1 : Pixel sector   ---------------------------------------
  if ( iType == 1 ) {
    Fatal("FindHits","Sorry, not implemented yet");
  }     // Pixel sensor
  // ---------------------------------------------------------------------

  // -----  Type 2: Strip sector OSU   -----------------------------------
  /*AZ
  else if ( iType == 2 ) {
    Fatal("FindHits","Sorry, not implemented yet");
  }         // Strip OSU
  */
  // ---------------------------------------------------------------------
      
  // -----  Type 3: Strip sector GSI   -----------------------------------
  //AZ else if (iType == 3 ) {
  else if ( iType == 2 || iType == 3 ) {
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

    Double_t vX, vY, vXY;
    if (stereoF==0.&&stereoB*180/TMath::Pi()<80) {
      vX  = dx / TMath::Sqrt(12.);
      vY  = dx / TMath::Sqrt(6.) / TMath::Abs(TMath::Tan(stereoB));
      vXY = -1. * dx * dx / 12. / TMath::Tan(stereoB);
    }
    else if (stereoF==0.&&stereoB*180/TMath::Pi()>80) {
      vX  = dx / TMath::Sqrt(12.);
      vY  = dx / TMath::Sqrt(12.);
      vXY = 0.;
    }
    else {
      vX  = dx / TMath::Sqrt(12.);
      vY  = dx / TMath::Sqrt(12.) / TMath::Abs(TMath::Tan(stereoB));
      vXY = 0.;
    }
    
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
	
	//AZ Add extra smearing
	/* Wrong - no correlation ???
	Double_t sigma = 0.3; // 800*0.3=240 um
	chanF += gRandom->Gaus(0.0,sigma);
	chanB += gRandom->Gaus(0.0,sigma);
	*/
	//AZ

	Int_t sensorDetId = sector->IntersectClusters(chanF,chanB,xHit,yHit,zHit);

	if ( sensorDetId == -1 ) continue;

        //AZ Add extra smearing
	/*
        Double_t sigma = 0.06; // 400 um
	Double_t dx = gRandom->Gaus(0.0,sigma);
        xHit += dx;
        yHit += gRandom->Gaus(0.0,sigma*3.5);
	*/
        //AZ
        //AZ - Lorentz shift (Ar:CO2 70:30)
        ///* 
        //Double_t lorShift = 0.1, zShift = -0.3; // (Ar:CO2 70:30)
        Double_t lorShift = 0.1575, zShift = -0.27; // (Ar:C4H10 80:20 - 0.8T)
	if (ifield == 4) { lorShift = 0.0765; zShift = -0.27; } // (Ar:C4H10 80:20 - 0.4T - Au 1.5 GeV)
        else if (ifield == 6) { lorShift = 0.1145; zShift = -0.27; } // (Ar:C4H10 80:20 - 0.6T - Au 2.5 GeV)
        //else if (ifield == 6) { lorShift = 0.1190; zShift = -0.27; } // (Ar:C4H10 80:20 - 0.6T - Au 2.5 GeV)
        else if (ifield == 7) { lorShift = 0.1360; zShift = -0.27; } // (Ar:C4H10 80:20 - 0.7T - Au 3.5 GeV)
        else if (ifield == 9) { lorShift = 0.1800; zShift = -0.27; } // (Ar:C4H10 80:20 - 0.9T - Au 4.5 GeV)

        //if (station->GetStationNr() == 1) lorShift = zShift = 0.0; //Si - run6
        //if (station->GetStationNr() <= 3) lorShift = zShift = 0.0; //Si - run7
	if (sector->GetDx() < 0.02) lorShift = zShift = 0.0; // Si
	else if (station->GetStationNr() % 2 == 0) {
	  lorShift *= -1;
	  zShift *= -1;
	}
	//zShift = (station->GetStationNr() % 2 == 0) ? -0.27 : 0.27; //AZ-220322
	xHit += lorShift;
	//xHit += gRandom->Gaus(0.0,0.15); //AZ-220322
	//AZ zHit += zShift; //???
        //*/
        //AZ

	pos.SetXYZ(xHit, yHit, zHit);

	Double_t vXTemp, vYTemp, vXYTemp;

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

	if (stereoF==0.&&stereoB*180/TMath::Pi()<80) {		//0&15 case

          vXTemp = vX * clusterF->GetMeanError();
	  vYTemp = (vX/(TMath::Tan(stereoB))) * TMath::Sqrt(clusterF->GetMeanError()*clusterF->GetMeanError() + clusterB->GetMeanError()*clusterB->GetMeanError());
	  vXYTemp = - vXTemp * vXTemp / (TMath::Tan(stereoB));
	
	  wX = vXTemp * vXTemp  * cosrot * cosrot 
                 - 2. * vXYTemp * cosrot * sinrot
                 + vYTemp * vYTemp  * sinrot * sinrot;
	  wY = vXTemp * vXTemp  * sinrot * sinrot
                 + 2. * vXYTemp * cosrot * sinrot
	         + vYTemp * vYTemp  * cosrot * cosrot; 
	  wXY = (vXTemp*vXTemp - vYTemp*vYTemp) * cosrot * sinrot
                 + vXYTemp * ( cosrot*cosrot - sinrot*sinrot );
	  //dpos.SetXYZ(TMath::Sqrt(wX),TMath::Sqrt(wY), 0.);
	  //dpos.SetXYZ(vX, vX, 0.); //AZ - errors on each side
	  //if (zHit > 10.0) dpos.SetXYZ(vX*6, vX*6, 0.); //AZ - errors on each side
	  //if (zHit > 10.0) dpos.SetXYZ(vX*2, vX*2, 0.); //AZ - errors on each side
	  //else dpos.SetXYZ(vX*2, vX*10, 0.); //AZ - errors on each side
	  /*run6
	  if (zHit > 10.0) dpos.SetXYZ(0.015, 0.02, 0.); //AZ - errors on each side
	  else dpos.SetXYZ(0.002, 0.04, 0.); //AZ - errors on each side
	  */
	  //if (zHit > 35.0) dpos.SetXYZ(0.015, 0.058, 0.); //AZ - run7 - /sin(15)
	  //else dpos.SetXYZ(0.01, 0.24, 0.); //AZ - errors on each side
	  //if (zHit > 85.0) dpos.SetXYZ(0.015, 0.058, 0.); //AZ - CBM+BM@N
	  //else dpos.SetXYZ(0.01, 0.077, 0.); //AZ - errors on each side
	  if (sector->GetDx() < 0.02)
	    dpos.SetXYZ(sector->GetDx()/TMath::Sqrt(12.),
			sector->GetDx()/TMath::Sqrt(12.)/TMath::Sin(7.5*TMath::DegToRad()), 0.);
	  else dpos.SetXYZ(0.015, 0.058, 0.); //GEMS
	  //else dpos.SetXYZ(vX*10, vX*100, 0.); //AZ - errors on each side
	  vXTemp = clusterF->GetMeanError(); //AZ
	  vYTemp = clusterB->GetMeanError(); //AZ
	  //dpos.SetXYZ(vXTemp, vYTemp, 0.); //AZ - errors on each side
	}
	else if (stereoF==0.&&stereoB*180/TMath::Pi()>80) {	//0&90 case

	  //vXTemp = vX * clusterF->GetMeanError();
	  //vYTemp = vX * clusterB->GetMeanError();
	  //vXTemp = vX * 1.; // * clusterF->GetMeanError(); //AZ
	  //vYTemp = vX * 1.; // * clusterB->GetMeanError(); //AZ
	  vXTemp = clusterF->GetMeanError(); //AZ
	  vYTemp = clusterB->GetMeanError(); //AZ
	  vXYTemp = 0.;

          wX = vXTemp * vXTemp  * cosrot * cosrot 
                 - 2. * vXYTemp * cosrot * sinrot
                 + vYTemp * vYTemp  * sinrot * sinrot;
	  wY = vXTemp * vXTemp  * sinrot * sinrot
                 + 2. * vXYTemp * cosrot * sinrot
	         + vYTemp * vYTemp  * cosrot * cosrot; 
	  wXY = (vXTemp*vXTemp - vYTemp*vYTemp) * cosrot * sinrot
                 + vXYTemp * ( cosrot*cosrot - sinrot*sinrot );
	  //dpos.SetXYZ(TMath::Sqrt(wX),TMath::Sqrt(wY), 0.);
	  dpos.SetXYZ(vX*6, vX*6, 0.); //AZ - errors on each side
	  //dpos.SetXYZ(vXTemp, vYTemp, 0.); //AZ - errors on each side
	}

	else {							//7.5&-7.5 case

	  vXTemp = (vX/2.) * TMath::Sqrt(clusterF->GetMeanError()*clusterF->GetMeanError() + clusterB->GetMeanError()*clusterB->GetMeanError());
	  vYTemp = (vX/(2.*(TMath::Tan(stereoB)))) * TMath::Sqrt(clusterF->GetMeanError()*clusterF->GetMeanError() + clusterB->GetMeanError()*clusterB->GetMeanError());
	  vXYTemp = (vX*vX/(4.*(TMath::Tan(stereoB)))) * (clusterB->GetMeanError()*clusterB->GetMeanError() - clusterF->GetMeanError()*clusterF->GetMeanError());
	
	  wX = vXTemp * vXTemp  * cosrot * cosrot 
                 - 2. * vXYTemp * cosrot * sinrot
                 + vYTemp * vYTemp  * sinrot * sinrot;
	  wY = vXTemp * vXTemp  * sinrot * sinrot
                 + 2. * vXYTemp * cosrot * sinrot
	         + vYTemp * vYTemp  * cosrot * cosrot; 
	  wXY = (vXTemp*vXTemp - vYTemp*vYTemp) * cosrot * sinrot
                 + vXYTemp * ( cosrot*cosrot - sinrot*sinrot );
	  dpos.SetXYZ(TMath::Sqrt(wX),TMath::Sqrt(wY), 0.);

	}
	
	Int_t statLayer = -1;
	for ( Int_t istatL = station->GetNofZ() ; istatL > 0 ; istatL-- ) 
	  if ( TMath::Abs(zHit-station->GetZ(istatL-1)) < 0.00001 ) {
	    statLayer = istatL-1;
	    break;
	  }
	
	if ( statLayer == -1 ) 
	  cout << "unknown layer for hit at z = " << zHit << endl;

	if (sector->GetDx() > 0.03) pos[2] += zShift; // AZ - effective position (middle of GEM drift region)
	//AZ new ((*fHits)[nHits++]) CbmStsHit(sensorDetId, pos, dpos, wXY, 
	CbmStsHit *hhNew = new ((*fHits)[nHits++]) CbmStsHit(sensorDetId, pos, dpos, wXY, 
				iClusF, iClusB, (Int_t)chanF, (Int_t)chanB, statLayer);
	//max(iClusF-10,0), iClusB+1, max((Int_t)chanF-1,0), (Int_t)chanB+1, statLayer); //AZ-230322 - test
	hhNew->SetSignalDiv(TMath::Min(clusterF->GetQtot(),clusterB->GetQtot()) /
			    TMath::Max(clusterF->GetQtot(),clusterB->GetQtot())); //AZ
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
void CbmStsFindHits::Finish() {
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

ClassImp(CbmStsFindHits)


	
	
	
  
		      





 
