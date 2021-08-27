// -------------------------------------------------------------------------
// -----                     CbmStsSector header file                  -----
// -----                  Created 28/08/06  by V. Friese               -----
// -------------------------------------------------------------------------

/** CbmStsSector.h
 *@author V.Friese <v.friese@gsi.de>
 *@since 28.08.06
 *@version 1.0
 **
 ** This class describes the digitisation scheme for a sector of the STS.
 ** The sector is rectangular with extensions fLx and fLy. The following
 ** types are implemented:
 ** 1 = pixel detector without charge sharing
 ** 2 = double sides strip detector, all channels read out
 ** 3 = double sided strip detector with double metal layer,
 **     readout at the bottom corner
 ** In case of strip detectors, the internal coordinate system sees
 ** x across and y along the front side strips.
 ** The origin of the internal sector coordinate system is at the lower 
 ** left corner of the sector, i.e. x runs from 0 to fLx, 
 ** and y from 0 to fLy. 
 ** For strip detectors, the readout pitch is the same on the front and 
 ** the back side, i.e. the strip pitch on the back side is smaller.
 ** The sector can be translated and rotated in the c.s. of the station. 
 **/


#ifndef CBMSTSSECTOR_H
#define CBMSTSSECTOR_H 1

#include "TObjArray.h"
#include "TNamed.h"
#include "TString.h"

#include <map>
#include <list>
#include <set>
#include <vector>

class CbmStsSensor;
class CbmStsSensorDigiPar;

class CbmStsSector : public TNamed
{

 public:

  /** Default constructor **/
  CbmStsSector();


  /** Standard constructor
   *@param detId     Unique detector identifier
   *@param iType     Sensor type (1,2,3)
   *@param x0        sector centre x coordinate in global c.s [cm]
   *@param y0        sector centre y coordinate in global c.s [cm]
   *@param rotation  rotation in global c.s. [rad]
   *@param lx        sector length in x [cm]
   *@param ly        sector length in y [cm]
   *@param dx        Readout pitch (strip) or pixel width in x [cm]
   *@param dy        Pixel width in y [cm].
   *@param stereoF   StereoF angle [rad] (for double sided strip)
   *@param stereoB   StereoB angle [rad] (for double sided strip)
   **/
  CbmStsSector(TString tempName, Int_t detId);

  /** Destructor **/
  virtual ~CbmStsSector();


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
  Int_t    GetNChannels()  const { return fNChannelsFront + fNChannelsBack; }
  Int_t    GetNChannelsFront()  const { return fNChannelsFront; }
  Int_t    GetNChannelsBack ()  const { return fNChannelsBack;  }

  Int_t    GetNSensors()   const { return fSensors->GetEntriesFast(); }
  TObjArray* GetSensors() { return fSensors; }

  CbmStsSensor* GetSensor(Int_t iSensor) { 
    return (CbmStsSensor*) fSensors->At(iSensor); }
  CbmStsSensor* GetSensorByNr(Int_t sensorNr);

  void AddSensor(CbmStsSensorDigiPar* sensorPar);
  void AddSensor(CbmStsSensor* sensor);

  // Retrieve from sensor
  Int_t    GetType()       const { return fType; }
  Double_t GetRotation()   const { return fRotation; }
  Double_t GetDx()         const { return fDx; }
  Double_t GetDy()         const { return fDy; }
  Double_t GetStereoF()    const { return fStereoF; }
  Double_t GetStereoB()    const { return fStereoB; }
  Double_t GetSigmaX()     const { return fSigmaX; }
  Double_t GetSigmaY()     const { return fSigmaY; }
  Double_t GetSigmaXY()    const { return fSigmaXY; }

  /** The index of the MCPoint that has caused a combination of
   ** front and back strip to be fired. Returns -1 for combinations
   ** of strips fired by different points (fake hits) 
   *@param iFStrip   Front strip number
   *@param iBStrip   Back strip number
   *@value Index of MCPoint
   **/
  Int_t PointIndex(Int_t iFStrip, Int_t IBStrip);

  Int_t Intersect(Int_t iFStrip, Int_t iBStrip, Double_t& xCross, Double_t& yCross, Double_t& zCross);
  Int_t IntersectClusters(Double_t fChan, Double_t bChan, Double_t& xCross, Double_t& yCross, Double_t& zCross);

  /** Clear the maps of fired strips **/
  void Reset();


  /** Screen output  **/
  void Print();
  


 private:

  /** -------------   Data members   --------------------------**/

  Int_t    fDetectorId;   // Unique detector ID

  TObjArray* fSensors;          // Array of CbmStsSensors

  Int_t    fType;         // Sensor type 
  Double_t fRotation;     // Rotation angle in global c.m. [rad]
  Double_t fDx;           // Strip readout pitch or pixel size in x [cm]
  Double_t fDy;           // Pixel size in y [cm] for pixel sensor
  Double_t fStereoF;      // StereoB angle [rad] for strip sensor.
  Double_t fStereoB;      // StereoF angle [rad] for strip sensor.

  /** Errors of hit coordinates. For pixel sensor: size/sqrt(12),
   ** for strip sensors RMS of overlap of front and back side strip **/
  // Logically, this belongs to the HitFinder, but it is here
  // for performance reasons: to be executed once per sensor, not for
  // each MCPoint
  Double_t fSigmaX;   // RMS in x, global c.s. [cm]
  Double_t fSigmaY;   // RMS in y, global c.s. [cm]
  Double_t fSigmaXY;  // Covariance in global c.s. [cm**2]

  /** Number of channels in front and back plane **/
  Int_t fNChannelsFront;
  Int_t fNChannelsBack;

  /** STL sets containing the active channels **/
  std::set<Int_t> fFrontActive;       //!
  std::set<Int_t> fBackActive;        //!

  std::map<Int_t, Int_t> fSensorMap; //! Map from sensor number to index

  /** STL map from the indizes of the fired strips to the 
   ** index of the MCPoint **/
  std::map<std::pair<Int_t,Int_t>, Int_t > fTrueHits;      //!

  CbmStsSector(const CbmStsSector&);
  CbmStsSector operator=(const CbmStsSector&);


  ClassDef(CbmStsSector,1);

};




#endif
