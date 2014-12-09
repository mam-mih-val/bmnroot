//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsIdealFindHits source file                 -----
// -----                  Created 11/09/06  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsIdealFindHits.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h" //AZ
#include "CbmStsPoint.h" //AZ
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
CbmStsIdealFindHits::CbmStsIdealFindHits() 
  : FairTask("STS Ideal Hit Finder", 1), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fDigiMatch(NULL), //AZ
  fPoints(NULL), //AZ
  fHits(NULL),
  fDigiScheme(new CbmStsDigiScheme()),
  fNStations(0),
  fNEvents(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer(),
  fTime1(0.)
{}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsIdealFindHits::CbmStsIdealFindHits(Int_t iVerbose) 
  : FairTask("STSIdealFindHits", iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fDigiMatch(NULL), //AZ
  fPoints(NULL), //AZ
  fHits(NULL),
  fDigiScheme(new CbmStsDigiScheme()),
  fNStations(0),
  fNEvents(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer(),
  fTime1(0.)
{}
// -------------------------------------------------------------------------



// -----   Constructor with name   -----------------------------------------
CbmStsIdealFindHits::CbmStsIdealFindHits(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fDigis(NULL),
  fDigiMatch(NULL), //AZ
  fPoints(NULL), //AZ
  fHits(NULL),
  fDigiScheme(new CbmStsDigiScheme()),
  fNStations(0),
  fNEvents(0),
  fDigiMapF(),
  fDigiMapB(),
  fTimer(),
  fTime1(0.)
{}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsIdealFindHits::~CbmStsIdealFindHits() {
  if ( fHits ) {
    fHits->Delete();
    delete fHits;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void CbmStsIdealFindHits::Exec(Option_t* opt) {
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
  SortDigis();

  // Find hits in sectors
  Int_t nDigisF = 0;
  Int_t nDigisB = 0;
  Int_t nHits   = 0;
  Int_t nStations = fDigiScheme->GetNStations();
  CbmStsStation* station = NULL;
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    station = fDigiScheme->GetStation(iStation);

    Int_t nDigisFInStation = 0;
    Int_t nDigisBInStation = 0;
    Int_t nHitsInStation   = 0;
    Int_t nSectors = station->GetNSectors();
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
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
      Int_t nDigisFInSector = fSet.size();
      Int_t nDigisBInSector = bSet.size();
      Int_t nHitsInSector   = FindHits(station, sector, fSet, bSet);
      if ( fVerbose > 2 ) 
	cout << "Sector " << sector->GetSectorNr() 
	     << ", Digis front " << nDigisFInSector 
	     << ", Digis Back " << nDigisBInSector
	     << ", Hits " << nHitsInSector << endl;
      nHitsInStation   += nHitsInSector;
      nDigisFInStation += nDigisFInSector;
      nDigisBInStation += nDigisBInSector;      
    }      // Sector loop

    if ( fVerbose > 1 ) cout << "Total for station " 
			     << station->GetStationNr() << ": Digis front "
			     << nDigisFInStation << ", digis back "
			     << nDigisBInStation << ", hits "
			     << nHitsInStation << endl;
    nDigisB += nDigisBInStation;
    nDigisF += nDigisFInStation;
    nHits   += nHitsInStation;
    
  }       // Station loop

  fTimer.Stop();  
  if ( fVerbose > 1 ) {
    cout << endl;
    cout << "-I- " << fName << ":Event summary" << endl;
    cout << "    Active channels front side: " << nDigisF << endl;
    cout << "    Active channels back side : " << nDigisB << endl;
    cout << "    Hits created              : " << nHits   << endl;
    cout << "    Real time                 : " << fTimer.RealTime() 
	 << endl;
  }
  if ( fVerbose == 1 ) {
    if ( warn ) cout << "- ";
    else        cout << "+ ";
    cout << setw(15) << left << fName << ": " << setprecision(4) << setw(8) 
	 << fixed << right << fTimer.RealTime() 
	 << " s, digis " << nDigisF << " / " << nDigisB << ", hits: " 
	 << nHits << endl;
  }

  
}
// -------------------------------------------------------------------------

    


// -----   Private method SetParContainers   -------------------------------
void CbmStsIdealFindHits::SetParContainers() {

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
InitStatus CbmStsIdealFindHits::Init() {
  
  // Get input array
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fDigis = (TClonesArray*) ioman->GetObject("StsDigi");
  fDigiMatch = (TClonesArray*) ioman->GetObject("StsDigiMatch"); //AZ
  fPoints = (TClonesArray*) ioman->GetObject("StsPoint"); //AZ

  // Register output array
  fHits = new TClonesArray("CbmStsHit", 1000);
  ioman->Register("StsHit", "Hit in STS", fHits, kTRUE);

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
  
  return kSUCCESS; 
}
// -------------------------------------------------------------------------




// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsIdealFindHits::ReInit() {

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
void CbmStsIdealFindHits::MakeSets() {

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
void CbmStsIdealFindHits::SortDigis() {
 
  // Check input array
  if ( ! fDigis ) {
    cout << "-E- " << fName << "::SortDigis: No input array!" << endl;
    return;
  }

  // Clear sector digi sets
  map<CbmStsSector*, set<Int_t> >::iterator mapIt;
  for (mapIt=fDigiMapF.begin(); mapIt!=fDigiMapF.end(); mapIt++)
    ((*mapIt).second).clear();
  for (mapIt=fDigiMapB.begin(); mapIt!=fDigiMapB.end(); mapIt++)
    ((*mapIt).second).clear();

  // Fill digis into sets
  CbmStsDigi* digi = NULL;
  CbmStsSector* sector = NULL;
  Int_t stationNr = -1;
  Int_t sectorNr  = -1;
  Int_t iSide     = -1;
  Int_t nDigis = fDigis->GetEntriesFast();
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


    

// -----   Private method FindHits   ---------------------------------------
Int_t CbmStsIdealFindHits::FindHits(CbmStsStation* station,
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
  Double_t stereoF = sector->GetStereoF();
  Double_t stereoB = sector->GetStereoB();

  //  Double_t z      = station->GetZ();
  Int_t stationNr = station->GetStationNr();
  Int_t sectorNr  = sector->GetSectorNr();

  // Some auxiliary values
  Double_t sinrot = TMath::Sin(rot);
  Double_t cosrot = TMath::Cos(rot);
  Double_t tanstrB = TMath::Tan(stereoB);
  Double_t tanstrF = TMath::Tan(stereoF);
  
  // Calculate error matrix in sector system
  Double_t vX, vY, vXY;
  //AZ if ( iType == 1 ) {
  if ( iType == 1 || TMath::Abs(stereoF) < 0.01 && (TMath::Abs(stereoB*TMath::RadToDeg()-90) < 0.01 ||
						    TMath::Abs(stereoB*TMath::RadToDeg()+90) < 0.01)) { //AZ
    vX  = dx / TMath::Sqrt(12.);
    vY  = dy / TMath::Sqrt(12.);
    vXY = 0.;
  }
  else if ( iType == 2 || iType == 3 ) {
    if (stereoF==0.) {
      vX  = dx / TMath::Sqrt(12.);
      vY  = dx / TMath::Sqrt(6.) / TMath::Abs(tanstrB);
      vXY = -1. * dx * dx / 12. / tanstrB;
    }
    else {
      vX  = dx / TMath::Sqrt(24.);
      vY  = dx / TMath::Sqrt(24.) / TMath::Abs(tanstrB);
      vXY = 0.;
    }
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
    Int_t iDigiF = -1;
    Int_t iDigiB = -1;
    Int_t iChanF = -1;
    Int_t iChanB = -1;
    Int_t nHits = fHits->GetEntriesFast();
    Double_t xHit;
    Double_t yHit;
    Double_t zHit;
    TVector3 pos, dpos;
    CbmStsDigi* digiF = NULL;
    CbmStsDigi* digiB = NULL;
    CbmStsDigiMatch *digiFM = NULL, *digiBM = NULL; //AZ

    for (it1=fSet.begin(); it1!=fSet.end(); it1++) {
      iDigiF = (*it1);
      digiF  = (CbmStsDigi*) fDigis->At(iDigiF);
      if ( ! digiF ) {
	cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	     << iDigiF << " in front set of sector " 
	     << sector->GetSectorNr() << ", station " 
	     << station->GetStationNr() << endl;
	continue;
      }
      iChanF = digiF->GetChannelNr();
      digiFM = (CbmStsDigiMatch*) fDigiMatch->UncheckedAt(iDigiF); // AZ
      
      for (it2=bSet.begin(); it2!=bSet.end(); it2++ ) {
	iDigiB = (*it2);
	digiB = (CbmStsDigi*) fDigis->At(iDigiB);
	if ( ! digiB ) {
	  cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	       << iDigiB << " in back set of sector " 
	       << sector->GetSectorNr() << ", station " 
	       << station->GetStationNr() << endl;
	  continue;
	}
	// AZ - pixel simulation: intersect only digits from the same point
	digiBM = (CbmStsDigiMatch*) fDigiMatch->UncheckedAt(iDigiB); // AZ
	/*
	if (TMath::Abs(TMath::Abs(stereoF-stereoB)*TMath::RadToDeg()-90) < 0.1) {
	  Bool_t ok = kFALSE;
	  for (Int_t ii = 0; ii < 3; ++ii) {
	    if (digiFM->GetRefIndex(ii) < 0) break;
	    for (Int_t jj = 0; jj < 3; ++jj) {
	      if (digiBM->GetRefIndex(jj) < 0) break;
	      if (digiFM->GetRefIndex(ii) == digiBM->GetRefIndex(jj)) { 
		ok = kTRUE; 
		// Take MCPoint coordinates
		CbmStsPoint *point = (CbmStsPoint*) fPoints->UncheckedAt(digiFM->GetRefIndex(ii));
		xHit = point->GetXIn();
		yHit = point->GetYIn();
		break; 
	      }
	    }
	  }
	  if (!ok) continue;
	}
	*/
	//AZ

	iChanB = digiB->GetChannelNr();

	Int_t sensorDetId = sector->Intersect(iChanF,iChanB,xHit,yHit,zHit);

	if ( sensorDetId == -1 ) continue;
	
	pos.SetXYZ(xHit, yHit, zHit);
	dpos.SetXYZ(sigmaX, sigmaY, 0.);

	Int_t statLayer = -1;
	for ( Int_t istatL = station->GetNofZ() ; istatL > 0 ; istatL-- ) 
	  if ( TMath::Abs(zHit-station->GetZ(istatL-1)) < 0.00001 ) {
	    statLayer = istatL-1;
	    break;
	  }
       
	if ( statLayer == -1 ) 
	  cout << "unknown layer for hit at z = " << zHit << endl;

	new ((*fHits)[nHits++]) CbmStsHit(sensorDetId, pos, dpos, wXY, 
					  iDigiF, iDigiB, iChanF, iChanB, statLayer);

	nNew++;
	if ( fVerbose > 3 ) cout << "New StsHit at (" << xHit << ", " << yHit
				 << ", " << zHit << "), station " 
				 << stationNr << ", sector " << sectorNr 
				 << ", channel " << iChanF << " / " 
				 << iChanB 
				 << endl;
	
      }      // back side strip loop
    }        // front side strip loop
    
  }          // strip GSI
  // ---------------------------------------------------------------------
 
  
  return nNew; 
}
// -------------------------------------------------------------------------

/*
// -----   Private method FindHits   ---------------------------------------
Int_t CbmStsIdealFindHits::FindHits(CbmStsStation* station,
			       CbmStsSector* sector, 
			       set<Int_t>& fSet, set<Int_t>& bSet) {

  // Counter
  Int_t nNew = 0;

  // Get sector parameters
  Int_t    detId  = sector->GetDetectorId();
  Int_t    iType  = sector->GetType();
  Double_t xc     = sector->GetX0();
  Double_t yc     = sector->GetY0();
  Double_t rot    = sector->GetRotation();
  Double_t lx     = sector->GetLx();
  Double_t ly     = sector->GetLy();
  Double_t dx     = sector->GetDx();
  Double_t dy     = sector->GetDy();
  Double_t stereoB = sector->GetStereoB();
  Double_t z      = sector->GetZ0();
  //  Double_t z      = station->GetZ();
  Int_t stationNr = station->GetStationNr();
  Int_t sectorNr  = sector->GetSectorNr();

  Int_t statLayer = 0;
  for ( Int_t isz = 1 ; isz < 11 ; isz++ )
    if ( TMath::Abs(station->GetZ(isz)-z) < 0.001 ) {
      statLayer = isz;
      break;
    }

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
    Int_t nColumns = Int_t(TMath::Ceil ( lx / dx ));
    Int_t iDigi = -1;
    Int_t iChan = -1;
    Int_t iCol  = -1;
    Int_t iRow  = -1;
    Int_t nHits = fHits->GetEntriesFast();
    Double_t xint, yint;
    Double_t x, y;
    TVector3 pos, dpos;
    CbmStsDigi* digi = NULL;
    for (it1=fSet.begin(); it1!=fSet.end(); it1++) {
      iDigi = (*it1);
      digi  = (CbmStsDigi*) fDigis->At(iDigi);
      if ( ! digi ) {
	cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	     << iDigi << " in front set of sector " 
	     << sector->GetSectorNr() << ", station " 
	     << station->GetStationNr() << endl;
	continue;
      }
      iChan = digi->GetChannelNr();
      iRow  = Int_t( iChan / nColumns );
      iCol  = iChan - iRow * nColumns;
      xint = ( Double_t(iCol) + 0.5 ) * dx;
      yint = ( Double_t(iRow) + 0.5 ) * dy;
      
      // Translation to centre of sector
      xint = xint - lx/2.;
      yint = yint - ly/2.;

      // Rotation around sector centre
      x = xint * cosrot - yint * sinrot;
      y = xint * sinrot + yint * cosrot;

      // Translation into global c.s.
      x = x + xc;
      y = y + yc;

      // Make new hit
      pos.SetXYZ(x, y, z);
      dpos.SetXYZ(sigmaX, sigmaY, 0.);
      new ((*fHits)[nHits++]) CbmStsHit(detId, pos, dpos, wXY, iDigi, -1);
      nNew++;
      if ( fVerbose > 3 ) cout << "New StsHit at (" << x << ", " << y 
			       << ", " << z << "), station " << stationNr
			       << ", sector " << sectorNr << ", channel "
			       << iChan << endl;
    }    // Loop over digi set

  }     // Pixel sensor
  // ---------------------------------------------------------------------

      
      
  // -----  Type 2: Strip sector OSU   -----------------------------------
  else if ( iType == 2 ) {
    Int_t iDigiF = -1;
    Int_t iDigiB = -1;
    Int_t iChanF = -1;
    Int_t iChanB = -1;
    Int_t nHits  = fHits->GetEntriesFast();
    Double_t x0  = 0.;
    Double_t xint, yint;
    Double_t xtemp, ytemp;
    Double_t x, y;
    TVector3 pos, dpos;
    CbmStsDigi* digiF = NULL;
    CbmStsDigi* digiB = NULL;
    for (it1=fSet.begin(); it1!=fSet.end(); it1++) {
      iDigiF = (*it1);
      digiF  = (CbmStsDigi*) fDigis->At(iDigiF);
      if ( ! digiF ) {
	cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	     << iDigiF << " in front set of sector " 
	     << sector->GetSectorNr() << ", station " 
	     << station->GetStationNr() << endl;
	continue;
      }
      iChanF = digiF->GetChannelNr();
      xint = ( Double_t(iChanF) + 0.5 ) * dx;            
      for (it2=bSet.begin(); it2!=bSet.end(); it2++ ) {
	iDigiB = (*it2);
	digiB = (CbmStsDigi*) fDigis->At(iDigiB);
	if ( ! digiB ) {
	  cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	       << iDigiB << " in back set of sector " 
	       << sector->GetSectorNr() << ", station " 
	       << station->GetStationNr() << endl;
	  continue;
	}
	iChanB = digiB->GetChannelNr();;
	x0 = ( Double_t(iChanB) + 0.5 ) * dx;
	yint = (x0 - xint) / tanstr;
	if ( ! ( yint>0. && yint<ly) ) continue;

	// Translation to centre of sector
	xtemp = xint - lx/2.;
	ytemp = yint - ly/2.;

	// Rotation around sector centre
	x = xtemp * cosrot - ytemp * sinrot;
	y = xtemp * sinrot + ytemp * cosrot;

	// Translation into global c.s.
	x = x + xc;
	y = y + yc;

	// Make new hit
	pos.SetXYZ(x, y, z);
	dpos.SetXYZ(sigmaX, sigmaY, 0.);
	new ((*fHits)[nHits++]) CbmStsHit(detId, pos, dpos, wXY, 
					  iDigiF, iDigiB);
	nNew++;
	if ( fVerbose > 3 ) cout << "New StsHit at (" << x << ", " << y 
				 << ", " << z << "), station " << stationNr
				 << ", sector " << sectorNr << ", channel "
				 << iChanF << " / " << iChanB << endl;
      }     // Back side set loop
    }       // Front side set sloop

  }         // Strip OSU
  // ---------------------------------------------------------------------

      
      
  // -----  Type 3: Strip sector GSI   -----------------------------------
  else if (iType == 3 ) {
    Int_t iDigiF = -1;
    Int_t iDigiB = -1;
    Int_t iChanF = -1;
    Int_t iChanB = -1;
    Int_t nHits = fHits->GetEntriesFast();
    Int_t nStripMax = ( stereoB<0. ? 0 :  Int_t(ly*tanstr/lx)+1 ); // max. number of strips
    Int_t nStripBeg = ( stereoB>0. ? 0 : -Int_t(ly*tanstr/lx)-1 );
    Double_t x0 = 0.;
    Double_t xint, yint;
    Double_t xtemp, ytemp;
    Double_t x,y;
    TVector3 pos, dpos;
    CbmStsDigi* digiF = NULL;
    CbmStsDigi* digiB = NULL;
    for (it1=fSet.begin(); it1!=fSet.end(); it1++) {
      iDigiF = (*it1);
      digiF  = (CbmStsDigi*) fDigis->At(iDigiF);
      if ( ! digiF ) {
	cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	     << iDigiF << " in front set of sector " 
	     << sector->GetSectorNr() << ", station " 
	     << station->GetStationNr() << endl;
	continue;
      }
      iChanF = digiF->GetChannelNr();
      xint = ( Double_t(iChanF) + 0.5 ) * dx;             
      for (it2=bSet.begin(); it2!=bSet.end(); it2++ ) {
	iDigiB = (*it2);
	digiB = (CbmStsDigi*) fDigis->At(iDigiB);
	if ( ! digiB ) {
	  cout << "-W- " << GetName() << "::FindHits: Invalid digi index " 
	       << iDigiB << " in back set of sector " 
	       << sector->GetSectorNr() << ", station " 
	       << station->GetStationNr() << endl;
	  continue;
	}
	iChanB = digiB->GetChannelNr();;
	x0 = ( Double_t(iChanB) + 0.5 ) * dx;
	for (Int_t iStrip=nStripBeg; iStrip<=nStripMax; iStrip++) {
	  yint = ( x0 - xint + Double_t(iStrip) * lx ) / tanstr;

	  if ( ! ( yint>0. && yint<ly ) ) continue;

	  Int_t discreteNetYPos = (Int_t)(0.5+yint*tanstr/dx);

	  // Translation to centre of sector
	  xtemp = xint - lx/2.;
	  ytemp = yint - ly/2.;
	  
	  // Rotation around sector centre
	  x = xtemp * cosrot - ytemp * sinrot;
	  y = xtemp * sinrot + ytemp * cosrot;

	  // Translation into global c.s.
	  x = x + xc;
	  y = y + yc;

	  // Make new hit
	  pos.SetXYZ(x, y, z);
	  dpos.SetXYZ(sigmaX, sigmaY, 0.);
	  new ((*fHits)[nHits++]) CbmStsHit(detId, pos, dpos, wXY, 
					    iDigiF, iDigiB, iChanF, discreteNetYPos, statLayer);
	  nNew++;
	  if ( fVerbose > 3 ) cout << "New StsHit at (" << x << ", " << y 
				   << ", " << z << "), station " 
				   << stationNr << ", sector " << sectorNr 
				   << ", channel " << iChanF << " / " 
				   << iChanB  << ", segment " << iStrip 
				   << endl;

	}    // strip segment loop
      }      // back side strip loop
    }        // front side strip loop

  }          // strip GSI
  // ---------------------------------------------------------------------

 
  return nNew;
}
// -------------------------------------------------------------------------

*/
    

ClassImp(CbmStsIdealFindHits)


	
	
	
  
		      





 
