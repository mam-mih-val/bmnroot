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
  if ( ! geoPar ) {
    cout << "-W- CbmStsDigiScheme::Init: "
	 << "No geometry parameters available!" << endl;
    return kFALSE;
  }
  if ( ! digiPar ) {
    cout << "-W-  CbmStsDigiScheme::Init: "
	 << "No digitisation parameters available!" << endl;
    return kFALSE;
  }

  TGeoVolume* test = NULL;

  // Check for old geometry version. Should contain stsstation01keepvol.
  test = (TGeoVolume*) gGeoManager->GetListOfVolumes()->FindObject("stsstation01keepvol");
  if ( test ) {
    cout << "CbmStsDigiScheme:: Init for old geometry" << endl;
    fIsNewGeometry = kFALSE;
    return InitOld(geoPar, digiPar);
  }

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
    return InitNew(geoPar, digiPar);
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



// ---- Init old  ----------------------------------------------------------
Bool_t CbmStsDigiScheme::InitOld(CbmGeoStsPar* geoPar, 
				 CbmStsDigiPar* digiPar) {

  // Loop over stations in DigiPar
  Int_t nStations = digiPar->GetNStations();
  CbmStsStationDigiPar*  statPar = NULL;
  CbmStsSectorDigiPar* sectorPar = NULL;
  CbmStsSensorDigiPar* sensorPar = NULL;
  TObjArray* passNodes = geoPar->GetGeoPassiveNodes();
  TObjArray* sensNodes = geoPar->GetGeoSensitiveNodes();
  for (Int_t iStation=0; iStation<nStations; iStation++) {

    // Get digitisation parameters of station
    statPar = (CbmStsStationDigiPar*) digiPar->GetStation(iStation);
    Int_t    statNr   = statPar->GetStationNr();
    Double_t statRot  = statPar->GetRotation();
    Int_t    nSectors = statPar->GetNSectors();
    Double_t statZPos = -666.;// = statPar->GetZPosition();

    CbmStsStation* station;
    TString stationName = Form("stat%02d",iStation+1);

    TString statVolName = Form("stsstation%02ikeepvol",statNr);
    FairGeoNode* statKeepVol = (FairGeoNode*) (passNodes->FindObject(statVolName));
    if ( statKeepVol ) {
      FairGeoTransform* transform = statKeepVol->getLabTransform();
      FairGeoVector translat = transform->getTranslation();
      FairGeoTransform center = statKeepVol->getCenterPosition();
      FairGeoVector centerV = center.getTranslation();

      statZPos = translat.Z() + centerV.Z();
    }

    Int_t sensorByIndex = 0;

    for (Int_t iSector=0; iSector<nSectors; iSector++) {
      sectorPar = (CbmStsSectorDigiPar*) statPar->GetSector(iSector);
      Int_t sectorNr = iSector+1;
      Int_t sectorDetId  = 2 << 24 | statNr << 16 | sectorNr << 4;

      sectorPar = (CbmStsSectorDigiPar*) statPar->GetSector(iSector);
      Int_t nSensors = sectorPar->GetNSensors();

      CbmStsSector* sector;
      TString sectorName = Form("stat%02dsect%d",iStation+1,iSector+1);

      for (Int_t iSensor=0; iSensor<nSensors; iSensor++) {
	Int_t sensorNr = iSensor+1;
	Int_t    detId  = 2 << 24 | statNr << 16 | sectorNr << 4 | sensorNr << 1;
	  
	FairGeoNode* geoSensor;
	if ( sensorByIndex ) geoSensor = (FairGeoNode*) (sensNodes->At(sensorByIndex++));
	else {
	  Int_t nofNodes = sensNodes->GetEntries();
	  TString tempLookName = Form("stsstation%02isensor1#1",statNr);
	  for (Int_t it=0; it<nofNodes; it++) {
	    geoSensor = (FairGeoNode*) (sensNodes->At(it));
	    TString tempNodeName = geoSensor->getName();
	    if ( tempNodeName.Contains(tempLookName.Data()) ) {
	      sensorByIndex = it+1;
	      break;
	    }
	  }
	}
	fDetIdByName.insert(pair<TString, Int_t> (geoSensor->GetName(), detId));

	sensorPar = (CbmStsSensorDigiPar*) sectorPar->GetSensor(iSensor);

	Int_t    sensorType   = sensorPar->GetType();
	Double_t sensorX      = sensorPar->GetX0();
	Double_t sensorY      = sensorPar->GetY0();
	Double_t sensorZ      = sensorPar->GetZ0();
	Double_t sensorRot    = sensorPar->GetRotation();
	Double_t sensorXDim   = sensorPar->GetLx();
	Double_t sensorYDim   = sensorPar->GetLy();
	Double_t sensorZDim   = sensorPar->GetD();
	Double_t sensorDX     = sensorPar->GetDx();
	Double_t sensorDY     = sensorPar->GetDy();
	Double_t sensorStereoF = sensorPar->GetStereoF();
	Double_t sensorStereoB = sensorPar->GetStereoB();

	TString sensorName = geoSensor->GetName();
	sensorName.ReplaceAll("#","_");

	CbmStsSensor* sensor = new CbmStsSensor(sensorName.Data(), detId, 
						sensorType, 
						sensorX, sensorY, sensorZ,
						sensorRot, 
						sensorXDim, sensorYDim, sensorZDim, 
						sensorDX, sensorDY, sensorStereoF, sensorStereoB);
	

	// create sector that will keep the sensor
	if ( iSensor == 0 ) {
	  sector  = new CbmStsSector(sectorName.Data(), sectorDetId);

	  // create station that will keep the sector
	  if ( iSector == 0 ) {
	    FairGeoMedium* material = geoSensor->getMedium();
	    Double_t sensorRL    = material->getRadiationLength();

	    station = new CbmStsStation(stationName.Data(), statNr, statZPos,
					sensorZDim, sensorRL, 0., 
					100., statRot);
	    fStations->Add(station);
	    fStationMap[statNr] = station;
	  }	
	  station->AddSector(sector);
	}
	sector->AddSensor(sensor);

	// put sensor into name/sensor map
	map < TString, CbmStsSensor*>::iterator p;
   	p=fSensorByName.find(sensorName);
	if(p!=fSensorByName.end()){
	  cout << " -E- Sensor \"" << sensorName.Data() << "\" is already inserted " << endl;
	}else{
	  fSensorByName.insert(pair<TString, CbmStsSensor*> (sensorName, sensor));
	}

      } // Loop over sensors

      fNSensors  += sector->GetNSensors();
    } // Loop over sectors
    
    fNSectors  += station->GetNSectors();
    fNChannels += station->GetNChannels();

  } // Loop over stations

  Print();
  return kTRUE;

}
// -------------------------------------------------------------------------



