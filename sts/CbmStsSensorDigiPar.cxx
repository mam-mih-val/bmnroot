// -------------------------------------------------------------------------
// -----                 CbmStsSensorDigiPar source file               -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


#include <iostream>

#include "TMath.h"

#include "CbmStsSensorDigiPar.h"

using std::cout;
using std::endl;



// -----   Default constructor   -------------------------------------------
CbmStsSensorDigiPar::CbmStsSensorDigiPar() 
: fSensorNr(-1),
  fType    (0),
  fX0(0.),
  fY0(0.),
  fZ0(0.),
  fRotation(0.),
  fLx(0.),
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fD(0.),
  fStereoF(0.),
  fStereoB(0.)

{
 
  cout << "-W- CbmStsSensorDigiPar: Do not use this constructor! " 
       << endl;
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
CbmStsSensorDigiPar::CbmStsSensorDigiPar(Int_t iSensor, Int_t iType,
					 Double_t x0, Double_t y0,
					 Double_t rotation, 
					 Double_t lx, Double_t ly, 
					 Double_t dx, Double_t dy,
					 Double_t stereoF, Double_t stereoB) 
: fSensorNr(-1),
  fType    (0),
  fX0(0.),
  fY0(0.),
  fZ0(0.),
  fRotation(0.),
  fLx(0.),
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fD(0.),
  fStereoF(0.),
  fStereoB(0.)  
{

  fSensorNr = iSensor;
  fType     = iType;
  fX0       = x0;
  fY0       = y0;
  fRotation = rotation;
  fLx       = lx;
  fLy       = ly;
  fDx       = dx;
  fDy       = dy;
  fStereoF  = stereoF;
  fStereoB  = stereoB;

  // Check for stereoB angle
  // (Mathematics has not been checked for angles > 90 degrees)
  if (TMath::Abs(fStereoB) > TMath::Pi()/2.) {
    cout << "-E- CbmStsSensorDigiPar: StereoB angle > 90 degrees !" 
	 << endl;
    cout << iSensor << " " << iType << " " << x0 << " " << y0 << " "
	 << rotation << " " << lx << " " << ly << " " << dx << " "
	 << dy << " " << stereoB << endl;
    cout << "                       Revise sector layout! " << endl;
    Fatal("CbmStsSensorDigiPar", "StereoB Angle");
  }
  // Check for stereoF angle
  // (Mathematics has not been checked for angles > 90 degrees)
  if (TMath::Abs(fStereoF) > TMath::Pi()/2.) {
    cout << "-E- CbmStsSensorDigiPar: StereoF angle > 90 degrees !" 
	 << endl;
    cout << iSensor << " " << iType << " " << x0 << " " << y0 << " "
	 << rotation << " " << lx << " " << ly << " " << dx << " "
	 << dy << " " << stereoF << endl;
    cout << "                       Revise sector layout! " << endl;
    Fatal("CbmStsSensorDigiPar", "StereoF Angle");
  }
}
// -------------------------------------------------------------------------

CbmStsSensorDigiPar::CbmStsSensorDigiPar(Int_t iSensor, Int_t iType,
					 Double_t x0, Double_t y0, Double_t z0,
					 Double_t rotation, 
					 Double_t lx, Double_t ly, 
					 Double_t d, Double_t dx, Double_t dy,
					 Double_t stereoF, Double_t stereoB) 
: fSensorNr(-1),
  fType    (0),
  fX0(0.),
  fY0(0.),
  fZ0(0.),
  fRotation(0.),
  fLx(0.),
  fLy(0.),
  fDx(0.),
  fDy(0.),
  fD(0.),
  fStereoF(0.),
  fStereoB(0.)  
{

  fSensorNr = iSensor;
  fType     = iType;
  fX0       = x0;
  fY0       = y0;
  fZ0       = z0;
  fRotation = rotation;
  fLx       = lx;
  fLy       = ly;
  fD        = d;
  fDx       = dx;
  fDy       = dy;
  fStereoF  = stereoF;
  fStereoB  = stereoB;

  // Check for stereoB angle
  // (Mathematics has not been checked for angles > 90 degrees)
  if (TMath::Abs(fStereoB) > TMath::Pi()/2.) {
    cout << "-E- CbmStsSensorDigiPar: StereoB angle > 90 degrees !" 
	 << endl;
    cout << iSensor << " " << iType << " " << x0 << " " << y0 << " " << z0 << " "
	 << rotation << " " << lx << " " << ly << " " << dx << " "
	 << dy << " " << stereoB << endl;
    cout << "                       Revise sector layout! " << endl;
    Fatal("CbmStsSensorDigiPar", "StereoB Angle");
  }
  // Check for stereoF angle
  // (Mathematics has not been checked for angles > 90 degrees)
  if (TMath::Abs(fStereoF) > TMath::Pi()/2.) {
    cout << "-E- CbmStsSensorDigiPar: StereoF angle > 90 degrees !" 
	 << endl;
    cout << iSensor << " " << iType << " " << x0 << " " << y0 << " " << z0 << " "
	 << rotation << " " << lx << " " << ly << " " << dx << " "
	 << dy << " " << stereoF << endl;
    cout << "                       Revise sector layout! " << endl;
    Fatal("CbmStsSensorDigiPar", "StereoF Angle");
  }
}
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
CbmStsSensorDigiPar::~CbmStsSensorDigiPar() { }
// -------------------------------------------------------------------------



ClassImp(CbmStsSensorDigiPar)
