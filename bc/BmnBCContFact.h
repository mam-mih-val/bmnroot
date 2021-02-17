// -------------------------------------------------------------------------
// -----                     BmnBdContFact header file                -----
// -------------------------------------------------------------------------


#ifndef BMNBCCONTFACT_H
#define BMNBCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnBCContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnBCContFact();
  ~BmnBCContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnBCContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNBCCONTFACT_H */
