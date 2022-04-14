//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsRealClusterFinder source fil         -----
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
#include "CbmStsRealClusterFinder.h"
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
CbmStsRealClusterFinder::CbmStsRealClusterFinder() : FairTask("STS Cluster Finder", 1) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fDigis   = NULL;
  fClusters= NULL;
  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsRealClusterFinder::CbmStsRealClusterFinder(Int_t iVerbose) 
  : FairTask("STSClusterFinder", iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fDigis   = NULL;
  fClusters= NULL;
  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsRealClusterFinder::CbmStsRealClusterFinder(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) {
  fGeoPar  = NULL;
  fDigiPar = NULL;
  fDigis   = NULL;
  fClusters= NULL;
  fDigiScheme = new CbmStsDigiScheme();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsRealClusterFinder::~CbmStsRealClusterFinder() {
  if ( fClusters ) { 
    fClusters->Delete();
    delete fClusters;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsRealClusterFinder::Exec(Option_t* opt) {

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
  fNofClusters = 0;
  //  fClusters->Clear();
  fClusters->Delete();
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
	 << " s, " << fNofClusters 
	 << "("    << fNofClustersGood
	 << "+"    << fNofClustersWP 
	 << ") clusters, longest till now " << fLongestCluster << endl;
    //    cout << fDigis->GetEntriesFast() << " vs " << fClusters->GetEntriesFast() << endl;
  }
  

}
// -------------------------------------------------------------------------

    


// -----   Private method SetParContainers   -------------------------------
void CbmStsRealClusterFinder::SetParContainers() {

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
InitStatus CbmStsRealClusterFinder::Init() {

  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("StsDigi");

  fClusters = new TClonesArray("CbmStsCluster", 1000);
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
  
  fNofClusters   = 0;
  fNofClustersGood   = 0;
  fNofClustersWP = 0;
  fNofClustersWM = 0;

  fLongestCluster = 0;
  fLongestGoodCluster = 0;

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsRealClusterFinder::ReInit() {

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
  
  fNofClusters   = 0;
  fNofClustersGood   = 0;
  fNofClustersWP = 0;
  fNofClustersWM = 0;

  fLongestCluster = 0;
  fLongestGoodCluster = 0;

  return kSUCCESS;
}
// -------------------------------------------------------------------------




// -----   Private method MakeSets   ---------------------------------------
void CbmStsRealClusterFinder::MakeSets() {

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
void CbmStsRealClusterFinder::SortDigis() {

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
Int_t CbmStsRealClusterFinder::FindClusters(Int_t stationNr, Int_t sectorNr, Int_t iSide, set<Int_t>& digiSet) {

  set<Int_t>::iterator it1;

  Int_t      iDigi = -1;
  CbmStsDigi* digi = NULL;
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
    digiSig = digi->GetADC();
    
    if ( lastDigiPos == -1 ) {
      cluster = new ((*fClusters)[fNofClusters++]) CbmStsCluster(iDigi,stationNr,sectorNr,iSide);
       //   cout << "first cluster ADC        "  << digiSig << endl;
      clusterBeginPos = digiPos;
    }

    if ( digiPos == lastDigiPos+1 ) {

      if ( digiSig == lastDigiSig ) {
	cluster->SetMeanError(digiSig);
	clusterHasPlateau = kTRUE;
      }

      // if the signal is larger that last one and the previous one is smaller than maximum
      if ( digiSig > lastDigiSig && lastDigiSig < clusterMaxSig ) {
	cluster->SetMean(clusterMaxNr);
      //      cluster->SetMeanError(clusterMaxSig);
	cluster = new ((*fClusters)[fNofClusters++]) CbmStsCluster(iDigi,stationNr,sectorNr,iSide);

	cluster->AddDigi(lastDigiNr);
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
      cluster->SetMean(clusterMaxNr);
      //      cluster->SetMeanError(clusterMaxSig);
      cluster = new ((*fClusters)[fNofClusters++]) CbmStsCluster(iDigi,stationNr,sectorNr,iSide);
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
    if ( clusterWidth < 90 ) 
      cluster->AddDigi(iDigi);
    else {
      //      return 1;
      cluster->SetMean(clusterMaxNr);
      //      cluster->SetMeanError(clusterMaxSig);

      cluster = new ((*fClusters)[fNofClusters++]) CbmStsCluster(iDigi,stationNr,sectorNr,iSide);
      cluster->AddDigi(iDigi);

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
    cluster->SetMean(clusterMaxNr);

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
void CbmStsRealClusterFinder::AnalyzeClusters() {
  for ( Int_t iclus = 0 ; iclus < fNofClusters ; iclus++ ) {
    AnalyzeCluster(iclus);
  }
}
// -------------------------------------------------------------------------

// -----   Method AnalyzeClusters   ----------------------------------------
void CbmStsRealClusterFinder::AnalyzeCluster(Int_t iCluster) {

  CbmStsCluster* cluster  = (CbmStsCluster*) fClusters->At(iCluster);
  Int_t maxDigiNr = (Int_t)cluster->GetMean();
  Double_t plateau = cluster->GetMeanError();
  Double_t maxDigiSig = 0.;
  CbmStsDigi* digi = NULL;
  digi = (CbmStsDigi*)fDigis->At(maxDigiNr);
  Int_t maxDigiPos = digi->GetChannelNr();
  maxDigiSig = digi->GetADC() - plateau;
  if ( plateau ) {
    if ( TMath::Abs(plateau-maxDigiSig)<0.0001 ) plateau = 0.;
    //    cout << "PLATEAU, but maxdigisig = " << maxDigiSig << " at " << maxDigiNr << " -> plateau = " << plateau << endl;
  }

  //  cout << "Cluster " << iCluster+1 << " has " << cluster->GetNDigis() << " digis, max at " << maxDigiNr << endl;
  Double_t chanNr  = 0;
  Double_t chanADC = 0.;
  Double_t sumW    = 0;
  Double_t sumWX   = 0;
  for ( Int_t itemp = 0 ; itemp < cluster->GetNDigis() ; itemp++ ) {
    digi = (CbmStsDigi*)fDigis->At(cluster->GetDigi(itemp));
    chanNr  = (Double_t)digi->GetChannelNr();
    chanADC = digi->GetADC();
    chanADC = ( chanADC < plateau ? 0 : chanADC-plateau );
    sumW  +=        chanADC;
    sumWX += chanNr*chanADC;
    //    cout << chanADC << " + " << flush;
    //    cout << "channel " << digi->GetChannelNr() << " with signal = " << digi->GetADC() << " (" << sumWX << "/" << sumW << ") - plateau = " << plateau << endl;
  }
  //  cout << " mean at = " << sumWX/sumW << endl;
  cluster->SetMean(sumWX/sumW);
  cluster->SetMeanError(sumW/maxDigiSig);
  if ( sumW < maxDigiSig ) {
    cout << " MAX DIGI = " << maxDigiSig << ", while SUMW = " << sumW << endl;
    for ( Int_t itemp = 0 ; itemp < cluster->GetNDigis() ; itemp++ ) {
      digi = (CbmStsDigi*)fDigis->At(cluster->GetDigi(itemp));
      cout << "digi ADC = " << digi->GetADC() << " at channel# " << digi->GetChannelNr() << endl;
    }
  }
  //   cout << sumWE/sumW << " mean at " << endl;
  //  cout << "error = " << sumW/maxDigiSig << endl;
}
// -------------------------------------------------------------------------

// -----   Virtual method Finish   -----------------------------------------
void CbmStsRealClusterFinder::Finish() {
  fNofClusters = fNofClustersGood+fNofClustersWP;
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Number of clusters              : " 
       << setw(8) << setprecision(2) 
       << fNofClusters << endl;
  cout << "===== Number of good clusters         : " 
       << setw(8) << setprecision(2) 
       << fNofClustersGood << " (" 
       << setw(8) << setprecision(2) 
       << 100.*fNofClustersGood/fNofClusters << "%)" << endl;
  cout << "===== Number of plateau clusters      : " 
       << setw(8) << setprecision(2) 
       << fNofClustersWP << " (" 
       << setw(8) << setprecision(2) 
       << 100.*fNofClustersWP/fNofClusters << "%)" << endl;
  cout << "===== Number of minimum clusters      : " 
       << setw(8) << setprecision(2) 
       << fNofClustersWM << " (" 
       << setw(8) << setprecision(2) 
       << 100.*fNofClustersWM/fNofClusters << "%)" << endl;
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
    

ClassImp(CbmStsRealClusterFinder)


	
	
	
  
		      





 