// ---- Init new  ----------------------------------------------------------
Bool_t CbmStsDigiScheme::InitNew(CbmGeoStsPar* geoPar, 
				 CbmStsDigiPar* digiPar) {

  // Get STS node
  TGeoNode* sts = NULL;
  gGeoManager->CdTop();
  TGeoNode* cave = gGeoManager->GetCurrentNode();
  for (Int_t iNode = 0; iNode < cave->GetNdaughters(); iNode++) {
    TGeoNode* node = cave->GetDaughter(iNode);
    TString name = node->GetName();
    if ( name.Contains("GEMS") ) {
      sts = node;
      gGeoManager->CdDown(iNode);
      break;
    }
  }
  if ( ! sts ) {
    cout << "-E- CbmStsDigiScheme::InitNew: Cannot find top STS node" 
	 << endl;
    return kFALSE;
  }

  // Loop over stations in STS
  Int_t statNr = 0; //AZ
  for (Int_t iNode = 0; iNode < sts->GetNdaughters(); iNode++) {

    // Go to station node
    gGeoManager->CdDown(iNode);
    TGeoNode* stationNode = gGeoManager->GetCurrentNode();
    TString statName = stationNode->GetName();
    if ( ! statName.Contains("Station") ) {
      gGeoManager->CdUp();
      continue;
    }
    //AZ Int_t statNr = stationNode->GetNumber();
    ++statNr; //AZ

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
    Int_t moduleNr = 0;
    Int_t nModules = stationNode->GetNdaughters();

    for (Int_t iModule = 0; iModule < nModules; iModule++) {
      gGeoManager->CdDown(iModule);
      TGeoNode* moduleNode = gGeoManager->GetCurrentNode();
      TString moduleName = moduleNode->GetName();
      if ( ! moduleName.Contains("Module") ) {
	gGeoManager->CdUp();
	continue;
      }
      //moduleNr++;

      // Get module / sector parameters
      /*
      CbmStsSectorDigiPar* sectorPar = NULL;
      for (Int_t iSector = 0; iSector < stationPar->GetNSectors(); iSector++) {
	CbmStsSectorDigiPar* partest2 = stationPar->GetSector(iSector);
	if ( partest2->GetSectorNr() == moduleNr + 1 ) {
	  sectorPar = partest2;
	  break;
	}
      }
      if ( ! sectorPar ) {
	cout << "-E- CbmStsDigiScheme::InitNew: No parameters for module " 
	     << moduleNr << ", station " << statNr << endl;
	return kFALSE;
      }

      // Create CbmStsSector
      TString sectName = Form("stat%02dsect%d", statNr, moduleNr);
      Int_t   sectId = 2 << 24 | statNr << 16 | moduleNr << 4;
      CbmStsSector* sector = new CbmStsSector(sectName.Data(), sectId);
      station->AddSector(sector);
      */
      CbmStsSector* sector = NULL;

      // Loop over sensors in the sector
      Int_t nSensors = moduleNode->GetNdaughters();
      for (Int_t iSensor = 0; iSensor < moduleNode->GetNdaughters(); iSensor++) {
	gGeoManager->CdDown(iSensor);
	TGeoNode* sensorNode = gGeoManager->GetCurrentNode();
	if ( ! TString(sensorNode->GetName()).Contains("Sensor") ) {
	  gGeoManager->CdUp();
	  continue;
	}

	moduleNr++;
 
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
	  return kFALSE;
	}

 	// Create CbmStsSector
	TString sectName = Form("stat%02dsect%d", statNr, moduleNr);
	Int_t   sectId = 2 << 24 | statNr << 16 | moduleNr << 4;
	sector = new CbmStsSector(sectName.Data(), sectId);
	station->AddSector(sector);

	//Int_t sensNr = sensorNode->GetNumber();
	Int_t sensNr = 1;
	    
	// Get sensor parameters
	CbmStsSensorDigiPar* sensorPar = NULL;
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
	  return kFALSE;
	}

	// Create CbmStsSensor
	Double_t* sensTrans = gGeoManager->GetCurrentMatrix()->GetTranslation();
	TString   sensName = GetCurrentPath();
	Int_t     sensId = 2 << 24 | statNr << 16 | moduleNr << 4 | sensNr << 1;
	Int_t     sensType = sensorPar->GetType();
	Double_t  sensX = sensTrans[0];
	Double_t  sensY = sensTrans[1];
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
	  return kFALSE;
	}
	fDetIdByName[sensName] = sensId;
	if ( fSensorByName.find(sensName) != fSensorByName.end() ) {
	  cout << "-E- CbmStsDigiScheme: Duplicate sensor name " << sensName << endl;
	  return kFALSE;
	}
	fSensorByName[sensName] = sensor;
	    
	
	
	
	gGeoManager->CdUp();       // to module
      }                            // sensor loop
      fNSensors += sector->GetNSensors();
      gGeoManager->CdUp();             // to station
    }                                  // ladder loop
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


  


  
ClassImp(CbmStsDigiScheme)
