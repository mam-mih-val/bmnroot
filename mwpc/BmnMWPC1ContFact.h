#ifndef BMNMWPC1CONTFACT_H
#define BMNMWPC1CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnMWPC1ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnMWPC1ContFact();
  ~BmnMWPC1ContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnMWPC1ContFact,0) // Factory for all MWPC1 parameter containers
};

#endif  /* !BMNMWPC1CONTFACT_H */
