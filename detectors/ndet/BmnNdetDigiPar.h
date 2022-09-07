/*************************************************************************************
 *
 *         BmnNdetDigiPar
 *    Container class for BmnNdet digitisation parameters  
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015   
 *  Modified by M.Golubeva July 2022
 *
 ************************************************************************************/

#ifndef BMNNDETDIGIPAR_H
#define BMNNDETDIGIPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnNdetDigiPar: public FairParGenericSet
{

 public :

  BmnNdetDigiPar (const char *name="BmnNdetDigiPar", const char *title="NDET Digi Parameters", const char *context="TestDefaultContext");

  virtual ~BmnNdetDigiPar() {};

  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
 
   inline Int_t    GetADCBits() { return fADCBits; };
   inline Double_t GetADCResolution() { return fADCResolution; };
   inline Double_t GetEnergyDigiThreshold() { return fEnergyDigiThreshold; };

 private:

   Int_t    fADCBits;       // Hardware parameter
   Double_t fADCResolution; // Hardware parameter
   Double_t fEnergyDigiThreshold;

  ClassDef(BmnNdetDigiPar,2);

};
#endif // BMNNDETDIGIPAR_H
