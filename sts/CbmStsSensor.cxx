// -------------------------------------------------------------------------
// -----                   CbmStsStripSensor source file               -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

#include "CbmStsSensor.h"

#include "CbmStsSensorDigiPar.h"

#include "TGeoManager.h" //AZ
#include "TMath.h"
#include "TRandom.h"

#include <iostream>
#include <list>
#include <vector>

using std::cout;
using std::endl;
using std::pair;
using std::vector;


// -----   Default constructor   -------------------------------------------
CbmStsSensor::CbmStsSensor() 
  : 
  fDetectorId(0),
  fType(0),
  fX0(0.), 
  fY0(0.), 
  fZ0(0.),
  fRotation(0.),
  fLx(0.), 
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fStereoF(0.),
  fStereoB(0.),
  fD(0.),
  fNChannelsFront(0),
  fNChannelsBack(0),
  fBackStripShift(0.),
  fFrontStripShift(0.),
  fSigmaX(0.),
  fSigmaY(0.),
  fSigmaXY(0.),
  fXSmearWidth(0.0005),
  fZSmearSlope(0.04),
  fFrontLorentzShift(0.132),
  fBackLorentzShift(0.026),
  fFrontActive(),
  fBackActive(),
  fTrueHits()
{
  cout << "-W- CbmStsSensor: Do not use this constructor! " << endl;
}
// -------------------------------------------------------------------------



