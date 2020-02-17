// -------------------------------------------------------------------------
// -----                     CbmStsSensor header file                  -----
// -----                  Created 01/07/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------

/** CbmStsSensor.h
 *@author V.Friese <v.friese@gsi.de>
 *@since 28.08.06
 *@version 1.0
 **
 ** This class describes the digitisation scheme for a sensor of the STS.
 ** The sensor is rectangular with extensions fLx and fLy. The following
 ** types are implemented:
 ** 1 = pixel detector without charge sharing
 ** 2 = double sides strip detector, all channels read out
 ** 3 = double sided strip detector with double metal layer,
 **     readout at the bottom corner
 ** In case of strip detectors, the internal coordinate system sees
 ** x across and y along the front side strips.
 ** The origin of the internal sensor coordinate system is at the lower 
 ** left corner of the sensor, i.e. x runs from 0 to fLx, 
 ** and y from 0 to fLy. 
 ** For strip detectors, the readout pitch is the same on the front and 
 ** the back side, i.e. the strip pitch on the back side is smaller.
 ** The sensor can be translated and rotated in the c.s. of the station. 
 **/


#ifndef CBMSTSSENSOR_H
#define CBMSTSSENSOR_H 1

#include "TNamed.h"

#include <map>
#include <list>
#include <set>
#include <vector>

#include "TGeoManager.h" //GP
#include "TGeoNode.h"//GP
#include "TGeoVolume.h"//GP

class CbmStsSensorDigiPar;


class CbmStsSensor : public TNamed
{

 public:

  /** Default constructor **/
  CbmStsSensor();


  /** Standard constructor
   *@param detId     Unique detector identifier
   *@param iType     Sensor type (1,2,3)
   *@param x0        sensor centre x coordinate in global c.s [cm]
   *@param y0        sensor centre y coordinate in global c.s [cm]
   *@param rotation  rotation in global c.s. [rad]
   *@param lx        sensor length in x [cm]
   *@param ly        sensor length in y [cm]
   *@param dx        Readout pitch (strip) or pixel width in x [cm]
   *@param dy        Pixel width in y [cm].
   *@param stereoF   StereoF angle [rad] (for double sided strip)
   *@param stereoB   StereoB angle [rad] (for double sided strip)
   **/
  CbmStsSensor(Int_t detId, Int_t iType, Double_t x0, Double_t y0,
	       Double_t rotation, Double_t lx, Double_t ly, 
	       Double_t dx, Double_t dy, Double_t stereoF, Double_t stereoB);
  CbmStsSensor(TString tempName, Int_t detId, Int_t iType, Double_t x0, Double_t y0, Double_t z0,
	       Double_t rotation, Double_t lx, Double_t ly, 
	       Double_t d, Double_t dx, Double_t dy, Double_t stereoF, Double_t stereoB);
  

  /** Destructor **/
  virtual ~CbmStsSensor();

  /** Accessors **/
  TString GetDetectorName() const { return fName.Data(); }
  Int_t    GetDetectorId() const { 
    return fDetectorId; }
  Int_t    GetSystemId()   const {  
    return ( fDetectorId & (15<<24) ) >> 24; }
  Int_t    GetStationNr()  const { 
    return ( fDetectorId & (255<<16) ) >> 16; }
  Int_t    GetSectorNr()   const {  // sector number within station
    return ( fDetectorId & (4095<<4) ) >> 4; }
  /*   Int_t    GetSensorNr()   const {  // sensor number within sector 
     return ( fDetectorId & (32767<<1) ) >> 1; } */
  Int_t    GetSensorNr()   const {  // sensor number within sector
    return ( fDetectorId & (7<<1) ) >> 1; }
  Int_t    GetType()       const { return fType; }
  Double_t GetX0()         const { return fX0; }
  Double_t GetY0()         const { return fY0; }
  Double_t GetZ0()         const { return fZ0; }
  Double_t GetRotation()   const { return fRotation; }
  Double_t GetLx()         const { return fLx; }
  Double_t GetLy()         const { return fLy; }
  Double_t GetD()          const { return fD;  }
  Double_t GetDx()         const { return fDx; }
  Double_t GetDy()         const { return fDy; }
  Double_t GetStereoF()    const { return fStereoF; }
  Double_t GetStereoB()    const { return fStereoB; }
  Double_t GetSigmaX()     const { return fSigmaX; }
  Double_t GetSigmaY()     const { return fSigmaY; }
  Double_t GetSigmaXY()    const { return fSigmaXY; }
  Int_t    GetNChannels()  const { return fNChannelsFront + fNChannelsBack; }
  Int_t    GetNChannelsFront()  const { return fNChannelsFront; }
  Int_t    GetNChannelsBack ()  const { return fNChannelsBack;  }
  Double_t GetBackStripShift()  const { return fBackStripShift; }
  Double_t GetFrontStripShift() const { return fFrontStripShift; }

  /** Calculate channel number for a coordinate pair.
   *@param x      x coordinate in global c.s. [cm]
   *@param y      y coordinate in global c.s. [cm]
   *@param iSide  0 = front side, 1 = back side
   *@value iChan  channel number. -1 if point is outside sensor.
   ** Will return the same for both sides in case of pixel sensor.
   **/
  Int_t GetChannel(Double_t x, Double_t y, Int_t iSide);

  Float_t GetChannelPlus(Double_t x, Double_t y, Int_t iSide);

 // Int_t GetFrontChannel(Double_t x, Double_t y, Double_t z);
 // Int_t GetBackChannel (Double_t x, Double_t y, Double_t z);
  Int_t GetFrontChannel(Double_t x, Double_t y, Double_t z, Double_t &dPitch); //AZ
  Int_t GetBackChannel (Double_t x, Double_t y, Double_t z, Double_t &dPitch); //AZ

