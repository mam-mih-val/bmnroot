// -------------------------------------------------------------------------
// -----                       CbmStsHit header file                   -----
// -----                  Created 30/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** CbmStsHit
 *@author Volker Friese <v.friese@gsi.de>
 *@since 30.08.06
 *@version 1.1
 **
 ** Data class for a reconstructed hit in the STS
 ** Data level RECO
 ** Derived from CbmHit.
 ** Additional members: Channel numbers and covariance in x and y
 ** In case of pixel sensor backside channel is -1
 **/



#ifndef CBMSTSSHIT_H
#define CBMSTSSHIT_H 1

#include "CbmHit.h"

#include "BmnDetectorList.h"

class TVector3;

class CbmStsHit : public CbmHit
{

 public:

  /** Default constructor (not for use) **/
  CbmStsHit();


  /** Standard constructor
  *@param detId     Unique detector ID
  *@param pos       Position in global c.s. [cm]
  *@param dPos      Errors of position in global c.s. [cm]
  *@param covXY     Covariance of x and y [cm*2]
  *@param iDigiF    Digi index front side
  *@param iDigiB    Digi index back side
  **/
  CbmStsHit(Int_t detId, TVector3& pos, TVector3& dPos, Double_t covXY, 
	    Int_t iDigiF, Int_t iDigiB);

  CbmStsHit(Int_t detId, TVector3& pos, TVector3& dPos, Double_t covXY, 
	    Int_t iDigiF, Int_t iDigiB, Int_t iPosSX, Int_t iPosSY, Int_t iLayer);

  CbmStsHit(Int_t detId, TVector3& pos, TVector3& dPos, Double_t dxy, 
	    Int_t iDigiF, Int_t iDigiB, Double_t dSignalDiv);


  /** Destructor **/
  virtual ~CbmStsHit();


  /** Accessors **/

  
  Int_t    GetSystemId()           const { 
    return ( fDetectorID & (15<<24) ) >> 24; }
  virtual  Int_t    GetStationNr()  const { 
    return ( fDetectorID & (255<<16) ) >> 16; }
  Int_t    GetSectorNr()   const {  // sector number within station
    return ( fDetectorID & (4095<<4) ) >> 4; }
  Int_t    GetSensorNr()   const {  // sensor number within sector
    return ( fDetectorID & (7<<1) ) >> 1; }
  Int_t    GetDigi(Int_t iSide) const; // front side = 0; back side = 1

  Double_t GetSignalDiv()          const { return fSignalDiv; }

  Int_t GetPosSensorX()               const { return fPosSX;}
  Int_t GetPosSensorY()               const { return fPosSY;}

  void SetStatLayer(Int_t it)              { fStatLayer = it; }
  Int_t GetStatLayer()                const { return fStatLayer; }
  //AZ void AddIndex(int index){ AddLink(FairLink(kStsCluster, index)); }
  //void AddIndex(int index) { SetInsertHistory(kFALSE); AddLink(FairLink(kStsCluster, index)); } //AZ
  //void AddIndex(int index) { SetInsertHistory(kFALSE); if (GetPointerToLinks() == NULL) AddLink(FairLink()); SetInsertHistory(kFALSE); AddLink(FairLink(kStsCluster, index)); } //AZ
  void AddIndex(int index); //AZ
  void SetSignalDiv(Double_t sigDiv) { fSignalDiv = sigDiv; } //AZ

  /** Screen output **/
  virtual void Print(Option_t* opt) const;

  Int_t fDigiF;          // Channel number front side
  Int_t fDigiB;
 private:
  //Int_t fDigiF;          // Channel number front side
  //Int_t fDigiB;          // Channel number back side

  Int_t fPosSX;          // discretized X position in sensor
  Int_t fPosSY;          // discretized Y position in sensor

  Int_t fStatLayer;

  Double_t fSignalDiv;   // ADC front/ADC back, theoretically if 1 then hit is valid

  ClassDef(CbmStsHit,1);

};


#endif
