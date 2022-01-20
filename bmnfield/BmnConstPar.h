#ifndef BMNCONSTPAR_H
#define BMNCONSTPAR_H 1

#include "BmnMapPar.h"
#include "BmnFieldMap.h"
#include "BmnMapPar.h"

class FairParamList;

class BmnConstPar : public BmnMapPar
{
  
 public:
  
  /** Standard constructor  **/
  BmnConstPar(const char* name, const char* title, const char* context);
  
  /** default constructor  **/
  BmnConstPar();
  
  /** Destructor **/
  ~BmnConstPar();
  
  void putParams(FairParamList* list);

  /** Get parameters **/
  Bool_t getParams(FairParamList* list);
  
  /** Set parameters from FairField  **/
  void SetParameters(FairField* field);
  
  Double_t GetBx()        const { return fBx; }
  Double_t GetBy()        const { return fBy; }
  Double_t GetBz()        const { return fBz; }

 protected:
  
  /** Field values in [kG] **/
  Double_t fBx, fBy, fBz;
  
  ClassDef(BmnConstPar,1);
};

#endif
