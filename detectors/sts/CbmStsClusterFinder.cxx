//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsClusterFinder source fil         -----
// -----                  Created 26/06/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
#include "TClonesArray.h"
#include "TH1S.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include "CbmGeoStsPar.h"
#include "CbmStsCluster.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsClusterFinder.h"
#include "CbmStsHit.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
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
CbmStsClusterFinder::CbmStsClusterFinder() 
  : FairTask("STS Cluster Finder", 1), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fClustersCand(NULL),
  fClusters(NULL),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fNofDigis(0),
  fNofClustersCand(0),
  fNofClusters(0),
  fNofClustersGood(0),
  fNofClustersWP(0),
  fNofClustersWM(0),
  fLongestCluster(0),
  fLongestGoodCluster(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer()
{}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsClusterFinder::CbmStsClusterFinder(Int_t iVerbose) 
  : FairTask("STSClusterFinder", iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fClustersCand(NULL),
  fClusters(NULL),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fNofDigis(0),
  fNofClustersCand(0),
  fNofClusters(0),
  fNofClustersGood(0),
  fNofClustersWP(0),
  fNofClustersWM(0),
  fLongestCluster(0),
  fLongestGoodCluster(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer()
{}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsClusterFinder::CbmStsClusterFinder(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fClustersCand(NULL),
  fClusters(NULL),
  fDigiScheme(CbmStsDigiScheme::Instance()),
  fNofDigis(0),
  fNofClustersCand(0),
  fNofClusters(0),
  fNofClustersGood(0),
  fNofClustersWP(0),
  fNofClustersWM(0),
  fLongestCluster(0),
  fLongestGoodCluster(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer()
{}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsClusterFinder::~CbmStsClusterFinder() {
  if ( fClustersCand ) { 
    fClustersCand->Delete();
    delete fClustersCand;
  }
  if ( fClusters ) { 
    fClusters->Delete();
    delete fClusters;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsClusterFinder::Exec(Option_t* opt) {

  fTimer.Start();
  Bool_t warn = kFALSE;

  if ( fVerbose ) {
    cout << endl << "-I- " << fName << ": executing event with " << fDigis->GetEntriesFast() << " digis." << endl;
    cout << "----------------------------------------------" << endl;
  }

  // Check for digi scheme
  if ( ! fDigiScheme ) {
    cerr << "-E- " << fName << "::Exec: No digi scheme!" << endl;
    return;
  }

  // Clear output array
  //  cout << "before clear: " << fClusters->GetEntriesFast() << endl;
  fNofClustersCand = 0;
  fNofClusters = 0;
//   fClustersCand->Clear();
//  fClusters->Clear();
  fClusters->Delete();
  fClustersCand->Delete();
  //  cout << " after clear: " << fClusters->GetEntriesFast() << endl;

  // Sort STS digis with respect to sectors
  SortDigis();

  // Find hits in sectors
  Int_t nDigisF = 0;
  Int_t nDigisB = 0;
  Int_t nStations = fDigiScheme->GetNStations();
  CbmStsStation* station = NULL;
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    station = fDigiScheme->GetStation(iStation);
    Int_t nDigisFInStation = 0;
    Int_t nDigisBInStation = 0;
    Int_t nSectors = station->GetNSectors();

    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      //      cout << "=============================================================================" << endl;
      //      cout << "station " << iStation+1 << " sector " << iSector+1 << "  " << endl;
      set <Int_t> fSet, bSet;
      if ( fDigiMapF.find(sector) == fDigiMapF.end() ) {
	cout << "-E- " << fName << "::Exec: sector " 
	     << sector->GetSectorNr() << " of station " 
	     << station->GetStationNr() << "not found in front map!" 
	     << endl;
	warn = kTRUE;
	continue;
      }
      fSet = fDigiMapF[sector];
      FindClusters(iStation+1,iSector+1,0, fSet);
      if ( sector->GetType() == 2 || sector->GetType() == 3 ) {
	if ( fDigiMapB.find(sector) == fDigiMapB.end() ) {
	  cout << "-E- " << fName << "::Exec: sector " 
	       << sector->GetSectorNr() << " of station " 
	       << station->GetStationNr() << "not found in back map!" 
	       << endl;
	  warn = kTRUE;
	  continue;
	}
      }
      bSet = fDigiMapB[sector];
      FindClusters(iStation+1,iSector+1,1, bSet);
      Int_t nDigisFInSector = fSet.size();
      Int_t nDigisBInSector = bSet.size();
      nDigisFInStation += nDigisFInSector;
      nDigisBInStation += nDigisBInSector;      
    }      // Sector loop
    
    nDigisF += nDigisFInStation;
    nDigisB += nDigisBInStation;
    
  }       // Station loop

  // analyze clusters...
  AnalyzeClusters();

  fTimer.Stop();  
  if ( fVerbose ) {
    cout << endl;
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels front side: " << nDigisF << endl;
    cout << "    Active channels back side : " << nDigisB << endl;
    cout << "    Real time                 : " << fTimer.RealTime() 
	 << endl;
  }
  else {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) 
	 << fixed << right << fTimer.RealTime() 
	 << " s, " << fNofClustersCand 
	 << "("    << fNofClustersGood
	 << "+"    << fNofClustersWP 
	 << ") clusters, longest till now " << fLongestCluster << endl;
    //    cout << fDigis->GetEntriesFast() << " vs " << fClusters->GetEntriesFast() << endl;
  }
  
	
  //AZ - add links to clusters (digits)
  Int_t nClust = fClusters->GetEntriesFast();
  for (Int_t i = 0; i < nClust; ++i) {
    CbmStsCluster *clus = (CbmStsCluster*) fClusters->UncheckedAt(i);

    Int_t nDigis = clus->GetNDigis();
    for (Int_t j = 0; j < nDigis; ++j) {
      Int_t idigi = clus->GetDigi(j);
      clus->AddLink(FairLink(kStsDigi, idigi));
    }
  }

}
// -------------------------------------------------------------------------

    


// -----   Private method SetParContainers   -------------------------------
void CbmStsClusterFinder::SetParContainers() {

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
InitStatus CbmStsClusterFinder::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("StsDigi");

  fClustersCand = new TClonesArray("CbmStsCluster", 30000);
  ioman->Register("StsClusterCand", "Cluster in STS", fClustersCand, kTRUE);

  fClusters = new TClonesArray("CbmStsCluster", 30000);
  ioman->Register("StsCluster", "Cluster in STS", fClusters, kTRUE);

  // Build digitisation scheme
  Bool_t success = fDigiScheme->Init(fGeoPar, fDigiPar);
  if ( ! success ) return kERROR;

  // Create sectorwise digi sets
  MakeSets();

  // Control output
  if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
  else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
  cout << "-I- " << fName << "::Init: "
       << "STS digitisation scheme succesfully initialised" << endl;
  cout << "    Stations: " << fDigiScheme->GetNStations() 
       << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
       << fDigiScheme->GetNChannels() << endl;
  
  fNofClustersCand   = 0;
  fNofClusters       = 0;
  fNofClustersGood   = 0;
  fNofClustersWP = 0;
  fNofClustersWM = 0;

  fLongestCluster = 0;
  fLongestGoodCluster = 0;

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsClusterFinder::ReInit() {

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
  
  fNofClustersCand   = 0;
  fNofClusters       = 0;
  fNofClustersGood   = 0;
  fNofClustersWP = 0;
  fNofClustersWM = 0;

  fLongestCluster = 0;
  fLongestGoodCluster = 0;

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method MakeSets   ---------------------------------------
void CbmStsClusterFinder::MakeSets() {

  fDigiMapF.clear();
  fDigiMapB.clear();
  Int_t nStations = fDigiScheme->GetNStations();
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    CbmStsStation* station = fDigiScheme->GetStation(iStation);
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      set<Int_t> a;
      fDigiMapF[sector] = a;
      if ( sector->GetType() == 2 || sector->GetType() ==3 ) {
	set<Int_t> b;
	fDigiMapB[sector] = b;
      }
    }
  }

}
// -------------------------------------------------------------------------




// -----   Private method SortDigis   --------------------------------------
void CbmStsClusterFinder::SortDigis() {

  // Check input array
  if ( ! fDigis ) {
    cout << "-E- " << fName << "::SortDigis: No input array!" << endl;
    return;
  }

  CbmStsDigi* digi = NULL;
  const Int_t nDigis = fDigis->GetEntriesFast();

  // Clear sector digi sets
  map<CbmStsSector*, set<Int_t> >::iterator mapIt;
  for (mapIt=fDigiMapF.begin(); mapIt!=fDigiMapF.end(); mapIt++)
    ((*mapIt).second).clear();
  for (mapIt=fDigiMapB.begin(); mapIt!=fDigiMapB.end(); mapIt++)
    ((*mapIt).second).clear();

  // Fill digis into sets
  CbmStsSector* sector = NULL;
  Int_t stationNr = -1;
  Int_t sectorNr  = -1;
  Int_t iSide     = -1;
  Int_t channelNr = -1;

  for (Int_t iDigi=0; iDigi<nDigis; iDigi++) {
    digi = (CbmStsDigi*) fDigis->At(iDigi);

    stationNr = digi->GetStationNr();
    sectorNr  = digi->GetSectorNr();
    iSide     = digi->GetSide();
    sector = fDigiScheme->GetSector(stationNr, sectorNr);
    
    if (iSide == 0 ) {
      if ( fDigiMapF.find(sector) == fDigiMapF.end() ) {
	cerr << "-E- " << fName << "::SortDigits:: sector " << sectorNr
	     << " of station " << stationNr 
	     << " not found in digi scheme (F)!" << endl;
	continue;
      }
      fDigiMapF[sector].insert(iDigi);      
    }
    else if (iSide == 1 ) {
      if ( fDigiMapB.find(sector) == fDigiMapB.end() ) {
	cerr << "-E- " << fName << "::SortDigits:: sector " << sectorNr
	     << " of station " << stationNr 
	     << " not found in digi scheme (B)!" << endl;
	continue;
      }
      fDigiMapB[sector].insert(iDigi);      
    }
  }
}
// -------------------------------------------------------------------------

// -----   Private method RealisticResponse   ------------------------------
Int_t CbmStsClusterFinder::FindClusters(Int_t stationNr, Int_t sectorNr, Int_t iSide, set<Int_t>& digiSet) {

  set<Int_t>::iterator it1;

  Int_t      iDigi = -1;
  CbmStsDigi* digi = NULL;
  CbmStsCluster* clusterCand = NULL;
  CbmStsCluster* cluster = NULL;

  Int_t    digiPos       = -1;
  Double_t digiSig       = -1.;
  Int_t    lastDigiNr    = -1;
  Int_t    lastDigiPos   = -1;
  Double_t lastDigiSig   = 100000.;
  Int_t    clusterMaxNr  = -1;
  Int_t    clusterMaxPos = -1;
  Double_t clusterMaxSig = -1.;
  Bool_t clusterHasMinimum = kFALSE;
  Bool_t clusterHasPlateau = kFALSE;
  Int_t clusterBeginPos = 0;
  Int_t clusterWidth = 0;
  //cout << "====================================================================" << digiSet.size() << endl;
  //cout << "   cluster   " << flush;
  //  if ( fNofClusters != fNofClustersGood+fNofClustersWP ) cout << fNofClusters-(fNofClustersGood+fNofClustersWP) << " -> " <<  fNofClusters <<"!="<< fNofClustersGood<<"+"<<fNofClustersWP<<endl;
  for (it1=digiSet.begin(); it1!=digiSet.end(); it1++) {
    iDigi = (*it1);
    digi  = (CbmStsDigi*) fDigis->At(iDigi);

    digiPos = digi->GetChannelNr();
    digiSig = digi->GetAdc();

    if ( lastDigiPos == -1 ) {
      clusterCand = new ((*fClustersCand)[fNofClustersCand++]) CbmStsCluster(digiSig, stationNr,sectorNr,iSide);
       //   cout << "first cluster ADC        "  << digiSig << endl;
      clusterBeginPos = digiPos;
    }

    if ( digiPos == lastDigiPos+1 ) {

      if ( digiSig == lastDigiSig ) {
	clusterCand->SetMeanError(digiSig);
	clusterHasPlateau = kTRUE;
      }

      // if the signal is larger that last one and the previous one is smaller than maximum
      if ( digiSig > lastDigiSig && lastDigiSig < clusterMaxSig &&  digiSig != clusterMaxSig) {
	clusterCand->SetMean(clusterMaxNr);
      //      cluster->SetMeanError(clusterMaxSig);
	clusterCand = new ((*fClustersCand)[fNofClustersCand++]) CbmStsCluster(digiSig, stationNr,sectorNr,iSide);
	clusterCand->AddDigi(lastDigiNr);
	//	cout << "         +end cluster " << lastDigiPos << endl;

	//	cout << "+new cluster          " << digiPos << endl;
	clusterWidth = lastDigiPos - clusterBeginPos + 1;
	if ( clusterWidth > fLongestCluster ) 
	  fLongestCluster = clusterWidth;
	if ( clusterHasPlateau ) {
	  fNofClustersWP++;
	}
	if ( !clusterHasMinimum && !clusterHasPlateau ) {
	  fNofClustersGood++; 
	  if ( clusterWidth > fLongestGoodCluster ) 
	    fLongestGoodCluster = clusterWidth;
	}
	
	clusterHasPlateau = kFALSE;
	clusterMaxPos = -1;
	clusterMaxSig = -1.;
	clusterBeginPos = digiPos;
      }
    }
    else if ( lastDigiPos>=0 ) {
      clusterCand->SetMean(clusterMaxNr);
      //      cluster->SetMeanError(clusterMaxSig);
      clusterCand = new ((*fClustersCand)[fNofClustersCand++]) CbmStsCluster(digiSig, stationNr,sectorNr,iSide);
      //    cout << "          end cluster " << lastDigiPos << endl;
      //    cout << "new cluster           " << digiPos << endl;
      clusterWidth = lastDigiPos - clusterBeginPos + 1;
      if ( clusterWidth > fLongestCluster ) 
	fLongestCluster = clusterWidth;
      if ( clusterHasPlateau ) {
	fNofClustersWP++;
      }
      if ( !clusterHasMinimum && !clusterHasPlateau ) {
	fNofClustersGood++; 
	if ( clusterWidth > fLongestGoodCluster ) 
	  fLongestGoodCluster = clusterWidth;
      }
      
      clusterHasPlateau = kFALSE;
      clusterMaxPos = -1;
      clusterMaxSig = -1.;
      clusterBeginPos = digiPos;
    }
    if ( digiSig > clusterMaxSig ) {
      clusterMaxNr  = iDigi;
      clusterMaxPos = digiPos;
      clusterMaxSig = digiSig;
    }
    
    clusterWidth = lastDigiPos - clusterBeginPos + 1;
    if ( clusterWidth < 90 ) {
      clusterCand->AddDigi(iDigi);}
//       clusterCand->AddIndex(iDigi,digiSig);}
    else {
      //      return 1;
      clusterCand->SetMean(clusterMaxNr);
      //      cluster->SetMeanError(clusterMaxSig);

      clusterCand = new ((*fClustersCand)[fNofClustersCand++]) CbmStsCluster(digiSig, stationNr,sectorNr,iSide);
      clusterCand->AddDigi(iDigi);
      clusterWidth = lastDigiPos - clusterBeginPos + 1;
      if ( clusterWidth > fLongestCluster ) 
	fLongestCluster = clusterWidth;
      if ( clusterHasPlateau ) {
	fNofClustersWP++;
      }
      if ( !clusterHasMinimum && !clusterHasPlateau ) {
	fNofClustersGood++; 
	if ( clusterWidth > fLongestGoodCluster ) 
	  fLongestGoodCluster = clusterWidth;
      }
      
      clusterHasPlateau = kFALSE;
      clusterMaxPos = -1;
      clusterMaxSig = -1.;
      clusterBeginPos = digiPos;
    }

    lastDigiNr  = iDigi;
    lastDigiPos = digiPos;
    lastDigiSig = digiSig;
  }
  if ( digiSig > 0. ) {
    clusterCand->SetMean(clusterMaxNr);

    //  cout << "         last cluster " << lastDigiPos << endl;
    clusterWidth = lastDigiPos - clusterBeginPos + 1;
    if ( clusterWidth > fLongestCluster ) 
      fLongestCluster = clusterWidth;
    if ( clusterHasPlateau ) {
      fNofClustersWP++;
    }
    if ( lastDigiPos && !clusterHasPlateau ) {
      fNofClustersGood++;
      if ( clusterWidth > fLongestGoodCluster ) 
	fLongestGoodCluster = clusterWidth;
    }
  }
  //  delete clusterCand;
  
  return 1;

  map<Int_t , Int_t> channelSorted;
  for (it1=digiSet.begin(); it1!=digiSet.end(); it1++) {
    iDigi = (*it1);
    digi  = (CbmStsDigi*) fDigis->At(iDigi);
    cout << digi->GetChannelNr() << " " << flush;
    channelSorted[digi->GetChannelNr()] = iDigi+1;
  }
  cout << endl;
  // print channels/signals
  Int_t iFS = 0;
  for (Int_t iter=0; iter<1100; iter++) {
    iDigi = channelSorted[iter];
    if ( iDigi == 0 ) continue;
    iFS++;
    digi  = (CbmStsDigi*) fDigis->At(iDigi-1);
    if ( iFS >= channelSorted.size() ) break;
  }
  //  cout << endl;

  //  cout << "size = " << channelSorted.size() << endl;
  /*  Int_t iFS = 0;
  Float_t lastSignal = 0.;
  for (Int_t iter=0; iter<1100; iter++) {
    iDigi = channelSorted[iter];
    if ( iDigi == 0 ) {lastSignal=0.;continue;}
    iFS++;
    digi  = (CbmStsDigi*) fDigis->At(iDigi-1);
    Float_t signal = digi->GetADC();
    if ( signal <= lastSignal ) { lastSignal = signal; continue; }

    if ( iFS >= channelSorted.size() ) break;


    }*/
  //  cout << endl << "=============================================" << endl;

}
// -------------------------------------------------------------------------

// -----   Method AnalyzeClusters   ----------------------------------------
void CbmStsClusterFinder::AnalyzeClusters() {
  for ( Int_t iclus = 0 ; iclus < fNofClustersCand ; iclus++ ) {
    AnalyzeCluster(iclus);
  }
}
// -------------------------------------------------------------------------

// -----   Method AnalyzeClusters   ----------------------------------------
void CbmStsClusterFinder::AnalyzeCluster(Int_t iCluster) {

  CbmStsCluster* clusterCand  = (CbmStsCluster*) fClustersCand->At(iCluster);
  CbmStsCluster* cluster      = (CbmStsCluster*) fClusters->At(iCluster);
//   CbmStsSector* sector = NULL;
  Int_t maxDigiNr = (Int_t)clusterCand->GetMean();
  Double_t plateau = clusterCand->GetMeanError();
  Double_t maxDigiSig = 0.;
  CbmStsDigi* digi = NULL;
  digi = (CbmStsDigi*)fDigis->At(maxDigiNr);
  Int_t maxDigiPos = digi->GetChannelNr();
  maxDigiSig = digi->GetAdc();

  if (clusterCand->GetNDigis() > 5) {
    // AZ - split large clusters
    //SplitCluster(iCluster);
    //return;
  }
  //  cout << "Cluster " << iCluster+1 << " has " << cluster->GetNDigis() << " digis, max at " << maxDigiNr << endl;
  Double_t chanNr  = 0;
  Double_t chanADC = 0.;
  Double_t sumW    = 0;
  Double_t sumWX   = 0;
  Double_t sumCh   = 0;
  Double_t error   = 0;
  Double_t sumWX2 = 0.0;
  for ( Int_t itemp = 0 ; itemp < clusterCand->GetNDigis() ; itemp++ ) {
    digi = (CbmStsDigi*)fDigis->At(clusterCand->GetDigi(itemp));
    chanNr  = (Double_t)digi->GetChannelNr();
    chanADC = digi->GetAdc();
    sumW  +=        chanADC;
    sumWX += chanNr*chanADC;
    sumCh += chanNr;
    error += ( chanADC*chanADC );
    sumWX2 += chanADC * chanNr * chanNr; //AZ
    //    cout << chanADC << " + " << flush;
    //    cout << "channel " << digi->GetChannelNr() << " with signal = " << digi->GetADC() << " (" << sumWX << "/" << sumW << ") - plateau = " << plateau << endl;
  }

  if (sumW>50.) {
    Int_t imean = TMath::Nint (sumWX/sumW); //AZ
    for ( Int_t itemp = 0 ; itemp < clusterCand->GetNDigis() ; itemp++ ) {
  
      digi = (CbmStsDigi*)fDigis->At(clusterCand->GetDigi(itemp));
      if (itemp==0) {
        cluster = new ((*fClusters)[fNofClusters++]) CbmStsCluster(digi->GetAdc(), digi->GetStationNr(), digi->GetSectorNr(), digi->GetSide());
        //AZ cluster->AddDigi(itemp);
        cluster->AddDigi(clusterCand->GetDigi(itemp));
        //if (digi->GetChannelNr() == imean) cluster->AddDigi(clusterCand->GetDigi(itemp));
      }
      else if (itemp>0) {
        //AZ cluster->AddDigi(itemp);
        cluster->AddDigi(clusterCand->GetDigi(itemp));
        //if (digi->GetChannelNr() == imean) cluster->AddDigi(clusterCand->GetDigi(itemp));
      }
    }
    //  cout << " mean at = " << sumWX/sumW << endl;
    cluster->SetMean(sumWX/sumW);
    cluster->SetQtot(sumW); //AZ
    cluster->SetMeanError(  (1./(sumW)) * TMath::Sqrt(error)   );
    if ( sumW < maxDigiSig ) {
      cout << " MAX DIGI = " << maxDigiSig << ", while SUMW = " << sumW << endl;
      for ( Int_t itemp = 0 ; itemp < clusterCand->GetNDigis() ; itemp++ ) {
        digi = (CbmStsDigi*)fDigis->At(clusterCand->GetDigi(itemp));
        cout << "digi ADC = " << digi->GetAdc() << " at channel# " << digi->GetChannelNr() << endl;
      }
    }
    // AZ - correct coordinate for 2 adjacent clusters, i.e. the ones sharing 1 channel
    //if (0) {
    if (fNofClusters > 1) {
      CbmStsCluster *clusPrev = (CbmStsCluster*) fClusters->UncheckedAt(fNofClusters-2);
      if (clusPrev->GetDetectorId() == cluster->GetDetectorId()) {

	Int_t nDigis = clusPrev->GetNDigis();
	CbmStsDigi *digPrev = (CbmStsDigi*) fDigis->UncheckedAt(clusPrev->GetDigi(nDigis-1));
	Int_t chan2 = digPrev->GetChannelNr();
	if (chan2 == ((CbmStsDigi*)fDigis->UncheckedAt(cluster->GetDigi(0)))->GetChannelNr()) {

	  Double_t adcPrev = digPrev->GetAdc();
	  Double_t qPrev = clusPrev->GetQtot();
	  Double_t xPrev = clusPrev->GetMean() * qPrev;
	  xPrev -= chan2 * adcPrev * 0.5;
	  xPrev /= (qPrev - adcPrev * 0.5);
	  clusPrev->SetMean(xPrev);
	  clusPrev->SetQtot(qPrev - adcPrev * 0.5);
	  Double_t xCor = sumWX;
	  xCor -= chan2 * adcPrev * 0.5;
	  xCor /= (sumW - adcPrev * 0.5);
	  cluster->SetMean(xCor);
	  cluster->SetQtot(sumW - adcPrev * 0.5);
	}
      }
    }
    //* AZ - correct coordinate and choose error estimate for previous cluster
    Int_t chan2 = -1;
    if (fNofClusters > 1) {
      CbmStsCluster *clusPrev = (CbmStsCluster*) fClusters->UncheckedAt(fNofClusters-2);
      if (clusPrev->GetDetectorId() == cluster->GetDetectorId()) {
	Int_t nDigis = clusPrev->GetNDigis();
	CbmStsDigi *digPrev = (CbmStsDigi*) fDigis->UncheckedAt(clusPrev->GetDigi(nDigis-1));
	chan2 = digPrev->GetChannelNr();
	if (chan2 != ((CbmStsDigi*)fDigis->UncheckedAt(cluster->GetDigi(0)))->GetChannelNr()) chan2 = -1;
      }
      EvalErrors(clusPrev, chan2);
    }
    //Double_t rms = sumWX2 / sumW - cluster->GetMean() * cluster->GetMean();
    Double_t rms = (sumWX2 - sumWX * sumWX / sumW) / sumW;
    cluster->SetMeanError(TMath::Sqrt(rms));
    //cluster->SetMeanError(0.04/TMath::Sqrt(12.)); //rms);
    cluster->SetLeft(chan2); // if > 0 - there is a left neighbour
    if (iCluster == fNofClustersCand-1) EvalErrors(cluster, -1); // last cluster
    //AZ
    //*/
  } // if (sumW>50.)
  //   cout << sumWE/sumW << " mean at " << endl;
  //  cout << "error = " << sumW/maxDigiSig << endl;
}
// -------------------------------------------------------------------------

//AZ -----------------------------------------------------------------------
void CbmStsClusterFinder::SplitCluster(Int_t iCluster) 
{
  // Split large cluster into subclusters
  
  const Int_t nDigMax = 13;
  static Int_t first = 1, nSubs[nDigMax] = {0};

  if (first) {
    first = 0;
    nSubs[6] = nSubs[7] = nSubs[8] = nSubs[9] = 2; // number of subclusters
    nSubs[10] = nSubs[11] = nSubs[12] = 3;
  }

  CbmStsCluster* clusterCand  = (CbmStsCluster*) fClustersCand->At(iCluster);
  CbmStsCluster* cluster = NULL;
  CbmStsDigi *digi = NULL;

  Int_t mult = clusterCand->GetNDigis(), nsub = 1;
  if (mult < nDigMax) nsub = nSubs[mult];
  else nsub = TMath::Nint (mult * 1.0 / 4 - 0.1);
  Int_t leng0 = TMath::Nint (mult * 1.0 / nsub);
  Int_t i0 = 0, itemp = 0;
  Double_t chanNr = 0.0, chanADC = 0.0, sumW = 0.0, sumWX = 0.0, sumWX2 = 0.0;

  //for ( Int_t itemp = 0; itemp < mult; itemp++ ) {
  for (Int_t icl = 0; icl < nsub; ++icl) {
    Int_t leng = leng0;
    if (icl == 0 && mult - leng0 * nsub == 1) ++leng; // add extra digi to first subcluster
    else if (icl == nsub - 1 && mult - leng0 * nsub == 2) ++leng; // add extra digi to last subclus.
    while (1) {
      digi = (CbmStsDigi*) fDigis->UncheckedAt(clusterCand->GetDigi(itemp));
      if (i0 == 0) {
	cluster = new ((*fClusters)[fNofClusters++]) CbmStsCluster(digi->GetAdc(), digi->GetStationNr(), digi->GetSectorNr(), digi->GetSide());
	cluster->AddDigi(clusterCand->GetDigi(itemp));
      } else {
	cluster->AddDigi(clusterCand->GetDigi(itemp));
      }
      ++i0;
      ++itemp;
      chanNr = digi->GetChannelNr();
      chanADC = digi->GetAdc();
      sumW += chanADC;
      sumWX += chanNr * chanADC;
      sumWX2 += chanADC * chanNr * chanNr;
      if (i0 == leng) {
	// Subcluster
	cluster->SetMean(sumWX/sumW);
	cluster->SetQtot(sumW); //AZ
	cluster->SetDefaultType(mult); 

	// AZ - correct coordinate for 2 adjacent clusters, i.e. the ones sharing 1 channel or back to back
	//if (0) {
	if (fNofClusters > 1) {
	  CbmStsCluster *clusPrev = (CbmStsCluster*) fClusters->UncheckedAt(fNofClusters-2);
	  if (clusPrev->GetDetectorId() == cluster->GetDetectorId()) {
	    
	    Int_t nDigis = clusPrev->GetNDigis();
	    CbmStsDigi *digPrev = (CbmStsDigi*) fDigis->UncheckedAt(clusPrev->GetDigi(nDigis-1));
	    Int_t chan2 = digPrev->GetChannelNr();
	    CbmStsDigi *digThis = (CbmStsDigi*) fDigis->UncheckedAt(cluster->GetDigi(0));
	    Int_t chan1 = digThis->GetChannelNr();
	    if (TMath::Abs(chan2-chan1) < 1) {
	      // Adjacent clusters
	      Double_t adcPrev = digPrev->GetAdc();
	      Double_t qPrev = clusPrev->GetQtot();
	      Double_t xPrev = clusPrev->GetMean() * qPrev;
	      xPrev -= chan2 * adcPrev * 0.5;
	      xPrev /= (qPrev - adcPrev * 0.5);
	      clusPrev->SetMean(xPrev);
	      clusPrev->SetQtot(qPrev - adcPrev * 0.5);
	      adcPrev = digThis->GetAdc();
	      Double_t xCor = sumWX;
	      xCor -= chan1 * adcPrev * 0.5;
	      xCor /= (sumW - adcPrev * 0.5);
	      cluster->SetMean(xCor);
	      cluster->SetQtot(sumW - adcPrev * 0.5);
	    }
	  }
	}
	//* AZ - correct coordinate and choose error estimate for previous cluster
	Int_t chan2 = -1;
	if (fNofClusters > 1) {
	  CbmStsCluster *clusPrev = (CbmStsCluster*) fClusters->UncheckedAt(fNofClusters-2);
	  if (clusPrev->GetDetectorId() == cluster->GetDetectorId()) {
	    Int_t nDigis = clusPrev->GetNDigis();
	    CbmStsDigi *digPrev = (CbmStsDigi*) fDigis->UncheckedAt(clusPrev->GetDigi(nDigis-1));
	    chan2 = digPrev->GetChannelNr();
	    if (TMath::Abs(chan2-((CbmStsDigi*)fDigis->UncheckedAt(cluster->GetDigi(0)))->GetChannelNr()) > 0) chan2 = -1;
	  }
	  EvalErrors(clusPrev, chan2);
	}
	//Double_t rms = sumWX2 / sumW - cluster->GetMean() * cluster->GetMean();
	Double_t rms = (sumWX2 - sumWX * sumWX / sumW) / sumW;
	cluster->SetMeanError(TMath::Sqrt(rms));
	//cluster->SetMeanError(0.04/TMath::Sqrt(12.)); //rms);
	cluster->SetLeft(chan2); // if > 0 - there is a left neighbour
	if (iCluster == fNofClustersCand-1) EvalErrors(cluster, -1); // last cluster

	//cluster->SetMeanError(  (1./(sumW)) * TMath::Sqrt(error)   );
	i0 = 0;
	if (itemp + leng * (nsub - icl - 1) > mult) --itemp; // move one digi backward
	sumW = sumWX = sumWX2 = 0.0;
	break;
      }
    }
  }
}
// -------------------------------------------------------------------------

//AZ -----------------------------------------------------------------------
void CbmStsClusterFinder::EvalErrors(CbmStsCluster *clus, Int_t chan2) {
  // Evaluate errors and correct coordinate if necessary

  Int_t mult = clus->GetNDigis();
  Double_t sigma = 0.0, xcor = clus->GetMean(), rms = clus->GetMeanError();

  if (chan2 >= 0 && clus->GetLeft() >= 0) {
    // Cluster has 2 neighbours
    if (mult <= 3) sigma = 0.0298;
    else if (mult == 4) {
      if (clus->GetDefaultType()) sigma = 0.0398; // split cluster - check that !
      else sigma = 0.0135; // fit
    }
    else if (mult == 5) {
      if (clus->GetDefaultType()) sigma = 0.0463; // split cluster - check that !
      else sigma = 0.0109; // fit
    }
  } 

  else if (chan2 >= 0 || clus->GetLeft() >= 0) {
    // 1 neighbour
    Int_t ishft = 1;
    if (chan2 >= 0) ishft = -1;
    if (mult <= 2) {
      sigma = 0.0220;
      //xcor = xcor - ishft * 0.0122 / 0.0400;
    }
    else if (mult == 5) {
      ////if (rms < 1.2) sigma = 0.0075;
      ////else sigma = 0.0121;
      ////sigma = 0.0105;
      //sigma = 0.0055; // fit
      //xcor = xcor + ishft * (0.0070 / 0.0400 + (rms - 1.234) * 0.44 / 4.5 / 0.0400);
      if (clus->GetDefaultType()) sigma = 0.0487; // split cluster - check that !
      else sigma = 0.0118; // fit
    }
    else if (mult == 3) {
      //sigma = 0.0080;
      //xcor = xcor + ishft * (-0.0025 / 0.0400 + (rms - 0.776) * 0.54 / 2.5 / 0.0400);
      if (clus->GetDefaultType()) sigma = 0.0315; // split cluster - check that !
      else sigma = 0.0098; // fit
    }
    else if (mult == 4) {
      //sigma = 0.0065;
      //xcor = xcor + ishft * (0.0045 / 0.0400 + (rms - 1.028) * 0.62 / 3.5 / 0.0400);
      if (clus->GetDefaultType()) sigma = 0.0366; // split cluster - check that !
      else sigma = 0.0100; // fit
    }
    xcor = TMath::Max (0.0, xcor);
  }

  else {
    // No neighbours
    if (mult == 1) sigma = 0.0122;
    else if (mult == 2) {
      if (rms > 0.495) sigma = 0.0071; // RMS
      else sigma = 0.0224; // edge effect - fix !
    }
    else if (mult == 3) {
      if (clus->GetDefaultType()) sigma = 0.0255; // split cluster - check that !
      else sigma = 0.0064; // fit
    }
    else if (mult == 4) {
      if (clus->GetDefaultType()) sigma = 0.0337; // split cluster - check that !
      sigma = 0.0048; // fit
    }
    else if (mult == 5) {
      //if (rms < 1.19) sigma = 0.0108;
      if (rms < 1.19) sigma = 0.0038; // fit
      else sigma = 0.0415; // should be split also !
    }
  }

  clus->SetMean(xcor);
  clus->SetMeanError(sigma);
}
//AZ -----------------------------------------------------------------------

// -----   Virtual method Finish   -----------------------------------------
void CbmStsClusterFinder::Finish() {
  fNofClustersCand = fNofClustersGood+fNofClustersWP;
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Number of clusters              : " 
       << setw(8) << setprecision(2) 
       << fNofClustersCand << endl;
  cout << "===== Number of good clusters         : " 
       << setw(8) << setprecision(2) 
       << fNofClustersGood << " (" 
       << setw(8) << setprecision(2) 
       << 100.*fNofClustersGood/fNofClustersCand << "%)" << endl;
  cout << "===== Number of plateau clusters      : " 
       << setw(8) << setprecision(2) 
       << fNofClustersWP << " (" 
       << setw(8) << setprecision(2) 
       << 100.*fNofClustersWP/fNofClustersCand << "%)" << endl;
  cout << "===== Number of minimum clusters      : " 
       << setw(8) << setprecision(2) 
       << fNofClustersWM << " (" 
       << setw(8) << setprecision(2) 
       << 100.*fNofClustersWM/fNofClustersCand << "%)" << endl;
  cout << "===== Longest cluster                 : "
       << setw(8) << setprecision(2) 
       << fLongestCluster << endl;
  cout << "===== Longest good cluster            : "
       << setw(8) << setprecision(2) 
       << fLongestGoodCluster << endl;
  cout << "============================================================"
       << endl;

}					       
// -------------------------------------------------------------------------
    

ClassImp(CbmStsClusterFinder)


	
	
	
  
		      





 
