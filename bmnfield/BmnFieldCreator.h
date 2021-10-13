// -------------------------------------------------------------------------
// -----                    BmnFieldCreator header file                  -----
// -----                Created 15/01/07  by M. Al-Turany              -----
// -------------------------------------------------------------------------


#ifndef CBMFIELDCREATOR_H
#define CBMFIELDCREATOR_H

#include "FairFieldFactory.h"

class BmnFieldPar;

class FairField;

class BmnFieldCreator : public FairFieldFactory 
{

 public:
  BmnFieldCreator();
  virtual ~BmnFieldCreator();
  virtual FairField* createFairField();
  virtual void SetParm();
  ClassDef(BmnFieldCreator,1);
  
 protected:
  BmnFieldPar* fFieldPar;
  
 private:
  BmnFieldCreator(const BmnFieldCreator&) = delete;
  BmnFieldCreator& operator=(const BmnFieldCreator&) = delete;
};

#endif //CBMFIELDCREATOR_H
