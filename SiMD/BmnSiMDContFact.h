// -------------------------------------------------------------------------
// -----                     BmnFdContFact header file                -----
// -------------------------------------------------------------------------


#ifndef BMNSiMDCONTFACT_H
#define BMNSiMDCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnSiMDContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnSiMDContFact();
  ~BmnSiMDContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnSiMDContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNSiMDCONTFACT_H */
