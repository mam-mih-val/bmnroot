/*************************************************************************************
 *
 *            BmnHodoDigi
 *    Class for digital data taken from BmnHodo detector
 *
 *  Author:   Sergey Morozov
 *  Version:  17-03-2022
 *
 ************************************************************************************/

#ifndef BMNHODOGIGIT_H
#define BMNHODOGIGIT_H

#include "BmnHodoPoint.h"

class BmnHodoDigit: public TObject
{
 public:

  BmnHodoDigit();
  BmnHodoDigit(Int_t pfStripID, Double_t   pfELoss, Double_t   pfELossDigi=0 );
  BmnHodoDigit(BmnHodoPoint *p);

  virtual ~BmnHodoDigit();
  void Clear();

  virtual void Print(const Option_t* opt ="");


  inline Int_t  GetStripID() { return fStripID; }
  inline Double_t GetELossDigi()  { return fELossDigi; }
  inline Double_t GetELoss()  { return fELoss; }

  inline Int_t SetStripID(UInt_t pfStripID) { fStripID = pfStripID; return fStripID; }
  inline Double_t SetELoss(Double_t pfELoss) { fELoss = pfELoss; return fELoss; }
  inline Double_t SetELossDigi(Double_t pfELossDigi) { fELossDigi = pfELossDigi; return fELossDigi; }



 protected:


  Int_t    fStripID;          // Strip number
  Double_t   fELossDigi;     // Digitized signal
  Double_t fELoss;           // Sum of the energy losses from MC

  ClassDef(BmnHodoDigit,1);

};

#endif // BMNHODOGIGIT_H
