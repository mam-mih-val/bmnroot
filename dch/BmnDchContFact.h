#ifndef BMNDCHCONTFACT_H
#define BMNDCHCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnDchContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnDchContFact();
  ~BmnDchContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnDchContFact,0) // Factory for all DCH parameter containers
};

#endif  /* !BMNDCHCONTFACT_H */
