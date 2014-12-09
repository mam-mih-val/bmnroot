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
#include "CbmStsDigitize.h"
#include "CbmStsPoint.h"
#include "CbmStsSensor.h"
#include "CbmStsSector.h"
#include "CbmStsStation.h"

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
  fFNofBits(12),
  fBNofBits(12),
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
  fPointMap()
{
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsDigitize::CbmStsDigitize(Int_t iVerbose) 
  : FairTask("STSDigitize", iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
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
  fFNofBits(12),
  fBNofBits(12),
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
  fPointMap()
{ 
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();
}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
CbmStsDigitize::CbmStsDigitize(const char* name, Int_t iVerbose) 
  : FairTask(name, iVerbose), 
  fGeoPar(NULL),
  fDigiPar(NULL),
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
  fFNofBits(12),
  fBNofBits(12),
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
  fPointMap()
{ 
  fGeoPar      = NULL;
  fDigiPar     = NULL;
  fPoints      = NULL;
  fDigis       = NULL;
  fDigiMatches = NULL;
  fRealistic   = kFALSE;
  fDigiScheme  = new CbmStsDigiScheme();
  Reset();

  fStep = 0.001;

  fFThreshold  = 4.0;
  fBThreshold  = 4.0;
  fFNoiseWidth = 0.1;
  fBNoiseWidth = 0.1;

  fFNofBits    = 12;
  fBNofBits    = 12;
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
  
  for (Int_t iPoint=0; iPoint<fPoints->GetEntriesFast(); iPoint++) {
    CbmStsPoint* point = (CbmStsPoint*) fPoints->At(iPoint);

    Double_t xin = point->GetXIn();
    Double_t yin = point->GetYIn();
    Double_t zin = point->GetZIn();
    gGeoManager->FindNode(xin,yin,zin);
    TGeoNode* curNode = gGeoManager->GetCurrentNode();
    
    CbmStsSensor* sensor = NULL;
    if ( fDigiScheme->IsNewGeometry() ) {
      TString curPath = fDigiScheme->GetCurrentPath();
      if (CrossSpacer(curNode, point)) continue; //AZ - emulate spacers
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
 	fStripSignalF[--iChannel] = TMath::Abs(gRandom->Gaus(0.,fFNoiseWidth));
 	//AZ fStripSignalB[  iChannel] = TMath::Abs(gRandom->Gaus(0.,fBNoiseWidth));
      }
      //AZ 
      Int_t nChannelsB = sector->GetNChannelsBack();
      //-----aplying noise on every channel-----
      for (Int_t iChannel=nChannelsB ; iChannel > 0 ; ) {
 	fStripSignalB[--iChannel] = TMath::Abs(gRandom->Gaus(0.,fBNoiseWidth));
      }
      //AZ 
      
      for (Int_t iSensor=sector->GetNSensors(); iSensor > 0 ; ) {
	CbmStsSensor* sensor = sector->GetSensor(--iSensor);
	
	ProduceHitResponse(sensor);
      }

      Int_t   stationNr = sector->GetStationNr();
      Int_t    sectorNr = sector->GetSectorNr();
      Int_t sectorDetId = sector->GetDetectorId();
      
      for ( Int_t ifstr = 0 ; ifstr < nChannels ; ifstr++ ) {
	if ( fStripSignalF[ifstr] < (fFThreshold*1000.) ) continue;//threshold cut
        
        //-----random strip inefficiency-----
	Double_t generator;
	generator = gRandom->Rndm()*100.;
	if (generator< (fStripDeadTime/100.)*occupancy [iStation][iSector][ifstr/125]) continue;
	//-----------------------------------
	
	Int_t digiFSignal = 1+(Int_t)((fStripSignalF[ifstr])/fFNofElPerAdc);
	if ( digiFSignal >= fFNofSteps ) digiFSignal = fFNofSteps-1;
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
	if ( fStripSignalB[ibstr] < (fBThreshold*1000.) ) continue; //threshold cut
	
	//-----random strip inefficiency-----
	Double_t generator;
	generator = gRandom->Rndm()*100.;
	if (generator< (fStripDeadTime/100.)*occupancy [iStation][iSector][ibstr/125]) continue;
	//-----------------------------------
	
	Int_t digiBSignal = 1+(Int_t)((fStripSignalB[ibstr])/fBNofElPerAdc);
	if ( digiBSignal >= fBNofSteps ) digiBSignal = fBNofSteps-1;
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
  if (name.Contains("SensorSV")) return kFALSE; // short strip area

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
  if (transl[0] > 0) dist = xyzloc[0] + dx;
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
void CbmStsDigitize::ProduceHitResponse(CbmStsSensor* sensor) {
  set <Int_t> pSet;
  if ( fPointMap.find(sensor) == fPointMap.end() ) {
    cerr << "-E- " << fName << "::ProduceHitResponse:: sensor" 
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

      // AZ - introduce smearing
      Double_t xinS = xin, yinS = yin, sigma = 0.05; // 500um
      xinS += gRandom->Gaus(0.0,sigma);
      yinS += gRandom->Gaus(0.0,sigma);
      //AZ
      
      //AZ Int_t   iIChan = sensor->GetFrontChannel(xin,yin,zin);
      Int_t   iIChan = sensor->GetFrontChannel(xinS,yinS,zin);
      
      if ( iIChan != -1 ) {
	fStripSignalF[iIChan] += stepEL;
	fFChannelPointsMap[iIChan].insert(iPoint);
      }

      //AZ iIChan = sensor->GetBackChannel (xin,yin,zin);
      iIChan = sensor->GetBackChannel (xinS,yinS,zin);
      
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

  fEnergyLossToSignal    = 280000000.;

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

