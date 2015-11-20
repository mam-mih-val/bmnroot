/*************************************************************************************
 *
 *            BmnZdcDigi 
 *    Class for digital data taken from BmnZdc detector 
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015
 *
 ************************************************************************************/

#ifndef BMNZDCDIGI_H
#define BMNZDCDIGI_H

#include "BmnZdcPoint.h"

class BmnZdcDigi: public TObject
{
 public:

  BmnZdcDigi();
  BmnZdcDigi(Int_t pfGroupID, Int_t pfModuleID, Int_t pfChannelID, Double_t   pfELoss, Double_t   pfELossDigi=0 );
  BmnZdcDigi(BmnZdcPoint *p);

  virtual ~BmnZdcDigi();
  void Clear();
  
  void InitStatic();
  virtual void Print(const Option_t* opt ="");


  inline Int_t  GetGroupID() { return fGroupID; } 
  inline Int_t  GetModuleID() { return fModuleID; } 
  inline Int_t  GetChannelID()  { return fChannelID; } 
  inline Double_t GetELossDigi()  { return fELossDigi; } 
  inline Double_t  GetELoss()               { return fELoss; }

  inline Int_t    SetGroupID(UInt_t pfGroupID)         { fGroupID=pfGroupID;       return fGroupID; } 
  inline Int_t    SetChannelID(UInt_t pfChannelID)           { fChannelID=pfChannelID;         return fChannelID; } 


  inline Double_t AddEloss    (Double_t pfELoss)       { fELoss += pfELoss;  return fELoss;}
  UInt_t   AddZdcPoint (BmnZdcPoint *p);

 protected:


  Int_t    fGroupID;         // Module Group number = "size" (1: central small, 2 - periferal big, can be also 0 - for hardware summators)
  Int_t    fModuleID;        // Module number inside module group (36 central small modules, up to 58 periferal big modules)
  Int_t    fChannelID;       // Hardware ZDC channel number (corresponds to the ascii map file)
  Double_t   fELossDigi;     // Digital response of the ZDC channel (taken from experimental data)
  Double_t fELoss;           // Sum of the energy losses from MC or from experimental data after calibration

  ClassDef(BmnZdcDigi,1);

};

#endif // BMNZDCDIGI_H
