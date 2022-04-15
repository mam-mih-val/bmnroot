//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize source file                 -----
// -----                  Created 30/08/06  by V. Friese               -----
// -------------------------------------------------------------------------

// Includes from STS
#include "CbmStsIdealDigitize.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

// Includes from base
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from ROOT
#include "TClonesArray.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TGeoManager.h"
#include "TGeoNode.h"

#include <iostream>
#include <iomanip>
#include <map>

using std::cout;
using std::cerr;
using std::endl;
using std::pair;
using std::setw;
using std::left;
using std::right;
using std::fixed;
using std::setprecision;
using std::map;



// -----   Default constructor   ------------------------------------------
CbmStsIdealDigitize::CbmStsIdealDigitize() 
  : FairTask("STS Digitizer", 1),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigiScheme(NULL),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fNPoints(0),
  fNFailed(0),
  fNOutside(0),
  fNMulti(0),
  fNDigis(0),
  fTimer(),
  fChannelMap()
{
  Reset();
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsIdealDigitize::CbmStsIdealDigitize(Int_t iVerbose) 
  : FairTask("STSDigitize", iVerbose),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigiScheme(NULL),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fNPoints(0),
  fNFailed(0),
  fNOutside(0),
  fNMulti(0),
  fNDigis(0),
  fTimer(),
  fChannelMap()
{ 
  Reset();
}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsIdealDigitize::CbmStsIdealDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigiScheme(NULL),
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fNPoints(0),
  fNFailed(0),
  fNOutside(0),
  fNMulti(0),
  fNDigis(0),
  fTimer(),
  fChannelMap()
{ 
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsIdealDigitize::~CbmStsIdealDigitize() { 
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
void CbmStsIdealDigitize::Exec(Option_t* opt) {

  // Reset all eventwise counters
  fTimer.Start();
  Reset();

  // Verbose screen output
  if ( fVerbose > 2 ) {
    cout << endl << "-I- " << fName << ": executing event" << endl;
    cout << "----------------------------------------------" << endl;
  }

  // Loop over all StsPoints
  if ( ! fPoints ) {
    cerr << "-W- " << fName << "::Exec: No input array (STSPoint) "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }

  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);
    fNPoints++;

    Double_t xIn = point->GetXIn(); 
    Double_t yIn = point->GetYIn(); 
    Double_t zIn = point->GetZIn(); 

    gGeoManager->FindNode(xIn,yIn,zIn);
    TGeoNode* curNode = gGeoManager->GetCurrentNode();

    //AZ CbmStsSensor* sensor = fDigiScheme->GetSensorByName(curNode->GetName());
    //AZ
    CbmStsSensor* sensor = NULL;
    if ( fDigiScheme->IsNewGeometry() ) {
      TString curPath = fDigiScheme->GetCurrentPath();
      sensor = fDigiScheme->GetSensorByName(curPath);
    }
    else sensor = fDigiScheme->GetSensorByName(curNode->GetName());
    //AZ
    
    if ( !sensor ) 
      LOG(fatal) << "No sensor in DigiScheme with name: " << curNode->GetName();

    Int_t stationNr = sensor->GetStationNr();
    Int_t  sectorNr = sensor->GetSectorNr();

    // Take point coordinates in the midplane of the sensor
    if ( ! point->IsUsable() ) continue;
    Double_t xpt = point->GetX(sensor->GetZ0());
    Double_t ypt = point->GetY(sensor->GetZ0());

    Int_t iChan    = -1;
    Int_t channelF = -1;
    Int_t channelB = -1;

    iChan = sensor->GetChannel(xpt, ypt, 0);

    // point outside active sensor area
    if ( iChan < 0 ) {
      if ( fVerbose ) {
	Float_t tempX = sensor->GetX0();
	Float_t tempY = sensor->GetY0();
	Float_t rot = sensor->GetRotation();
	Float_t tempLX = sensor->GetLx()/2.*TMath::Cos(rot)+sensor->GetLy()/2.*TMath::Sin(rot);
	Float_t tempLY = sensor->GetLy()/2.*TMath::Cos(rot)+sensor->GetLx()/2.*TMath::Sin(rot);
	
	cout.precision(8);
	cout << fName << ". Warning: Point " << xpt << ", " << ypt << ", " << sensor->GetZ0() << " not inside sector." << endl;
      }
      continue;
    }

    channelF = iChan;

    Int_t sectorDetId  = 2 << 24 | stationNr << 16 | sectorNr << 4;
   
    // Treat front side
    pair<Int_t, Int_t> a(sectorDetId, channelF);
    if ( fChannelMap.find(a) == fChannelMap.end() ) {
      // Channel not yet active. Create new Digi and Match.
      new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
					  0, channelF, 0, 0);
      new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(iPoint);
      fChannelMap[a] = fNDigis;
      fNDigis++;
    }
    else {
      // Channel already active. Update DigiMatch.
      Int_t iDigi = fChannelMap[a];
      CbmStsDigiMatch* match 
	= dynamic_cast<CbmStsDigiMatch*>(fDigiMatches->At(iDigi));
      if ( match ) {
	match->AddPoint(iPoint);
	fNMulti++;
      }
    }
    
    // Treat back side (strip sensors only)
    if ( sensor->GetType() < 2 || sensor->GetType() > 3 ) continue;

    iChan = sensor->GetChannel(xpt, ypt, 1);
    if ( iChan <0 ) {
      cerr << "-W- " << fName << "::Exec: No back side channel "
	   << " for StsPoint " << iPoint << ", station "
	   << stationNr << ", sector " << sectorNr 
	   << ", front side channel " << channelF << endl;
      continue;
    }
    sectorDetId = sectorDetId | (1<<0);  // for back side channel
    channelB = iChan;
    pair<Int_t, Int_t> b(sectorDetId, channelB);
    if ( fChannelMap.find(b) == fChannelMap.end() ) {
      // Channel not yet active. Create new Digi and Match.
      new ((*fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
					  1, channelB, 0, 0);
      new ((*fDigiMatches)[fNDigis]) CbmStsDigiMatch(iPoint);
      fChannelMap[b] = fNDigis;
      fNDigis++;
    }
    else {
      // Channel already active. Update DigiMatch.
      Int_t iDigi = fChannelMap[b];
      CbmStsDigiMatch* match 
	= dynamic_cast<CbmStsDigiMatch*>(fDigiMatches->At(iDigi));
      if ( match ) {
	  match->AddPoint(iPoint);
	  fNMulti++;
      }
    }

    // Verbose screen output
    if ( fVerbose > 2 ) {
      cout.precision(6);
      cout << "StsPoint " << iPoint << ", station " << stationNr
	   << ", (" << xpt << ", " << ypt << ") cm, sector " 
	   << sectorNr << ", front " << channelF << ", back "
	   << channelB << endl;
    }


    // Not found in any sector?
    if ( sectorNr == -1 ) fNOutside++;      

  }  // StsPoint loop

  // Screen output
  fTimer.Stop();
  if ( fVerbose ) {
    cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8)
	 << fixed << right << fTimer.RealTime()
	 << " s, points " << fNPoints << ", failed " << fNFailed 
	 << ", outside " << fNOutside << ", multihits " << fNMulti 
	 << ", digis " << fNDigis << endl;
  }
}
// -------------------------------------------------------------------------


 
// -----   Private method SetParContainers   -------------------------------
void CbmStsIdealDigitize::SetParContainers() {

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
InitStatus CbmStsIdealDigitize::Init() {

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
InitStatus CbmStsIdealDigitize::ReInit() {

  // Clear digitisation scheme
  fDigiScheme->Clear();

  // Build new digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) return kSUCCESS;

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method Reset   ------------------------------------------
void CbmStsIdealDigitize::Reset() {
  fNPoints = fNFailed = fNOutside = fNMulti = fNDigis = 0;
  fChannelMap.clear();
  //  if ( fDigis ) fDigis->Clear();
  //  if ( fDigiMatches ) fDigiMatches->Clear();
  if ( fDigis ) fDigis->Delete();
  if ( fDigiMatches ) fDigiMatches->Delete();
}
// -------------------------------------------------------------------------





ClassImp(CbmStsIdealDigitize)

