// -------------------------------------------------------------------------
// -----                 CbmStsSensorDigiPar header file               -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsSensorDigiPar.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Parameter container for the digitisation of a sensor of a STS
 ** strip station.
 ** Supports the following digitisation types:
 ** 1 = hybrid pixel
 ** 2 = strip, readout of all strips
 ** 3 = strip, readout at bottom corner, double metal layer
 ** Coordinates and rot. angle are in the global coordinate system. 
 ** In the internal coordinate system, x is across the front side strips
 ** and y along the front side strips. Irrelevant for pixel detectors.
 ** Angles are stored and accessed in radians, but must be given in
 ** degrees in the constructor.
 **/



#ifndef CBMSTSSENSORDIGIPAR_H
#define CBMSTSSENSORDIGIPAR_H 1



#include "TObject.h"



class CbmStsSensorDigiPar : public TObject
{

 public:

  /** Default constructor **/
  CbmStsSensorDigiPar();


  /** Standard constructor. For the parameters, see description of
   ** private data members. Angles must be given in radians. **/
  CbmStsSensorDigiPar(Int_t iSensor, Int_t iType, 
		      Double_t x0, Double_t y0, 
		      Double_t rotation, 
		      Double_t lx, Double_t ly, 
		      Double_t dx, Double_t dy,
		      Double_t stereoF, Double_t stereoB);
  CbmStsSensorDigiPar(Int_t iSensor, Int_t iType, 
		      Double_t x0, Double_t y0, Double_t z0, 
		      Double_t rotation, 
		      Double_t lx, Double_t ly, 
		      Double_t d, Double_t dx, Double_t dy,
		      Double_t stereoF, Double_t stereoB);


  /** Destructor **/
  virtual ~CbmStsSensorDigiPar();


  /** Accessors. See private data members for description **/
  Int_t    GetSensorNr()   const { return fSensorNr; }
  Int_t    GetType()       const { return fType; }       // 1, 2 or 3
  Double_t GetX0()         const { return fX0; }         // [cm]
  Double_t GetY0()         const { return fY0; }         // [cm]
  Double_t GetZ0()         const { return fZ0; }         // [cm]
  Double_t GetRotation()   const { return fRotation; }   // [rad]
  Double_t GetLx()         const { return fLx; }         // [cm]
  Double_t GetLy()         const { return fLy; }         // [cm]
  Double_t GetD()          const { return fD;  }
  Double_t GetDx()         const { return fDx; }         // [cm]
  Double_t GetDy()         const { return fDy; }         // [cm]
  Double_t GetStereoF()    const { return fStereoF; }    // [rad]
  Double_t GetStereoB()    const { return fStereoB; }    // [rad]
  void SetZ0(Double_t z0)        { fZ0 = z0; }         // [cm]
  void SetD (Double_t d)         { fD  = d;  }         // [cm]


 private:

  /** Sensor number in station **/	
  Int_t    fSensorNr;

  /** Type of sector.
   ** 1 = hybrid pixel sensor, no charge sharing;
   ** 2 = strip sensor, readout of all strips on the back side
   ** 3 = strip sensor, readout at bottom corner, double metal layer **/
  Int_t    fType;

  /** x coordinate of sector centre in the station c.s. [cm] **/
  Double_t fX0;

  /** y coordinate of sector centre in the station c.s. [cm] **/
  Double_t fY0;

  /** z coordinate of sector centre in the global c.s. [cm] **/
  Double_t fZ0;

  /** Rotation angle around z axis in the station c.s [rad] **/
  Double_t fRotation;

  /** Length of sector along x axis [cm] **/
  Double_t fLx;

  /** Length of sector along y axis [cm] **/
  Double_t fLy;

  /** Strip readout pitch or pixel size along x [cm] **/ 
  Double_t fDx;

  /** Pixel size along y [cm]. Irrelevant for strip sensor. **/
  Double_t fDy;

  /** StereoB angle of back side strips. Irrelevant for pixel sensors. **/
  Double_t fStereoF;

  /** StereoF angle of front side strips. Irrelevant for pixel sensors. **/
  Double_t fStereoB;
  
  Double_t fD;            // thickness of the sector [cm]

  ClassDef(CbmStsSensorDigiPar,1);

};


#endif
