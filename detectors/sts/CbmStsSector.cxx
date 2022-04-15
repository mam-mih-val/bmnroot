// -------------------------------------------------------------------------
// -----                   CbmStsStripSector source file               -----
// -----                  Created 02/03/05  by V. Friese               -----
// -------------------------------------------------------------------------

#include "CbmStsSector.h"

#include "CbmStsSensor.h"
#include "CbmStsSensorDigiPar.h"

#include "FairLogger.h"

#include "TMath.h"

#include <iostream>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::pair;
using std::vector;


// -----   Default constructor   -------------------------------------------
CbmStsSector::CbmStsSector() 
  : TNamed(),
    fDetectorId(0),
    fType(-666),
    fRotation(-666.),
    fDx(-666.),
    fDy(-666.),
    fStereoF(-666.),
    fStereoB(-666.),
    fSigmaX(0.),
    fSigmaY(0.),
    fSigmaXY(0.),
    fNChannelsFront(0),
    fNChannelsBack(0),
    fSensors(new TObjArray(100)),
    fFrontActive(),
    fBackActive(),
    fSensorMap(),
    fTrueHits()
{
  cout << "-W- CbmStsSector: Do not use this constructor! " << endl;
  fSensorMap.clear();
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsSector::CbmStsSector(TString tempName, Int_t detId)
  : TNamed(tempName.Data(),""),
    fDetectorId(detId),
    fType(-1),
    fRotation(-666.),
    fDx(-666.),
    fDy(-666.),
    fStereoF(-666.),
    fStereoB(-666.),
    fSigmaX(0.),
    fSigmaY(0.),
    fSigmaXY(0.),
    fNChannelsFront(0),
    fNChannelsBack(0),
    fSensors(new TObjArray(100)),
    fFrontActive(),
    fBackActive(),
    fSensorMap(),
    fTrueHits()
{
  fSensorMap.clear();
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSector::~CbmStsSector() { };
// -------------------------------------------------------------------------


// -----   Public method PointIndex   --------------------------------------
Int_t CbmStsSector::PointIndex(Int_t iFStrip, Int_t iBStrip) {
  pair<Int_t,Int_t> a(iFStrip, iBStrip);
  if (fTrueHits.find(a) == fTrueHits.end()) return -1;
  return fTrueHits[a];
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmStsSector::Reset() {
  fFrontActive.clear();
  fBackActive.clear();
  fTrueHits.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmStsSector::Print() {
  cout << "   Sector Nr. ";
  cout.width(3);
  cout << GetSectorNr() << ", Type ";
  cout << GetNChannels() << endl;
}
// -------------------------------------------------------------------------



// -----   Public method GetSectorByNr   -----------------------------------
CbmStsSensor* CbmStsSector::GetSensorByNr(Int_t sensorNr) {
  if ( fSensorMap.find(sensorNr) != fSensorMap.end() ) {
    Int_t index = fSensorMap[sensorNr];
    return (CbmStsSensor*) fSensors->At(index);
  }
  else {
    cout << "-W- CbmStsSector::GetSensorByNr: sensor " << sensorNr
	 << " not found (sector " << GetSectorNr() << ")." << endl;
    return NULL;
  }
}
// -------------------------------------------------------------------------

// -----   Public method AddSensor   ---------------------------------------
void CbmStsSector::AddSensor(CbmStsSensorDigiPar* sensorPar) {

  // Get digitisation parameters
  Int_t iSensor   = sensorPar->GetSensorNr();
  Int_t iType     = sensorPar->GetType();
  Double_t x0     = sensorPar->GetX0();
  Double_t y0     = sensorPar->GetY0();
  Double_t z0     = sensorPar->GetZ0();
  Double_t rotRel = sensorPar->GetRotation();
  Double_t lx     = sensorPar->GetLx();
  Double_t ly     = sensorPar->GetLy();
  Double_t d      = sensorPar->GetD();
  Double_t dx     = sensorPar->GetDx();
  Double_t dy     = sensorPar->GetDy();
  Double_t stereoF = sensorPar->GetStereoF();
  Double_t stereoB = sensorPar->GetStereoB();

  // Check for previous existence of sensor
  if ( fSensorMap.find(iSensor) != fSensorMap.end() ) {
    cout << "-W- " << fName << "::AddSensor: Sensor number " << iSensor
	 << " already existing for sector " << GetSectorNr() << endl;
    return;
  }

  // Calculate detectorId
  Int_t sensorId = fDetectorId | iSensor << 1;

  // Number of existing sensors
  Int_t nSensors = fSensors->GetEntries();

  // Create and add new sensor
  CbmStsSensor* tempSens = new CbmStsSensor(Form("%ssens%d",fName.Data(),iSensor), sensorId, 
					    iType, 
					    x0,y0,z0,
					    rotRel, 
					    lx,ly,d,
					    dx,dy,stereoF,stereoB);
  fSensors->Add(tempSens);

  Double_t sigmaX   = tempSens->GetSigmaX();
  Double_t sigmaY   = tempSens->GetSigmaY();
  Double_t sigmaXY  = tempSens->GetSigmaXY();
  Int_t nofChF   = tempSens->GetNChannelsFront();
  Int_t nofChB   = tempSens->GetNChannelsBack();
  if ( nSensors == 0 ) {
    fType     = iType;
    fRotation = rotRel;
    fDx       = dx;
    fDy       = dy;
    fStereoF  = stereoF;
    fStereoB  = stereoB;
    fSigmaX   = sigmaX;
    fSigmaY   = sigmaY;
    fSigmaXY  = sigmaXY;
    fNChannelsFront = nofChF;
    fNChannelsBack  = nofChB;
  }
  else {
    if ( fType     != iType   ) LOG(fatal) << "Sensor types do not match: " << fType << " vs " << iType;
    if ( fRotation != rotRel  ) LOG(fatal) << "Sensor rotation do not match";
    if ( fDx       != dx      ) LOG(fatal) << "Sensor dx do not match";
    if ( fDy       != dy      ) LOG(fatal) << "Sensor dy do not match";
    if ( fStereoF  != stereoF ) LOG(fatal) << "Sensor stereoF do not match";
    if ( fStereoB  != stereoB ) LOG(fatal) << "Sensor stereoB do not match";
    if ( fSigmaX   != sigmaX  ) LOG(fatal) << "Sensor sigmaX do not match";
    
    if ( fSigmaY   != sigmaY ) LOG(fatal) << "Sensor sigmaY do not match";
    if ( fSigmaXY  != sigmaXY) LOG(fatal) << "Sensor sigmaXY do not match";
    if ( fNChannelsFront != nofChF ) LOG(fatal) << "Sensor nofChanF do not match";
    if ( fNChannelsBack  != nofChB ) LOG(fatal) << "Sensor nofChanB do not match";
  }



  fSensorMap[iSensor] = nSensors;
}
// -------------------------------------------------------------------------

// -----   Public method AddSensor   ---------------------------------------
void CbmStsSector::AddSensor(CbmStsSensor* sensor) {

  Int_t iSensor    = sensor->GetSensorNr();
  Int_t nSensors   = fSensors->GetEntries();

  Int_t iType      = sensor->GetType();
  Double_t rotRel  = sensor->GetRotation();
  Double_t dx      = sensor->GetDx();
  Double_t dy      = sensor->GetDy();
  Double_t stereoF = sensor->GetStereoF();
  Double_t stereoB = sensor->GetStereoB();
  Double_t sigmaX  = sensor->GetSigmaX();
  Double_t sigmaY  = sensor->GetSigmaY();
  Double_t sigmaXY = sensor->GetSigmaXY();
  Int_t    nofChF  = sensor->GetNChannelsFront();
  Int_t    nofChB  = sensor->GetNChannelsBack();
  if ( nSensors == 0 ) {
    fType     = iType;
    fRotation = rotRel;
    fDx       = dx;
    fDy       = dy;
    fStereoF  = stereoF;
    fStereoB  = stereoB;
    fSigmaX   = sigmaX;
    fSigmaY   = sigmaY;
    fSigmaXY  = sigmaXY;
    fNChannelsFront = nofChF;
    fNChannelsBack  = nofChB;
  }
  else {
    if ( fType     != iType  )  LOG(fatal) << "Sensor types do not match: " << fType << " vs " << iType;
    if ( fRotation != rotRel )  LOG(fatal) << "Sensor rotation do not match";
    if ( fDx       != dx     )  LOG(fatal) << "Sensor dx do not match";
    if ( fDy       != dy     )  LOG(fatal) << "Sensor dy do not match";
    if ( fStereoF  != stereoF ) LOG(fatal) << "Sensor stereoF do not match";
    if ( fStereoB  != stereoB ) LOG(fatal) << "Sensor stereoB do not match";
    if ( fSigmaX   != sigmaX )  LOG(fatal) << "Sensor sigmaX do not match";
    
    if ( fSigmaY   != sigmaY ) LOG(fatal) << "Sensor sigmaY do not match";
    if ( fSigmaXY  != sigmaXY) LOG(fatal) << "Sensor sigmaXY do not match";
    
    if ( fNChannelsFront != nofChF ) LOG(fatal) << "Sensor nofChanF do not match";
    if ( fNChannelsBack  != nofChB ) LOG(fatal) << "Sensor nofChanB do not match";
  }

  fSensors->Add(sensor);

  fSensorMap[iSensor] = nSensors;

}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSector::Intersect(Int_t iFStrip, Int_t iBStrip, 
			      Double_t& xCross, Double_t& yCross, Double_t& zCross) {

  Double_t xPoint = xCross, yPoint = yCross; //AZ
  xCross = 0.;
  yCross = 0.;
  zCross = 0.;

  //AZ Double_t xTemp, yTemp, zTemp=0.;
  Double_t xTemp = xPoint, yTemp = yPoint, zTemp=0.; //AZ

  Int_t tempDetId = -1;
  Int_t returnDetId = -1;

  CbmStsSensor* sensor;
  for (Int_t iSens=0; iSens<GetNSensors(); iSens++) {
    sensor = (CbmStsSensor*)GetSensor(iSens);

    tempDetId = sensor->Intersect(iFStrip, iBStrip, xTemp, yTemp, zTemp);

    if ( tempDetId  == -1 ) continue; //not in the sensor
    //AZ - pixel simulation
    if (TMath::Abs(TMath::Abs(fStereoF-fStereoB)*TMath::RadToDeg()-90) < 0.1) {
      //if (TMath::Abs(xTemp-xPoint) > 0.5 || TMath::Abs(yTemp-yPoint) > 0.5) continue;
    } //
    
    if ( zCross > 0.001 ) {
      LOG(fatal) << "Software do not cope with two strips intersecting in different regions";
      
    }

    xCross = xTemp;
    yCross = yTemp;
    zCross = zTemp;
    returnDetId = tempDetId;
  }

  return returnDetId;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSector::IntersectClusters(Double_t fChan, Double_t bChan,
				      Double_t& xCross, Double_t& yCross, Double_t& zCross) {

  xCross = 0.;
  yCross = 0.;
  zCross = 0.;
  
  Double_t xTemp, yTemp, zTemp=0.;
  
  Int_t tempDetId = -1;
  Int_t returnDetId = -1;
  
  CbmStsSensor* sensor;
  for (Int_t iSens=0; iSens<GetNSensors(); iSens++) {
    sensor = (CbmStsSensor*)GetSensor(iSens);

    tempDetId = sensor->IntersectClusters(fChan, bChan, xTemp, yTemp, zTemp);
    
    if ( tempDetId  == -1 ) continue; //not in the sensor
    
    if ( zCross > 0.001 ) 
      LOG(fatal) << "Software do not cope with two strips intersecting in different regions";
    
    xCross = xTemp;
    yCross = yTemp;
    zCross = zTemp;
    returnDetId = tempDetId;
  }
  
  return returnDetId;
}
// -------------------------------------------------------------------------

ClassImp(CbmStsSector)
