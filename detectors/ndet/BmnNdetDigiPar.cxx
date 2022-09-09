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

#include "BmnNdetDigiPar.h"
#include "FairParamList.h"

ClassImp(BmnNdetDigiPar)

BmnNdetDigiPar::BmnNdetDigiPar (const char *name, const char *title,
			      const char *context)
  : FairParGenericSet(name, title, context)
{

}

void BmnNdetDigiPar::putParams(FairParamList* list)
{
  if(!list) return;
   list->add("ADCBits", fADCBits);
   list->add("ADCResolution", fADCResolution);
   list->add("EnergyDigiThreshold", fEnergyDigiThreshold);
  //  list->add("param",fparam);
}
Bool_t BmnNdetDigiPar::getParams(FairParamList* list)
{
  if (!list) return kFALSE;
   if (!list->fill("ADCBits", &fADCBits)) return kFALSE;
   if (!list->fill("ADCResolution", &fADCResolution)) return kFALSE;
   if (!list->fill("EnergyDigiThreshold", &fEnergyDigiThreshold)) return kFALSE;
  //  if (!list->fill("param",&fparam)) return kFALSE;
  return kTRUE;
}
 