  /** Test whether a coordinate pair (x,y) in global coordinates is
   ** inside the sensor **/
  Bool_t Inside(Double_t x, Double_t y);


  /** Activate the channels corresponding to a MCPoint.
   *@param ipt   Index of MCPoint
   *@param x     x coordinate of point (global c.s.)
   *@param y     y coordinate of point (global c.s.)
   *@value  kTRUE if the point is inside the sensor, else kFALSE
   **/
  Bool_t ActivateChannels(Int_t ipt, Double_t x, Double_t y);


  /** Calculates the coordinates of the intersections of front strip i
   ** with back strip j in the global coordinate system
   *@param iFStrip   Front strip number
   *@param iBStrip   Back strip number
   *@param xCross    Vector of x coordinates of crossings [cm]
   *@param yCross    Vector of y coordinates of crossings [cm]
   *@value           Number of intersections
   **/
  Int_t Intersect(Int_t iFStrip, Int_t iBStrip,
		  std::vector<Double_t>& xCross, std::vector<Double_t>& yCross);

  Int_t Intersect(Int_t iFStrip, Int_t iBStrip, Double_t& xCross, Double_t& yCross, Double_t& zCross);

  Int_t IntersectClusters(Double_t fChan, Double_t bChan, Double_t& xCross, Double_t& yCross, Double_t& zCross);

  /** The index of the MCPoint that has caused a combination of
   ** front and back strip to be fired. Returns -1 for combinations
   ** of strips fired by different points (fake hits) 
   *@param iFStrip   Front strip number
   *@param iBStrip   Back strip number
   *@value Index of MCPoint
   **/
  Int_t PointIndex(Int_t iFStrip, Int_t IBStrip);


  /** Clear the maps of fired strips **/
  void Reset();


  /** Screen output  **/
  void Print();
  


 private:
  TString GeoMaskSi[3][8];
  TString GeoMaskGem[6][4];
  TGeoVolume* TopVolume;
  Double_t PIR = 3.1415926536 / 180.; // Ruf

  /** -------------   Data members   --------------------------**/

  //TString    fName;             // Station name
  Int_t    fDetectorId;   // Unique detector ID
  Int_t    fType;         // Sensor type 
  Double_t fX0, fY0, fZ0; // Coordinates of the sensor centre [cm]
  Double_t fRotation;     // Rotation angle in global c.m. [rad]
  Double_t fLx, fLy;      // Dimensions of the sensor [cm]
  Double_t fDx;           // Strip readout pitch or pixel size in x [cm]
  Double_t fDy;           // Pixel size in y [cm] for pixel sensor
  Double_t fStereoF;      // StereoF angle [rad] for strip sensor.
  Double_t fStereoB;      // StereoB angle [rad] for strip sensor.
  Double_t fTanF,fTanB,fSinRot,fCosRot;
  
  Double_t fD;            // thickness of the sensor [cm]

  /** Number of channels in front and back plane **/
  Int_t fNChannelsFront;
  Int_t fNChannelsBack;

  Double_t fBackStripShift;
  Double_t fFrontStripShift;

  /** Errors of hit coordinates. For pixel sensor: size/sqrt(12),
   ** for strip sensors RMS of overlap of front and back side strip **/
  // Logically, this belongs to the HitFinder, but it is here
  // for performance reasons: to be executed once per sensor, not for
  // each MCPoint
  Float_t fSigmaX;   // RMS in x, global c.s. [cm]
  Double_t fSigmaY;   // RMS in y, global c.s. [cm]
  Double_t fSigmaXY;  // Covariance in global c.s. [cm**2]

  Double_t fXSmearWidth;
  Double_t fZSmearSlope;

  Double_t fFrontLorentzShift;
  Double_t fBackLorentzShift;

  /** STL sets containing the active channels **/
  std::set<Int_t> fFrontActive;       //!
  std::set<Int_t> fBackActive;        //!


  /** STL map from the indizes of the fired strips to the 
   ** index of the MCPoint **/
  std::map<std::pair<Int_t,Int_t>, Int_t > fTrueHits;      //!



  /** -------------   Private methods   ------------------------**/

  /** Strip number of a point in the front plane.
   ** Returns -1 if the point is outside the sensor
   *@param x  x coordinate of point (global c.s.)
   *@param y  y coordinate of point (global c.s.)
   *@value  Number of strip hitted by the point 
   **/
  Int_t FrontStripNumber(Double_t x, Double_t y) const;


  /** Strip number of a point in the back plane.
   ** Returns -1 if the point is outside the sensor
   *@param x  x coordinate of point (global c.s.)
   *@param y  y coordinate of point (global c.s.)
   *@value  Number of strip hitted by the point 
   **/
  Int_t BackStripNumber(Double_t x, Double_t y) const;


  /** Calculate coordinates in internal coordinate system
   *@param x     x coordinate in global system
   *@param y     y coordinate in global system
   *@param xint  internal x coordinate (return)
   *@param yint  internal y coordinate (return)
   *@value kTRUE if point is inside the sensor
   **/
  Bool_t IntCoord(Double_t x, Double_t y, 
		  Double_t& xint, Double_t& yint) const;
  Bool_t IntCoord(Double_t x, Double_t y, Double_t z, 
		  Double_t& xint, Double_t& yint, Double_t& zint) const;

  
  /** Check whether a point is inside the sensor
   *@param xpt  x coordinate of point (internal system)
   *@param ypt  y coordinate of point (internal system)
   *@value kTRUE if inside sensor, else kFALSE
   **/
  Bool_t IsInside(Double_t xint, Double_t yint) const;




  ClassDef(CbmStsSensor,1);

};




#endif
