#ifndef BMNMWPC3CONTFACT_H
#define BMNMWPC3CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnMWPC3ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnMWPC3ContFact();
  ~BmnMWPC3ContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnMWPC3ContFact,0) // Factory for all MWPC3 parameter containers
};

#endif  /* !BMNMWPC3CONTFACT_H */
