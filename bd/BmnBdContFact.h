
#ifndef BMNBDCONTFACT_H
#define BMNBDCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnBdContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnBdContFact();
  ~BmnBdContFact() {}
  FairParSet* createContainer(FairContainer*);

  ClassDef( BmnBdContFact,0) // Factory for all HYP parameter containers
};

#endif  /* !BMNBDCONTFACT_H */