// -----   Enhanced constructor (by z0 and d)  ------------------------------------------
CbmStsSensor::CbmStsSensor(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0,
			   Double_t rotation, Double_t lx, Double_t ly, 
			   Double_t d, Double_t dx, Double_t dy, Double_t stereoF, Double_t stereoB)
  :
  TNamed(tempName.Data(), ""),
  fDetectorId(0),
  fType(0),
  fX0(0.), 
  fY0(0.), 
  fZ0(0.),
  fRotation(0.),
  fLx(0.), 
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fStereoF(0.),
  fStereoB(0.),
  fD(0.),
  fNChannelsFront(0),
  fNChannelsBack(0),
  fBackStripShift(0.),
  fFrontStripShift(0.),
  fSigmaX(0.),
  fSigmaY(0.),
  fSigmaXY(0.),
  fXSmearWidth(0.0005),
  fZSmearSlope(0.04),
  fFrontLorentzShift(0.132),
  fBackLorentzShift(0.026),
  fFrontActive(),
  fBackActive(),
  fTrueHits()
{
  fName = tempName.Data();
  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fZ0         = z0;  // z position of the station
  fRotation   = rotation;
  fLx         = lx;
  fLy         = ly;
  fD          = d;   // thickness of the station
  fDx         = dx;
  fDy         = dy;
  fStereoF    = stereoF;
  fStereoB    = stereoB;
  fBackStripShift  = 0.;
  fFrontStripShift = 0.;
  Double_t dbNoX = fLx / fDx;
  //AZ
  fTanF = TMath::Tan (fStereoF);
  fTanB = TMath::Tan (fStereoB);
  fSinRot = TMath::Sin(fRotation);
  fCosRot = TMath::Cos(fRotation);
  
  // Calculate number of channels
  if ( fType == 1 ) {             // Pixel sensor
    Double_t dbNoY = fLy / fDy;           
    fNChannelsFront = Int_t( TMath::Ceil ( dbNoX ) 
			    *TMath::Ceil ( dbNoY ));
    fNChannelsBack  = 0;
  }
  else if ( fType == 2 ) {        // strip sensor
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    Double_t aPlus  = TMath::Abs( fLy * TMath::Tan(fStereoB) );
    //AZ Double_t dbNoB = ( fLx + aPlus ) / fDx ;
    Double_t dbNoB = ( fLx + aPlus ) / fDy ;
    fNChannelsBack  = Int_t(TMath::Ceil ( dbNoB ));
  }
  else if ( fType == 3 ) {        // strip sensor with double metal layer
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    if (fStereoB*180/TMath::Pi()>80) {
      fNChannelsBack  = Int_t(TMath::Ceil ( fLy / fDy ));
    }
    else {
      fNChannelsBack = fNChannelsFront;
    }
  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Calculate errors and covariance
  if ( fType == 1 ) {
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else if ( fType == 2 || fType == 3 ) {

    if (fStereoF==0. && fStereoB*180/TMath::Pi()<80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDx / (TMath::Sqrt(6)*TMath::Tan(fStereoB));
      fSigmaXY = (-1. * fDx * fDx)/(12.*TMath::Tan(fStereoB));
    }
    else if (fStereoF==0. && fStereoB*180/TMath::Pi()>80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDy / TMath::Sqrt(12);
      fSigmaXY = 0.;
    }
    else {
      fSigmaX  = fDx / TMath::Sqrt(24);
      fSigmaY  = fDx / (TMath::Sqrt(24)*TMath::Tan(fStereoB));
      fSigmaXY = 0.;
    }
    
    Int_t sensorNumber = ( fDetectorId & (7<<1) ) >> 1;

    if ( sensorNumber == 1 ) {
      fBackStripShift  = 0.;
      fFrontStripShift = 0.;
      if (fStereoB < 0) fBackStripShift  = fLy * TMath::Tan(-fStereoB); //AZ
    }
    else if ( sensorNumber == 2 ) {
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 1. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
    }
    else if ( sensorNumber == 3 ){
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 2. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
    }
    
  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Transform errors to global c.s. 
  Double_t cosRot = TMath::Cos(fRotation);
  Double_t sinRot = TMath::Sin(fRotation);
  Double_t vX  = cosRot * cosRot * fSigmaX * fSigmaX
               - 2. * cosRot * sinRot * fSigmaXY
               + sinRot * sinRot * fSigmaY * fSigmaY;
  Double_t vY  = sinRot * sinRot * fSigmaX * fSigmaX
               + 2. * cosRot * sinRot * fSigmaXY
               + cosRot * cosRot * fSigmaY * fSigmaY;
  Double_t vXY = cosRot * sinRot * fSigmaX * fSigmaX
               + ( cosRot*cosRot - sinRot*sinRot ) * fSigmaXY
               - cosRot * sinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;

  fXSmearWidth = 0.0005;
  fZSmearSlope = 0.04;

  fFrontLorentzShift = 0.132;
  fBackLorentzShift  = 0.026;
  fFrontLorentzShift = fBackLorentzShift = 0.0; //AZ
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
CbmStsSensor::CbmStsSensor(Int_t detId, Int_t iType, Double_t x0, 
			   Double_t y0, Double_t rotation, Double_t lx, 
			   Double_t ly, Double_t dx, Double_t dy, 
			   Double_t stereoF, Double_t stereoB) 
  :
  fDetectorId(0),
  fType(0),
  fX0(0.), 
  fY0(0.), 
  fZ0(0.),
  fRotation(0.),
  fLx(0.), 
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fStereoF(0.),
  fStereoB(0.),
  fD(0.),
  fNChannelsFront(0),
  fNChannelsBack(0),
  fBackStripShift(0.),
  fFrontStripShift(0.),
  fSigmaX(0.),
  fSigmaY(0.),
  fSigmaXY(0.),
  fXSmearWidth(0.0005),
  fZSmearSlope(0.04),
  fFrontLorentzShift(0.132),
  fBackLorentzShift(0.026),
  fFrontActive(),
  fBackActive(),
  fTrueHits()
{
  fDetectorId = detId;
  fType       = iType;
  fX0         = x0;
  fY0         = y0;
  fRotation   = rotation;
  fLx         = lx;
  fLy         = ly;
  fDx         = dx;
  fDy         = dy;
  fStereoF    = stereoF;
  fStereoB    = stereoB;
  fBackStripShift  = 0.;
  fFrontStripShift = 0.;
  Double_t dbNoX = fLx / fDx;
  //AZ
  fTanF = TMath::Tan (fStereoF);
  fTanB = TMath::Tan (fStereoB);
  fSinRot = TMath::Sin(fRotation);
  fCosRot = TMath::Cos(fRotation);
  
  // Calculate number of channels
  if ( fType == 1 ) {             // Pixel sensor
    Double_t dbNoY = fLy / fDy;
    fNChannelsFront = Int_t( TMath::Ceil ( dbNoX ) 
			    *TMath::Ceil ( dbNoY ));
    fNChannelsBack  = 0;
  }
  else if ( fType == 2 ) {        // strip sensor
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    Double_t aPlus  = TMath::Abs( fLy * TMath::Tan(fStereoB) );
    //AZ Double_t dbNoB = ( fLx + aPlus ) / fDx ;
    Double_t dbNoB = ( fLx + aPlus ) / fDy ;
    fNChannelsBack  = Int_t(TMath::Ceil ( dbNoB ));
  }
  else if ( fType == 3 ) {        // strip sensor with double metal layer
    fNChannelsFront = Int_t(TMath::Ceil ( dbNoX ));
    fNChannelsBack  = fNChannelsFront;
  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Calculate errors and covariance
  if ( fType == 1 ) {
    fSigmaX  = fDx / TMath::Sqrt(12);
    fSigmaY  = fDy / TMath::Sqrt(12);
    fSigmaXY = 0.;
  }
  else if ( fType == 2 || fType == 3 ) {
     if (fStereoF==0. && fStereoB*180/TMath::Pi()<80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDx / (TMath::Sqrt(6)*TMath::Tan(fStereoB));
      fSigmaXY = (-1. * fDx * fDx)/(12.*TMath::Tan(fStereoB));
    }
    else if (fStereoF==0. && fStereoB*180/TMath::Pi()>80 ) {
      fSigmaX  = fDx / TMath::Sqrt(12);
      fSigmaY  = fDy / TMath::Sqrt(12);
      fSigmaXY = 0.;
    }
    else {
      fSigmaX  = fDx / TMath::Sqrt(24);
      fSigmaY  = fDx / (TMath::Sqrt(24)*TMath::Tan(fStereoB));
      fSigmaXY = 0.;
    }
    
    Int_t sensorNumber = ( fDetectorId & (7<<1) ) >> 1;
    if ( sensorNumber == 1 ) {
      fBackStripShift  = 0.;
      fFrontStripShift = 0.;
      if (fStereoB < 0) fBackStripShift  = fLy * TMath::Tan(-fStereoB); //AZ
    }
    else if ( sensorNumber == 2 ) {
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 1. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 1. * fLy * TMath::Tan(fStereoF);
      }
    }
    else if ( sensorNumber == 3 ){
      if (fStereoB*180/TMath::Pi()>80) {
        fBackStripShift  = 0.;
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
      else {
        fBackStripShift  = 2. * fLy * TMath::Tan(fStereoB);
        fFrontStripShift = 2. * fLy * TMath::Tan(fStereoF);
      }
    }
    
  }
  else {
    cout << "-E- CbmStsSensor: Illegal sensor type " << fType << endl;
    Fatal("", "Illegal sensor type");
  }

  // Transform errors to global c.s. 
  Double_t cosRot = TMath::Cos(fRotation);
  Double_t sinRot = TMath::Sin(fRotation);
  Double_t vX  = cosRot * cosRot * fSigmaX * fSigmaX
               - 2. * cosRot * sinRot * fSigmaXY
               + sinRot * sinRot * fSigmaY * fSigmaY;
  Double_t vY  = sinRot * sinRot * fSigmaX * fSigmaX
               + 2. * cosRot * sinRot * fSigmaXY
               + cosRot * cosRot * fSigmaY * fSigmaY;
  Double_t vXY = cosRot * sinRot * fSigmaX * fSigmaX
               + ( cosRot*cosRot - sinRot*sinRot ) * fSigmaXY
               - cosRot * sinRot * fSigmaY * fSigmaY;
  fSigmaX  = TMath::Sqrt(vX);
  fSigmaY  = TMath::Sqrt(vY);
  fSigmaXY = vXY;

  fXSmearWidth = 0.0005;
  fZSmearSlope = 0.04;

  fFrontLorentzShift = 0.132;
  fBackLorentzShift  = 0.026;
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSensor::~CbmStsSensor() { };
// -------------------------------------------------------------------------



// -----   Public method GetChannel   --------------------------------------
Int_t CbmStsSensor::GetChannel(Double_t x, Double_t y, Int_t iSide) {

  // Check iSide
  if (iSide !=0 && iSide != 1) {
    cout << "-W- CbmStsSensor::GetChannel: Illegal side number " 
	 << iSide << endl;
    return -1;
  }

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = 0;

  Double_t dbNoX = fLx / fDx;
  // Case pixel sensor: iChannel = iRow * nColumns + iColumn
  if ( fType == 1 ) {
    Int_t nCol = Int_t(TMath::Ceil( dbNoX));
    Int_t iCol = Int_t(xint / fDx);
    Int_t iRow = Int_t(yint / fDy);
    iChan = iRow * nCol + iCol;
    if ( iChan < 0 || iChan > fNChannelsFront ) {
      cout << "-E- CbmStsSensor::GetChannel: "
	   << "Channel number " << iChan << " exceeds limit " 
	   << fNChannelsFront << endl;
      cout << GetStationNr() << " " << GetSensorNr() << endl;
      cout << x << " " << y << " " << iSide << endl;
      Fatal("GetChannel", "illegal channel number");
    }
  }

  // Case strip sensor 1
  else if ( fType == 2 ) {
    if ( iSide == 0 )
     {        // Front side
      iChan = Int_t ( xint / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront )
       {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
       }
    }
    else {                     // Back side
      // Project point along backside strip to y=0
      Double_t xp = xint + yint * TMath::Tan(fStereoB);
      // Digitise
      iChan = Int_t ( xp / fDx );
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  // Case strip sensor 2 (with double metal layer)
  else if ( fType == 3 ) {
    if ( iSide == 0 ) {      // Front side
     Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
     xf = xf - TMath::Floor(xf/fLx) * fLx;
     iChan = Int_t ( xf / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                   // Back side
      if (fStereoB*180/TMath::Pi()>80) {
        Double_t xp = yint;
	xp = xp - TMath::Floor(xp/fLy) * fLy;
	iChan = Int_t( xp / fDy );
      }
      else {
	// Project point along backside strip to y = 0 
        Double_t xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
        // Calculate modulo w.r.t. sensor x width
        xp = xp - TMath::Floor(xp/fLx) * fLx;
        // Digitise
        iChan = Int_t( xp / fDx );
      }
      
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;	
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  return iChan;
}
// -------------------------------------------------------------------------

    
// -----   Public method GetChannel   --------------------------------------
Float_t CbmStsSensor::GetChannelPlus(Double_t x, Double_t y, Int_t iSide) {

  // Check iSide
  if (iSide !=0 && iSide != 1) {
    cout << "-W- CbmStsSensor::GetChannel: Illegal side number " 
	 << iSide << endl;
    return -1;
  }

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Float_t    iChan = 0;

  Double_t dbNoX = fLx / fDx;
  // Case pixel sensor: iChannel = iRow * nColumns + iColumn
  if ( fType == 1 ) {
    Int_t nCol = Int_t(TMath::Ceil( dbNoX));
    Int_t iCol = Int_t(xint / fDx);
    Int_t iRow = Int_t(yint / fDy);
    iChan = iRow * nCol + iCol;
    if ( iChan < 0 || iChan > fNChannelsFront ) {
      cout << "-E- CbmStsSensor::GetChannel: "
	   << "Channel number " << iChan << " exceeds limit " 
	   << fNChannelsFront << endl;
      cout << GetStationNr() << " " << GetSensorNr() << endl;
      cout << x << " " << y << " " << iSide << endl;
      Fatal("GetChannel", "illegal channel number");
    }
  }

  // Case strip sensor 1
  else if ( fType == 2 ) {
    if ( iSide == 0 ) {        // Front side
      iChan = Int_t ( xint / fDx );
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                     // Back side
      // Project point along backside strip to y=0
      //AZ Double_t xp = xint + yint * TMath::Tan(fStereoB);
      Double_t xp = xint + yint * TMath::Tan(fStereoB) + fBackStripShift; //AZ
      // Digitise
      iChan = Int_t ( xp / fDx );
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  // Case strip sensor 2 (with double metal layer)
  else if ( fType == 3 ) {

    if ( iSide == 0 ) {      // Front side
      Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
      xf = xf - TMath::Floor(xf/fLx) * fLx;
      iChan = xf / fDx;
      if ( iChan < 0 || iChan > fNChannelsFront ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Front channel number " << iChan << " exceeds limit " 
	     << fNChannelsFront << endl;
	Fatal("GetChannel", "illegal channel number");
      }
    }
    else {                   // Back side

      if (fStereoB*180/TMath::Pi()>80) {
        Double_t xp = yint;
        xp = xp - TMath::Floor(xp/fLy) * fLy;
        iChan = xp / fDy;
      }
      else {
	Double_t xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
        xp = xp - TMath::Floor(xp/fLx) * fLx;
        iChan = xp / fDx;
      }
      
      if ( iChan < 0 || iChan > fNChannelsBack ) {
	cout << "-E- CbmStsSensor::GetChannel: "
	     << "Back channel number " << iChan << " exceeds limit " 
	     << fNChannelsBack << endl;
	cout << "    Sensor " << GetSensorNr() << " of station " 
	     << GetStationNr() << endl;
	cout << "    Point coordinates: (" << x << ", " << y 
	     << ") cm" << endl;	
	Fatal("GetChannel", "illegal channel number");
      }
    }
  }

  return iChan;
}
// -------------------------------------------------------------------------

    
// -----   Public method GetFrontChannel   ---------------------------------
//Int_t CbmStsSensor::GetFrontChannel(Double_t x, Double_t y, Double_t z) {
Int_t CbmStsSensor::GetFrontChannel(Double_t x, Double_t y, Double_t z, Double_t &dPitch) { //AZ
  //cout << "frontchannel for " << x << " " << y << " " << z << " (" << fZ0-fD/2. << " " << fZ0+fD/2.<<")" << endl;
  z = z - fZ0 + fD/2.;
  //cout << " temp z = " << z << endl;
  if ( z > fD ) return -1;
  if ( z < 0. ) return -1;

  // Lorentz shift due to movement of the charges in the magnetic field
  // translates into an angle: 8.5 deg for electrons
  x += fFrontLorentzShift*z;

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = 0;

//  xint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);
//  yint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);
  
  Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
  //AZ xf = xf - TMath::Floor(xf/fLx) * fLx;
  if (fType != 2) xf = xf - TMath::Floor(xf/fLx) * fLx;
  
  //AZ iChan = (Int_t)(xf/fDx);
  iChan = TMath::Nint(xf/fDx); //AZ
 
  if ( iChan < 0 || iChan > fNChannelsFront ) return -1;
  dPitch = xf - iChan * fDx; //AZ
  return iChan;
}
// -------------------------------------------------------------------------

    
// -----   Public method GetBackChannel   ----------------------------------
//Int_t CbmStsSensor::GetBackChannel (Double_t x, Double_t y, Double_t z) {
Int_t CbmStsSensor::GetBackChannel (Double_t x, Double_t y, Double_t z, Double_t &dPitch) {

  z = fZ0 + fD/2. - z;
  if ( z > fD ) return -1;
  if ( z < 0. ) return -1;

  // Lorentz shift due to movement of the charges in the magnetic field
  // translates into an angle: 1.5 deg for electrons
  x += fBackLorentzShift*z;

  // Calculate internal coordinates. Return -1 if outside sensor.
  Double_t xint  = 0;
  Double_t yint  = 0;
  Double_t xp = 0;
  if ( ! IntCoord(x, y, xint, yint) ) return -1;
  Int_t    iChan = 0;

//  xint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);
//  yint += gRandom->Gaus(0.,fXSmearWidth+fZSmearSlope*z);
  if (fStereoB*180/TMath::Pi()>80) {
    xp = yint;
    xp = xp - TMath::Floor(xp/fLy) * fLy;
    iChan = (Int_t)(xp/fDy);
  }
  else {
    xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
    //AZ xp = xp - TMath::Floor(xp/fLx) * fLx;
    if (fType != 2) xp = xp - TMath::Floor(xp/fLx) * fLx;
    //AZ iChan = (Int_t)(xp/fDx);
    //AZ iChan = (Int_t)(xp/fDy);
    iChan = TMath::Nint(xp/fDy); //AZ
  }
  

  //if (fZ0 < 25) cout << "  " << xint << " " << yint << " -> " << xp << " -> " << iChan << " " << fNChannelsBack << endl;
  if ( iChan < 0 || iChan > fNChannelsBack ) return -1;
  //AZ dPitch = xp - iChan * fDx; //AZ
  dPitch = xp - iChan * fDy; //AZ
  return iChan;
}
// -------------------------------------------------------------------------


// -----   Public method Inside   ------------------------------------------
Bool_t CbmStsSensor::Inside(Double_t x, Double_t y) {
  Double_t xint, yint;
  return IntCoord(x, y, xint, yint);
}
// -------------------------------------------------------------------------


// -----   Public method ActivateChannels   --------------------------------
Bool_t CbmStsSensor::ActivateChannels(Int_t ipt, 
					   Double_t x, Double_t y) {

  Int_t iFront = FrontStripNumber(x, y);
  if (iFront < 0) return kFALSE;
  Int_t iBack  = BackStripNumber(x, y);
  if (iBack < 0) {
    cout << "-W- CbmStsSensor::ActivateChannels: "
	 << "No back strip number!" << endl;
    cout << "    Coordinates: (" << x << ", " << y << ")" << endl;
    cout << "    Sensor: " << fDetectorId << ", Front Strip: " 
	 << iFront << endl;
    return kFALSE;
  }

  fFrontActive.insert(iFront);
  fBackActive.insert(iBack);
  pair<Int_t,Int_t> a(iFront,iBack);
  fTrueHits[a] = ipt;

  return kTRUE;
}
// -------------------------------------------------------------------------

  


// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSensor::Intersect(Int_t iFStrip, Int_t iBStrip,
				    vector<Double_t>& xCross, 
				    vector<Double_t>& yCross) {

  // Reset STL vector of intersection y coordinates
  xCross.clear();
  yCross.clear();

  // Check for strip numbers
  if ( iFStrip < 0 || iFStrip > fNChannelsFront) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid front channel number ! "
	 << iFStrip << " " << fNChannelsFront << endl;
    return 0;
  }
  if ( iBStrip < 0 || iBStrip > fNChannelsBack) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid back channel number ! "
	 << iBStrip << " " << fNChannelsBack << endl;
    return 0;
  }

  // x coordinate of centre of front side strip readout pad
  Double_t xF = Double_t( iFStrip + 0.5 ) * fDx;

  // x coordinate of centre of back side strip readout pad
  Double_t xB = Double_t( iBStrip + 0.5 ) * fDx;
  
  Double_t x;
  Double_t y;
  // Maximal number of back & front strip segments
  Int_t nSegB = Int_t ( fLy * TMath::Tan(fStereoB) / fLx ) + 2;
  Int_t nSegF = Int_t ( fLy * TMath::Tan(fStereoF) / fLx ) + 2;
  
  for (Int_t iSegB=0; iSegB<nSegB; iSegB++) {
     
    for (Int_t iSegF=0; iSegF<nSegF; iSegF++) {
     
      x = ((-1./TMath::Tan(fStereoB)) * (xB -  Double_t(iSegB)*fLx) + ((1./TMath::Tan(fStereoF)) * (xF - Double_t(iSegF)*fLx)))/((1./TMath::Tan(fStereoF))-(1./TMath::Tan(fStereoB)));
      y = (-1./TMath::Tan(fStereoB)) * x + (1./TMath::Tan(fStereoB)) * (xB - fBackStripShift + Double_t(iSegB)*fLx);
      // y & x coordinate of intersection of back strip segment with front strip
      //Double_t y = (xB - x - Double_t(iSegB) * fLx) / TMath::Tan(fStereoB);
    
      if ( y < 0. || y > fLy ) continue;
      if ( x < 0. || x > fLx ) continue;

      // Transform x and y coordinates to the global c.s.
      Double_t xHit = x * TMath::Cos(fRotation) - y * TMath::Sin(fRotation);
      Double_t yHit = y * TMath::Cos(fRotation) + x * TMath::Sin(fRotation);

      // Fill coordinates in return arrays
      xCross.push_back(xHit);
      yCross.push_back(yHit);
    }
  }

  return xCross.size();

}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSensor::Intersect(Int_t iFStrip, Int_t iBStrip, 
			      Double_t& xCross, Double_t& yCross, Double_t& zCross) {

  // Check for strip numbers
  if ( iFStrip < 0 || iFStrip > fNChannelsFront) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid front channel number ! "
	 << iFStrip << " " << fNChannelsFront << endl;
    return -1;
  }
  if ( iBStrip < 0 || iBStrip > fNChannelsBack) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid back channel number ! "
	 << iBStrip << " " << fNChannelsBack << endl;
    return -1;
  }
  Double_t xPoint = xCross, yPoint = yCross; //AZ
  xCross = 0.;
  yCross = 0.;
  zCross = 0.;

  // x coordinate of centre of front side strip readout pad
  Double_t xint = ( Double_t(iFStrip) + 0.5 ) * fDx;            
  
  Double_t sinrot = TMath::Sin(fRotation);
  Double_t cosrot = TMath::Cos(fRotation);
  Double_t tanstrB = 0.0, sterB = fStereoB * TMath::RadToDeg();
  if (!(TMath::Abs(sterB-90.) < 1 || TMath::Abs(sterB+90.) < 1)) tanstrB = TMath::Tan(fStereoB);
  Double_t tanstrF = TMath::Tan(fStereoF);
  Int_t nStripMaxB = ( fStereoB<0. ? 0 :  Int_t(fLy*(tanstrB)/fLx)+1 ); // max. number of strips
  Int_t nStripBegB = ( fStereoB>0. ? 0 : -Int_t(fLy*(tanstrB)/fLx)-1 );
  
  Int_t nStripMaxF = ( fStereoF<=0. ? 0 :  Int_t(fLy*(tanstrF)/fLx)+1 ); // max. number of strips
  Int_t nStripBegF = ( fStereoF>0. ? 0 : -Int_t(fLy*(tanstrF)/fLx)-1 );

  Double_t x0 = ( Double_t(iBStrip) + 0.5 ) * fDx;
  
  Double_t yint;
  Double_t x;
 
  Double_t xtemp, ytemp;

  //AZ Y and X strips (0 and 90 degs)
  if (tanstrF==0. && tanstrB == 0.0) {
    yint = ( Double_t(iBStrip) + 0.5 ) * fDy;            

    // Translation to centre of sector
    xtemp = xint - fLx/2.;
    ytemp = yint - fLy/2.;
    
    // Rotation around sector centre
    xCross = xtemp * cosrot - ytemp * sinrot;
    yCross = xtemp * sinrot + ytemp * cosrot;
    
    // Translation into global c.s.
    xCross = xCross + fX0;
    yCross = yCross + fY0;
    zCross = fZ0;
    
  } else if (tanstrF==0.) {
       
    for (Int_t iStrip=nStripBegB; iStrip<=nStripMaxB; iStrip++) {       
         
      yint = ( x0 - xint - fBackStripShift + Double_t(iStrip) * fLx ) / tanstrB;

      if ( ! ( yint>0. && yint<fLy ) ) continue;

      if ( zCross > 0.001 ) {
        Fatal("Intersect","Intersection of two strips in two different points not valid");
        return -1;
      }

      // Translation to centre of sector
      xtemp = xint - fLx/2.;
      ytemp = yint - fLy/2.;
    
      // Rotation around sector centre
      xCross = xtemp * cosrot - ytemp * sinrot;
      yCross = xtemp * sinrot + ytemp * cosrot;
    
      // Translation into global c.s.
      xCross = xCross + fX0;
      yCross = yCross + fY0;
      zCross = fZ0;
    }
  }
  else {
   
    for (Int_t iStripB=0; iStripB<=1; iStripB++) {
     
      for (Int_t iStripF=-1; iStripF<=0; iStripF++) {
    
        x = ((-1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx) + (1./tanstrF) * (xint - fFrontStripShift +  Double_t(iStripF)*fLx))/((1./tanstrF)-(1./tanstrB));
        yint = (-1./tanstrB) * x + (1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx);
    
        if ( ! ( yint>0. && yint<fLy ) ) continue;
        if ( ! ( x>0. && x<fLx ) ) continue;
     
        if ( zCross > 0.001 ) {
          //AZ Fatal("Intersect","1 Intersection of two strips in two different points not valid");
          //AZ return -1;
          Warning("Intersect","1 Intersection of two strips in two different points not valid");
        }

        // Translation to centre of sector
        xtemp = x - fLx/2.;
        ytemp = yint - fLy/2.;
    
        // Rotation around sector centre
        //AZ xCross = xtemp * cosrot - ytemp * sinrot;
        //AZ yCross = xtemp * sinrot + ytemp * cosrot;
        Double_t xCross1 = xtemp * cosrot - ytemp * sinrot;
        Double_t yCross1 = xtemp * sinrot + ytemp * cosrot;
    
        // Translation into global c.s.
        //AZ xCross = xCross + fX0;
        //AZ yCross = yCross + fY0;
        xCross1 += fX0;
        yCross1 += fY0;
        zCross = fZ0;

	//AZ - pixel simulation
	if (TMath::Abs(TMath::Abs(fStereoF-fStereoB)*TMath::RadToDeg()-90) < 0.1) {
	  if (TMath::Abs(xCross1-xPoint) > 0.5 || TMath::Abs(yCross1-yPoint) > 0.5) continue;
	} //
	xCross = xCross1; //AZ
	yCross = yCross1; //AZ
      } 
    }
  }
  // No intersection found
  if ( zCross < 0.001 ) return -1;

  return fDetectorId;
}
// -------------------------------------------------------------------------

// -----   Public method Intersect   ---------------------------------------
Int_t CbmStsSensor::IntersectClusters(Double_t fChan, Double_t bChan,
				      Double_t& xCross, Double_t& yCross, Double_t& zCross) {

  // Check for strip numbers
  if ( fChan < 0 || fChan > fNChannelsFront) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid front channel number ! "
	 << fChan << " " << fNChannelsFront << endl;
    return -1;
  }
  if ( bChan < 0 || bChan > fNChannelsBack) {
    cout << "-W- CbmStsSensor::Intersect: "
	 << "Invalid back channel number ! "
	 << bChan << " " << fNChannelsBack << endl;
    return -1;
  }
  
  xCross = 0.;
  yCross = 0.;
  zCross = 0.;
  
  Double_t sinrot = TMath::Sin(fRotation);
  Double_t cosrot = TMath::Cos(fRotation);
           
  //AZ Double_t xint  = ( fChan + 0.5 ) * fDx - cosrot*fFrontLorentzShift*fD/2.;
  Double_t xint  = ( fChan + 0.0 ) * fDx - cosrot*fFrontLorentzShift*fD/2.;
  
  //AZ Double_t x0  = ( bChan + 0.5 ) * fDx - cosrot*fBackLorentzShift*fD/2. ;
  //AZ Double_t x0  = ( bChan + 0.5 ) * fDy - cosrot*fBackLorentzShift*fD/2. ;
  Double_t x0  = ( bChan + 0.0 ) * fDy - cosrot*fBackLorentzShift*fD/2. ;

  Double_t yint;
  Double_t x;

  Double_t xtemp, ytemp;
  
  if (fStereoF==0.&&fStereoB*180/TMath::Pi()<80) {
    //exit(0); //AZ-220322
    Double_t tanstrB = TMath::Tan(fStereoB);
    Double_t tanstrF = TMath::Tan(fStereoF);
    Int_t nStripMaxB = ( fStereoB<0. ? 0 :  Int_t(fLy*tanstrB/fLx)+1 ); // max. number of BSts
    Int_t nStripBegB = ( fStereoB>0. ? 0 : -Int_t(fLy*tanstrB/fLx)-1 );
  
    Int_t nStripMaxF = ( fStereoF<=0. ? 0 :  Int_t(fLy*tanstrF/fLx)+1 ); // max. number of Fstrips
    Int_t nStripBegF = ( fStereoF>0. ? 0 : -Int_t(fLy*tanstrF/fLx)-1 );
    if (fType == 2) nStripBegB = nStripMaxB = 0; //AZ

    for (Int_t iStrip=nStripBegB; iStrip<=nStripMaxB; iStrip++) {
	
      yint = ( x0 - xint - fBackStripShift + Double_t(iStrip) * fLx ) / tanstrB;

      if ( ! ( yint>0. && yint<fLy ) ) continue;

      if ( zCross > 0.001 ) {
        Fatal("Intersect","Intersection of two strips in two different points not valid");
        return -1;
      }

      // Translation to centre of sector
      xtemp = xint - fLx/2.;
      ytemp = yint - fLy/2.;
    
      // Rotation around sector centre
      xCross = xtemp * cosrot - ytemp * sinrot;
      yCross = xtemp * sinrot + ytemp * cosrot;
    
      // Translation into global c.s.
      xCross = xCross + fX0;
      yCross = yCross + fY0;
      zCross = fZ0;
    }
  }
  else if (fStereoB*180/TMath::Pi()>80) {
    yint = ( bChan + 0.5 ) * fDy;

    if ( zCross > 0.001 ) {
      Fatal("Intersect","Intersection of two strips in two different points not valid");
      return -1;
    }

    // Translation to centre of sector
    xtemp = xint - fLx/2.;
    ytemp = yint - fLy/2.;
    
    // Rotation around sector centre
    xCross = xtemp * cosrot - ytemp * sinrot;
    yCross = xtemp * sinrot + ytemp * cosrot;
    
    // Translation into global c.s.
    xCross = xCross + fX0;
    yCross = yCross + fY0;
    zCross = fZ0;
  }
  else {
    Double_t tanstrB = TMath::Tan(fStereoB);
    Double_t tanstrF = TMath::Tan(fStereoF);
    Int_t nStripMaxB = ( fStereoB<0. ? 0 :  Int_t(fLy*tanstrB/fLx)+1 ); // max. number of Bstrips
    Int_t nStripBegB = ( fStereoB>0. ? 0 : -Int_t(fLy*tanstrB/fLx)-1 );
  
    Int_t nStripMaxF = ( fStereoF<=0. ? 0 :  Int_t(fLy*tanstrF/fLx)+1 ); // max. number of Fstrips
    Int_t nStripBegF = ( fStereoF>0. ? 0 : -Int_t(fLy*tanstrF/fLx)-1 );

    //AZ for (Int_t iStripB=0; iStripB<=1; iStripB++) {
     
    //AZ for (Int_t iStripF=-1; iStripF<=0; iStripF++) {

    //AZ
    if (fType == 2) {
      nStripBegB = nStripMaxB = 0; 
      nStripBegF = nStripMaxF = 0;
    } else { 
      nStripBegB = 0;
      nStripMaxB = 1;
      nStripBegF = -1;
      nStripMaxF = 0;
    }
    for (Int_t iStripB = nStripBegB; iStripB <= nStripMaxB; ++iStripB) {
     
      for (Int_t iStripF = nStripBegF; iStripF <= nStripMaxF; ++iStripF) {
	//AZ
    
        //x = ((-1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx) + (1./tanstrF) * (xint - fFrontStripShift +  Double_t(iStripF)*fLx))/((1./tanstrF)-(1./tanstrB));
        x = ((1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx) + (1./tanstrF) * (xint - fFrontStripShift +  Double_t(iStripF)*fLx))/((1./tanstrF)+(1./tanstrB)); //AZ
        //yint = (-1./tanstrB) * x + (1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx);
        yint = (1./tanstrB) * x + (1./tanstrB) * (x0 - fBackStripShift + Double_t(iStripB)*fLx); //AZ
     
        if ( ! ( yint>0. && yint<fLy ) ) continue;  
        if ( ! ( x>0. && x<fLx ) ) continue;
    
        if ( zCross > 0.001 ) {
          Fatal("Intersect","2 Intersection of two strips in two different points not valid");
          return -1;
        }

        // Translation to centre of sector
        xtemp = x - fLx/2.;
        ytemp = yint - fLy/2.;
    
        // Rotation around sector centre
        xCross = xtemp * cosrot - ytemp * sinrot;
        yCross = xtemp * sinrot + ytemp * cosrot;
    
        // Translation into global c.s.
        xCross = xCross + fX0;
        yCross = yCross + fY0;
        zCross = fZ0;
      }
    }
  }
 
  // No intersection found
  if ( zCross < 0.001 ) return -1;
  
  //AZ - Reject points outside of the sensor boundaries (for complex geometries)
  // AZ!!! - add extra smearing (for tests)
  //Double_t sigmaEx = 0.02; // 200 um
  //xCross += sigmaEx * gRandom->Gaus();
  //
  //AZ 15.02.2019 - add some safety margin around sensor (edge effect)
  Double_t safety = 0.3;
  Double_t glob[3] = {xCross, yCross, zCross}, loc[3] = {0};
  gGeoManager->cd(fName);
  gGeoManager->MasterToLocal(glob, loc);
  //cout << loc[0] << " " << loc[1] << " " << fLx << " " << fLy << endl;
  if (TMath::Abs(loc[0])-fLx/2 > safety || TMath::Abs(loc[1])-fLy/2 > safety) return -1;
  //*
  if (fLx > 20.0 && abs(loc[0]) < fLx/2 && abs(loc[1]) < fLy/2) {
    // GEMs - inner boundaries of hot and cold zones
    TString where = gGeoManager->FindNode(xCross+TMath::Sign(safety,fX0), yCross+TMath::Sign(safety,fY0), zCross)->GetName();
    if (!where.Contains("sens",TString::kIgnoreCase)) return -1;
    if (!fName.Contains(where)) {
      if (fName.Contains("sens",TString::kIgnoreCase) && !fName.Contains(where)) return -1; // GEMS
    }
  }
  //*/
  //AZ-180322 - Exclude beam hole for BM@N "virtual" hot zones
  if (fName.Contains("Sensor_hot")) {
    gGeoManager->cd(fName);
    gGeoManager->CdUp();
    TString where = gGeoManager->FindNode(xCross+TMath::Sign(safety,fX0), yCross+TMath::Sign(safety,fY0), zCross)->GetName();
    //cout << " WHERE: " << where << " " << fName << endl;
    if (!where.Contains("hot")) return -1;
  }
  return fDetectorId;
}

// -------------------------------------------------------------------------


// -----   Public method PointIndex   --------------------------------------
Int_t CbmStsSensor::PointIndex(Int_t iFStrip, Int_t iBStrip) {
  pair<Int_t,Int_t> a(iFStrip, iBStrip);
  if (fTrueHits.find(a) == fTrueHits.end()) return -1;
  return fTrueHits[a];
}
// -------------------------------------------------------------------------



// -----   Public method Reset   -------------------------------------------
void CbmStsSensor::Reset() {
  fFrontActive.clear();
  fBackActive.clear();
  fTrueHits.clear();
}
// -------------------------------------------------------------------------



// -----   Public method Print   -------------------------------------------
void CbmStsSensor::Print() {
  cout << "   Sensor Nr. ";
  cout.width(3);
  cout << GetSensorNr() << ", Type ";
  cout.width(1);
  cout << fType << ", centre (";
  cout.width(6);
  cout << fX0 << ", ";
  cout.width(6);
  cout << fY0 << ") cm, rotation " ;
  cout.width(6);
  cout << fRotation*180./TMath::Pi() << " deg., lx = ";
  cout.width(3);
  cout << fLx << " cm, ly = ";
  cout.width(3);
  cout << fLy << " cm, channels: " << GetNChannels() << endl;
}
// -------------------------------------------------------------------------



// -----   Private method FrontStripNumber   -------------------------------
Int_t CbmStsSensor::FrontStripNumber(Double_t x, Double_t y) const {

  Double_t xint = 0., yint = 0.;

  // Calculate internal coordinates. 
  // If point is inside sensor, return strip number
  if ( IntCoord(x, y, xint, yint) ) {
    // Double_t xf = xint + fFrontStripShift + yint * TMath::Tan(fStereoF);
    Double_t xf = xint + fFrontStripShift + yint * fTanF; //AZ
  xf = xf - TMath::Floor(xf/fLx) * fLx;
  Int_t iStrip = Int_t( xf / fDx );
  
  if (iStrip < 0 || iStrip > fNChannelsFront) {
    cout << "-E- CbmStsSensor::FrontStripNumber: Invalid strip number"
	 << "  " << iStrip << endl;
    cout << "    Sensor " << fDetectorId << ", x = " << xint << ", y = " 
	 << yint << endl;
    Fatal("FrontStripNumber", "Invalid strip number" );
  }
  return iStrip;
  }

  // Return -1 if point is outside sensor
  return -1;

}
// -------------------------------------------------------------------------



// -----   Private method BackStripNumber   --------------------------------
Int_t CbmStsSensor::BackStripNumber(Double_t x, Double_t y) const {

  Double_t xint = 0., yint = 0.;

  // Calculate internal coordinates. 
  // If point is inside sensor, calculate projection onto readout line
  // and determine channel number
  if ( IntCoord(x, y, xint, yint) ) {
    if (fStereoB*180/TMath::Pi()>80) {
      Double_t xp = yint;
      xp = xp - TMath::Floor(xp/fLy) * fLy;
      Int_t iStrip = (Int_t) ( xp / fDy );
      if (iStrip < 0 || iStrip > fNChannelsBack) {
        cout << "-E- CbmStsSensor::BackStripNumber: Invalid strip number"
	     << "  " << iStrip << endl;
        cout << "    Sensor " << fDetectorId << ", x = " << xint << ", y = " 
	     << yint << endl;
        Fatal("BackStripNumber", "Invalid strip number" );
      }
      return iStrip;
    }
    else {
      // Project point along backside strip to y = 0 
      //Double_t xp = xint + fBackStripShift + yint * TMath::Tan(fStereoB);
      Double_t xp = xint + fBackStripShift + yint * fTanB; //AZ
      // Calculate modulo w.r.t. sensor x width
      xp = xp - TMath::Floor(xp/fLx) * fLx;
      // Digitise
      Int_t iStrip = (Int_t) ( xp / fDx );
      if (iStrip < 0 || iStrip > fNChannelsBack) {
        cout << "-E- CbmStsSensor::BackStripNumber: Invalid strip number"
	     << "  " << iStrip << endl;
        cout << "    Sensor " << fDetectorId << ", x = " << xint << ", y = " 
	     << yint << endl;
        Fatal("BackStripNumber", "Invalid strip number" );
      }
      return iStrip;
    }
    
   
  }

  // Return -1 if point is outside sensor
  return -1;

}
// -------------------------------------------------------------------------



// -----   Private method IntCoord   ---------------------------------------
Bool_t CbmStsSensor::IntCoord(Double_t x, Double_t y,
			      Double_t& xint, Double_t& yint) const {

  // Translation into sensor centre system
  x = x - fX0;
  y = y - fY0;

  // Rotation around the sensor centre
  //xint = x * TMath::Cos(fRotation) + y * TMath::Sin(fRotation);
  //yint = y * TMath::Cos(fRotation) - x * TMath::Sin(fRotation);
  xint = x * fCosRot + y * fSinRot; //AZ 
  yint = y * fCosRot - x * fSinRot; //AZ

  // Translation into sensor corner system
  xint = xint + fLx/2.;
  yint = yint + fLy/2.;

  // Check whether point is inside the sensor
  if ( ! IsInside(xint, yint) ) {
    xint = yint = 0.;
    return kFALSE;
  }

  return kTRUE;

}
// -------------------------------------------------------------------------


// -----   Private method IntCoord   ---------------------------------------
Bool_t CbmStsSensor::IntCoord(Double_t x, Double_t y, Double_t z,
			      Double_t& xint, Double_t& yint, Double_t& zint) const {

  // Translation into sensor centre system
  x = x - fX0;
  y = y - fY0;

  // Rotation around the sensor centre
  xint = x * TMath::Cos(fRotation) + y * TMath::Sin(fRotation);
  yint = y * TMath::Cos(fRotation) - x * TMath::Sin(fRotation);

  // Translation into sensor corner system
  xint = xint + fLx/2.;
  yint = yint + fLy/2.;

  // Check whether point is inside the sensor
  if ( ! IsInside(xint, yint) ) {
    xint = yint = 0.;
    return kFALSE;
  }

  return kTRUE;

}
// -------------------------------------------------------------------------



// -----   Private method IsInside   ---------------------------------------
Bool_t CbmStsSensor::IsInside(Double_t xint, Double_t yint) const {
  if ( xint < 0.  ) return kFALSE;
  if ( xint > fLx ) return kFALSE;
  if ( yint < 0.  ) return kFALSE;
  if ( yint > fLy ) return kFALSE;
  return kTRUE;
}
// -------------------------------------------------------------------------




ClassImp(CbmStsSensor)
