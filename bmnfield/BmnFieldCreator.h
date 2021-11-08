// -------------------------------------------------------------------------
// -----                   BmnFieldCreator header file                 -----
// -----                Created 15/01/07 by M. Al-Turany               -----
// -------------------------------------------------------------------------

#ifndef BMNFIELDCREATOR_H
#define BMNFIELDCREATOR_H

#include "BmnFieldPar.h"

#include "FairFieldFactory.h"
#include "FairField.h"

class BmnFieldCreator : public FairFieldFactory 
{
 public:
  BmnFieldCreator();
  virtual ~BmnFieldCreator();

  virtual FairField* createFairField();
  virtual void SetParm();
  
 protected:
  BmnFieldPar* fFieldPar;
  
 private:
  BmnFieldCreator(const BmnFieldCreator&) = delete;
  BmnFieldCreator& operator=(const BmnFieldCreator&) = delete;

  ClassDef(BmnFieldCreator,1);
};

#endif //BMNFIELDCREATOR_H
