/*************************************************************************************
 *
 *         BmnNdetDigitPar
 *    Container class for BmnNdet digitisation parameters  
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015   
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#ifndef BMNNDETDIGITPAR_H
#define BMNNDETDIGITPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnNdetDigitPar: public FairParGenericSet
{

 public :

  BmnNdetDigitPar (const char *name="BmnNdetDigitPar", const char *title="NDET Digit Parameters", const char *context="TestDefaultContext");

  virtual ~BmnNdetDigitPar() {};

  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
 
   inline Int_t    GetADCBits() { return fADCBits; };
   inline Double_t GetADCResolution() { return fADCResolution; };
   inline Double_t GetEnergyDigiThreshold() { return fEnergyDigiThreshold; };

 private:

   Int_t    fADCBits;       // Hardware parameter
   Double_t fADCResolution; // Hardware parameter
   Double_t fEnergyDigiThreshold;

  ClassDef(BmnNdetDigitPar,2);

};
#endif // BMNNDETDIGITPAR_H
