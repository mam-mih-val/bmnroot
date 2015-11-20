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

#ifndef BMNZDCDIGIPAR_H
#define BMNZDCDIGIPAR_H

#include "FairParGenericSet.h"
#include "TObjArray.h"

class BmnZdcDigiPar: public FairParGenericSet
{

 public :

  BmnZdcDigiPar (const char *name="BmnZdcDigiPar", const char *title="ZDC Digi Parameters",
		 const char *context="TestDefaultContext");
  virtual ~BmnZdcDigiPar() {};

  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
 
 private:


  ClassDef(BmnZdcDigiPar,1);

};
#endif // BMNZDCDIGIPAR_H
