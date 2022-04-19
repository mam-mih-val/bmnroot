//* $Id: */

// -------------------------------------------------------------------------
// -----                      CbmStsCluster header file                -----
// -----                  Created 25/06/2008  by R. Karabowicz         -----
// -------------------------------------------------------------------------


/** CbmStsCluster.h
 **@author V.Friese <v.friese@gsi.de>
 **@since 28.08.06
 **@version 1.0
 **
 ** Data class for digital STS information
 ** Data level: RAW
 **
 ** The detector ID consists of:
 **   system ID (0-31, kSTS=2), bits 0-4, see base class
 **   station number (0-255), bits 5-12 
 **   sector number (0-32767), bits 13-27
 **   side (0=front side, 1=back side, bit 28, always 0 for pixel sectors).
 ** 
 ** The index of the (first) MCPoint having activated this channel
 ** is accessible via GetMcPointIndex(). 
 **/

#ifndef CBMSTSCLUSTER_H
#define CBMSTSCLUSTER_H 1

#include "FairMultiLinkedData.h"

class CbmStsCluster : public FairMultiLinkedData
{
 public:

  /** Default constructor **/
  CbmStsCluster();


  /** Constructor from station number, sector number, 
   ** front/back side and channel number
   **@param iStation  station number (0-255)
   **@param iSector   sector number  (0-32767)
   **@param iSide     0=front side; 1=back side
   **@param iChannel  channel number
   **/
  CbmStsCluster(Double_t digiSig, Int_t iStation, Int_t iSector, Int_t iSide);

  /** Destructor **/
  virtual ~CbmStsCluster();

  void AddDigi(Int_t idigi)  { fDigis[fNofDigis++] = idigi; }
//   void AddIndex(int index, double digiSig){
// 			AddLink(FairLink(kStsDigi, index));
// 		}
  void SetMean     (Double_t chan) { fChannel = chan; }
  void SetMeanError(Double_t err)  { fError   = err;  }

  /** Accessors **/
  Int_t    GetDetectorId() const { return fDetectorId; }

  Int_t    GetStationNr()  const { 
    return ( fDetectorId & (255<<16) ) >> 16; }
  Int_t    GetSectorNr()   const {  // sector number within station
    return ( fDetectorId & (4095<<4) ) >> 4; }
  Int_t    GetSide()        const {
    return ( fDetectorId & (1<<0) ) >> 0; }  // 0=front, 1=back

  Int_t    GetNDigis()       const { return fNofDigis; }
  Int_t    GetDigi(Int_t inum )    { return fDigis[inum]; }
  Double_t GetMean()      const { return fChannel;  }
  Double_t GetMeanError() const { return fError;    }
  Double_t GetQtot()      const { return fQtot; } //AZ
  Int_t GetLeft()         const { return fLeftEdge; } //AZ
  void SetQtot(Double_t qtot) { fQtot = qtot; } //AZ
  void SetLeft(Int_t left) { fLeftEdge = left; } //AZ: < 0 if no left neighbour

 private:

  Int_t    fDetectorId;
  //  Int_t*   fDigis;//[100];
  Int_t   fDigis[100];
  Int_t    fNofDigis;
  Double_t fChannel;
  Double_t fError;
  Double32_t fQtot; //AZ
  Int_t fLeftEdge; //AZ

  ClassDef(CbmStsCluster,1);

};

#endif
