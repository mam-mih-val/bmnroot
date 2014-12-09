#ifndef BMNSTTCONTFACT_H
#define BMNSTTCONTFACT_H

#include "FairContFact.h"

class FairContainer;

class BmnSttContFact : public FairContFact {
private:
  void setAllContainers();
public:
  BmnSttContFact();
  ~BmnSttContFact() {}
  FairParSet* createContainer(FairContainer*);
  ClassDef( BmnSttContFact,0) // Factory for all Stt parameter containers
};

#endif  /* !BMNSTTCONTFACT_H */
