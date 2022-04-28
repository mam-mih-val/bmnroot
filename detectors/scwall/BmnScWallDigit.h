/*************************************************************************************
 *
 *            BmnScWallDigi
 *    Class for digital data taken from BmnScWall detector
 *
 *  Author:   Sergey Morozov
 *  Version:  17-03-2022
 *
 ************************************************************************************/

#ifndef BMNSCWALLGIGIT_H
#define BMNSCWALLGIGIT_H

#include "BmnScWallPoint.h"

class BmnScWallDigit: public TObject
{
 public:

  BmnScWallDigit();
  BmnScWallDigit(Int_t pfCellID, Double_t   pfELoss, Double_t   pfELossDigi=0 );
  BmnScWallDigit(BmnScWallPoint *p);

  virtual ~BmnScWallDigit();
  void Clear();

  virtual void Print(const Option_t* opt ="");


  inline Int_t  GetCellID() { return fCellID; }
  inline Double_t GetELossDigi()  { return fELossDigi; }
  inline Double_t GetELoss()  { return fELoss; }

  inline Int_t SetCellID(UInt_t pfCellID) { fCellID = pfCellID; return fCellID; }
  inline Double_t SetELoss(Double_t pfELoss) { fELoss = pfELoss; return fELoss; }
  inline Double_t SetELossDigi(Double_t pfELossDigi) { fELossDigi = pfELossDigi; return fELossDigi; }



 protected:


  Int_t    fCellID;          // Cell number
  Double_t   fELossDigi;     // Digitized signal
  Double_t fELoss;           // Sum of the energy losses from MC

  ClassDef(BmnScWallDigit,1);

};

#endif // BMNSCWALLGIGIT_H
