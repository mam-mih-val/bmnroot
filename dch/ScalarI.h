#ifndef SCALARI_H
#define SCALARI_H

// *************************************************************************
// Author: Jan Fedorishin, e-mail: fedorisin@jinr.ru
//   
// scalar Int_t quantity
//   
//
// Created: 08-11-2013
// Modified:
//
// *************************************************************************

#include "TObject.h"

//#include "FairTask.h"

//class Scalar : public FairTask {
class ScalarI : public TObject {

protected:
 
  Int_t scalval; 

public:    

  ScalarI();
  //ScalarI(const char *name, const char *title="CPC Task");
  ~ScalarI();

  void SetSV(Int_t x);
  Int_t GetSV();
  ClassDef(ScalarI,1)     

};

#endif
