/*************************************************************************************
 *
 *         BmnZdcDigiPar
 *    Container class for BmnZdc digitisation parameters  
 *         
 *  Author:   Elena Litvinenko
 *  e-mail:   litvin@nf.jinr.ru
 *  Version:  18-11-2015   
 *
 ************************************************************************************/

#include "BmnZdcDigiPar.h"
#include "FairParamList.h"

ClassImp(BmnZdcDigiPar)

BmnZdcDigiPar::BmnZdcDigiPar (const char *name, const char *title,
			      const char *context)
  : FairParGenericSet(name, title, context)
{

}

void BmnZdcDigiPar::putParams(FairParamList* list)
{
  if(!list) return;
  //  list->add("param",fparam);
}
Bool_t BmnZdcDigiPar::getParams(FairParamList* list)
{
  if (!list) return kFALSE;
  //  if (!list->fill("param",&fparam)) return kFALSE;
  return kTRUE;
}
 
