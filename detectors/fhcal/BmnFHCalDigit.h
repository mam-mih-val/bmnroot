/*************************************************************************************
 *
 *            BmnFHCalDigi
 *    Class for digital data taken from BmnFHCal detector
 *
 *  Author:   Sergey Morozov
 *  Version:  17-03-2022
 *
 ************************************************************************************/

#ifndef BMNFHCALDIGIT_H
#define BMNFHCALDIGIT_H

#include "BmnFHCalPoint.h"

class BmnFHCalDigit: public TObject
{
 public:

  BmnFHCalDigit();
  BmnFHCalDigit(Int_t pfModuleID, Int_t pfSectionID, Double_t   pfELoss, Double_t   pfELossDigi=0 );
  BmnFHCalDigit(BmnFHCalPoint *p);

  virtual ~BmnFHCalDigit();
  void Clear();

  virtual void Print(const Option_t* opt ="");


  inline Int_t  GetModuleID() { return fModuleID; }
  inline Int_t  GetSectionID()  { return fSectionID; }
  inline Double_t GetELossDigi()  { return fELossDigi; }
  inline Double_t GetELoss()  { return fELoss; }

  inline Int_t SetModuleID(UInt_t pfModuleID) { fModuleID = pfModuleID; return fModuleID; }
  inline Int_t SetSectionID(UInt_t pfSectionID) { fSectionID = pfSectionID; return fSectionID; }
  inline Double_t SetELoss(Double_t pfELoss) { fELoss = pfELoss; return fELoss; }
  inline Double_t SetELossDigi(Double_t pfELossDigi) { fELossDigi = pfELossDigi; return fELossDigi; }



 protected:


  Int_t    fModuleID;        // Module number
  Int_t    fSectionID;       // Section number
  Double_t   fELossDigi;     // Digitized signal
  Double_t fELoss;           // Sum of the energy losses from MC

  ClassDef(BmnFHCalDigit,1);

};

#endif // BMNFHCALDIGIT_H
