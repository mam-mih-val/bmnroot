//* $Id: */

// -------------------------------------------------------------------------
// -----                    CbmStsDigitize source file             -----
// -----                  Created 08/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------




// Includes from ROOT
#include "TClonesArray.h"
#include "TGeoBBox.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TF1.h"
#include <TFile.h> //AZ-290322
#include "TRandom3.h"

#include "TGeoManager.h"
#include "TGeoNode.h"

// Includes from base
#include "FairField.h" //AZ
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// Includes from STS
#include "CbmGeoStsPar.h"
#include "CbmStsDigi.h"
#include "CbmStsDigiMatch.h"
#include "CbmStsDigiPar.h"
#include "CbmStsDigiScheme.h"
#include "CbmStsDigitize.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"
#include "CbmMCTrack.h" //AZ
#include "BmnGemFastDigitize.h" //AZ
#include "BmnSiliconPoint.h" //AZ-280322

#include <TDatabasePDG.h>

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
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
using std::vector;

// -----   Default constructor   ------------------------------------------
CbmStsDigitize::CbmStsDigitize() 
  : FairTask("STS Digitizer", 1),
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPointsSi(nullptr), //AZ-280322
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fRealistic(kFALSE),
  fDigiScheme(NULL),
  fNDigis(0),
  fNMulti(0),
  fNEvents(0),
  fNPoints(0),
  fNDigisFront(0),
  fNDigisBack(0),
  fStep(0.001),
  fEnergyLossToSignal(0.),
  fFThreshold(4.0),
  fBThreshold(4.0),
  fFNoiseWidth(0.1),
  fBNoiseWidth(0.1),
  fStripDeadTime(10),
  fFNofBits(11),
  fBNofBits(11),
  fFNofElPerAdc(10.),
  fBNofElPerAdc(10.),
  fFNofSteps(0),
  fBNofSteps(0),
  fStripSignalF(NULL),
  fStripSignalB(NULL),
  fTime(0.),
  fTimer(),
  fFChannelPointsMap(),
  fBChannelPointsMap(),
  fPointMap(),
  fastDig(NULL) //AZ
{
  fDigiScheme  = CbmStsDigiScheme::Instance();
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsDigitize::CbmStsDigitize(Int_t iVerbose) 
  : FairTask("STSDigitize", iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPointsSi(nullptr), //AZ-280322
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fRealistic(kFALSE),
  fDigiScheme(NULL),
  fNDigis(0),
  fNMulti(0),
  fNEvents(0),
  fNPoints(0),
  fNDigisFront(0),
  fNDigisBack(0),
  fStep(0.001),
  fEnergyLossToSignal(0.),
  fFThreshold(4.0),
  fBThreshold(4.0),
  fFNoiseWidth(0.1),
  fBNoiseWidth(0.1),
  fStripDeadTime(10),
  fFNofBits(11),
  fBNofBits(11),
  fFNofElPerAdc(10.),
  fBNofElPerAdc(10.),
  fFNofSteps(0),
  fBNofSteps(0),
  fStripSignalF(NULL),
  fStripSignalB(NULL),
  fTime(0.),
  fTimer(),
  fFChannelPointsMap(),
  fBChannelPointsMap(),
  fPointMap(),
  fastDig(NULL) //AZ
{ 
  fDigiScheme  = CbmStsDigiScheme::Instance();
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmStsDigitize::CbmStsDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
  fPointsSi(nullptr), //AZ-280322
  fPoints(NULL),
  fDigis(NULL),
  fDigiMatches(NULL),
  fRealistic(kFALSE),
  fDigiScheme(NULL),
  fNDigis(0),
  fNMulti(0),
  fNEvents(0),
  fNPoints(0),
  fNDigisFront(0),
  fNDigisBack(0),
  fStep(0.001),
  fEnergyLossToSignal(0.),
  fFThreshold(4.0),
  fBThreshold(4.0),
  fFNoiseWidth(0.1),
  fBNoiseWidth(0.1),
  fStripDeadTime(10),
  fFNofBits(11),
  fBNofBits(11),
  fFNofElPerAdc(10.),
  fBNofElPerAdc(10.),
  fFNofSteps(0),
  fBNofSteps(0),
  fStripSignalF(NULL),
  fStripSignalB(NULL),
  fTime(0.),
  fTimer(),
  fFChannelPointsMap(),
  fBChannelPointsMap(),
  fPointMap(),
  fastDig(NULL) //AZ
{ 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fRealistic   = kFALSE;
  fDigiScheme  = CbmStsDigiScheme::Instance();
  Reset();

  fStep = 0.001;

  fFThreshold  = 4.0;
  fBThreshold  = 4.0;
  fFNoiseWidth = 0.1;
  fBNoiseWidth = 0.1;

  fFNofBits    = 11;
  fBNofBits    = 11;
  fFNofElPerAdc    = 10.;
  fBNofElPerAdc    = 10.;
  fStripDeadTime = 10;
  fNEvents = 0.;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
CbmStsDigitize::~CbmStsDigitize() { 
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
void CbmStsDigitize::Exec(Option_t* opt) {

  // Reset all eventwise counters
  fTimer.Start();
  Reset();
  // Verbose screen output
  if ( fVerbose > 2 ) {
    cout << endl << "-I- " << fName << ": executing event" << endl; 
    cout << "----------------------------------------------" << endl;
  }

  Int_t nPoints  = 0;
  Int_t nDigisF  = 0;
  Int_t nDigisB  = 0;

  // Loop over all StsPoints
  if ( ! fPoints ) {
    cerr << "-W- " << fName << "::Exec: No input array (STSPoint) "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }
  if (fFNofBits>CbmStsDigi::GetNofAdcBits()||fBNofBits>CbmStsDigi::GetNofAdcBits()) {
    cerr << "-W- " << fName << "::Exec: Number of AdcBits("<<fFNofBits<<") during digitization exceeds ADC range("<<CbmStsDigi::GetNofAdcBits()<<") defined in data class "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }
  //AZ if (fFNofBits<=CbmStsDigi::GetNofAdcBits()&& fFNofElPerAdc!=10*TMath::Power(2,(12-fFNofBits))) {
  if (0 && fFNofBits<=CbmStsDigi::GetNofAdcBits()&& fFNofElPerAdc!=10*TMath::Power(2,(12-fFNofBits))) {
    cerr << "-W- " << fName << "::Exec: Number of electrons per AdcChannel does not match Adc range "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }
  //AZ if (fBNofBits<=CbmStsDigi::GetNofAdcBits()&& fBNofElPerAdc!=10*TMath::Power(2,(12-fFNofBits))) {
  if (0 && fBNofBits<=CbmStsDigi::GetNofAdcBits()&& fBNofElPerAdc!=10*TMath::Power(2,(12-fFNofBits))) {
    cerr << "-W- " << fName << "::Exec: Number of electrons per AdcChannel does not match Adc range "
	 << endl;
    cout << "- " << fName << endl;
    return;
  }
  map<CbmStsSensor*, set<Int_t> >::iterator mapIt;
  for (mapIt=fPointMap.begin(); mapIt!=fPointMap.end(); mapIt++)
    ((*mapIt).second).clear();
  
  for (int idet = 0; idet < 2; ++idet) { //AZ-280322
    TClonesArray *points = (idet == 0) ? fPointsSi : fPoints;
    if (points == nullptr) continue;

    //AZ-280322 for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    for (Int_t iPoint = 0; iPoint < points->GetEntriesFast(); iPoint++) {
      //AZ-280322 CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);
      CbmStsPoint* point = (CbmStsPoint*) points->At(iPoint);
      /*AZ!!! Exclude some points !!!
	CbmMCTrack *mcTr = (CbmMCTrack*) fMCTracks->UncheckedAt(point->GetTrackID());
	if (TMath::Abs(mcTr->GetPdgCode()) != 211) continue;
	TVector3 mom;
	mcTr->GetMomentum(mom);
	if (mom.Eta() > 2) continue;
	//AZ
	*/
      //if (point->GetZIn() > 100.0) continue; //!!!AZ - exclude GEMs (010920)
      
      Double_t xin = point->GetXIn();
      Double_t yin = point->GetYIn();
      Double_t zin = point->GetZIn();
      gGeoManager->FindNode(xin,yin,zin);
      TGeoNode* curNode = gGeoManager->GetCurrentNode();
      if (TString(curNode->GetName()).Contains("part")) {
	// AZ - Test beam silicon
	//cout << curNode->GetName() << " " << gGeoManager->GetPath() << endl;
	gGeoManager->CdUp();
	curNode = gGeoManager->GetCurrentNode();
	//cout << curNode->GetName() << " " << gGeoManager->GetPath() << endl;
	//exit(0);
      }
      
      CbmStsSensor* sensor = NULL;
      if ( fDigiScheme->IsNewGeometry() ) {
	TString curPath = fDigiScheme->GetCurrentPath();
	//if (!curPath.Contains("Si") && CrossSpacer(curNode, point)) continue; //AZ - emulate spacers
	sensor = fDigiScheme->GetSensorByName(curPath);
      }
      else sensor = fDigiScheme->GetSensorByName(curNode->GetName());
      
      if ( fPointMap.find(sensor) == fPointMap.end() ) {
	cerr << "-E- " << fName << "::Exec:: sensor " << curNode->GetName()
	     << " not found in digi scheme!" << endl;
	continue;
      }
      fPointMap[sensor].insert(iPoint); 
      nPoints++;
    }
  } // for (int idet = 0; idet < 2;
    
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
      
      //-----aplying noise on every channel-----
      for (Int_t iChannel=nChannels ; iChannel > 0 ; ) {
	// 	fStripSignalF[--iChannel] = fGen->Landau(.1,.02);
	// 	fStripSignalB[  iChannel] = fGen->Landau(.1,.02);
	// 	fStripSignalF[--iChannel] = 0.;
	// 	fStripSignalB[  iChannel] = 0.;
	//AZ fStripSignalF[--iChannel] = TMath::Abs(gRandom->Gaus(0.,fFNoiseWidth));
	fStripSignalF[--iChannel] = 0;
	//AZ fStripSignalB[  iChannel] = TMath::Abs(gRandom->Gaus(0.,fBNoiseWidth));
      }
      //AZ 
      Int_t nChannelsB = sector->GetNChannelsBack();
      //-----aplying noise on every channel-----
      for (Int_t iChannel=nChannelsB ; iChannel > 0 ; ) {
	//AZ fStripSignalB[--iChannel] = TMath::Abs(gRandom->Gaus(0.,fBNoiseWidth));
	fStripSignalB[--iChannel] = 0;
      }
      //AZ 
      //if (sector->GetStationNr() > 3) cout << " yyyy " << sector->GetStationNr() << " " << sector->GetSectorNr() << endl; //AZ-290322
      
      for (Int_t iSensor=sector->GetNSensors(); iSensor > 0 ; ) {
	CbmStsSensor* sensor = sector->GetSensor(--iSensor);
	
	if (sensor->GetDx() < 0.02) ProduceHitResponseSi(sensor);
	else if (fastDig) {
	  if (fPointMap.find(sensor) != fPointMap.end())
	    ((BmnGemFastDigitize*)fastDig)->ProduceHitResponseFast(sensor,fPointMap[sensor],fStripSignalF,
								   fStripSignalB,fFChannelPointsMap,fBChannelPointsMap);
	} else ProduceHitResponse(sensor);
	//ProduceHitResponseAZ(sensor); //AZ
      }
      
      Int_t   stationNr = sector->GetStationNr();
      Int_t    sectorNr = sector->GetSectorNr();
      Int_t sectorDetId = sector->GetDetectorId();
      Double_t dx = sector->GetDx();
      
      for ( Int_t ifstr = 0 ; ifstr < nChannels ; ifstr++ ) {
	if ( fStripSignalF[ifstr] < 1) continue; //AZ
	//AZ if ( fStripSignalF[ifstr] < (fFThreshold*1000.) ) continue;//threshold cut
	//if (fastDig && dx > 0.02) {
	//if (fStripSignalF[ifstr] < 13.0*1) continue;
	//} else if ( fStripSignalF[ifstr] < (fFThreshold*1000.) ) continue;//threshold cut
	
	//-----random strip inefficiency-----
	Double_t generator;
	generator = gRandom->Rndm()*100.;
	//AZ if (generator< (fStripDeadTime/100.)*occupancy [iStation][iSector][ifstr/125]) continue;
	//-----------------------------------
	
	Int_t digiFSignal = 0;
	if (fastDig && dx > 0.02) digiFSignal = fStripSignalF[ifstr]/1.;
	else digiFSignal = 1 + (Int_t)((fStripSignalF[ifstr])/fFNofElPerAdc);
	if ( digiFSignal >= fFNofSteps ) digiFSignal = fFNofSteps-1;
	if (digiFSignal < 13) continue; //AZ
	//if (digiFSignal < 1) continue; //AZ - for cluster library
	//if (digiFSignal < 5) continue; //AZ - for cluster library
	new ((      *fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
						  0, ifstr, 
						  digiFSignal, 0);
	set<Int_t>::iterator it1;
	set<Int_t> chPnt = fFChannelPointsMap[ifstr];
	Int_t pnt;
	CbmStsDigiMatch* match;
	if ( chPnt.size() == 0 ) {
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
	nDigisF++;
      }
      
      nChannels = sector->GetNChannelsBack(); //AZ 
      
      for ( Int_t ibstr = 0 ; ibstr < nChannels ; ibstr++ ) {
	if ( fStripSignalB[ibstr] < 1) continue; //AZ
	//AZ if ( fStripSignalB[ibstr] < (fBThreshold*1000.) ) continue; //threshold cut
	//if (fastDig && dx > 0.02) {
	//if (fStripSignalB[ibstr] < 13.0*1) continue;
	//} else if ( fStripSignalB[ibstr] < (fBThreshold*1000.) ) continue; //threshold cut
	
	//-----random strip inefficiency-----
	Double_t generator;
	generator = gRandom->Rndm()*100.;
	//AZ if (generator< (fStripDeadTime/100.)*occupancy [iStation][iSector][ibstr/125]) continue;
	//-----------------------------------
	
	Int_t digiBSignal = 0;
	if (fastDig && dx > 0.02) digiBSignal = fStripSignalB[ibstr]/1.;
	else digiBSignal = 1 + (Int_t)((fStripSignalB[ibstr])/fBNofElPerAdc);
	if ( digiBSignal >= fBNofSteps ) digiBSignal = fBNofSteps-1;
	if (digiBSignal < 13) continue; //AZ
	//if (digiBSignal < 1) continue; //AZ - for cluster library
	//if (digiBSignal < 5) continue; //AZ - for cluster library
	new ((      *fDigis)[fNDigis]) CbmStsDigi(stationNr, sectorNr,
						  1, ibstr, 
						  digiBSignal, 0);
	set<Int_t>::iterator it1;
	set<Int_t> chPnt = fBChannelPointsMap[ibstr];
	Int_t pnt;
	CbmStsDigiMatch* match;
	if ( chPnt.size() == 0 ) {
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
	nDigisB++;
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
  fNDigisFront += Double_t(nDigisF);
  fNDigisBack  += Double_t(nDigisB);
  fTime        += fTimer.RealTime();

}
// -------------------------------------------------------------------------

// -----   Private method CrossSpacer   ------------------------------------     
Bool_t CbmStsDigitize::CrossSpacer(const TGeoNode *node, const CbmStsPoint *point) 
{
  // AZ - Check if particle goes thru the spacer (dead space)

  TString name = node->GetName();
  //if (name.Contains("SensorSV")) return kFALSE; // short strip area

  Double_t width = 0.1, pitch = 10.0; // spacer width/2 and pitch
  Double_t xyzloc[3], xyz[3] = {(point->GetXIn()+point->GetXOut())/2,
				(point->GetYIn()+point->GetYOut())/2,
				(point->GetZIn()+point->GetZOut())/2};
  gGeoManager->MasterToLocal(xyz, xyzloc);

  TGeoVolume *vol = node->GetVolume();
  TGeoBBox *box = (TGeoBBox*) vol->GetShape();
  Double_t dx = box->GetDX(), dy = box->GetDY();
  gGeoManager->CdUp();
  TGeoNode *nodeM = gGeoManager->GetCurrentNode();
  const Double_t *transl = nodeM->GetMatrix()->GetTranslation();

  // X-spacers: check distance from the beam pipe corner
  Double_t dist = 0;
  if (transl[0] > -1) dist = xyzloc[0] + dx;
  else dist = dx - xyzloc[0];
  Double_t dscaled = dist / pitch;
  Int_t nspacer = TMath::Nint(dscaled);
  if (nspacer == 0) return kFALSE; // at the border
  if (TMath::Abs(dist - nspacer * pitch) < width) return kTRUE;

  // Y-spacers: check distance from the beam pipe corner
  if (transl[1] > 0) dist = xyzloc[1] + dy;
  else dist = dy - xyzloc[1];
  dscaled = dist / pitch;
  nspacer = TMath::Nint(dscaled);
  if (nspacer == 0) return kFALSE; // at the border
  if (TMath::Abs(dist - nspacer * pitch) < width) return kTRUE; // Y-spacer
  
  return kFALSE;
}
// ------------------------------------------------------------------------- 

// -----   Private method ProduceHitResponse   --------------------------------
void CbmStsDigitize::ProduceHitResponseSi(CbmStsSensor* sensor) 
{
  // Produce  response in Silicon

  //AZ set <Int_t> pSet;
  if ( fPointMap.find(sensor) == fPointMap.end() ) {
    cerr << "-E- " << fName << "::ProduceHitResponse:: sensor" 
	 << " not found in digi scheme!" << endl;
    return;
  }

  //AZ pSet = fPointMap[sensor];
  set<Int_t> &pSet = fPointMap[sensor];

  Int_t       iPoint = -1;
  CbmStsPoint* point = NULL;
  BmnSiliconPoint* pointsi = nullptr; //AZ-280322

  set<Int_t>::iterator it1;

  Double_t dPitch, step = fStep; //AZ
  if (TString(sensor->GetName()).Contains("Si") || sensor->GetDx() < 0.0200) step = 0.001; //AZ - silicon
  TClonesArray *points = (fPointsSi == nullptr) ? fPoints : fPointsSi; //AZ-280322
  TVector3 posIn, posOut; //AZ-280322

  for (it1=pSet.begin(); it1!=pSet.end(); it1++) {
    iPoint = (*it1);
    //AZ-280322 point  = (CbmStsPoint*) fPoints->At(iPoint);
    if (fPointsSi) {
      pointsi  = (BmnSiliconPoint*) points->UncheckedAt(iPoint);
      pointsi->PositionIn(posIn);
      pointsi->PositionOut(posOut);
    } else {
      point  = (CbmStsPoint*) points->At(iPoint);
      point->PositionIn(posIn);
      point->PositionOut(posOut);
    }

    /*AZ-280322
    Double_t xin = point->GetXIn();
    Double_t yin = point->GetYIn();
    Double_t zin = point->GetZIn();
    
    Double_t xvec = point->GetXOut()-xin;
    Double_t yvec = point->GetYOut()-yin;
    Double_t zvec = point->GetZOut()-zin;
    cout << xin << " " << yin << " " << zin << " " << point->GetXOut() << " " << yvec << " " << point->GetZOut() << endl; //AZ-280322
    */
    posOut -= posIn; //AZ-280322
    Double_t xin = posIn.X(), yin = posIn.Y(), zin = posIn.Z(); //AZ-280322
    Double_t xvec = posOut.X(), yvec = posOut.Y(), zvec = posOut.Z(); //AZ-280322
    
    //Int_t nofSteps = (Int_t)(TMath::Sqrt(xvec*xvec+yvec*yvec+zvec*zvec)/fStep+1);
    Int_t nofSteps = (Int_t)(TMath::Sqrt(xvec*xvec+yvec*yvec+zvec*zvec)/step+1);
    
    //AZ-280322 Double_t stepEL = fEnergyLossToSignal*point->GetEnergyLoss()/(nofSteps+1);
    Double_t eloss = (fPointsSi == nullptr) ? point->GetEnergyLoss() : pointsi->GetEnergyLoss(); //AZ-280322
    Double_t stepEL = fEnergyLossToSignal * eloss / (nofSteps + 1); //AZ-280322
    //Double_t stepEL = 0.0; //AZ
    //if (sensor->GetDx() < 0.0200) stepEL = 280000000 * 1.5 * point->GetEnergyLoss() / (nofSteps+1); //AZ - Si
    //else stepEL = fEnergyLossToSignal*point->GetEnergyLoss()/(nofSteps+1); //AZ
    
    xvec = xvec/((Double_t)nofSteps);  
    yvec = yvec/((Double_t)nofSteps);  
    zvec = zvec/((Double_t)nofSteps);  

    for ( Int_t istep = 0 ; istep <= nofSteps ; istep++ ) {

      // AZ - introduce smearing
      Double_t xinS = xin, yinS = yin, sigma = 0.05; // 500um
      if (step < 0.0009) {
	// Smearing only for GEMs
	xinS += gRandom->Gaus(0.0,sigma);
	yinS += gRandom->Gaus(0.0,sigma);
      }
      //AZ
      
      //AZ Int_t   iIChan = sensor->GetFrontChannel(xin,yin,zin);
      Int_t   iIChan = sensor->GetFrontChannel(xinS,yinS,zin,dPitch);

      //AZ!!! Exclude 50% of channels for large stations
      //if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 500) iIChan = -1;
      //else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 500) iIChan = -1;
      
      if ( iIChan != -1 ) {
	fStripSignalF[iIChan] += stepEL;
	fFChannelPointsMap[iIChan].insert(iPoint);
      }

      //AZ iIChan = sensor->GetBackChannel (xin,yin,zin);
      iIChan = sensor->GetBackChannel (xinS,yinS,zin,dPitch);
      
      //AZ!!! Exclude 50% of channels for large stations
      //if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 600) iIChan = -1;
      //else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 600) iIChan = -1;

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

Double_t CbmStsDigitize::GetNumberOfClusters(Double_t beta, Double_t gamma, Double_t charge, Double_t p0, Double_t p1)
{ //ES

  Double_t beta2 = beta * beta;
  Double_t gamma2 = gamma * gamma;
  Double_t val = p0/beta2*(p1 + TMath::Log(beta2*gamma2) - beta2);
  //AZ return val;
  return TMath::Min(val*charge*charge,20000.);
}

// -----   Private method ProduceHitResponse   --------------------------------
void CbmStsDigitize::ProduceHitResponse(CbmStsSensor* sensor)
{
  //

  //AZ - get magnetic field (for digitizer parameters in GEMs)
  static Int_t ifield = -1;
  if (ifield < 0) {
    FairRunAna* run = FairRunAna::Instance();
    FairField *magField = run->GetField();
    ifield = TMath::Nint(TMath::Abs(magField->GetBy(0,0,135.0)));
  }
  
  //AZ set <Int_t> pSet;

  if ( fPointMap.find(sensor) == fPointMap.end() ) {
    cerr << "-E- " << fName << "::ProduceHitResponse:: sensor" 
    << " not found in digi scheme!" << endl;
    return;
  }

  //AZ pSet = fPointMap[sensor];
  set<Int_t> &pSet = fPointMap[sensor]; //AZ

  Int_t       iPoint = -1;
  CbmStsPoint* point = NULL;

  set<Int_t>::iterator it1;

  Double_t dPitch =  fStep, dx = sensor->GetDx(), dy = sensor->GetDy(); //AZ

  for (it1=pSet.begin(); it1!=pSet.end(); it1++) {

    //AZ Apply overall efficiency 
    //const Double_t eff0 = 0.65; 
    //if (gRandom->Rndm() > eff0) continue;
  
    iPoint = (*it1);
    //AZ point  = (CbmStsPoint*)fPoints->At(iPoint);
    point  = (CbmStsPoint*)fPoints->UncheckedAt(iPoint); //AZ
    TVector3 mom;
    point->Momentum(mom);

    CbmMCTrack *mcTr = (CbmMCTrack*) fMCTracks->UncheckedAt(point->GetTrackID()); //ES
    TParticlePDG *part = TDatabasePDG::Instance()->GetParticle(mcTr->GetPdgCode());
    Double_t mnoc = 1.0, mass = 3.7283999; // He4
    if (part) mass = part->Mass();
    if (mass > 0.0001) {
      TLorentzVector lorv;
      lorv.SetVectM(mom, mass);
      Double_t beta = lorv.Beta();
      Double_t gamma = lorv.Gamma();
      Double_t charge = (part) ? part->Charge()/3 : 1;
      mnoc = GetNumberOfClusters(beta, gamma, charge, 1.787, 12.33); //ES
      if (mnoc < 0.5) continue; //AZ
      //cout << " ***clus*** " << mass << " " << beta << " " << gamma << " " << mnoc << endl;
    }

    Double_t  moduleThickness = 0.3 + 0.25 +  0.2 + 0.15 ; // DriftGapThickness + FirstTransferGapThickness + SecondTransferGapThickness + InductionGapThickness;
    Int_t stationNr = Int_t(sensor->GetStationNr());

    Double_t xin = point->GetXIn();
    Double_t yin = point->GetYIn();
    Double_t zin = point->GetZIn();
    
    Double_t xvec = point->GetXOut()-xin;
    Double_t yvec = point->GetYOut()-yin;
    //AZ Double_t zvec;

    //AZ if (point->GetPz() > 0.0) zvec = point->GetZOut()-zin;
    //AZ else zvec = zin - point->GetZOut();
    Double_t zvec = point->GetZOut() - zin; //AZ
    
    //length of track
    Double_t track_length = TMath::Sqrt(xvec*xvec+yvec*yvec+zvec*zvec);
    
    //Energy loss step for cluster, for electron 
    //Double_t stepEL, stepEl_current; 

    // Exponential dependency on a distance between clusters
    Double_t current_length = 0.0;  //traversed length (counter)
    Double_t current_length_ratio = 0.0; //ratio of the traversed length to common track length (counter)

    Int_t collisions_cnt = 0; //total collision counter
    Double_t current_step = 0.0; //current distance between two collision points

    //Collection of collision points
    std::vector<std::vector<Double_t>> collision_points;
    ////mean collision distance (mean free flight path) [cm]
    Double_t mcd = 1/mnoc; //ES

    mcd = 0.0245; //AZ-190322 - test - pi+, p = 1 GeV/c

    while(current_length < track_length) {

      current_step = gRandom->Exp(mcd);
      current_length += current_step;

      if(current_length > track_length) break;

      current_length_ratio = current_length/track_length;

      //In terms of distance
      Double_t current_x = xin + current_length_ratio*xvec;
      Double_t current_y = yin + current_length_ratio*yvec;
      Double_t current_z = zin + current_length_ratio*zvec;

      std::vector<Double_t> collPoint;
      collPoint.push_back(current_x);
      collPoint.push_back(current_y);
      collPoint.push_back(current_z);
      collision_points.push_back(collPoint);
      collisions_cnt++;
    }

    //Each level - distance to the readout plane
    const Double_t level1 = 0.15;//InductionGapThickness;
    const Double_t level2 = 0.2 + 0.15;//InductionGapThickness+SecondTransferGapThickness;
    const Double_t level3 = 0.2 + 0.15 + 0.25;//InductionGapThickness+SecondTransferGapThickness+FirstTransferGapThickness;

    //Mean electron shift along x-axis (under the influence of the Lorentz force)
    Double_t xmean, sigma; // the dependence fitted by polynomial: f(x) = (p0 + p1*x + p2*x^2 + p3*x^3)
    //Double_t p0_xmean, p1_xmean, p2_xmean, p3_xmean;
    //Double_t p0_sigma, p1_sigma, p2_sigma, p3_sigma, p4_sigma, p5_sigma;
    Double_t p_xmean4[4] = { +4.45E-04, +0.0518626, +0.0676802, +0.00195376 };   // ArC4H10 (80:20, 0.4T - T=1.5 GeV)
    Double_t p_xmean5[4] = { +5.52E-04, +0.0659937, +0.0809207, +0.00718051 };   // ArC4H10 (80:20, 0.5T - Kr T = 2.36 Gev, B=0.5036 T)
    Double_t p_xmean6[4] = { +0.000602183, +0.0809795, +0.0918447, +0.0157542 }; // ArC4H10 (80:20, 0.6T - T=2.5 GeV)
    Double_t p_xmean7[4] = { +0.000909404, +0.0896112, +0.125486, +0.00723235 }; // ArC4H10 (80:20, 0.7T - T=3.5 GeV)
    Double_t p_xmean8[4] = { +0.000962017, +0.104726, +0.137705, +0.016483 };    // ArC4H10 (80:20, 0.8T - T=4.0 GeV)
    Double_t p_xmean9[4] = { +0.00103555, +0.117826, +0.160241, +0.0178768 };    // ArC4H10 (80:20, 0.9T - T=4.5 GeV)
    //Sigma electron smearing
    //Double_t sigma; //depends on the distance from current z-position to the readout plane
    Double_t p_sigma4[6] = { +0.000181581, +0.120391, -0.425589, +0.887732, -0.885041, +0.332821 };
    Double_t p_sigma5[6] = { +1.87E-04, +0.12034, -0.412872, +0.830354, -0.800251, +0.292069 };
    Double_t p_sigma6[6] = { +0.000137712, +0.125844, -0.468967, +1.01775, -1.04621, +0.403336 }; 
    Double_t p_sigma7[6] = { +0.000179813, +0.122855, -0.442513, +0.950076, -0.978707, +0.380569 };
    Double_t p_sigma8[6] = { +1.86E-04, +0.122116, -0.435668, +0.934093, -0.963362, +0.376114 };
    Double_t p_sigma9[6] = { +0.000190609, +0.121689, -0.424431, +0.881712, -0.871142, +0.323333 };

    Double_t *p_xmean = p_xmean8, *p_sigma = p_sigma8;

    if (ifield == 4) { p_xmean = p_xmean4; p_sigma = p_sigma4; }
    else if (ifield == 5) { p_xmean = p_xmean5; p_sigma = p_sigma5; }
    else if (ifield == 6) { p_xmean = p_xmean6; p_sigma = p_sigma6; }
    else if (ifield == 7) { p_xmean = p_xmean7; p_sigma = p_sigma7; }
    else if (ifield == 9) { p_xmean = p_xmean9; p_sigma = p_sigma9; }

    //stepEL = fEnergyLossToSignal*point->GetEnergyLoss()/(collisions_cnt + 1);

    for(Int_t icoll = 0; icoll < collision_points.size(); ++icoll) {

      Double_t xcoll = collision_points[icoll][0]; //x
      Double_t ycoll = collision_points[icoll][1]; //y
      Double_t zcoll = collision_points[icoll][2]; //z

      Double_t zdist; // current z-distance to the readout

      //Find z-distance to the readout depending on the electron drift direction
      //Consider that electron drift direction equals to forward Z axis drift

      //AZ if(stationNr % 2 != 0) zdist = moduleThickness - (zcoll - zin);
      //AZ else zdist = zcoll - zin;  
      zdist = TMath::Abs (zcoll - zin); //AZ
      if (stationNr % 2 != 0 && point->GetPz() > 0) zdist = moduleThickness - zdist; //AZ
      else if (stationNr % 2 == 0 && point->GetPz() < 0) zdist = moduleThickness - zdist; //AZ
      //std::cout << "zdist , zvec" << zdist << vec << std::endl;
      Double_t zdist2 = zdist * zdist, zdist3 = zdist2 * zdist;

      xmean = p_xmean[0] + p_xmean[1]*zdist + p_xmean[2]*zdist2 + p_xmean[3]*zdist3;
      sigma = p_sigma[0] + p_sigma[1]*zdist + p_sigma[2]*zdist2 + p_sigma[3]*zdist3 + 
	p_sigma[4]*zdist3*zdist + p_sigma[5]*zdist3*zdist2;

      //Number of electrons in the current collision
      Int_t n_electrons_cluster = gRandom->Landau(1.027, 0.11);
      if(n_electrons_cluster < 1) n_electrons_cluster = 1; //min
      if(n_electrons_cluster > 6) n_electrons_cluster = 6; //max

      for(Int_t ielectron = 0; ielectron < n_electrons_cluster; ++ielectron) {

        //Electron gain in each GEM cascade
        //Polya distribution is better, but Exponential is good too in our case
        Double_t gain_gem1 = gRandom->Exp(15); //...->Exp(V), where V is the mean value of the exponential distribution
        Double_t gain_gem2 = gRandom->Exp(15);
        Double_t gain_gem3 = gRandom->Exp(15);

        //if(gain_gem1 < 1.0) gain_gem1 = 1.0;
        //if(gain_gem2 < 1.0) gain_gem2 = 1.0;
        //if(gain_gem3 < 1.0) gain_gem3 = 1.0;

        int total_gain = 0;

        if (zdist < level1) {
            total_gain = 1.0;
        }
        else if (zdist >= level1 && zdist < level2) {
	  if (gain_gem3 < 1.0) gain_gem3 = 1.0;
	  total_gain = gain_gem3;
        }
        else if (zdist >= level2 && zdist < level3) {
	  if (gain_gem3 < 1.0) gain_gem3 = 1.0;
	  if (gain_gem2 < 1.0) gain_gem2 = 1.0;
	  total_gain = gain_gem3*gain_gem2;
        }
        else if (zdist >= level3) {
	  if (gain_gem3 < 1.0) gain_gem3 = 1.0;
	  if (gain_gem2 < 1.0) gain_gem2 = 1.0;
	  if (gain_gem1 < 1.0) gain_gem1 = 1.0;
	  total_gain = gain_gem3*gain_gem2*gain_gem1;
        }

        //Projection of the current electron on the readout (x,y-coordinates)
        double x_readout, y_readout;
        //stepEl_current = stepEL/(total_gain + 1);
        for(int igain = 0; igain < total_gain; ++igain) {

          //x-shift of the electon depends on the electron drift direction
          //Consider that electron drift direction equals to forward Z axis drift

          if(stationNr % 2 != 0) x_readout = gRandom->Gaus(xcoll-xmean, sigma);
          else x_readout = gRandom->Gaus(xcoll+xmean, sigma);

          y_readout = gRandom->Gaus(ycoll, sigma);

          Int_t   iIChan = sensor->GetFrontChannel(x_readout, y_readout, zcoll, dPitch);
          //AZ!!! Exclude 50% of channels for large stations
          //if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 500) iIChan = -1;
          //else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 500) iIChan = -1;
	  /*
          if ( iIChan != -1 ) {
	    fStripSignalF[iIChan] += 1.0;//stepEl_current;
	    fFChannelPointsMap[iIChan].insert(iPoint);
          }
	  */
	  Double_t dPrel = dPitch / dx;
          //if ( iIChan != -1 && dPitch/dx < 0.2) { // strip width 0.16 mm
          //AZ-200322 if ( iIChan != -1 && TMath::Abs(dPitch/dx) < 0.1) { // strip width 0.16 mm
	  if ( iIChan != -1) { //AZ-200322
	    //AZ-200322 fStripSignalF[iIChan] += 1.0;//stepEl_current;
	    fStripSignalF[iIChan] += 0.25;//stepEl_current;
	    fFChannelPointsMap[iIChan].insert(iPoint);
	    //AZ-200322 continue; // electron does not reach backside strips
          }

          //AZ iIChan = sensor->GetBackChannel (xin,yin,zin);
          iIChan = sensor->GetBackChannel(x_readout, y_readout, zcoll, dPitch);
          
          //AZ!!! Exclude 50% of channels for large stations
          //if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 600) iIChan = -1;
          //else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 600) iIChan = -1;

          //if ( iIChan != -1 && dPitch/dy < 0.85) { // strip width 0.68 mm
          //AZ-200322 if ( iIChan != -1 && TMath::Abs(dPitch/dy) < 0.425) { // strip width 0.68 mm
	  if ( iIChan != -1) { //AZ-200322
	    //AZ-200322 if (gRandom->Rndm() < 0.68) continue; // kill electron to equalize responses from 2 sides
	    //AZ-200322 fStripSignalB[iIChan] += 1.0;//stepEl_current;
	    fStripSignalB[iIChan] += 0.25;//stepEl_current;
	    fBChannelPointsMap[iIChan].insert(iPoint);
          }
        }
      }
    }
  }
}

//AZ -----   Private method ProduceHitResponseAZ   ----------------------------
void CbmStsDigitize::ProduceHitResponseAZ(CbmStsSensor* sensor) 
{

  set <Int_t> pSet;
  if ( fPointMap.find(sensor) == fPointMap.end() ) {
    cerr << "-E- " << fName << "::ProduceHitResponseAZ:: sensor" 
	 << " not found in digi scheme!" << endl;
    return;
  }
  pSet = fPointMap[sensor];

  Int_t       iPoint = -1;
  CbmStsPoint* point = NULL;

  set<Int_t>::iterator it1;

  const Int_t nclus = 25; // number of primary clusters / cm
  const Double_t mfpath = 1.0 / nclus; // mean free path
  const Double_t sigma = 0.05; // charge spread
  const Int_t maxClus = 102;
  //Double_t clusSize[maxClus] = {};
  const Double_t clusSize[maxClus] = {0.63403, 0.19005, 0.07334, 0.03335, 0.017906, 0.010391, 0.0066257, 0.0046042,
				      0.0034062, 0.0025641, 0.0020853, 0.001613, 0.0012888, 0.0010765, 0.00094932,
				      0.00087466, 0.00079435, 0.00072317, 0.00062637, 0.00053044, 0.00055171,
				      0.00053304, 0.00056429, 0.00058426, 0.00061465, 0.00057688, 0.00058166,
				      0.00050613, 0.00049571, 0.00046055, 0.00039327, 0.00035898, 0.00034162,
				      0.00030255, 0.00028475, 0.00027998, 0.00023961, 0.0002179, 0.0002153,
				      0.00019012, 0.00018188, 0.00015844, 0.000158, 0.00014368, 0.00013413,
				      0.00012805, 0.00011373, 0.00011112, 0.00011633, 0.000097666, 0.000099837,
				      0.000082474, 0.000095062, 0.000082474, 0.000091155, 0.000077699, 0.000074661,
				      0.000077699, 0.000074661, 0.000077699, 0.000062072, 0.000059468, 0.000054693,
				      0.000056429, 0.000049484, 0.000043407, 0.000054693, 0.000048616, 0.000044275,
				      0.000041237, 0.000041671, 0.000043407, 0.000041671, 0.000034292, 0.000033858,
				      0.000039501, 0.000027781, 0.000031687, 0.000029083, 0.000033424, 0.000030819,
				      0.000031253, 0.000032555, 0.000027781, 0.000024308, 0.000022138, 0.000026044,
				      0.000029083, 0.000019099, 0.000023006, 0.000022572, 0.000020401, 0.000027781,
				      0.000018231, 0.000024308, 0.000026478, 0.00002344, 0.000018665, 0.000018231,
				      0.000019099, 0.000016061, 0.0014806};
  static TH1D *hClSize = NULL;
  if (hClSize == NULL) {
    hClSize = new TH1D("hClSize","",maxClus,0,maxClus);
    for (Int_t i = 0; i < maxClus; ++i) hClSize->Fill(i+0.5,clusSize[i]);
  }

  for (it1 = pSet.begin(); it1 != pSet.end(); it1++) {
    iPoint = (*it1);
    point  = (CbmStsPoint*) fPoints->At(iPoint);

    TVector3 posIn, posOut, posCur, vec;
    point->PositionIn(posIn);
    point->PositionOut(posOut);
    vec = posOut;
    vec -= posIn;
    Double_t leng0 = vec.Mag(), leng = 0.0, dPitch;
    vec.SetMag(1.0);
    vector<TVector3> clusPos;
    vector<Double_t> vClusSize;

    // Go thru active material
    while (1) {
      Double_t step = gRandom->Exp(mfpath);
      posCur += step * vec;
      leng = posCur.Mag();
      if (leng <= leng0) {
	clusPos.push_back(posCur);
	vClusSize.push_back(Int_t(hClSize->GetRandom()+1));
	if (leng == leng0) break;
      } else break;
    }

    // Rescale cluster size sample to account for GEANT energy loss
    Int_t nofSteps = clusPos.size();
    Double_t nEl = 0, nElGeant = fEnergyLossToSignal * point->GetEnergyLoss();
    for (Int_t i = 0; i < nofSteps; ++i) nEl += vClusSize[i];
    Double_t scale = nElGeant / nEl;
    for (Int_t i = 0; i < nofSteps; ++i) vClusSize[i] *= scale;
    //cout << " steps: " << nofSteps << " " << nElGeant << " " << leng0 << endl;
    //for (Int_t i = 0; i < nofSteps; ++i) 
    //cout << i << " " << clusPos[i].Mag() << " " << vClusSize[i] << endl;

    //Double_t stepEL = fEnergyLossToSignal * point->GetEnergyLoss() / nofSteps;
    
    for ( Int_t istep = 0 ; istep < nofSteps ; istep++ ) {
      clusPos[istep] += posIn;
      Double_t stepEL = vClusSize[istep];

      Int_t iIChan = sensor->GetFrontChannel(clusPos[istep].X(),clusPos[istep].Y(),clusPos[istep].Z(),dPitch);

      //AZ!!! Exclude 50% of channels for large stations
      //if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 500) iIChan = -1;
      //else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 500) iIChan = -1;
      
      if ( iIChan != -1 ) {
	//fStripSignalF[iIChan] += stepEL;
	//fFChannelPointsMap[iIChan].insert(iPoint);
	for (Int_t jj = -4; jj < 5; ++jj) {
	  Int_t ich = iIChan + jj;
	  if (ich < 0 || ich > sensor->GetNChannelsFront()) continue;
	  Double_t cx1 = TMath::Erf((-dPitch+jj*sensor->GetDx())/sigma);
	  Double_t cx2 = TMath::Erf((-dPitch+(jj+1)*sensor->GetDx())/sigma);
	  Double_t frac = 0.5 * TMath::Abs(cx2-cx1);
	  if (frac < 1.e-7) continue;
	  Double_t dQ = stepEL * frac;
	  fStripSignalF[ich] += dQ;
	  fFChannelPointsMap[ich].insert(iPoint);
	}
      }

      iIChan = sensor->GetBackChannel(clusPos[istep].X(),clusPos[istep].Y(),clusPos[istep].Z(),dPitch);
      
      //AZ!!! Exclude 50% of channels for large stations
      //if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 1 && iIChan > 600) iIChan = -1;
      //else if (sensor->GetNChannelsFront() > 900 && sensor->GetSectorNr() == 3 && iIChan < 600) iIChan = -1;

      if ( iIChan != -1 ) {
	//fStripSignalB[iIChan] += stepEL;
	//fBChannelPointsMap[iIChan].insert(iPoint);
	for (Int_t jj = -4; jj < 5; ++jj) {
	  Int_t ich = iIChan + jj;
	  if (ich < 0 || ich > sensor->GetNChannelsBack()) continue;
	  Double_t cx1 = TMath::Erf((-dPitch+jj*sensor->GetDx())/sigma);
	  Double_t cx2 = TMath::Erf((-dPitch+(jj+1)*sensor->GetDx())/sigma);
	  Double_t frac = 0.5 * TMath::Abs(cx2-cx1);
	  if (frac < 1.e-7) continue;
	  Double_t dQ = stepEL * frac;
	  fStripSignalB[ich] += dQ;
	  fBChannelPointsMap[ich].insert(iPoint);
	}
      }
    }
  }
  
}
 
// -----   Private method SetParContainers   -------------------------------
void CbmStsDigitize::SetParContainers() {

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
InitStatus CbmStsDigitize::Init() {

  // Get input array 
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) Fatal("Init", "No FairRootManager");
  fPointsSi = (TClonesArray*) ioman->GetObject("SiliconPoint"); //AZ-280322
  fPoints = (TClonesArray*) ioman->GetObject("StsPoint");
  fMCTracks = (TClonesArray*) ioman->GetObject("MCTrack"); //AZ

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

  //AZ fEnergyLossToSignal    = 280000000.;

  fFNofSteps = (Int_t)TMath::Power(2,(Double_t)fFNofBits);
  fBNofSteps = (Int_t)TMath::Power(2,(Double_t)fBNofBits);

  // Build digitisation scheme
  if ( fDigiScheme->Init(fGeoPar, fDigiPar) ) {
    MakeSets();

    if      (fVerbose == 1 || fVerbose == 2) fDigiScheme->Print(kFALSE);
    else if (fVerbose >  2) fDigiScheme->Print(kTRUE);
    cout << "-I- " << fName << "::Init: "
	 << "STS digitisation scheme succesfully initialised" << endl;
    if ( fDigiScheme->IsNewGeometry() ) cout << "-I- Using new geometry" << endl;
    cout << "    Stations: " << fDigiScheme->GetNStations() 
	 << ", Sectors: " << fDigiScheme->GetNSectors() << ", Channels: " 
	 << fDigiScheme->GetNChannels() << endl;

    //AZ
    //fastDig = new BmnGemFastDigitize();
    //Add(fastDig);
    fastDig = (BmnGemFastDigitize*) FairRun::Instance()->GetTask("GemFastDigitize");

    return kSUCCESS;
  }

  return kERROR;

}
// -------------------------------------------------------------------------



// -----   Private method ReInit   -----------------------------------------
InitStatus CbmStsDigitize::ReInit() {

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
void CbmStsDigitize::MakeSets() {

  fPointMap.clear();
  Int_t nStations = fDigiScheme->GetNStations();
  
  Double_t fSectorWidth = 0.;
  
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
        fSectorWidth = 10.*sensor->GetLx();
      
        Int_t nofChips = (Int_t)(TMath::Ceil(fSectorWidth/7.5));  // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
        Int_t lastChip = (Int_t)(TMath::Ceil(10.*fSectorWidth));
        lastChip = lastChip%75;
        lastChip = (Int_t)(lastChip/.6);
         //     cout << nofChips << " chips on " << iStation+1 << " " << iSector+1 << endl;
        TString addInfo = "";
	if ( nofChips != 8 ) {
	addInfo = Form(", only %d strips",lastChip);
	//	cout << fSectorWidth << " -> " << addInfo.Data() << endl;
        }
   
      
        for ( Int_t iChip = 0 ; iChip < nofChips ; iChip++ ) {
          occupancy [iStation][iSector][iChip] = 3.;
//           cout << "OCCUPANCY [" << iStation+1 << "][" << iSector+1 << "][" << iChip << "] "<< occupancy [iStation][iSector][iChip] << "%" << endl;
        }
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
void CbmStsDigitize::MakeSets1() { //with occupancy file - default not used


  fPointMap.clear();
  Int_t nStations = fDigiScheme->GetNStations();
      
  TH1F* fhFNofDigisPChip[10][1000][20];
  TH1F* fhBNofDigisPChip[10][1000][20];
  TString qaFileName;
  qaFileName = "occup.sts.reco.root";
  cout << "Occupancy read from file: \"" << qaFileName.Data() << "\"" << endl;
  TFile* occuF = TFile::Open(qaFileName.Data());  

  TString directoryName = "STSFindHitsQA";
  
  Double_t fSectorWidth = 0.;
  
  for (Int_t iStation=0; iStation<nStations; iStation++) {
    CbmStsStation* station = fDigiScheme->GetStation(iStation);
    Int_t nSectors = station->GetNSectors();
    
    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      CbmStsSector* sector = station->GetSector(iSector);
      Int_t nSensors = sector->GetNSensors();
      Int_t nChannels = sector->GetNChannelsFront();
      
      for (Int_t iSensor=0; iSensor<nSensors; iSensor++) {
	CbmStsSensor* sensor = sector->GetSensor(iSensor);
	set<Int_t> a;
	fPointMap[sensor] = a;
	fSectorWidth = 10.*sensor->GetLx();
      
      
        Int_t nofChips = (Int_t)(TMath::Ceil(fSectorWidth/7.5));  // fwidth in mm, 7.5mm = 125(channels)*60mum(pitch)
        Int_t lastChip = (Int_t)(TMath::Ceil(10.*fSectorWidth));
        lastChip = lastChip%75;
        lastChip = (Int_t)(lastChip/.6);
         //     cout << nofChips << " chips on " << iStation+1 << " " << iSector+1 << endl;
        TString addInfo = "";
	if ( nofChips != 8 ) {
	addInfo = Form(", only %d strips",lastChip);
	//	cout << fSectorWidth << " -> " << addInfo.Data() << endl;
        }
   
      
        for ( Int_t iChip = 0 ; iChip < nofChips ; iChip++ ) {
          fhFNofDigisPChip[iStation][iSector][iChip]=(TH1F*)occuF->Get(Form("%s/Station%d/hNofFiredDigisFSt%dSect%dChip%d",directoryName.Data(),iStation+1,iStation+1,iSector+1,iChip+1));
          fhBNofDigisPChip[iStation][iSector][iChip]=(TH1F*)occuF->Get(Form("%s/Station%d/hNofFiredDigisBSt%dSect%dChip%d",directoryName.Data(),iStation+1,iStation+1,iSector+1,iChip+1));
          occupancy [iStation][iSector][iChip] = 100.*fhFNofDigisPChip[iStation][iSector][iChip]->GetMean()/125.;
          occupancy [iStation][iSector][iChip] = 100.*fhBNofDigisPChip[iStation][iSector][iChip]->GetMean()/125.;
// 	    if ( !occuF ) {
// 	      occupancy [iStation][iSector][iChip] = 3.;
// 	    }
//           cout << "OCCUPANCY [" << iStation+1 << "][" << iSector+1 << "][" << iChip << "] "<< occupancy [iStation][iSector][iChip] << "%" << endl;
        }
        

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
void CbmStsDigitize::Reset() {
  //  fNPoints = fNDigisFront = fNDigisBack = fTime = 0.;
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
void CbmStsDigitize::Finish() {
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
  cout << "===== StsDigis per event        : " 
       << setw(8) << setprecision(2) 
       << (fNDigisFront+fNDigisBack) / fNEvents << endl;
  cout << "===== Front digis per point     : " 
       << setw(8) << setprecision(2) 
       << fNDigisFront / (fNPoints) << endl;
  cout << "===== Back digis per point      : " 
       << setw(8) << setprecision(2) 
       << fNDigisBack / (fNPoints) << endl;
  cout << "============================================================"
       << endl;
	
}					       
// -------------------------------------------------------------------------



ClassImp(CbmStsDigitize)

