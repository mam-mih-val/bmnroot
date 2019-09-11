//* $Id: */

// -------------------------------------------------------------------------
// -----                   CbmStsDigiScheme source file                -----
// -----                  Created 31/08/06  by V. Friese               -----
// -------------------------------------------------------------------------
#include "CbmStsDigiScheme.h"
#include "CbmStsDetectorId.h"

#include "CbmGeoStsPar.h"
#include "CbmStsDigiPar.h"
#include "CbmStsSensor.h"
#include "CbmStsSensorDigiPar.h"
#include "CbmStsSector.h"
#include "CbmStsSectorDigiPar.h"
#include "CbmStsStation.h"
#include "CbmStsStationDigiPar.h"

#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairLogger.h"

#include "TArrayD.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"
#include "TGeoMatrix.h"

#include <iostream>
#include <map>

using std::flush;
using std::map;
using std::cout;
using std::endl;
using std::pair;

// -----   Constructor   ---------------------------------------------------
CbmStsDigiScheme::CbmStsDigiScheme()
  :
  fStations(new TObjArray(10)),
  fNSectors(0),
  fNSensors(0),
  fNChannels(0),
  fIsNewGeometry(kFALSE),
  fStationMap(),
  fDetIdByName(),
  fSensorByName()
{
  fStations = new TObjArray(10);
  fNSectors = fNSensors = fNChannels = 0;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsDigiScheme::~CbmStsDigiScheme() {
  if ( fStations) {
    fStations->Delete();
    delete fStations;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
Bool_t CbmStsDigiScheme::Init(CbmGeoStsPar*  geoPar,
			      CbmStsDigiPar* digiPar) {

  // Check availability of parameters
/*  if ( ! geoPar ) {
    cout << "-W- CbmStsDigiScheme::Init: "
	 << "No geometry parameters available!" << endl;
    return kFALSE;
  } */
  if ( ! digiPar ) {
    cout << "-W-  CbmStsDigiScheme::Init: "
	 << "No digitisation parameters available!" << endl;
    return kFALSE;
  }

  TGeoVolume* test = NULL;


  // Check for new geometry version. Should contain STS volume on level 2.
  TGeoNode* top = gGeoManager->GetTopNode();
  for (Int_t iNode = 0; iNode < top->GetNdaughters(); iNode++) {
    TString nodeName = top->GetDaughter(iNode)->GetName();
    if (nodeName.Contains("GEMS")) {
      test = top->GetDaughter(iNode)->GetVolume();
      break;
    }
  }
  if (test) {
    cout << "CbmStsDigiScheme:: Init for new geometry "
	 << test->GetName() << endl;
    fIsNewGeometry = kTRUE;
    return InitNew(NULL, digiPar);
  }

  // If none found, bail out
  cout << "-E- CbmStsDigiScheme::Init: unknown geometry version" << endl;
  return kFALSE;

}
// -------------------------------------------------------------------------



// -------------------------------------------------------------------------


// -----   Public method Clear   -------------------------------------------
void CbmStsDigiScheme::Clear() {
  fStations->Delete();
  fStationMap.clear();
  fDetIdByName.clear();
  fSensorByName.clear();
  fNSectors = fNSensors = fNChannels = 0;
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmStsDigiScheme::Reset() {
  for (Int_t iStation=0; iStation<fStations->GetEntries(); iStation++)
    GetStation(iStation)->Reset();
}
// -------------------------------------------------------------------------



// -----   Public method GetStation   --------------------------------------
CbmStsStation* CbmStsDigiScheme::GetStation(Int_t iStation) {
  if ( ! fStations ) return NULL;
  return dynamic_cast<CbmStsStation*>(fStations->At(iStation));
}
// -------------------------------------------------------------------------



// -----   Public method GetStationByNr   ----------------------------------
CbmStsStation* CbmStsDigiScheme::GetStationByNr(Int_t stationNr) {
  if ( ! fStations ) return NULL;
  if ( fStationMap.find(stationNr) == fStationMap.end() ) return NULL;
  return fStationMap[stationNr];
}
// -------------------------------------------------------------------------


// -----   Public method GetSector   ---------------------------------------
CbmStsSector* CbmStsDigiScheme::GetSector(Int_t stationNr, Int_t sectorNr) {
  return ( GetStationByNr(stationNr)->GetSectorByNr(sectorNr) );
}
// -------------------------------------------------------------------------

// -----   Public method GetSensor   ---------------------------------------
CbmStsSensor* CbmStsDigiScheme::GetSensor(Int_t stationNr, Int_t sectorNr, Int_t sensorNr) {
  return ( GetStationByNr(stationNr)->GetSectorByNr(sectorNr)->GetSensorByNr(sensorNr) );
}
// -------------------------------------------------------------------------

// -----   Public method GetDetectorIdByName  ------------------------------
Int_t CbmStsDigiScheme::GetDetectorIdByName(TString sensorName)
{
  map < TString, Int_t>::iterator p;
  p=fDetIdByName.find(sensorName);

  if(p!=fDetIdByName.end()){
    return p->second;
  }else{
    cout << " -E- StsDigiScheme::GetDetectorIdByName \"" << sensorName.Data() << "\" not found " << endl;
    return -1;
  }
}
// -------------------------------------------------------------------------

// -----   Public method GetSensorIdByName  --------------------------------
CbmStsSensor* CbmStsDigiScheme::GetSensorByName(TString sensorName)
{
  map < TString, CbmStsSensor*>::iterator p;
  p=fSensorByName.find(sensorName);

  if(p!=fSensorByName.end()){
    return p->second;
  }else{
    LOG(DEBUG) << "StsDigiScheme: sensor " << sensorName.Data()
               << " not found ";
    return NULL;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void CbmStsDigiScheme::Print(Bool_t kLong) {
  cout << endl;
  cout << "===================  STS digitisation scheme   ================="
       << endl;
  for (Int_t iStat=0; iStat<GetNStations(); iStat++) {
    CbmStsStation* station = GetStation(iStat);
    station->Print(kLong);
  }
  cout << "================================================================"
       << endl << endl;
}
// -------------------------------------------------------------------------



// ---- Init old  ----------------------------------------------------------
Bool_t CbmStsDigiScheme::InitOld(CbmGeoStsPar* geoPar,
				 CbmStsDigiPar* digiPar) {

  return kTRUE;

}
// -------------------------------------------------------------------------



// ---- Init new  ----------------------------------------------------------
Bool_t CbmStsDigiScheme::InitNew(CbmGeoStsPar* geoPar,
				 CbmStsDigiPar* digiPar) {

//cout<<digiPar->GetNStations()<<endl;

  // Get STS node
  TGeoNode* sts = NULL;
  gGeoManager->CdTop();
  TGeoNode* cave = gGeoManager->GetCurrentNode();
  for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
    TGeoNode* node = cave->GetDaughter(iNode);
    TString name = node->GetName();
    cout<<"NODE: "<<name<<endl;
    if ( name.Contains("GEMS_0") ) {
      sts = node;
      gGeoManager->CdDown(iNode);
      break;
    }
  }
  if ( ! sts ) {
    cout << "-E- CbmStsDigiScheme::InitNew: Cannot find top GEM node"
	 << endl;
    return kFALSE;
  }

  cout<<"stantion: "<<sts->GetNdaughters()<<endl;
//cout<<sts->GetNdaughters()<<endl;
  // Loop over stations in STS

  Int_t statNr = 0, stNr=0; //AZ
  for (Int_t iNode = 0; iNode < sts->GetNdaughters(); iNode++) {
    // Go to station node
//if(iNode==0) continue;
    gGeoManager->CdDown(iNode);
    TGeoNode* stationNode = gGeoManager->GetCurrentNode();
    TString statName = stationNode->GetName();
    if ( ! statName.Contains("station") ) { //GP Station -> station
      gGeoManager->CdUp();
      continue;
    }
    cout<<"stantion: "<< iNode << " " << sts->GetNdaughters()<< " " << statName << endl;
    //AZ Int_t statNr = stationNode->GetNumber();
    
    ++stNr;
    /*
    if(stNr<4){
gGeoManager->CdUp();
cout<<" Station skipped !!!"<<endl;
 continue;} */

 ++statNr; //AZ
//skip Si tracker



    // Get station parameters
    CbmStsStationDigiPar* stationPar = NULL;
    for (Int_t iStation = 0; iStation < digiPar->GetNStations(); iStation++) {
      CbmStsStationDigiPar* partest1 = digiPar->GetStation(iStation);
      if ( partest1->GetStationNr() == statNr ) {
	stationPar = partest1;
	break;
      }
    }
    if ( ! stationPar ) {
      cout << "-E- CbmStsDigiScheme::InitNew: No parameters for station "
	   << statNr << endl;
      return kFALSE;
    }

    // Create CbmStsStation
    TGeoBBox* shape = (TGeoBBox*) (stationNode->GetVolume()->GetShape());
    statName = Form("stat%02d", statNr);
    Double_t* statTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
    Double_t  statZ = statTrans[2];
    //Double_t  statD = 2. * shape->GetDZ();
    Double_t statD = 0.03;   // A dirty fix. TODO
    Double_t  statRadLength = 9.34953;  // TODO
    //AZ
    if ( TString(shape->ClassName()).Contains("Composite") ) {
      statD = 0.02;
      statRadLength = 3.56; // Titanium
    }
    //AZ
    Double_t  statRmin = 0.;
    Double_t  statRmax = 2. * TMath::Max(shape->GetDX(), shape->GetDY());
    Double_t  statRot = stationPar->GetRotation();
    CbmStsStation* station = new CbmStsStation(statName, statNr, statZ, statD,
					       statRadLength, statRmin,
					       statRmax, statRot);
    fStations->Add(station);
    fStationMap[statNr] = station;



    // Loop over modules in the station
    Int_t moduleNr = 0; // moduleNr ---> sector number in current station !!
    Int_t sensNr = 0;
    Int_t nModules = stationNode->GetNdaughters();

    //cout<<"nModules: "<<nModules<<endl;

    //if(statNr==1) nModules=nModules/2; /// FIX AZ

    for (Int_t iModule = 0; iModule < nModules; iModule++) {
     // gGeoManager->CdDown(iModule);
     //if(statNr>1) gGeoManager->CdDown(iModule);
     if(statNr>-1) gGeoManager->CdDown(iModule); ///FIX AZ
     //else gGeoManager->CdDown(iModule+8); /// FIX AZ
     
      TGeoNode* moduleNode = gGeoManager->GetCurrentNode();
      TString moduleName = moduleNode->GetName();
      cout<<"moduleName: "<<moduleName<<endl;
      if ( ! moduleName.Contains("module") ) {
	gGeoManager->CdUp();
	continue;
      }
      //-----------------------------------GP
if(statNr<=3){
	moduleNr++;
fNSensors += SetSensor(moduleNr,statNr,station,stationPar)->GetNSensors();
//cout <<"!!!!!!!!!!!!!!!!!!!! DIGI SHEME SI: " << gGeoManager->GetCurrentNode()->GetName() << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << endl;

 /*  Int_t nSensors = moduleNode->GetNdaughters();

      for (Int_t iSensor = 0; iSensor <  nSensors; iSensor++) {

//cout<<"iSensor: "<<iSensor<<endl;
	gGeoManager->CdDown(iSensor);
//cout<<"dbg: 1"<<endl;
	TGeoNode* sensorNode = gGeoManager->GetCurrentNode();
//cout<<"Ses name: "<<sensorNode->GetName()<<endl;
	if ( ! TString(sensorNode->GetName()).Contains("Sensor") ) {
	  gGeoManager->CdUp();
	  continue;
	}

	moduleNr++;
fNSensors += SetSensor(moduleNr,statNr,station,stationPar)->GetNSensors();
//cout <<"!!!!!!!!!!!!!!!!!!!! DIGI SHEME SI: " << gGeoManager->GetCurrentNode()->GetName() << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << endl;
	gGeoManager->CdUp();       // to module
      }
*/
//	  moduleNr++;
	//  fprintf(parFile, "%4d %4d\n", moduleNr, 1);
	  // ---> Sensor number
   // geoMan->CdUp(); // back to module sensor
	//  SaveSensor(geoMan, parFile, phiStat, sensor);
//fNSensors += SetSensor(moduleNr,statNr, station, stationPar)->GetNSensors();
	
  	gGeoManager->CdUp();       // to station
}else {



      	moduleNr++;
	cout << gGeoManager->GetCurrentNode()->GetName() << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << endl;

fNSensors += SetSensor(moduleNr,statNr, station, stationPar)->GetNSensors();

      // Loop over sensors in the sector

      Int_t nSensors = moduleNode->GetNdaughters();

      for (Int_t iSensor = 0; iSensor <  nSensors; iSensor++) {

//cout<<"iSensor: "<<iSensor<<endl;
	gGeoManager->CdDown(iSensor);
//cout<<"dbg: 1"<<endl;
	TGeoNode* sensorNode = gGeoManager->GetCurrentNode();
//cout<<"Ses name: "<<sensorNode->GetName()<<endl;
	if ( ! TString(sensorNode->GetName()).Contains("Sensor") ) {
	  gGeoManager->CdUp();
	  continue;
	}

	moduleNr++;
fNSensors += SetSensor(moduleNr,statNr,station,stationPar)->GetNSensors();

	gGeoManager->CdUp();       // to module
      }
                        // sensor loop
    //  cout<<"dbg: 2"<<endl;

    //  cout<<"dbg: 3"<<endl;
      gGeoManager->CdUp();             // to station
    }                                  // ladder loop
  }
    fNSectors  += station->GetNSectors();
    fNChannels += station->GetNChannels();
    gGeoManager->CdUp();               // to sts
  }                                    // station loop

  cout << "-I- CbmStsDigiScheme::InitNew: Intialisation successful" << endl;
  Print();
  return kTRUE;
}
// -------------------------------------------------------------------------



// -----   GetPath   -------------------------------------------------------
TString CbmStsDigiScheme::GetCurrentPath() {

  TString path;


  if ( ! gGeoManager ) {
    cout << "-E- CbmStsDigiScheme::GetCurrentPath:: No TGeoManager" << endl;
    return path;
  }

  TGeoNode* cNode = gGeoManager->GetCurrentNode();
  while (cNode) {
    TString nodeName = cNode->GetName();
    path = "/" + nodeName + path;
    if ( cNode == gGeoManager->GetTopNode() ) break;
    gGeoManager->CdUp();
    cNode = gGeoManager->GetCurrentNode();
  }

  gGeoManager->cd(path.Data());

  return path;
}
// -------------------------------------------------------------------------

CbmStsSector* CbmStsDigiScheme::SetSensor(Int_t moduleNr,Int_t statNr, CbmStsStation* station, CbmStsStationDigiPar* stationPar)
{

  CbmStsSector* sector = NULL;
  CbmStsSensorDigiPar* sensorPar = NULL;
  CbmStsSectorDigiPar* sectorPar = NULL;

	for (Int_t iSector = 0; iSector < stationPar->GetNSectors(); iSector++) {
	  CbmStsSectorDigiPar* partest2 = stationPar->GetSector(iSector);
	  if ( partest2->GetSectorNr() == moduleNr ) {
	    sectorPar = partest2;
	    break;
	  }
	}
	if ( ! sectorPar ) {
	  cout << "-E- CbmStsDigiScheme::InitNew: No parameters for module "
	       << moduleNr << ", station " << statNr << endl;
	//  return kFALSE;
	}

 	// Create CbmStsSector
	TString sectName = Form("stat%02dsect%d", statNr, moduleNr);
	Int_t   sectId = 2 << 24 | statNr << 16 | moduleNr << 4;
	sector = new CbmStsSector(sectName.Data(), sectId);
	station->AddSector(sector);
  //cout<<"DIGI SHEME: SEN N: "<<sector->GetSectorNr()<<endl;
	//Int_t sensNr = sensorNode->GetNumber();
	Int_t sensNr = 1;

	// Get sensor parameters

	for (Int_t iPar = 0; iPar < sectorPar->GetNSensors(); iPar++) {
	  CbmStsSensorDigiPar* partest3 = sectorPar->GetSensor(iPar);
	  if ( partest3->GetSensorNr() == sensNr ) {
	    sensorPar = partest3;
	    break;
	  }
	}
	if ( ! sensorPar ) {
	  cout << "-E- CbmStsDigiScheme::InitNew: No parameters for sensor "
	       << sensNr << ", module " << moduleNr << ", station " << statNr << endl;
	//  return kFALSE;
	}
	// Create CbmStsSensor
	Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
	TString   sensName = gGeoManager->GetPath();//GetCurrentPath(); //GP FIX
	Int_t     sensId = 2 << 24 | statNr << 16 | moduleNr << 4 | sensNr << 1;
	Int_t     sensType = sensorPar->GetType();
	Double_t  sensX = sensTrans[0];
	Double_t  sensY = sensTrans[1];
	//Double_t  sensZ =sensorPar->GetZ0();// sensTrans[2]; GP
	Double_t  sensZ = sensTrans[2];
	Double_t  sensRot = sensorPar->GetRotation();
	Double_t  sensLx = sensorPar->GetLx();
	Double_t  sensLy = sensorPar->GetLy();
	Double_t  sensD = sensorPar->GetD();
	Double_t  sensDx = sensorPar->GetDx();
	Double_t  sensDy = sensorPar->GetDy();
	Double_t  sensStereoF = sensorPar->GetStereoF();
	Double_t  sensStereoB = sensorPar->GetStereoB();
	CbmStsSensor* sensor = new CbmStsSensor(sensName, sensId, sensType,
						sensX, sensY, sensZ,
						sensRot, sensLx, sensLy,
						sensD, sensDx, sensDy,
						sensStereoF, sensStereoB);
	sector->AddSensor(sensor);
	if ( fDetIdByName.find(sensName) != fDetIdByName.end() ) {
	  cout << "-E- CbmStsDigiScheme: Duplicate sensor name " << sensName << endl;
	  //return kFALSE;
	}
  
	fDetIdByName[sensName] = sensId;
	if ( fSensorByName.find(sensName) != fSensorByName.end() ) {
	  cout << "-E- CbmStsDigiScheme: Duplicate sensor name " << sensName << endl;
	  //return kFALSE;
	}
	fSensorByName[sensName] = sensor;
	//if (sensX > 0) station->SetZ(sensZ); //AZ
	//station->SetZ(sensZ); //AZ


return sector;
}





ClassImp(CbmStsDigiScheme)
