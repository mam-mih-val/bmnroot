
#ifndef BMNFDCONTFACT_H
#define BMNFDCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnFDContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnFDContFact();
  ~BmnFDContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnFDContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNFDCONTFACT_H */
