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
#include "FairRunAna.h" //AZ
#include "FairRuntimeDb.h" //AZ

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

// -----   Initialisation of static singleton pointer   --------------------
CbmStsDigiScheme* CbmStsDigiScheme::fgInstance = NULL;
// -------------------------------------------------------------------------

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
  //AZ fStations = new TObjArray(10);
  //AZ fNSectors = fNSensors = fNChannels = 0;
  if (fgInstance == NULL) fgInstance = this;
  if (gGeoManager) Init();
}
// -------------------------------------------------------------------------

CbmStsDigiScheme* CbmStsDigiScheme::Instance()
{
  // Singleton instance

  if (fgInstance == NULL) new CbmStsDigiScheme();
  return fgInstance;
}

// -----   Destructor   ----------------------------------------------------
CbmStsDigiScheme::~CbmStsDigiScheme() {
  if ( fStations) {
    fStations->Delete();
    delete fStations;
  }
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
Bool_t CbmStsDigiScheme::Init()
{
  FairRunAna *run = FairRunAna::Instance();
  FairRuntimeDb *rtdb = run->GetRuntimeDb();

  // Get STS geometry parameter container                                       
  CbmGeoStsPar *geoPar = (CbmGeoStsPar*) rtdb->getContainer("CbmGeoStsPar");

  // Get STS digitisation parameter container                                   
  CbmStsDigiPar *digiPar = (CbmStsDigiPar*) rtdb->getContainer("CbmStsDigiPar");

  Init(geoPar, digiPar);
}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
Bool_t CbmStsDigiScheme::Init(CbmGeoStsPar*  geoPar,
			      CbmStsDigiPar* digiPar) {

  //if (fNChannels) return kTRUE; //AZ - for singleton
  if (fStations->GetEntries()) return kTRUE; //SM - for singleton

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
  p = fDetIdByName.find(sensorName);

  if (p != fDetIdByName.end()){
    return p->second;
  } else {
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
               << " not found " << FairLogger::endl;
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


// ---- Init new  ----------------------------------------------------------
Bool_t CbmStsDigiScheme::InitNew(CbmGeoStsPar* geoPar,
				 CbmStsDigiPar* digiPar) {

  Double_t statD = 0.03;   // A dirty fix. TODO
    Double_t  statRadLength = 9.34953;  // TODO
    //AZ
    // if ( TString(shape->ClassName()).Contains("Composite") ) {
    //   statD = 0.02;
    //   statRadLength = 3.56; // Titanium
    // }
    //AZ
    Double_t  statRmin = 0.;
    Double_t  statRmax = 6;//2. * TMath::Max(shape->GetDX(), shape->GetDY());
    //Double_t  statRot = stationPar->GetRotation();
    
    TString gPathConfig = gSystem->Getenv("VMCWORKDIR");

    //Create GEM detector ------------------------------------------------------
    GemStationSet = new BmnGemStripStationSet(gPathConfig + "/parameters/gem/XMLConfigs/GemFutureConfig2020.xml");
    SilStationSet = new BmnSiliconStationSet(gPathConfig + "/parameters/silicon/XMLConfigs/SiliconFutureConfig2020.xml");
    
    printf("SilStationSet->GetNStations() = %d\n", SilStationSet->GetNStations());
    for (Int_t iSt = 0; iSt < SilStationSet->GetNStations(); ++iSt) {
      BmnSiliconStation* siStation = SilStationSet->GetStation(iSt);
      
      CbmStsStation* station = new CbmStsStation(Form("stat%02d", iSt + 1), iSt + 1, siStation->GetZPosition(), statD, statRadLength, 0, 0, 0);
      for (Int_t iMod = 0; iMod < siStation->GetNModules(); ++iMod) {
        BmnSiliconModule* siModule = siStation->GetModule(iMod);
        // for (Int_t iL = 0; iL < siModule->GetNStripLayers(); ++iL) {
        //   BmnSiliconLayer gemLay = siModule->GetStripLayer(iL);
        //   //cout << gemLay.GetAngleDeg() << endl;
        // }
        Int_t detId = 2 << 24 | (iSt + 1) << 16 | (iMod + 1) << 4 | 1 << 1;
        CbmStsSector* sector = new CbmStsSector(Form("sect%02d", iMod + 1), detId);
        Float_t x0 = 0.5 * (siModule->GetXMinModule() + siModule->GetXMaxModule());
        Float_t y0 = 0.5 * (siModule->GetYMinModule() + siModule->GetYMaxModule());
        CbmStsSensor* sensor = new CbmStsSensor(detId, 2, x0, y0, 0, siModule->GetXSize(), siModule->GetYSize(), 0.01, 0.0, 0.0, siModule->GetStripLayer(3).GetAngleRad());
        sector->AddSensor(sensor);
        station->AddSector(sector);
      }
      fStations->Add(station);
      fStationMap[iSt] = station;
    }
    
    printf("GemStationSet->GetNStations() = %d\n", GemStationSet->GetNStations());
    for (Int_t iSt = 0; iSt < GemStationSet->GetNStations(); ++iSt) {
      BmnGemStripStation* gemStation = GemStationSet->GetStation(iSt);
      Double_t statRot = (gemStation->GetModule(0)->GetElectronDriftDirection() == 1) ? 0 : Pi();
      CbmStsStation* station = new CbmStsStation(Form("stat%02d", iSt + 1 + SilStationSet->GetNStations()), iSt + 1 + SilStationSet->GetNStations(), gemStation->GetZPosition(), statD, statRadLength, statRmin, statRmax, statRot);
      for (Int_t iMod = 0; iMod < gemStation->GetNModules(); ++iMod) {
        BmnGemStripModule* gemModule = gemStation->GetModule(iMod);
        // for (Int_t iL = 0; iL < gemModule->GetNStripLayers(); ++iL) {
        //   BmnGemStripLayer gemLay = gemModule->GetStripLayer(iL);
        //   //cout << gemLay.GetAngleDeg() << endl;
        // }
        Int_t detId = 2 << 24 | (iSt + 1 + SilStationSet->GetNStations()) << 16 | (iMod + 1) << 4 | 1 << 1;
        CbmStsSector* sector = new CbmStsSector(Form("sect%02d", iMod + 1), detId);
        Float_t x0 = 0.5 * (gemModule->GetXMinModule() + gemModule->GetXMaxModule());
        Float_t y0 = 0.5 * (gemModule->GetYMinModule() + gemModule->GetYMaxModule());
        CbmStsSensor* sensor = new CbmStsSensor(detId, 2, x0, y0, 0, gemModule->GetXSize(), gemModule->GetYSize(), 0.08, 0.0, 0.0, gemModule->GetStripLayer(1).GetAngleRad());
        sector->AddSensor(sensor);
        station->AddSector(sector);
      }
      fStations->Add(station);
      fStationMap[iSt + SilStationSet->GetNStations()] = station;
    }
    printf("fStations->GetEntries() = %d\n", fStations->GetEntries());
    
  //   return kTRUE;
  
  // //cout<<digiPar->GetNStations()<<endl;

  // // Get STS node
  // TGeoNode* sts = NULL;
  // gGeoManager->CdTop();
  // TGeoNode* cave = gGeoManager->GetCurrentNode();
  // for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
  //   TGeoNode* node = cave->GetDaughter(iNode);
  //   TString name = node->GetName();
  //   cout<<"NODE: "<<name<<endl;
  //   if ( name.Contains("GEMS_0") ) {
  //     sts = node;
  //     gGeoManager->CdDown(iNode);
  //     break;
  //   }
  // }
  // if ( ! sts ) {
  //   cout << "-E- CbmStsDigiScheme::InitNew: Cannot find top GEM node"
	//  << endl;
  //   return kFALSE;
  // }
  
  // cout << "stations: " << sts->GetNdaughters() << endl;

  // // Loop over stations in STS
  
  // Int_t statNr = 0, stNr = 0; //AZ

  // for (Int_t iNode = 0; iNode < sts->GetNdaughters(); iNode++) {
  //   // Go to station node
  //   //if(iNode==0) continue;
  //   gGeoManager->CdDown(iNode);
  //   TGeoNode* stationNode = gGeoManager->GetCurrentNode();
  //   TString statName = stationNode->GetName();
  //   if ( ! statName.Contains("station") ) { //GP Station -> station
  //     gGeoManager->CdUp();
  //     continue;
  //   }
  //   cout<<"station: "<< iNode << " " << sts->GetNdaughters()<< " " << statName << endl;
  //   //AZ Int_t statNr = stationNode->GetNumber();
    
  //   ++stNr;
  //   /*
  //     if(stNr<4){
  //     gGeoManager->CdUp();
  //     cout<<" Station skipped !!!"<<endl;
  //     continue;} */
    
  //   ++statNr; //AZ
  //   //skip Si tracker
    
  //   // Get station parameters
  //   CbmStsStationDigiPar* stationPar = NULL;
  //   for (Int_t iStation = 0; iStation < digiPar->GetNStations(); iStation++) {
  //     CbmStsStationDigiPar* partest1 = digiPar->GetStation(iStation);
  //     if ( partest1->GetStationNr() == statNr ) {
	// stationPar = partest1;
	// break;
  //     }
  //   }
  //   if ( ! stationPar ) {
  //     cout << "-E- CbmStsDigiScheme::InitNew: No parameters for station "
	//    << statNr << endl;
  //     //AZ return kFALSE;
  //     return kTRUE; //AZ - reduced geometry
  //   }
    
  //   // Create CbmStsStation
  //   TGeoBBox* shape = (TGeoBBox*) (stationNode->GetVolume()->GetShape());
  //   statName = Form("stat%02d", statNr);
  //   Double_t* statTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
  //   Double_t  statZ = statTrans[2];
  //   //Double_t  statD = 2. * shape->GetDZ();
  //   Double_t statD = 0.03;   // A dirty fix. TODO
  //   Double_t  statRadLength = 9.34953;  // TODO
  //   //AZ
  //   /*
  //     if ( TString(shape->ClassName()).Contains("Composite") ) {
  //     statD = 0.02;
  //     statRadLength = 3.56; // Titanium
  //   }
  //   if (stationNode->GetNdaughters() % 2 > 0) {
  //     // GEMs - 3 daughters
  //     statRadLength = 0.9 / 0.02 * 3.56; // effective rad. length
  //   }
  //   */
  //   //AZ
  //   Double_t  statRmin = 0.;
  //   Double_t  statRmax = 2. * TMath::Max(shape->GetDX(), shape->GetDY());
  //   Double_t  statRot = stationPar->GetRotation();
  //   CbmStsStation* station = new CbmStsStation(statName, statNr, statZ, statD,
	// 				       statRadLength, statRmin,
	// 				       statRmax, statRot);
  //   //AZ station->SetXYminmax(-2*shape->GetDX(),-2*shape->GetDY(),2*shape->GetDX(),2*shape->GetDY()); //AZ
  //   //station->SetXYminmax(-1*shape->GetDX(),-1*shape->GetDY(),1*shape->GetDX(),1*shape->GetDY()); //AZ
  //   //station->SetXYminmax(-shape->GetDX()+statTrans[0], -shape->GetDY()+statTrans[1],
  //   //			 shape->GetDX()+statTrans[0], shape->GetDY()+statTrans[1]); //AZ
  //   fStations->Add(station);
  //   fStationMap[statNr] = station;
    

  //   // Loop over modules in the station
  //   Int_t moduleNr = 0; // moduleNr ---> sector number in current station !!
  //   Int_t sensNr = 0;
  //   Int_t nModules = stationNode->GetNdaughters();
    
  //   for (Int_t iModule = 0; iModule < nModules; iModule++) {
  //     if (statNr > -1) gGeoManager->CdDown(iModule); ///FIX AZ
      
  //     TGeoNode* moduleNode = gGeoManager->GetCurrentNode();
  //     TString moduleName = moduleNode->GetName();
  //     cout << "moduleName: " << moduleName << endl;
  //     if ( ! (moduleName.Contains("module") || moduleName.Contains("adder") || 
	//       moduleName.Contains("ensor")) ) {
	// gGeoManager->CdUp();
	// continue;
  //     }

  //     if (moduleName.Contains("adder")) {
	// // CBM Si stations
	// Int_t nsens = SetSensorsCbm(moduleNr, statNr, station, stationPar);
	// fNSensors += nsens;
	// moduleNr += nsens;
  // 	gGeoManager->CdUp();       // to station

  //     } else if (statNr <= 3) {
	// // Silicon in run 7
	// moduleNr++;
	// fNSensors += SetSensor(moduleNr,statNr,station,stationPar)->GetNSensors();
  // 	gGeoManager->CdUp();       // to station
	
  //     } else {
	// // GEMs
  //     	moduleNr++;
	// cout << gGeoManager->GetCurrentNode()->GetName() << " " << gGeoManager->GetCurrentMatrix()->GetTranslation()[2] << endl;
	
	// fNSensors += SetSensor(moduleNr,statNr, station, stationPar)->GetNSensors();
	
	// // Loop over sensors in the sector
	
	// Int_t nSensors = moduleNode->GetNdaughters();
	
	// for (Int_t iSensor = 0; iSensor <  nSensors; iSensor++) {
	  
	//   //cout<<"iSensor: "<<iSensor<<endl;
	//   gGeoManager->CdDown(iSensor);
	//   //cout<<"dbg: 1"<<endl;
	//   TGeoNode* sensorNode = gGeoManager->GetCurrentNode();
	//   //cout<<"Ses name: "<<sensorNode->GetName()<<endl;
	//   if ( ! TString(sensorNode->GetName()).Contains("Sensor") ) {
	//     gGeoManager->CdUp();
	//     continue;
	//   }
	  
	//   moduleNr++;
	//   fNSensors += SetSensor(moduleNr,statNr,station,stationPar)->GetNSensors();
	  
	//   gGeoManager->CdUp();       // to module
	// }
	// // sensor loop
	// gGeoManager->CdUp();             // to station
  //     }                                  // ladder loop
  //   }
  //   fNSectors  += station->GetNSectors();
  //   fNChannels += station->GetNChannels();
  //   gGeoManager->CdUp();               // to sts
  // }                                    // station loop
  
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

CbmStsSector* CbmStsDigiScheme::SetSensor(Int_t moduleNr, Int_t statNr, CbmStsStation* station,
					  CbmStsStationDigiPar* stationPar)
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
    cout << "-E- CbmStsDigiScheme::SetSensor: No parameters for module "
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
    cout << "-E- CbmStsDigiScheme::SetSensor: No parameters for sensor "
	 << sensNr << ", module " << moduleNr << ", station " << statNr << endl;
    //  return kFALSE;
  }
  // Create CbmStsSensor
  Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
  TString   sensName = gGeoManager->GetPath();
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
  //AZ - does not work Double_t  sensD = station->GetD();
  Double_t  sensDx = sensorPar->GetDx();
  Double_t  sensDy = sensorPar->GetDy();
  //AZ - 9feb2019
  if (sensDx > 0.02) { // using strip pitch
    Double_t zShift = 0.3;
    if (statNr % 2 == 1) zShift *= -1;
    //AZ sensZ += zShift;
  }
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

// -------------------------------------------------------------------------

Int_t CbmStsDigiScheme::SetSensorsCbm(Int_t moduleNr, Int_t statNr, CbmStsStation* statCBM,
				      CbmStsStationDigiPar* stationPar)
{
  // Process CBM Si station

  TGeoManager *geoMan = gGeoManager;
  TGeoNode *ladder = geoMan->GetCurrentNode();
  Int_t nD = ladder->GetNdaughters();
  Int_t modNr0 = moduleNr;
  
  // Loop over ladder daughters (half-ladders)
  for (Int_t i2 = 0; i2 < nD; i2++) {
    if ( !TString(ladder->GetDaughter(i2)->GetName()).Contains("HalfLadder") ) continue;

    // Down to half-ladder
    geoMan->CdDown(i2);
    TGeoNode* halflad = geoMan->GetCurrentNode();
    Int_t nHalfLadD = halflad->GetNdaughters();

    // Loop over half-ladder daughters (modules)
    for (Int_t imod = 0; imod < nHalfLadD; imod++) {
      TGeoNode *sens = halflad->GetDaughter(imod)->GetDaughter(0);
      //cout << sens->GetName() << endl;
      geoMan->CdDown(imod);
      geoMan->CdDown(0);
      ++moduleNr;
      SetSensor(moduleNr, statNr, statCBM, stationPar);
      //cout << moduleNr << " " << geoMan->GetPath() << endl;
      geoMan->CdUp();
      geoMan->CdUp();
    }

    geoMan->CdUp();
  } // loop over half-ladders

  return moduleNr - modNr0;
}

// -------------------------------------------------------------------------

ClassImp(CbmStsDigiScheme)
