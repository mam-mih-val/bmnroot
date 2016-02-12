// -------------------------------------------------------------------------
// -----                     BmnBarrelContFact header file                -----
// -------------------------------------------------------------------------


#ifndef BMNBARRELCONTFACT_H
#define BMNBARRELCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnBarrelContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnBarrelContFact();
  ~BmnBarrelContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnBarrelContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNBARRELCONTFACT_H */
