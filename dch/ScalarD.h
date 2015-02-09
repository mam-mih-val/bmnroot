#ifndef SCALARD_H
#define SCALARD_H

// *************************************************************************
// Author: Jan Fedorishin, e-mail: fedorisin@jinr.ru
//   
// scalar Double_t quantity
//   
//
// Created: 18-06-2013
// Modified:
//
// *************************************************************************

#include "TObject.h"

//#include "FairTask.h"

//class Scalar : public FairTask {
class ScalarD : public TObject {

protected:
 
  Double_t scalval; 

public:    

  ScalarD();
  //ScalarD(const char *name, const char *title="CPC Task");
  ~ScalarD();

  void SetSV(Double_t x);
  Double_t GetSV();
  ClassDef(ScalarD,1)     

};

#endif
