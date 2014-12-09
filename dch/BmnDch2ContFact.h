#ifndef BMNDCH2CONTFACT_H
#define BMNDCH2CONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnDch2ContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnDch2ContFact();
  ~BmnDch2ContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnDch2ContFact,0) // Factory for all DCH2 parameter containers
};

#endif  /* !BMNDCH2CONTFACT_H */
