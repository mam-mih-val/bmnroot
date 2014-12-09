#ifndef BMNDCH1CONTFACT_H
#define BMNDCH1CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnDch1ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnDch1ContFact();
  ~BmnDch1ContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnDch1ContFact,0) // Factory for all DCH1 parameter containers
};

#endif  /* !BMNDCH1CONTFACT_H */
