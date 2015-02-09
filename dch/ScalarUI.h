#ifndef SCALARUI_H
#define SCALARUI_H

// *************************************************************************
// Author: Jan Fedorishin, e-mail: fedorisin@jinr.ru
//   
// scalar Int_t quantity
//   
//
// Created: 16-12-2013
// Modified:
//
// *************************************************************************

#include "TObject.h"

//#include "FairTask.h"

//class Scalar : public FairTask {
class ScalarUI : public TObject {

protected:
 
  UInt_t scalval; 

public:    

  ScalarUI();
  //ScalarI(const char *name, const char *title="CPC Task");
  ~ScalarUI();

  void SetSV(UInt_t x);
  UInt_t GetSV();
  ClassDef(ScalarUI,1)     

};

#endif
