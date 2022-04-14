//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsRealDigitize source file             -----
// -----                  Created 08/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------




// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TF1.h"
#include "TRandom3.h"

#include "TGeoManager.h"
#include "TGeoNode.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsRealDigitize.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::flush;
using std::pair;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::set;
using std::map;
using std::ios_base;


// -----   Default constructor   ------------------------------------------
CbmStsRealDigitize::CbmStsRealDigitize() : FairTask("STS Digitizer", 1) {
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fRealistic   = kFALSE;
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();

  fStep = 0.001;

  fFThreshold  = 0.001;
  fBThreshold  = 0.001;
  fFNoiseWidth = 0.0;
  fBNoiseWidth = 0.0;

  fFNofBits    = 20;
  fBNofBits    = 20;
  fFMinStep    = 0.01;
  fBMinStep    = 0.01;

  fNEvents = 0.;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsRealDigitize::CbmStsRealDigitize(Int_t iVerbose) 
  : FairTask("STSDigitize", iVerbose) { 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fRealistic   = kFALSE;
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();

  fStep = 0.001;

  fFThreshold  = 0.001;
  fBThreshold  = 0.001;
  fFNoiseWidth = 0.0;
  fBNoiseWidth = 0.0;

  fFNofBits    = 20;
  fBNofBits    = 20;
  fFMinStep    = 0.01;
  fBMinStep    = 0.01;

  fNEvents = 0.;
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmStsRealDigitize::CbmStsRealDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose) { 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fRealistic   = kFALSE;
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();

  fStep = 0.001;

  fFThreshold  = 0.001;
  fBThreshold  = 0.001;
  fFNoiseWidth = 0.0;
  fBNoiseWidth = 0.0;

  fFNofBits    = 20;
  fBNofBits    = 20;
  fFMinStep    = 0.01;
  fBMinStep    = 0.01;

  fNEvents = 0.;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmStsRealDigitize::~CbmStsRealDigitize() { 
  if ( fGeoPar)    delete fGeoPar;
  if ( fDigiPar)   delete fDigiPar;
  if ( fDigis ) {
    fDigis->Delete();
    delete fDigis;
  }
  if ( fDigiMatches ) {
    fDigiMatches->Delete();
    delete fDigiMatches;
  }
  if ( fDigiScheme ) delete fDigiScheme;
  Reset();
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void CbmStsRealDigitize::Exec(Option_t* opt) {

//   cout << "threshold = " << fFThreshold << endl;

  // Reset all eventwise counters
  fTimer.Start();
  Reset();
  TF1* digiGausDist = new TF1("digiGausDist","gaus",-5.,5.);

  // Verbose screen output
  if ( fVerbose > 2 ) {
    cout << endl << "-I- " << fName << ": executing event" << endl; 
    cout << "----------------------------------------------" << endl;
  }

  Double_t nPoints  = 0.;
  Double_t nOutside = 0.;
  Double_t nDigisF  = 0.;
  Double_t nDigisB  = 0.;

  // Loop over all StsPoints
  if ( ! fPoints ) {
    cerr << "-W- " << fName << "::Exec: No input array (STSPoint) "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }

  map<CbmStsSensor*, set<Int_t> >::iterator mapIt;
  for (mapIt=fPointMap.begin(); mapIt!=fPointMap.end(); mapIt++)
    ((*mapIt).second).clear();
  
  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);

    Double_t xin = point->GetXIn();
    Double_t yin = point->GetYIn();
    Double_t zin = point->GetZIn();
    gGeoManager->FindNode(xin,yin,zin);
    TGeoNode* curNode = gGeoManager->GetCurrentNode();
    
    CbmStsSensor* sensor = fDigiScheme->GetSensorByName(curNode->GetName());

    if ( fPointMap.find(sensor) == fPointMap.end() ) {
      cerr << "-E- " << fName << "::Exec:: sensor " << curNode->GetName()
	   << " not found in digi scheme!" << endl;
      continue;
    }
    fPointMap[sensor].insert(iPoint);     
  }
  
  for (Int_t iStation=fDigiScheme->GetNStations(); iStation > 0 ; ) {
    CbmStsStation* station = fDigiScheme->GetStation(--iStation);
    for (Int_t iSector=station->GetNSectors(); iSector > 0 ; ) {
      CbmStsSector* sector = station->GetSector(--iSector);

      map<Int_t, set<Int_t> >::iterator mapCh;

      for (mapCh=fFChannelPointsMap.begin(); mapCh!=fFChannelPointsMap.end(); mapCh++)
	((*mapCh).second).clear();
      for (mapCh=fBChannelPointsMap.begin(); mapCh!=fBChannelPointsMap.end(); mapCh++)
	((*mapCh).second).clear();

      // simulating detector+cables+electronics noise 
      // should be more sophisticated...
      // the question is: sectorwise or sensorwise???
      Int_t nChannels = sector->GetNChannelsFront();
      for (Int_t iChannel=nChannels ; iChannel > 0 ; ) {
// 	fStripSignalF[--iChannel] = fGen->Landau(.1,.02);
// 	fStripSignalB[  iChannel] = fGen->Landau(.1,.02);
// 	fStripSignalF[--iChannel] = 0.;
// 	fStripSignalB[  iChannel] = 0.;
	fStripSignalF[--iChannel] = TMath::Abs(gRandom->Gaus(0.,fFNoiseWidth));
	fStripSignalB[  iChannel] = TMath::Abs(gRandom->Gaus(0.,fBNoiseWidth));
      }
      
      for (Int_t iSensor=sector->GetNSensors(); iSensor > 0 ; ) {
	CbmStsSensor* sensor = sector->GetSensor(--iSensor);
	
	ProduceHitResponse(sensor);
      }

      Int_t   stationNr = sector->GetStationNr();
      Int_t    sectorNr = sector->GetSectorNr();
      Int_t sectorDetId = sector->GetDetectorId();
      
      for ( Int_t ifstr = 0 ; ifstr < nChannels ; ifstr++ ) {
	if ( fStripSignalF[ifstr] < fFThreshold ) continue;
// 	cout << "digi#" << fNDigis << " -> making fdigi at " << stationNr << "," << sectorNr 
// 	     << " at channel " << ifstr << " with signal " << fStripSignalF[ifstr] << endl;
	Int_t digiFSignal = 1+(Int_t)((fStripSignalF[ifstr]-fFThreshold)/fFMinStep);
	if ( digiFSignal >= fFNofSteps ) digiFSignal = fFNofSteps-1;
	new ((      *fDigis)[fNDigis]) CbmStsDigi(0, stationNr, sectorNr,
						  0, ifstr, 
						  digiFSignal, 0);
	//						  fStripSignalF[ifstr], 0);
	set<Int_t>::iterator it1;
	set<Int_t> chPnt = fFChannelPointsMap[ifstr];
	Int_t pnt;
	CbmStsDigiMatch* match;
	if ( chPnt.size() == 0 ) {
// 	  cout << "digi#" << fNDigis << " -> making fdigi at " << stationNr << "," << sectorNr 
// 	       << " at channel " << ifstr << " with signal " << fStripSignalF[ifstr] << endl;
	  new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(-666);
	}
	else {
	  for (it1=chPnt.begin(); it1!=chPnt.end(); it1++) {
	    pnt = (*it1);
	    if ( it1==chPnt.begin() ) 
	      match = new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(pnt);
	    else {
	      match->AddPoint(pnt);
	      fNMulti++;
	    }
	  }
	}
	fNDigis++;
      }
      
      for ( Int_t ibstr = 0 ; ibstr < nChannels ; ibstr++ ) {
	if ( fStripSignalB[ibstr] < fBThreshold ) continue;
	Int_t digiBSignal = 1+(Int_t)((fStripSignalB[ibstr]-fBThreshold)/fBMinStep);
	if ( digiBSignal >= fBNofSteps ) digiBSignal = fBNofSteps-1;
	new ((      *fDigis)[fNDigis]) CbmStsDigi(0, stationNr, sectorNr,
						  1, ibstr, 
						  digiBSignal, 0);
	//						  fStripSignalB[ibstr], 0);
	set<Int_t>::iterator it1;
	set<Int_t> chPnt = fBChannelPointsMap[ibstr];
	Int_t pnt;
	CbmStsDigiMatch* match;
	if ( chPnt.size() == 0 ) {
// 	  cout << "digi#" << fNDigis << " -> making fdigi at " << stationNr << "," << sectorNr 
// 	       << " at channel " << ifstr << " with signal " << fStripSignalF[ifstr] << endl;
	  new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(-666);
	}
	else {
	  for (it1=chPnt.begin(); it1!=chPnt.end(); it1++) {
	    pnt = (*it1);
	    if ( it1==chPnt.begin() ) 
	    match = new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(pnt);
	    else {
	      match->AddPoint(pnt);
	      fNMulti++;
	    }
	  }
	}
	fNDigis++;
      }
    }
  }
  
  fTimer.Stop();
  cout << "+ " << flush;
  cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
       << fixed << right << fTimer.RealTime() 
       << " s, digis " << nDigisF << " / " << nDigisB << endl;
  
  fNEvents     += 1.;
  fNPoints     += Double_t(nPoints);
  fNOutside    += Double_t(nOutside);
  fNDigisFront += Double_t(nDigisF);
  fNDigisBack  += Double_t(nDigisB);
  fTime        += fTimer.RealTime();

}
// -------------------------------------------------------------------------

// -----   Private method ProduceHitResponse   --------------------------------
void CbmStsRealDigitize::ProduceHitResponse(CbmStsSensor* sensor) {
  set <Int_t> pSet;
  if ( fPointMap.find(sensor) == fPointMap.end() ) {
    cerr << "-E- " << fName << "::Exec:: sensor" 
	 << " not found in digi scheme!" << endl;
    return;
  }
  pSet = fPointMap[sensor];

  Int_t       iPoint = -1;
  CbmStsPoint* point = NULL;

  set<Int_t>::iterator it1;

  for (it1=pSet.begin(); it1!=pSet.end(); it1++) {
    iPoint = (*it1);
    point  = (CbmStsPoint*) fPoints->At(iPoint);

    Double_t xin = point->GetXIn();
    Double_t yin = point->GetYIn();
    Double_t zin = point->GetZIn();
    
    Double_t xvec = point->GetXOut()-xin;
    Double_t yvec = point->GetYOut()-yin;
    Double_t zvec = point->GetZOut()-zin;

    Int_t nofSteps = (Int_t)(TMath::Sqrt(xvec*xvec+yvec*yvec+zvec*zvec)/fStep+1);
    
    Double_t stepEL = fEnergyLossToSignal*point->GetEnergyLoss()/(nofSteps+1);
    
    xvec = xvec/((Double_t)nofSteps);  
    yvec = yvec/((Double_t)nofSteps);  
    zvec = zvec/((Double_t)nofSteps);  

    for ( Int_t istep = 0 ; istep <= nofSteps ; istep++ ) {
      //      Float_t iFChan = sensor->GetChannelPlus(xin, yin, 0);
      Int_t   iIChan = sensor->GetFrontChannel(xin,yin,zin);
	//(Int_t)iFChan;
      
      if ( iIChan != -1 ) {
	fStripSignalF[iIChan] += stepEL;
	fFChannelPointsMap[iIChan].insert(iPoint);
      }

      //      iFChan = sensor->GetChannelPlus(xin, yin, 1);
      //      iIChan = (Int_t)iFChan;
      iIChan = sensor->GetBackChannel (xin,yin,zin);
      
      if ( iIChan != -1 ) {
	fStripSignalB[iIChan] += stepEL;
	fBChannelPointsMap[iIChan].insert(iPoint);
      }
    
      xin+=xvec;
      yin+=yvec;
      zin+=zvec;
    }
    
  }

}
// -------------------------------------------------------------------------

// -----   Private method FindFiredStrips   --------------------------------
void CbmStsRealDigitize::FindFiredStrips(CbmStsPoint* pnt,Int_t& nofStr,Int_t*& strips,Double_t*& signals, Int_t side) {

  nofStr = 0;

  Double_t xin = pnt->GetXIn();
  Double_t yin = pnt->GetYIn();
  Double_t zin = pnt->GetZIn();

  gGeoManager->FindNode(xin,yin,zin);
  TGeoNode* curNode = gGeoManager->GetCurrentNode();
  
  CbmStsSensor* sensor = fDigiScheme->GetSensorByName(curNode->GetName());

  Double_t xvec = pnt->GetXOut()-xin;
  Double_t yvec = pnt->GetYOut()-yin;
  Double_t zvec = pnt->GetZOut()-zin;

  Int_t nofSteps = (Int_t)(TMath::Sqrt(xvec*xvec+yvec*yvec+zvec*zvec)/fStep+1);

  Double_t stepEL = fEnergyLossToSignal*pnt->GetEnergyLoss()/(nofSteps+1);

  xvec = xvec/((Double_t)nofSteps);  
  yvec = yvec/((Double_t)nofSteps);  
  zvec = zvec/((Double_t)nofSteps);  

  for ( Int_t istep = 0 ; istep <= nofSteps ; istep++ ) {
    Float_t iFChan = sensor->GetChannelPlus(xin, yin, side);
    Int_t   iIChan = (Int_t)iFChan;

    xin+=xvec;
    yin+=yvec;
    zin+=zvec;

    if ( iIChan == -1 ) continue;

    for ( Int_t ifstr = 0 ; ifstr < nofStr ; ifstr++ ) {
      if ( strips[ifstr] == iIChan ) {
	signals[ifstr] += stepEL;
	iIChan = -1;
	break;
      }
    }
    if ( iIChan == -1 ) continue;

    strips [nofStr] = iIChan;
    signals[nofStr] = stepEL;

    nofStr++;

  }
}
// -------------------------------------------------------------------------
 
// -----   Private method SetParContainers   -------------------------------
void CbmStsRealDigitize::SetParContainers() {

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
InitStatus CbmStsRealDigitize::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPoints = (TClonesArray*) ioman->GetObject("StsPoint");

  // Register output array StsDigi
  fDigis = new TClonesArray("CbmStsDigi",1000);
  ioman->Register("StsDigi", "Digital response in STS", fDigis, kTRUE);

  // Register output array StsDigiMatches
  fDigiMatches = new TClonesArray("CbmStsDigiMatch",1000);
  ioman->Register("StsDigiMatch", "Digi Match in STS", fDigiMatches, kTRUE);

//   fGen = new TRandom3();
//   time_t curtime;
//   time(&curtime);
//   fGen->SetSeed(curtime);

  fStripSignalF = new Double_t[2000];
  fStripSignalB = new Double_t[2000];

  fEnergyLossToSignal    = 200000.;

  fFNofSteps = (Int_t)TMath::Power(2,(Double_t)fFNofBits);
  fBNofSteps = (Int_t)TMath::Power(2,(Double_t)fBNofBits);

  // Build digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    MakeSets();

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
InitStatus CbmStsRealDigitize::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    MakeSets();
    return kSUCCESS;
  }

  return kERROR;

}
// -------------------------------------------------------------------------


