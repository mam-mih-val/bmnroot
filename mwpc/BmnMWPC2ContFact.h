#ifndef BMNMWPC2CONTFACT_H
#define BMNMWPC2CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnMWPC2ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnMWPC2ContFact();
  ~BmnMWPC2ContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnMWPC2ContFact,0) // Factory for all MWPC2 parameter containers
};

#endif  /* !BMNMWPC2CONTFACT_H */
