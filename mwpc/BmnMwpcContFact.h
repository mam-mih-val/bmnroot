#ifndef BMNMWPCCONTFACT_H
#define BMNMWPCCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnMwpcContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnMwpcContFact();
  ~BmnMwpcContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnMwpcContFact,0) // Factory for all MWPC1 parameter containers
};

#endif  /* !BMNMWPC1CONTFACT_H */