// -----   Private method MakeSets   ---------------------------------------
void CbmStsRealDigitize::MakeSets() {

  fPointMap.clear();
  Int_t nStations = fDigiScheme->GetNStations();
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    CbmStsStation* station = fDigiScheme->GetStation(iStation);
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      Int_t nSensors = sector->GetNSensors();
      for (Int_t iSensor=0; iSensor<nSensors; iSensor++) {
	CbmStsSensor* sensor = sector->GetSensor(iSensor);
	set<Int_t> a;
	fPointMap[sensor] = a;
      }
    }
  }
  fFChannelPointsMap.clear();
  fBChannelPointsMap.clear();
  for ( Int_t ichan = 2000 ; ichan > 0 ; ) {
    set<Int_t> a;
    fFChannelPointsMap[--ichan] = a;
    set<Int_t> b;
    fBChannelPointsMap[  ichan] = b;
  }
}
// -------------------------------------------------------------------------


// -----   Private method Reset   ------------------------------------------
void CbmStsRealDigitize::Reset() {
  //  fNPoints = fNOutside = fNDigisFront = fNDigisBack = fTime = 0.;
  fNDigis = fNMulti = 0;
  fFChannelPointsMap.clear();
  fBChannelPointsMap.clear();
  //  if ( fDigis ) fDigis->Clear();
  //  if ( fDigiMatches ) fDigiMatches->Clear();
  if ( fDigis ) fDigis->Delete();
  if ( fDigiMatches ) fDigiMatches->Delete();
}
// -------------------------------------------------------------------------


// -----   Virtual method Finish   -----------------------------------------
void CbmStsRealDigitize::Finish() {
  cout << endl;
  cout << "============================================================"
       << endl;
  cout << "===== " << fName << ": Run summary " << endl;
  cout << "===== " << endl;
  cout << "===== Events processed          : " << setw(8) << fNEvents << endl;
  cout.setf(ios_base::fixed, ios_base::floatfield);
  cout << "===== Real time per event       : " 
       << setw(8) << setprecision(4) 
       << fTime / fNEvents << " s" << endl;
  cout << "===== StsPoints per event       : " 
       << setw(8) << setprecision(2) 
       << fNPoints / fNEvents << endl;
  cout << "===== Outside hits per event    : " 
       << setw(8) << setprecision(2) 
       << fNOutside / fNEvents << " = " 
       << setw(6) << setprecision(2) 
       << fNOutside / fNPoints * 100. << " %" << endl;
  cout << "===== Front digis per point     : " 
       << setw(8) << setprecision(2) 
       << fNDigisFront / (fNPoints-fNOutside) << endl;
  cout << "===== Back digis per point      : " 
       << setw(8) << setprecision(2) 
       << fNDigisBack / (fNPoints-fNOutside) << endl;
  cout << "============================================================"
       << endl;
	
}					       
// -------------------------------------------------------------------------



ClassImp(CbmStsRealDigitize)

